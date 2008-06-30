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

#ifndef WebFrame_H
#define WebFrame_H


/**
 *  @file  WebFrame.h
 *  WebFrame description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"
#include "WebFrameLoaderClient.h"
#include "WebFrameLoaderClient.h"

#include "WebDataSource.h"

#include <Frame.h>
#include <KURL.h>
#include <PlatformString.h>
#include <ResourceHandleClient.h>

#include <wtf/RefPtr.h>
#include <wtf/HashMap.h>
#include <wtf/OwnPtr.h>
#include "BCObserverAddons.h"

namespace WebCore {
    class AuthenticationChallenge;
    class DocumentLoader;
    class Element;
    class Frame;
    class HTMLFrameOwnerElement;
    class IntRect;
    class Page;
    class ResourceError;
    class SharedBuffer;
    class String;
}

typedef const struct OpaqueJSContext* JSContextRef;
typedef struct OpaqueJSValue* JSObjectRef;
typedef struct OpaqueJSContext* JSGlobalContextRef;

class WebFrame;
class WebFramePolicyListener;
class WebHistory;
class WebView;
class WebMutableURLRequest;
#ifdef __BINDING_JS__
class BindingJS;
#endif

typedef enum {
    WebFrameLoadTypeStandard,
    WebFrameLoadTypeBack,
    WebFrameLoadTypeForward,
    WebFrameLoadTypeIndexedBackForward, // a multi-item hop in the backforward list
    WebFrameLoadTypeReload,
    WebFrameLoadTypeReloadAllowingStaleData,
    WebFrameLoadTypeSame,               // user loads same URL again (but not reload button)
    WebFrameLoadTypeInternal,           // maps to WebCore::FrameLoadTypeRedirectWithLockedHistory
    WebFrameLoadTypeReplace
} WebFrameLoadType;

typedef enum _WebURLRequestCachePolicy
{
    WebURLRequestUseProtocolCachePolicy,
    WebURLRequestReloadIgnoringCacheData,
    WebURLRequestReturnCacheDataElseLoad,
    WebURLRequestReturnCacheDataDontLoad
} WebURLRequestCachePolicy;


    /**
     *  kit description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
WebFrame* kit(WebCore::Frame*);

    /**
     *  core description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
WebCore::Frame* core(WebFrame*);

class WebFrame: public WebFrameLoaderClient, public OWBAL::BCObserverAddons
{
public:

    /**
     *  createInstance description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebFrame* createInstance();

    /**
     *  ~WebFrame description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual ~WebFrame();
protected:

    /**
     *  WebFrame description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebFrame();

public:

    /**
     *  name description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String name();

    /**
     *  *webView description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebView *webView();

    /**
     *  DOMDocument description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::Document* DOMDocument();

    /**
     *  loadRequest description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void loadRequest(WebMutableURLRequest *request);

    /**
     *  loadURL description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void loadURL(WebCore::String url);

    /**
     *  loadData description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void loadData(PassRefPtr<WebCore::SharedBuffer>data, WebCore::String mimeType, WebCore::String textEncodingName, WebCore::String url);

    /**
     *  loadHTMLString description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void loadHTMLString(WebCore::String string, WebCore::String baseURL);

    /**
     *  loadAlternateHTMLString description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void loadAlternateHTMLString(WebCore::String str, WebCore::String baseURL, WebCore::String unreachableURL);

    /**
     *  loadArchive description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void loadArchive(WebArchive *archive);

    /**
     *  *dataSource description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebDataSource *dataSource();

    /**
     *  *provisionalDataSource description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebDataSource *provisionalDataSource();

    /**
     *  stopLoading description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void stopLoading();

    /**
     *  reload description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void reload();

    /**
     *  findFrameNamed description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebFrame* findFrameNamed(WebCore::String name);

    /**
     *  parentFrame description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebFrame* parentFrame();

    /**
     *  currentForm description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::HTMLFormElement* currentForm();

    /**
     *  globalContext description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual JSGlobalContextRef globalContext();


    /**
     *  renderTreeAsExternalRepresentation description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String renderTreeAsExternalRepresentation();

    /**
     *  scrollOffset description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::IntSize scrollOffset();

    /**
     *  layout description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void layout();

    /**
     *  firstLayoutDone description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool firstLayoutDone();

    /**
     *  loadType description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebFrameLoadType loadType();

    /**
     *  setInPrintingMode description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual void setInPrintingMode(bool value, HDC printDC);  

    /**
     *  getPrintedPageCount description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual unsigned int getPrintedPageCount(HDC printDC);

    /**
     *  spoolPages description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual void* spoolPages(HDC printDC, UINT startPage, UINT endPage);


    /**
     *  isFrameSet description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool isFrameSet();

    /**
     *  string description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String string();

    /**
     *  size description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual IntSize size();

    /**
     *  hasScrollBars description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool hasScrollBars();

    /**
     *  contentBounds description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::IntRect contentBounds();

    /**
     *  frameBounds description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::IntRect frameBounds();


    /**
     *  isDescendantOfFrame description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool isDescendantOfFrame(WebFrame *ancestor);

    /**
     *  setAllowsScrolling description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setAllowsScrolling(bool flag);

    /**
     *  allowsScrolling description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool allowsScrolling();


    /**
     *  supportsTextEncoding description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool supportsTextEncoding();

    /**
     *  selectedString description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String selectedString();

    /**
     *  selectAll description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void selectAll();

    /**
     *  deselectAll description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void deselectAll();
    
    // FrameLoaderClient

    /**
     *  frameLoaderDestroyed description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void frameLoaderDestroyed();

    /**
     *  makeRepresentation description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void makeRepresentation(WebCore::DocumentLoader*);

    /**
     *  forceLayoutForNonHTML description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void forceLayoutForNonHTML();

    /**
     *  setCopiesOnScroll description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setCopiesOnScroll();

    /**
     *  detachedFromParent2 description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void detachedFromParent2();

    /**
     *  detachedFromParent3 description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void detachedFromParent3();

    /**
     *  detachedFromParent4 description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void detachedFromParent4();

    /**
     *  cancelPolicyCheck description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void cancelPolicyCheck();

    /**
     *  dispatchWillSubmitForm description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void dispatchWillSubmitForm(WebCore::FramePolicyFunction, PassRefPtr<WebCore::FormState>);

    /**
     *  revertToProvisionalState description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void revertToProvisionalState(WebCore::DocumentLoader*);

    /**
     *  setMainFrameDocumentReady description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setMainFrameDocumentReady(bool);

    /**
     *  willChangeTitle description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void willChangeTitle(WebCore::DocumentLoader*);

    /**
     *  didChangeTitle description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void didChangeTitle(WebCore::DocumentLoader*);
    
    /**
     * canHandleRequest description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canHandleRequest(const WebCore::ResourceRequest&) const;
    
    /**
     * canShowMIMEType description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canShowMIMEType(const WebCore::String& MIMEType) const;
    
    /**
     * representationExistsForURLScheme description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool representationExistsForURLScheme(const WebCore::String& URLScheme) const;
    
    /**
     * generatedMIMETypeForURLScheme description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String generatedMIMETypeForURLScheme(const WebCore::String& URLScheme) const;

    /**
     *  frameLoadCompleted description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void frameLoadCompleted();

    /**
     *  restoreViewState description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void restoreViewState();

    /**
     *  provisionalLoadStarted description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void provisionalLoadStarted();

    /**
     *  shouldTreatURLAsSameAsCurrent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool shouldTreatURLAsSameAsCurrent(const WebCore::KURL&) const;

    /**
     *  addHistoryItemForFragmentScroll description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void addHistoryItemForFragmentScroll();

    /**
     *  didFinishLoad description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void didFinishLoad();

    /**
     *  prepareForDataSourceReplacement description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void prepareForDataSourceReplacement();

    /**
     *  userAgent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String userAgent(const WebCore::KURL&);

    /**
     *  transitionToCommittedFromCachedPage description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void transitionToCommittedFromCachedPage(WebCore::CachedPage*);

    /**
     *  saveViewStateToItem description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void saveViewStateToItem(WebCore::HistoryItem *);

    /**
     *  cancelledError description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::ResourceError cancelledError(const WebCore::ResourceRequest&);

    /**
     *  blockedError description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::ResourceError blockedError(const WebCore::ResourceRequest&);

    /**
     *  cannotShowURLError description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::ResourceError cannotShowURLError(const WebCore::ResourceRequest&);

    /**
     *  interruptForPolicyChangeError description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::ResourceError interruptForPolicyChangeError(const WebCore::ResourceRequest&);

    /**
     *  cannotShowMIMETypeError description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::ResourceError cannotShowMIMETypeError(const WebCore::ResourceResponse&);

    /**
     *  fileDoesNotExistError description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::ResourceError fileDoesNotExistError(const WebCore::ResourceResponse&);

    /**
     *  shouldFallBack description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool shouldFallBack(const WebCore::ResourceError&);

    /**
     *  dispatchDecidePolicyForMIMEType description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void dispatchDecidePolicyForMIMEType(WebCore::FramePolicyFunction, const WebCore::String& MIMEType, const WebCore::ResourceRequest&);

    /**
     *  dispatchDecidePolicyForNewWindowAction description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void dispatchDecidePolicyForNewWindowAction(WebCore::FramePolicyFunction, const WebCore::NavigationAction&, const WebCore::ResourceRequest&, PassRefPtr<WebCore::FormState>, const WebCore::String& frameName);

    /**
     *  dispatchDecidePolicyForNavigationAction description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void dispatchDecidePolicyForNavigationAction(WebCore::FramePolicyFunction, const WebCore::NavigationAction&, const WebCore::ResourceRequest&, PassRefPtr<WebCore::FormState>);

    /**
     *  dispatchUnableToImplementPolicy description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void dispatchUnableToImplementPolicy(const WebCore::ResourceError&);

    /**
     *  download description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void download(WebCore::ResourceHandle*, const WebCore::ResourceRequest&, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&);


    /**
     *  dispatchDidLoadResourceFromMemoryCache description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool dispatchDidLoadResourceFromMemoryCache(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&, int length);

    /**
     *  dispatchDidFailProvisionalLoad description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void dispatchDidFailProvisionalLoad(const WebCore::ResourceError&);

    /**
     *  dispatchDidFailLoad description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void dispatchDidFailLoad(const WebCore::ResourceError&);

    /**
     *  startDownload description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void startDownload(const WebCore::ResourceRequest&);
        

    /**
     *  createJavaAppletWidget description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::Widget* createJavaAppletWidget(const WebCore::IntSize&, WebCore::Element*, const WebCore::KURL& baseURL, const Vector<WebCore::String>& paramNames, const Vector<WebCore::String>& paramValues);


    /**
     *  objectContentType description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::ObjectContentType objectContentType(const WebCore::KURL& url, const WebCore::String& mimeType);

    /**
     *  objectContentType description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String overrideMediaType() const;


    /**
     *  windowObjectCleared description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void windowObjectCleared();

    /**
     *  objectContentType description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void didPerformFirstNavigation() const;


    /**
     *  registerForIconNotification description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void registerForIconNotification(bool listen);

    // WebFrame

    /**
     *  init description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    PassRefPtr<WebCore::Frame> init(WebView*, WebCore::Page*, WebCore::HTMLFrameOwnerElement*);

    /**
     *  impl description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebCore::Frame* impl();

    /**
     *  invalidate description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void invalidate();

    /**
     *  unmarkAllMisspellings description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void unmarkAllMisspellings();

    /**
     *  unmarkAllBadGrammar description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void unmarkAllBadGrammar();

    // WebFrame (matching WebCoreFrameBridge)

    /**
     *  setTextSizeMultiplier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setTextSizeMultiplier(float multiplier);

    /**
     *  inViewSourceMode description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool inViewSourceMode();

    /**
     *  setInViewSourceMode description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setInViewSourceMode(bool flag);

    /**
     *  elementWithName description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT elementWithName(BSTR name, IDOMElement* form, IDOMElement** element);

    /**
     *  formForElement description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT formForElement(IDOMElement* element, IDOMElement** form);

    /**
     *  elementDoesAutoComplete description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT elementDoesAutoComplete(IDOMElement* element, bool* result);

    /**
     *  controlsInForm description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT controlsInForm(IDOMElement* form, IDOMElement** controls, int* cControls);

    /**
     *  elementIsPassword description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT elementIsPassword(IDOMElement* element, bool* result);

    /**
     *  searchForLabelsBeforeElement description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT searchForLabelsBeforeElement(const BSTR* labels, int cLabels, IDOMElement* beforeElement, BSTR* result);

    /**
     *  matchLabelsAgainstElement description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT matchLabelsAgainstElement(const BSTR* labels, int cLabels, IDOMElement* againstElement, BSTR* result);

    /**
     *  canProvideDocumentSource description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT canProvideDocumentSource(bool* result);


    /**
     *  setUpPolicyListener description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebFramePolicyListener* setUpPolicyListener(WebCore::FramePolicyFunction function);

    /**
     *  receivedPolicyDecision description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void receivedPolicyDecision(WebCore::PolicyAction);

    WebCore::KURL url() const;

    WebView* webView() const;

    /** 
     * addToJSWindowObject 
     */  
    void addToJSWindowObject(const char* name, void *object); 
    //BCObserverAddons 
    virtual void observe(const OWBAL::String &topic, BalObject *obj);

protected:

    /**
     *  loadData description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void loadData(PassRefPtr<WebCore::SharedBuffer>, WebCore::String mimeType, WebCore::String textEncodingName, WebCore::String baseURL, WebCore::String failingURL);

    /**
     *  loadHTMLString description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void loadHTMLString(WebCore::String string, WebCore::String baseURL, WebCore::String unreachableURL);

    /**
     *  computePageRects description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //const Vector<WebCore::IntRect>& computePageRects(HDC printDC);

    /**
     *  setPrinting description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //void setPrinting(bool printing, float minPageWidth, float maxPageWidth, bool adjustViewSize);

    /**
     *  headerAndFooterHeights description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //void headerAndFooterHeights(float*, float*);

    /**
     *  printerMarginRect description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //WebCore::IntRect printerMarginRect(HDC);

protected:
    class WebFramePrivate;
    WebFramePrivate*    d;
    bool                m_quickRedirectComing;
    WebCore::KURL       m_originalRequestURL;
    bool                m_inPrintingMode;
    Vector<WebCore::IntRect> m_pageRects;
    int m_pageHeight;   // height of the page adjusted by margins
#ifdef __BINDING_JS__ 
    BindingJS *m_bindingJS; 
#endif
};

#endif
