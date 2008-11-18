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
#include "WebChromeClient.h"

#include "WebElementPropertyBag.h"
#include "WebFrame.h"
#include "WebHistory.h"
#include "WebMutableURLRequest.h"
#include "WebSecurityOrigin.h"
#include "WebView.h"

#include <PlatformString.h>
#include "CString.h"
#include <ContextMenu.h>
#include <FloatRect.h>
#include <Frame.h>
#include <FrameLoadRequest.h>
#include <FrameView.h>
#include <Page.h>
#include <WindowFeatures.h>
#include DEEPSEE_INCLUDE
#if PLATFORM(AMIGAOS4)
#include <proto/requester.h>
#include <classes/requester.h>
#include <proto/intuition.h>
#include <intuition/gadgetclass.h>
#include <reaction/reaction_macros.h>
#endif

#include <cstdio>

using namespace WebCore;

WebChromeClient::WebChromeClient(WebView* webView)
    : m_webView(webView)
{
    DS_CONSTRUCT();
}

WebChromeClient::~WebChromeClient()
{
    DS_DESTRUCT();
}

void WebChromeClient::chromeDestroyed()
{
    delete this;
}

void WebChromeClient::setWindowRect(const FloatRect& r)
{
}

FloatRect WebChromeClient::windowRect()
{
    return FloatRect();
}

FloatRect WebChromeClient::pageRect()
{
    return m_webView->frameRect();
}

float WebChromeClient::scaleFactor()
{
    // Windows doesn't support UI scaling.
    return 1.0;
}

void WebChromeClient::focus()
{
}

void WebChromeClient::unfocus()
{
}

bool WebChromeClient::canTakeFocus(FocusDirection direction)
{
    return false;
}

void WebChromeClient::takeFocus(FocusDirection direction)
{
}

Page* WebChromeClient::createWindow(Frame*, const FrameLoadRequest& frameLoadRequest, const WindowFeatures& features)
{
#if PLATFORM(AMIGAOS4)
    if (features.dialog) {
        fprintf(stderr, "%s: features.dialog not implemented on AmigaOS4.\n", __PRETTY_FUNCTION__);
        return 0;
    }

    extern BalWidget *createAmigaWindow(WebView *);

    WebView* newWebView = WebView::createInstance();
    if (newWebView) {
        BalWidget *newowbwindow = createAmigaWindow(newWebView);
        if (newowbwindow) {
            IntRect clientRect(0, 0, newowbwindow->webViewWidth, newowbwindow->webViewHeight);
            newWebView->initWithFrame(clientRect, "", "");
            newWebView->setViewWindow(newowbwindow);

            return core(newWebView);
        }
        delete newWebView;
    }

    return 0;
#else
    if (features.dialog) {
        /*COMPtr<IWebUIDelegate3> delegate = uiDelegate3();
        if (!delegate)
            return 0;
        COMPtr<IWebMutableURLRequest> request(AdoptCOM, WebMutableURLRequest::createInstance(frameLoadRequest.resourceRequest()));
        COMPtr<IWebView> dialog;
        if (FAILED(delegate->createModalDialog(m_webView, request.get(), &dialog)))
            return 0;
        return core(dialog.get());*/
        return 0;
    }

    Page* page = 0;
    //IWebUIDelegate* uiDelegate = 0;
    WebMutableURLRequest* request = WebMutableURLRequest::createInstance(frameLoadRequest.resourceRequest());

    /*if (SUCCEEDED(m_webView->uiDelegate(&uiDelegate))) {
        IWebView* webView = 0;
        if (SUCCEEDED(uiDelegate->createWebViewWithRequest(m_webView, request, &webView))) {
            page = core(webView);
            webView->Release();
        }
    
        uiDelegate->Release();
    }*/
    delete request;

    return page;
#endif
}

void WebChromeClient::show()
{
}

bool WebChromeClient::canRunModal()
{
    return false;
}

void WebChromeClient::runModal()
{
}

void WebChromeClient::setToolbarsVisible(bool visible)
{
}

bool WebChromeClient::toolbarsVisible()
{
    return false;
}

void WebChromeClient::setStatusbarVisible(bool visible)
{
}

bool WebChromeClient::statusbarVisible()
{
    return false;
}

void WebChromeClient::setScrollbarsVisible(bool b)
{
    WebFrame* webFrame = m_webView->topLevelFrame();
    if (webFrame)
        webFrame->setAllowsScrolling(b);
}

bool WebChromeClient::scrollbarsVisible()
{
    WebFrame* webFrame = m_webView->topLevelFrame();
    bool b = false;
    if (webFrame)
        b = webFrame->allowsScrolling();

    return !!b;
}

void WebChromeClient::setMenubarVisible(bool visible)
{
}

bool WebChromeClient::menubarVisible()
{
    return false;
}

void WebChromeClient::setResizable(bool resizable)
{
}

void WebChromeClient::addMessageToConsole(const String& message, unsigned line, const String& url)
{
#if PLATFORM(AMIGAOS4)
    printf("JavaScript '%s' line %u: %s\n", url.latin1().data(), line, message.latin1().data());
#else
    printf("CONSOLE MESSAGE: line %d: %s\n", line, message.utf8().data());
#endif
}

bool WebChromeClient::canRunBeforeUnloadConfirmPanel()
{
    return false;
}

bool WebChromeClient::runBeforeUnloadConfirmPanel(const String& message, Frame* frame)
{
    printf("runBeforeUnloadConfirmPanel: %s\n", message.utf8().data());
    return false;
}

void WebChromeClient::closeWindowSoon()
{
    // We need to remove the parent WebView from WebViewSets here, before it actually
    // closes, to make sure that JavaScript code that executes before it closes
    // can't find it. Otherwise, window.open will select a closed WebView instead of 
    // opening a new one <rdar://problem/3572585>.

    // We also need to stop the load to prevent further parsing or JavaScript execution
    // after the window has torn down <rdar://problem/4161660>.
  
    // FIXME: This code assumes that the UI delegate will respond to a webViewClose
    // message by actually closing the WebView. Safari guarantees this behavior, but other apps might not.
    // This approach is an inherent limitation of not making a close execute immediately
    // after a call to window.close.

    m_webView->setGroupName("");
    m_webView->mainFrame()->stopLoading();
    m_webView->closeWindowSoon();
}

void WebChromeClient::runJavaScriptAlert(Frame *frame, const String& message)
{
#if PLATFORM(AMIGAOS4)
    CString messageLatin1 = message.latin1();
    Object *requester = (Object *)RequesterObject,
                                      REQ_CharSet, 4,
                                      REQ_TitleText, "OWB Javascript Alert",
                                      REQ_BodyText, messageLatin1.data(),
                                      REQ_GadgetText, "Ok",
                                  End;
    if (requester) {
        struct Window *window = m_webView->viewWindow()->window;
        struct Requester dummyRequester;

        if (window) {
            IIntuition->InitRequester(&dummyRequester);
            IIntuition->Request(&dummyRequester, window);
            IIntuition->SetWindowPointer(window, WA_BusyPointer, TRUE, WA_PointerDelay, TRUE, TAG_DONE);
        }

        OpenRequester(requester, window);

        if (window) {
            IIntuition->SetWindowPointer(window, WA_BusyPointer, FALSE, TAG_DONE);
            IIntuition->EndRequest(&dummyRequester, window);
        }

        IIntuition->DisposeObject(requester);
    }
    else
#endif
    printf("Javascript Alert: %s (from frame %p)\n", message.utf8().data(), frame);
}

bool WebChromeClient::runJavaScriptConfirm(Frame *frame, const String& message)
{
    printf("Javascript Confirm: %s (from frame %p), answer is 'false' by default.\n", message.utf8().data(), frame);
    return false;
}

bool WebChromeClient::runJavaScriptPrompt(Frame *frame, const String& message, const String& defaultValue, String& result)
{
    printf("Javascript Prompt: %s (from frame %p), answer is 'false' by default.\n", message.utf8().data(), frame);
    return false;
}

void WebChromeClient::setStatusbarText(const String& statusText)
{
#if PLATFORM(AMIGAOS4)
    BalWidget *widget = m_webView->viewWindow();
    if (widget && widget->gad_status) {
        CString statusLatin1 = statusText.latin1();
        snprintf(widget->statusBarText, sizeof(widget->statusBarText), "%s", statusLatin1.data());
        if (widget->statusBarText[0] && widget->toolTipText[0])
            snprintf(widget->statusToolTipText, sizeof(widget->statusToolTipText), "%s | %s", widget->statusBarText, widget->toolTipText);
        else
            snprintf(widget->statusToolTipText, sizeof(widget->statusToolTipText), "%s", widget->statusBarText[0] ? widget->statusBarText : widget->toolTipText);
        IIntuition->RefreshSetGadgetAttrs(widget->gad_status, widget->window, NULL,
                                          GA_Text, widget->statusToolTipText,
                                          TAG_DONE);
    }
#endif
}

bool WebChromeClient::shouldInterruptJavaScript()
{
    return false;
}

bool WebChromeClient::tabsToLinks() const
{
    bool enabled = false;
    WebPreferences* preferences = m_webView->preferences();

    return preferences->tabsToLinks();
}

IntRect WebChromeClient::windowResizerRect() const
{
    IntRect intRect;

    return intRect;
}

void WebChromeClient::repaint(const IntRect& windowRect, bool contentChanged, bool immediate, bool repaintContentOnly)
{
    ASSERT(core(m_webView->topLevelFrame()));
    m_webView->repaint(windowRect, contentChanged, immediate, repaintContentOnly);
}

void WebChromeClient::scroll(const IntSize& delta, const IntRect& scrollViewRect, const IntRect& clipRect)
{
    ASSERT(core(m_webView->topLevelFrame()));

    m_webView->scrollBackingStore(core(m_webView->topLevelFrame())->view(), delta.width(), delta.height(), scrollViewRect, clipRect);
}

IntRect WebChromeClient::windowToScreen(const IntRect& rect) const
{
    return rect;
}

PlatformWidget WebChromeClient::platformWindow() const                                                             
{                                                                                                                  
    return m_webView->viewWindow();
} 

void WebChromeClient::mouseDidMoveOverElement(const HitTestResult& result, unsigned modifierFlags)
{

}

void WebChromeClient::setToolTip(const String& toolTip)
{
    m_webView->setToolTip(toolTip);
}

void WebChromeClient::print(Frame* frame)
{
}

void WebChromeClient::exceededDatabaseQuota(Frame* frame, const String& databaseIdentifier)
{
#if ENABLE(DATABASE)
    WebSecurityOrigin *origin = WebSecurityOrigin::createInstance(frame->document()->securityOrigin());
    const unsigned long long defaultQuota = 5 * 1024 * 1024; // 5 megabytes should hopefully be enough to test storage support.
    origin->setQuota(defaultQuota);
    delete origin;
#endif
}

#if ENABLE(DASHBOARD_SUPPORT)
void WebChromeClient::dashboardRegionsChanged()
{
    // This option is not supported so use it at your own risk.
    ASSERT_NOT_REACHED();
}
#endif

void WebChromeClient::populateVisitedLinks()
{
    WebHistory *history = WebHistory::sharedHistory();
    if (!history)
        return;
    history->addVisitedLinksToPageGroup(m_webView->page()->group());
}

bool WebChromeClient::paintCustomScrollbar(GraphicsContext* context, const FloatRect& rect, ScrollbarControlSize size,
                                        ScrollbarControlState state, ScrollbarPart pressedPart, bool vertical,
                                        float value, float proportion, ScrollbarControlPartMask parts)
{
    //FIXME: implement me!
    return false;
}

bool WebChromeClient::paintCustomScrollCorner(GraphicsContext* context, const FloatRect& rect)
{
    //FIXME: implement me!
    return false;
}
void WebChromeClient::runOpenPanel(Frame*, PassRefPtr<FileChooser> prpFileChooser)
{

}

IntPoint WebChromeClient::screenToWindow(const WebCore::IntPoint& p) const 
{
    return p;
}

