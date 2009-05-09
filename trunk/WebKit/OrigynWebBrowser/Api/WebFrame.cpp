/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebFrame.h"

#include "DefaultPolicyDelegate.h"
#include "DOMCoreClasses.h"
#include "FormValuesPropertyBag.h"
#include "WebActionPropertyBag.h"
#include "WebChromeClient.h"
#include "WebDocumentLoader.h"
#include "WebDownload.h"
#include "WebError.h"
#include "WebFrameLoaderClient.h"
#include "WebMutableURLRequest.h"
#include "WebEditorClient.h"
#include "WebFramePolicyListener.h"
#include "WebHistory.h"
#include "WebView.h"
#include "WebDataSource.h"
#include "WebHistoryItem.h"
#include "WebPreferences.h"
#include "WebURLResponse.h"

#include <PlatformString.h>
#include <Bookmarklet.h>
#include <Cache.h>
#include <Event.h>
#include <FormState.h>
#include <FrameLoader.h>
#include <FrameLoadRequest.h>
#include <FrameTree.h>
#include <FrameView.h>
#include <Frame.h>
#include <GraphicsContext.h>
#include <HistoryItem.h>
#include <HTMLAppletElement.h>
#include <HTMLFormElement.h>
#include <HTMLPlugInElement.h> 
#include <JSDOMWindow.h>
#include <KeyboardEvent.h>
#include <MIMETypeRegistry.h>
#include <MouseRelatedEvent.h>
#include <NotImplemented.h>
#include <Page.h>
#include <PlatformKeyboardEvent.h>
#include <PluginInfoStore.h>
#include <PluginDatabase.h>
#include <PluginView.h>
#include <PutPropertySlot.h>
#include <ResourceHandle.h>
#include <ResourceHandle.h>
#include <ResourceRequest.h>
#include <RenderView.h>
#include <RenderTreeAsText.h>
#include <Settings.h>
#include <TextIterator.h>
#include <JSDOMBinding.h>
#include <ScriptController.h>
#include <API/APICast.h>
#include <wtf/MathExtras.h>
#include "ObserverServiceBookmarklet.h"
#include "ObserverBookmarklet.h"
#include "ObserverServiceData.h"
#include "ObserverData.h"
#include "WebObject.h"
#include "bal_instance.h"
#include "runtime.h"
#include "runtime_root.h"
#include "runtime_object.h"
#include "CallFrame.h"
#include "JSGlobalObject.h"
#include "JSLock.h"
#include "JSObject.h"
#include "JSDOMWindowBase.h"
#include "JSDOMWindow.h"
#if ENABLE(JS_ADDONS)
#include "BindingJS.h"
#endif

using namespace WebCore;

#define FLASH_REDRAW 0


// By imaging to a width a little wider than the available pixels,
// thin pages will be scaled down a little, matching the way they
// print in IE and Camino. This lets them use fewer sheets than they
// would otherwise, which is presumably why other browsers do this.
// Wide pages will be scaled down more than this.
const float PrintingMinimumShrinkFactor = 1.25f;

// This number determines how small we are willing to reduce the page content
// in order to accommodate the widest line. If the page would have to be
// reduced smaller to make the widest line fit, we just clip instead (this
// behavior matches MacIE and Mozilla, at least)
const float PrintingMaximumShrinkFactor = 2.0f;



class WebFrameObserver : public ObserverData, public ObserverBookmarklet {
public:
    WebFrameObserver(WebFrame *webFrame) : m_webFrame(webFrame) {}
    ~WebFrameObserver() {m_webFrame = 0;}

    virtual void observe(const WebCore::String &topic, const WebCore::String &data, void *userData);
    virtual void observe(const String &topic, Bookmarklet *bookmarklet);
private:
    WebFrame *m_webFrame;
};

void WebFrameObserver::observe(const WebCore::String &topic, const WebCore::String &data, void *userData)
{
}

void WebFrameObserver::observe(const String &topic, Bookmarklet *bookmarklet)
{
    ASSERT(bookmarklet);
    if (topic == "ExecuteBookmarklet")
        m_webFrame->webView()->stringByEvaluatingJavaScriptFromString(bookmarklet->data().utf8().data());
}


//-----------------------------------------------------------------------------
// Helpers to convert from WebCore to WebKit type
WebFrame* kit(Frame* frame)
{
    if (!frame)
        return 0;

    FrameLoaderClient* frameLoaderClient = frame->loader()->client();
    if (frameLoaderClient)
        return static_cast<WebFrameLoaderClient*>(frameLoaderClient)->webFrame();
    return 0;
}

Frame* core(WebFrame* webFrame)
{
    if (!webFrame)
        return 0;
    return webFrame->impl();
}

// This function is not in WebFrame.h because we don't want to advertise the ability to get a non-const Frame from a const WebFrame
Frame* core(const WebFrame* webFrame)
{
    if (!webFrame)
        return 0;
    return const_cast<WebFrame*>(webFrame)->impl();
}

//-----------------------------------------------------------------------------


class WebFrame::WebFramePrivate {
public:
    WebFramePrivate()
        : webView(0)
    {
    }

    ~WebFramePrivate()
    {
        webView = 0;
    }
    FrameView* frameView() { return frame ? frame->view() : 0; }

    RefPtr<Frame> frame;
    WebView* webView;
};

// WebFrame ----------------------------------------------------------------

WebFrame::WebFrame()
    : d(new WebFrame::WebFramePrivate)
    , m_loadClient(0)
    , m_quickRedirectComing(false)
    , m_inPrintingMode(false)
    , m_pageHeight(0)
    , m_webFrameObserver(new WebFrameObserver(this))
{
#if ENABLE(JS_ADDONS)
    m_bindingJS = new BindingJS(this);
#endif
    WebCore::ObserverServiceBookmarklet::createObserverService()->registerObserver("ExecuteBookmarklet", static_cast<ObserverBookmarklet*>(m_webFrameObserver));
}

WebFrame::~WebFrame()
{
    WebCore::ObserverServiceBookmarklet::createObserverService()->removeObserver("ExecuteBookmarklet", static_cast<ObserverBookmarklet*>(m_webFrameObserver));
#if ENABLE(JS_ADDONS)
    if (m_bindingJS)
        delete m_bindingJS;
    m_bindingJS = 0;
#endif

    std::vector<WebFrame*> child = children();
    std::vector<WebFrame*>::iterator it = child.begin();
    for (; it != child.end(); ++it)
        delete (*it);

    delete d;
    /*if (m_loadClient)
        delete m_loadClient;*/
}

WebFrame* WebFrame::createInstance()
{
    WebFrame* instance = new WebFrame();
    return instance;
}

void WebFrame::setAllowsScrolling(bool flag)
{
    /*if (Frame* frame = core(this))
        if (FrameView* view = frame->view())
            view->setCanHaveScrollbars(!!flag);*/
}

bool WebFrame::allowsScrolling()
{
    /*if (flag)
        if (Frame* frame = core(this))
            if (FrameView* view = frame->view())
                return view->canHaveScrollbars();*/

    return false;
}

void WebFrame::setIsDisconnected(bool flag)
{
    if (Frame* frame = core(this))
        frame->setIsDisconnected(flag);
}

void WebFrame::setExcludeFromTextSearch(bool flag)
{
    if (Frame* frame = core(this)) {
        frame->setExcludeFromTextSearch(flag);
        return;
    }
}

const char* WebFrame::name()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return strdup("");

    if (!coreFrame->document())
        return strdup("");

    const AtomicString& frameName = coreFrame->tree()->name();
    if (!frameName.isEmpty())
        return strdup(frameName.string().utf8().data());

    const CString& frameTitle = coreFrame->loader()->documentLoader()->title().utf8();
    return strdup(frameTitle.data());
}

void WebFrame::setName(const char* frameName)
{
    Frame* coreFrame = core(this);
    // FIXME: we should have a way of knowing it is too early to set the frame name
    // because the WebFrame was not initialized.
    if (!coreFrame)
        return;

    coreFrame->tree()->setName(frameName);
}

WebView* WebFrame::webView()
{
    return d->webView;
}

DOMDocument* WebFrame::domDocument()
{
    if (Frame* coreFrame = core(this))
        if (Document* document = coreFrame->document())
            return DOMDocument::createInstance(document);
    return 0;
}

DOMElement* WebFrame::currentForm()
{
    if (Frame* coreFrame = core(this))
        if (HTMLFormElement* formElement = coreFrame->currentForm())
            return DOMElement::createInstance(formElement);
    return 0;
}

JSGlobalContextRef WebFrame::globalContext()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return 0;

    return toGlobalRef(coreFrame->script()->globalObject()->globalExec());
}

bool isAbsolute(const char *url)
{
    if (!url)
        return false;
    if (url[0] == '/')
        return true;
    return false;
}

void WebFrame::loadURL(const char* url)
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return;

    if (isAbsolute(url)) {
        string u = "file://";
        u += url;
        coreFrame->loader()->load(ResourceRequest(KURL(KURL(), String::fromUTF8(u.c_str()))), false);
    } else
        coreFrame->loader()->load(ResourceRequest(KURL(KURL(), String::fromUTF8(url))), false);
}

void WebFrame::loadRequest(WebMutableURLRequest* request)
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return;

    coreFrame->loader()->load(request->resourceRequest(), false);
}

void WebFrame::loadHTMLString(const char* string, const char* baseURL, const char* unreachableURL)
{
    RefPtr<SharedBuffer> data = SharedBuffer::create(string, strlen(string));
    String utf8Encoding("utf-8");
    String mimeType("text/html");

    KURL baseKURL(baseURL);
    KURL failingKURL;

    ResourceRequest request(baseKURL);
    SubstituteData substituteData(data.release(), mimeType, utf8Encoding, failingKURL);

    if (Frame* coreFrame = core(this))
        coreFrame->loader()->load(request, substituteData, false);
}

void WebFrame::loadHTMLString(const char* string, const char* baseURL)
{
    loadHTMLString(string, baseURL, "");
}

void WebFrame::loadAlternateHTMLString(const char* str, const char* baseURL, const char* unreachableURL)
{
    loadHTMLString(str, baseURL, unreachableURL);
}

void WebFrame::loadArchive(WebArchive* /*archive*/)
{
    BalNotImplemented();
}

static inline WebDataSource *getWebDataSource(DocumentLoader* loader)
{
    return loader ? static_cast<WebDocumentLoader*>(loader)->dataSource() : 0;
}

WebDataSource* WebFrame::dataSource()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return 0;

    return getWebDataSource(coreFrame->loader()->documentLoader());
}

WebDataSource* WebFrame::provisionalDataSource()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return 0;

    return getWebDataSource(coreFrame->loader()->provisionalDocumentLoader());
}

const char* WebFrame::url() const
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return strdup("");

    return strdup(coreFrame->loader()->url().string().utf8().data());
}

void WebFrame::stopLoading()
{
    if (Frame* coreFrame = core(this))
        coreFrame->loader()->stopAllLoaders();
}

void WebFrame::reload()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return;

    coreFrame->loader()->reload();
}

WebFrame* WebFrame::findFrameNamed(const char* name)
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return 0;

    String nameString = String::fromUTF8(name);
    Frame* foundFrame = coreFrame->tree()->find(AtomicString(nameString));
    if (!foundFrame)
        return 0;

    WebFrame* foundWebFrame = kit(foundFrame);
    if (!foundWebFrame)
        return 0;

    return foundWebFrame;
}

WebFrame* WebFrame::parentFrame()
{
    if (Frame* coreFrame = core(this))
        if (WebFrame* webFrame = kit(coreFrame->tree()->parent()))
            return webFrame;

    return 0;
}

std::vector<WebFrame*> WebFrame::children()
{
    std::vector<WebFrame*> rc;
    if (Frame* frame = core(this)) {
        FrameTree *tree = frame->tree();
        for (Frame *child = tree->firstChild(); child; child = child->tree()->nextSibling()) {
            FrameLoader *loader = child->loader();
            WebFrameLoaderClient *client = static_cast<WebFrameLoaderClient*>(loader->client());
            if (client)
                rc.push_back(client->webFrame());
        }

    }
    return rc; 
}


const char* WebFrame::renderTreeAsExternalRepresentation()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return "";

    return externalRepresentation(coreFrame->contentRenderer()).utf8().data();
}

BalPoint WebFrame::scrollOffset()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return IntPoint();

    FrameView* view = coreFrame->view();
    if (!view)
        return IntPoint();

    IntPoint p(view->scrollOffset().width(), view->scrollOffset().height());
    return p;
}

void WebFrame::layout()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return ;

    FrameView* view = coreFrame->view();
    if (!view)
        return ;

    view->layout();
}

bool WebFrame::firstLayoutDone()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return false;

    return coreFrame->loader()->firstLayoutDone();
}

WebFrameLoadType WebFrame::loadType()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return WebFrameLoadTypeStandard;

    return (WebFrameLoadType)coreFrame->loader()->loadType();
}

bool WebFrame::supportsTextEncoding()
{
    return false;
}

const char* WebFrame::selectedString()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return "";

    String text = coreFrame->displayStringModifiedByEncoding(coreFrame->selectedText());
    return text.utf8().data();
}

void WebFrame::selectAll()
{
    BalNotImplemented();
}

void WebFrame::deselectAll()
{
    BalNotImplemented();
}

Frame* WebFrame::init(WebView* webView, Page* page, HTMLFrameOwnerElement* ownerElement)
{
    d->webView = webView;
    if (!m_loadClient)
        m_loadClient = new WebFrameLoaderClient(this);

    d->frame = Frame::create(page, ownerElement, m_loadClient);
    return d->frame.get();
}

Frame* WebFrame::impl()
{
    return d->frame.get();
}

void WebFrame::invalidate()
{
    Frame* coreFrame = core(this);
    ASSERT(coreFrame);

    if (Document* document = coreFrame->document())
        document->recalcStyle(WebCore::Node::Force);
}

void WebFrame::setTextSizeMultiplier(float multiplier)
{
    Frame* coreFrame = core(this);
    ASSERT(coreFrame);
    coreFrame->setZoomFactor(multiplier, true);
}

bool WebFrame::inViewSourceMode()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return false;

    return coreFrame->inViewSourceMode() ? true : false;
}

void WebFrame::setInViewSourceMode(bool flag)
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return ;

    coreFrame->setInViewSourceMode(!!flag);
}

bool WebFrame::isDisplayingStandaloneImage()
{
    Frame* frame = core(this);
    if (!frame)
        return false;

    Document* document = frame->document();
    return (document && document->isImageDocument());
}

/*HRESULT WebFrame::elementWithName(BSTR name, IDOMElement* form, IDOMElement** element)
{
    if (!form)
        return E_INVALIDARG;

    HTMLFormElement *formElement = formElementFromDOMElement(form);
    if (formElement) {
        Vector<HTMLGenericFormElement*>& elements = formElement->formElements;
        AtomicString targetName((UChar*)name, SysStringLen(name));
        for (unsigned int i = 0; i < elements.size(); i++) {
            HTMLGenericFormElement *elt = elements[i];
            // Skip option elements, other duds
            if (elt->name() == targetName) {
                *element = DOMElement::createInstance(elt);
                return S_OK;
            }
        }
    }
    return E_FAIL;
}

HRESULT WebFrame::formForElement(IDOMElement* element, IDOMElement** form)
{
    if (!element)
        return E_INVALIDARG;

    HTMLInputElement *inputElement = inputElementFromDOMElement(element);
    if (!inputElement)
        return E_FAIL;

    HTMLFormElement *formElement = inputElement->form();
    if (!formElement)
        return E_FAIL;

    *form = DOMElement::createInstance(formElement);
    return S_OK;
}

HRESULT WebFrame::elementDoesAutoComplete(IDOMElement *element, bool *result)
{
    *result = false;
    if (!element)
        return E_INVALIDARG;

    HTMLInputElement *inputElement = inputElementFromDOMElement(element);
    if (!inputElement)
        *result = false;
    else
        *result = (inputElement->inputType() == HTMLInputElement::TEXT && inputElement->autoComplete());

    return S_OK;
}

HRESULT WebFrame::controlsInForm(IDOMElement* form, IDOMElement** controls, int* cControls)
{
    if (!form)
        return E_INVALIDARG;

    HTMLFormElement *formElement = formElementFromDOMElement(form);
    if (!formElement)
        return E_FAIL;

    int inCount = *cControls;
    int count = (int) formElement->formElements.size();
    *cControls = count;
    if (!controls)
        return S_OK;
    if (inCount < count)
        return E_FAIL;

    *cControls = 0;
    Vector<HTMLGenericFormElement*>& elements = formElement->formElements;
    for (int i = 0; i < count; i++) {
        if (elements.at(i)->isEnumeratable()) { // Skip option elements, other duds
            controls[*cControls] = DOMElement::createInstance(elements.at(i));
            (*cControls)++;
        }
    }
    return S_OK;
}

HRESULT WebFrame::elementIsPassword(IDOMElement *element, bool *result)
{
    HTMLInputElement *inputElement = inputElementFromDOMElement(element);
    *result = inputElement != 0
        && inputElement->inputType() == HTMLInputElement::PASSWORD;
    return S_OK;
}

HRESULT WebFrame::searchForLabelsBeforeElement(const BSTR* labels, int cLabels, IDOMElement* beforeElement, BSTR* result)
{
    if (!result) {
        ASSERT_NOT_REACHED();
        return E_POINTER;
    }

    *result = 0;

    if (!cLabels)
        return S_OK;
    if (cLabels < 1)
        return E_INVALIDARG;

    Frame* coreFrame = core(this);
    if (!coreFrame)
        return E_FAIL;

    Vector<String> labelStrings(cLabels);
    for (int i=0; i<cLabels; i++)
        labelStrings[i] = String(labels[i], SysStringLen(labels[i]));
    Element *coreElement = elementFromDOMElement(beforeElement);
    if (!coreElement)
        return E_FAIL;

    String label = coreFrame->searchForLabelsBeforeElement(labelStrings, coreElement);

    *result = SysAllocStringLen(label.characters(), label.length());
    if (label.length() && !*result)
        return E_OUTOFMEMORY;
    return S_OK;
}

HRESULT WebFrame::matchLabelsAgainstElement(const BSTR* labels, int cLabels, IDOMElement* againstElement, BSTR* result)
{
    if (!result) {
        ASSERT_NOT_REACHED();
        return E_POINTER;
    }

    *result = 0;

    if (!cLabels)
        return S_OK;
    if (cLabels < 1)
        return E_INVALIDARG;

    Frame* coreFrame = core(this);
    if (!coreFrame)
        return E_FAIL;

    Vector<String> labelStrings(cLabels);
    for (int i=0; i<cLabels; i++)
        labelStrings[i] = String(labels[i], SysStringLen(labels[i]));
    Element *coreElement = elementFromDOMElement(againstElement);
    if (!coreElement)
        return E_FAIL;

    String label = coreFrame->matchLabelsAgainstElement(labelStrings, coreElement);

    *result = SysAllocStringLen(label.characters(), label.length());
    if (label.length() && !*result)
        return E_OUTOFMEMORY;
    return S_OK;
}

HRESULT WebFrame::canProvideDocumentSource(bool* result)
{
    HRESULT hr = S_OK;
    *result = false;

    COMPtr<IWebDataSource> dataSource;
    hr = WebFrame::dataSource(&dataSource);
    if (FAILED(hr))
        return hr;

    COMPtr<IWebURLResponse> urlResponse;
    hr = dataSource->response(&urlResponse);
    if (SUCCEEDED(hr) && urlResponse) {
        BSTR mimeTypeBStr;
        if (SUCCEEDED(urlResponse->MIMEType(&mimeTypeBStr))) {
            String mimeType(mimeTypeBStr, SysStringLen(mimeTypeBStr));
            *result = mimeType == "text/html" || WebCore::DOMImplementation::isXMLMIMEType(mimeType);
            SysFreeString(mimeTypeBStr);
        }
    }
    return hr;
}*/

/*static IntRect printerRect(HDC printDC)
{
    return IntRect(0, 0,
                   GetDeviceCaps(printDC, PHYSICALWIDTH)  - 2 * GetDeviceCaps(printDC, PHYSICALOFFSETX),
                   GetDeviceCaps(printDC, PHYSICALHEIGHT) - 2 * GetDeviceCaps(printDC, PHYSICALOFFSETY));
}

void WebFrame::setPrinting(bool printing, float minPageWidth, float maxPageWidth, bool adjustViewSize)
{
    Frame* coreFrame = core(this);
    ASSERT(coreFrame);
    coreFrame->setPrinting(printing, minPageWidth, maxPageWidth, adjustViewSize);
}

HRESULT STDMETHODCALLTYPE WebFrame::setInPrintingMode( BOOL value, HDC printDC)
{
    if (m_inPrintingMode == !!value)
        return S_OK;

    Frame* coreFrame = core(this);
    if (!coreFrame)
        return E_FAIL;

    m_inPrintingMode = !!value;

    // If we are a frameset just print with the layout we have onscreen, otherwise relayout
    // according to the paper size
    float minLayoutWidth = 0.0f;
    float maxLayoutWidth = 0.0f;
    if (m_inPrintingMode && !coreFrame->isFrameSet()) {
        if (!printDC) {
            ASSERT_NOT_REACHED();
            return E_POINTER;
        }

        const int desiredHorizontalPixelsPerInch = 72;
        int paperHorizontalPixelsPerInch = ::GetDeviceCaps(printDC, LOGPIXELSX);
        int paperWidth = printerRect(printDC).width() * desiredHorizontalPixelsPerInch / paperHorizontalPixelsPerInch;
        minLayoutWidth = paperWidth * PrintingMinimumShrinkFactor;
        maxLayoutWidth = paperWidth * PrintingMaximumShrinkFactor;
    }

    setPrinting(m_inPrintingMode, minLayoutWidth, maxLayoutWidth, true);

    if (!m_inPrintingMode)
        m_pageRects.clear();

    return S_OK;
}

void WebFrame::headerAndFooterHeights(float* headerHeight, float* footerHeight)
{
    if (headerHeight)
        *headerHeight = 0;
    if (footerHeight)
        *footerHeight = 0;
    float height = 0;
    COMPtr<IWebUIDelegate> ui;
    if (FAILED(d->webView->uiDelegate(&ui)))
        return;
    COMPtr<IWebUIDelegate2> ui2;
    if (FAILED(ui->QueryInterface(IID_IWebUIDelegate2, (void**) &ui2)))
        return;
    if (headerHeight && SUCCEEDED(ui2->webViewHeaderHeight(d->webView, &height)))
        *headerHeight = height;
    if (footerHeight && SUCCEEDED(ui2->webViewFooterHeight(d->webView, &height)))
        *footerHeight = height;
}

IntRect WebFrame::printerMarginRect(HDC printDC)
{
    IntRect emptyRect(0, 0, 0, 0);

    COMPtr<IWebUIDelegate> ui;
    if (FAILED(d->webView->uiDelegate(&ui)))
        return emptyRect;
    COMPtr<IWebUIDelegate2> ui2;
    if (FAILED(ui->QueryInterface(IID_IWebUIDelegate2, (void**) &ui2)))
        return emptyRect;

    RECT rect;
    if (FAILED(ui2->webViewPrintingMarginRect(d->webView, &rect)))
        return emptyRect;

    rect.left = MulDiv(rect.left, ::GetDeviceCaps(printDC, LOGPIXELSX), 1000);
    rect.top = MulDiv(rect.top, ::GetDeviceCaps(printDC, LOGPIXELSY), 1000);
    rect.right = MulDiv(rect.right, ::GetDeviceCaps(printDC, LOGPIXELSX), 1000);
    rect.bottom = MulDiv(rect.bottom, ::GetDeviceCaps(printDC, LOGPIXELSY), 1000);

    return IntRect(rect.left, rect.top, (rect.right - rect.left), rect.bottom - rect.top);
}

const Vector<WebCore::IntRect>& WebFrame::computePageRects(HDC printDC)
{
    ASSERT(m_inPrintingMode);

    Frame* coreFrame = core(this);
    ASSERT(coreFrame);
    ASSERT(coreFrame->document());

    if (!printDC)
        return m_pageRects;

    // adjust the page rect by the header and footer
    float headerHeight = 0, footerHeight = 0;
    headerAndFooterHeights(&headerHeight, &footerHeight);
    IntRect pageRect = printerRect(printDC);
    IntRect marginRect = printerMarginRect(printDC);
    IntRect adjustedRect = IntRect(
        pageRect.x() + marginRect.x(),
        pageRect.y() + marginRect.y(),
        pageRect.width() - marginRect.x() - marginRect.right(),
        pageRect.height() - marginRect.y() - marginRect.bottom());

    computePageRectsForFrame(coreFrame, adjustedRect, headerHeight, footerHeight, 1.0,m_pageRects, m_pageHeight);

    return m_pageRects;
}

UINT WebFrame::getPrintedPageCount( HDC printDC)
{
    if (!pageCount || !printDC) {
        ASSERT_NOT_REACHED();
        return E_POINTER;
    }

    *pageCount = 0;

    if (!m_inPrintingMode) {
        ASSERT_NOT_REACHED();
        return E_FAIL;
    }

    Frame* coreFrame = core(this);
    if (!coreFrame || !coreFrame->document())
        return E_FAIL;

    const Vector<IntRect>& pages = computePageRects(printDC);
    *pageCount = (UINT) pages.size();

    return S_OK;
}

void* WebFrame::spoolPages(HDC printDC, UINT startPage, UINT endPage)
{
    if (!printDC || !ctx) {
        ASSERT_NOT_REACHED();
        return E_POINTER;
    }

    if (!m_inPrintingMode) {
        ASSERT_NOT_REACHED();
        return E_FAIL;
    }

    Frame* coreFrame = core(this);
    if (!coreFrame || !coreFrame->document())
        return E_FAIL;

    UINT pageCount = (UINT) m_pageRects.size();
    PlatformGraphicsContext* pctx = (PlatformGraphicsContext*)ctx;

    if (!pageCount || startPage > pageCount) {
        ASSERT_NOT_REACHED();
        return E_FAIL;
    }

    if (startPage > 0)
        startPage--;

    if (endPage == 0)
        endPage = pageCount;

    COMPtr<IWebUIDelegate> ui;
    if (FAILED(d->webView->uiDelegate(&ui)))
        return E_FAIL;
    // FIXME: we can return early after the updated app is released
    COMPtr<IWebUIDelegate2> ui2;
    if (FAILED(ui->QueryInterface(IID_IWebUIDelegate2, (void**) &ui2)))
        ui2 = 0;

    float headerHeight = 0, footerHeight = 0;
    headerAndFooterHeights(&headerHeight, &footerHeight);
    GraphicsContext spoolCtx(pctx);

    for (UINT ii = startPage; ii < endPage; ii++) {
        IntRect pageRect = m_pageRects[ii];

        CGContextSaveGState(pctx);

        IntRect printRect = printerRect(printDC);
        CGRect mediaBox = CGRectMake(CGFloat(0),
                                     CGFloat(0),
                                     CGFloat(printRect.width()),
                                     CGFloat(printRect.height()));

        CGContextBeginPage(pctx, &mediaBox);

        CGFloat scale = (float)mediaBox.size.width/ (float)pageRect.width();
        CGAffineTransform ctm = CGContextGetBaseCTM(pctx);
        ctm = CGAffineTransformScale(ctm, -scale, -scale);
        ctm = CGAffineTransformTranslate(ctm, CGFloat(-pageRect.x()), CGFloat(-pageRect.y()+headerHeight)); // reserves space for header
        CGContextScaleCTM(pctx, scale, scale);
        CGContextTranslateCTM(pctx, CGFloat(-pageRect.x()), CGFloat(-pageRect.y()+headerHeight));   // reserves space for header
        CGContextSetBaseCTM(pctx, ctm);

	coreFrame->view()->paintContents(&spoolCtx, pageRect);

        if (ui2) {
            CGContextTranslateCTM(pctx, CGFloat(pageRect.x()), CGFloat(pageRect.y())-headerHeight);

            int x = pageRect.x();
            int y = 0;
            if (headerHeight) {
                RECT headerRect = {x, y, x+pageRect.width(), y+(int)headerHeight};
                ui2->drawHeaderInRect(d->webView, &headerRect, (OLE_HANDLE)(LONG64)pctx);
            }

            if (footerHeight) {
                y = max((int)headerHeight+pageRect.height(), m_pageHeight-(int)footerHeight);
                RECT footerRect = {x, y, x+pageRect.width(), y+(int)footerHeight};
                ui2->drawFooterInRect(d->webView, &footerRect, (OLE_HANDLE)(LONG64)pctx, ii+1, pageCount);
            }
        }

        CGContextEndPage(pctx);
        CGContextRestoreGState(pctx);
    }

    return S_OK;
}*/

bool WebFrame::isFrameSet()
{
    Frame* coreFrame = core(this);
    if (!coreFrame || !coreFrame->document())
        return false;

    return coreFrame->document()->isFrameSet() ? true : false;
}

const char* WebFrame::toString()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return "";
    if (coreFrame->view() && coreFrame->view()->layoutPending())
        coreFrame->view()->layout();

    Element *documentElement = coreFrame->document()->documentElement();
    String string =  documentElement->innerText();
    return strdup(string.utf8().data());
}

const char* WebFrame::renderTreeDump() const
{
    Frame* coreFrame = core(this);
    if (coreFrame->view() && coreFrame->view()->layoutPending())
        coreFrame->view()->layout();

    String string = externalRepresentation(coreFrame->contentRenderer());
    return strdup(string.utf8().data());
}

BalPoint WebFrame::size()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return IntPoint();
    FrameView* view = coreFrame->view();
    if (!view)
        return IntPoint();
    return IntPoint(view->width(), view->height());
}

bool WebFrame::hasScrollBars()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return false;

    FrameView* view = coreFrame->view();
    if (!view)
        return false;

    if (view->horizontalScrollbar() || view->verticalScrollbar())
        return true;
    return false;
}

BalRectangle WebFrame::contentBounds()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return IntRect();

    FrameView* view = coreFrame->view();
    if (!view)
        return IntRect();

    return IntRect(0, 0, view->contentsWidth(), view->contentsHeight());
}

BalRectangle WebFrame::frameBounds()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return IntRect();

    FrameView* view = coreFrame->view();
    if (!view)
        return IntRect();

    FloatRect bounds = view->visibleContentRect();
    return IntRect(0, 0, (int)bounds.width(), (int)bounds.height());
}

bool WebFrame::isDescendantOfFrame(WebFrame *ancestor)
{
    Frame* coreFrame = core(this);

    return (coreFrame && coreFrame->tree()->isDescendantOf(core(ancestor))) ? true : false;
}

void WebFrame::unmarkAllMisspellings()
{
    Frame* coreFrame = core(this);
    for (Frame* frame = coreFrame; frame; frame = frame->tree()->traverseNext(coreFrame)) {
        Document *doc = frame->document();
        if (!doc)
            return;

        doc->removeMarkers(DocumentMarker::Spelling);
    }
}

void WebFrame::unmarkAllBadGrammar()
{
    Frame* coreFrame = core(this);
    for (Frame* frame = coreFrame; frame; frame = frame->tree()->traverseNext(coreFrame)) {
        Document *doc = frame->document();
        if (!doc)
            return;

        doc->removeMarkers(DocumentMarker::Grammar);
    }
}

void WebFrame::updateBackground()
{
    Color backgroundColor = webView()->transparent() ? Color::transparent : Color::white;
    Frame* coreFrame = core(this);

    if (!coreFrame || !coreFrame->view())
        return;

    coreFrame->view()->updateBackgroundRecursively(backgroundColor, webView()->transparent());
}

void WebFrame::addHistoryItemForFragmentScroll()
{
    BalNotImplemented();
}

bool WebFrame::shouldTreatURLAsSameAsCurrent(const char*) const
{
    BalNotImplemented();
    return false;
}

int WebFrame::numberOfActiveAnimations()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return 0;

    AnimationController* controller = coreFrame->animation();
    if (!controller)
        return 0;

    return controller->numberOfActiveAnimations();
}

bool WebFrame::pauseAnimation(const char* name, double time, const char* element)
{
    Element* coreElement = core(this)->document()->getElementById(AtomicString(element));
    if (!coreElement || !coreElement->renderer())
        return false;
    return core(this)->animation()->pauseAnimationAtTime(coreElement->renderer(), AtomicString(name), time);
}

bool WebFrame::pauseTransition(const char* name, double time, const char* element)
{
    Element* coreElement = core(this)->document()->getElementById(AtomicString(element));
    if (!coreElement || !coreElement->renderer())
        return false;
    return core(this)->animation()->pauseTransitionAtTime(coreElement->renderer(), AtomicString(name), time);
}

void WebFrame::setEditable(bool flag)
{
    Frame* frame = core(this);
    if (!frame)
        return;
    
    if (flag)
        frame->applyEditingStyleToBodyElement();
    else
        frame->removeEditingStyleFromBodyElement();
}

WebView* WebFrame::webView() const
{
    return d->webView;
}

void WebFrame::addToJSWindowObject(void* object)
{
    JSC::JSLock lock(false);
    JSDOMWindow *window = toJSDOMWindow(core(this));
    if (!window)
        return;

    RefPtr<JSC::Bindings::RootObject> root = core(this)->script()->bindingRootObject();
    // we want to put custom properties here, so get global object directly
    JSC::JSGlobalObject* global = core(this)->script()->globalObject();
    // root must be valid! if not create it
    if (!root)
    	root = JSC::Bindings::RootObject::create(window, global);
    JSC::ExecState* exec = global->globalExec();
    JSC::PropertySlot pr;

    WebObject *obj = static_cast<WebObject*>(object);
    if (!global->getOwnPropertySlot(exec, JSC::Identifier(exec, obj->getName()), pr)) {
	JSC::JSObject* runtimeObject = JSC::Bindings::BalInstance::getBalInstance(obj, root)->createRuntimeObject(exec);
        JSC::PutPropertySlot prop;
        global->put(exec, JSC::Identifier(exec, obj->getName()), runtimeObject, prop);
    }
}



