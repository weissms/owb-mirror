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

//#include <Frame.h>
#include <IntSize.h>
#include <KURL.h>
#include <PlatformString.h>
#include <ResourceHandleClient.h>

#include <wtf/RefPtr.h>
#include <wtf/HashMap.h>
#include <wtf/OwnPtr.h>
#include "ObserverAddons.h"
#include "ObserverBookmarklet.h"
#include "Bookmarklet.h"

namespace WebCore {
    class AuthenticationChallenge;
    class Frame;
    class IntRect;
    class Page;
    class ResourceError;
    class SharedBuffer;
    class String;
}

typedef const struct OpaqueJSContext* JSContextRef;
typedef struct OpaqueJSValue* JSObjectRef;
typedef struct OpaqueJSContext* JSGlobalContextRef;

class DOMDocument;
class DOMElement;
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

#define WebKitErrorPlugInWillHandleLoad 204

    /**
     * get WebFrame from a frame
     * @param[in]: frame
     * @param[out]: WebFrame
     */
WebFrame* kit(WebCore::Frame*);

    /**
     * get frame from a webFrame
     * @param[in]: WebFrame
     * @param[out]: frame
     */
WebCore::Frame* core(WebFrame*);

class WebFrame: public WebFrameLoaderClient, public RefCounted<WebFrame>, public OWBAL::ObserverAddons, public OWBAL::ObserverBookmarklet
{
public:

    /**
     * create a new instance of WebFrame
     * @param[out]: WebFrame
     */
    static WebFrame* createInstance();

    /**
     * WebFrame destructor
     */
    virtual ~WebFrame();
protected:

    /**
     * WebFrame default constructor
     */
    WebFrame();

public:

    /**
     * @result The frame name.
     */
    virtual WebCore::String name();

    /**
     * @result Returns the WebView for the document that includes this frame.
     */
    virtual WebView *webView();

    /**
     * Returns the DOM document of the frame.
       @description Returns nil if the frame does not contain a DOM document such as a standalone image. DOMDocument description
     */
    virtual DOMDocument* domDocument();

    /**
     *  loadRequest 
     * @param request The web request to load.
     */
    virtual void loadRequest(WebMutableURLRequest *request);

    /**
     *  loadURL
     * @param request The url to load.
     */
    virtual void loadURL(WebCore::String url);

    /**
     *  loadData 
     * @param data The data to use for the main page of the document.
        @param MIMEType The MIME type of the data.
        @param encodingName The encoding of the data.
        @param URL The base URL to apply to relative URLs within the document.
     */
    virtual void loadData(PassRefPtr<WebCore::SharedBuffer>data, WebCore::String mimeType, WebCore::String textEncodingName, WebCore::String url);

    /**
     *  loadHTMLString
     * @param string The string to use for the main page of the document.
        @param URL The base URL to apply to relative URLs within the document.
     */
    virtual void loadHTMLString(WebCore::String string, WebCore::String baseURL);

    /**
     *  loadAlternateHTMLString 
     * Loads a page to display as a substitute for a URL that could not be reached.
        @discussion This allows clients to display page-loading errors in the webview itself.
        This is typically called while processing the WebFrameLoadDelegate method
        -webView:didFailProvisionalLoadWithError:forFrame: or one of the the DefaultPolicyDelegate methods
        -webView:decidePolicyForMIMEType:request:frame:decisionListener: or
        -webView:unableToImplementPolicyWithError:frame:. If it is called from within one of those
        three delegate methods then the back/forward list will be maintained appropriately.
        @param string The string to use for the main page of the document.
        @param baseURL The baseURL to apply to relative URLs within the document.
        @param unreachableURL The URL for which this page will serve as alternate content.
     */
    virtual void loadAlternateHTMLString(WebCore::String str, WebCore::String baseURL, WebCore::String unreachableURL);

    /**
     *  loadArchive
     * Causes WebFrame to load a WebArchive.
        @param archive The archive to be loaded.
     */
    virtual void loadArchive(WebArchive *archive);

    /**
     *  dataSource 
     * Returns the committed data source.  Will return nil if the
        provisional data source hasn't yet been loaded.
        @result The datasource for this frame.
     */
    virtual WebDataSource *dataSource();

    /**
     *  provisionalDataSource 
     * @discussion Will return the provisional data source.  The provisional data source will
        be nil if no data source has been set on the frame, or the data source
        has successfully transitioned to the committed data source.
        @result The provisional datasource of this frame.
     */
    virtual WebDataSource *provisionalDataSource();

    /**
     *  stopLoading 
     * @discussion Stop any pending loads on the frame's data source,
        and its children.
     */
    virtual void stopLoading();

    /**
     *  reload 
     */
    virtual void reload();

    /**
     *  findFrameNamed 
     * @discussion This method returns a frame with the given name. findFrameNamed returns self
        for _self and _current, the parent frame for _parent and the main frame for _top.
        findFrameNamed returns self for _parent and _top if the receiver is the mainFrame.
        findFrameNamed first searches from the current frame to all descending frames then the
        rest of the frames in the WebView. If still not found, findFrameNamed searches the
        frames of the other WebViews.
        @param name The name of the frame to find.
        @result The frame matching the provided name. nil if the frame is not found.
     */
    virtual WebFrame* findFrameNamed(WebCore::String name);

    /**
     *  parentFrame 
     * @result The frame containing this frame, or nil if this is a top level frame.
     */
    virtual WebFrame* parentFrame();

    /**
     * get current form element
     */
    virtual DOMElement* currentForm();

    /**
     *  globalContext 
     * @result The frame's global JavaScript execution context.  Use this method to
        bridge between the WebKit and JavaScriptCore APIs.
     */
    virtual JSGlobalContextRef globalContext();

    /**
     * get renderTree as external representation 
     * @param[out]: external representation
     */
    virtual WebCore::String renderTreeAsExternalRepresentation();

    /**
     * get the scroll offset 
     * @param[out]: scroll offset
     */
    virtual WebCore::IntSize scrollOffset();

    /**
     * layout 
     */
    virtual void layout();

    /**
     * get if the first layout is done 
     */
    virtual bool firstLayoutDone();

    /**
     * get load type 
     * @param[out]: WebFrame load type
     */
    virtual WebFrameLoadType loadType();

    /**
     * setInPrintingMode 
     * Not Implemented
     */
    //virtual void setInPrintingMode(bool value, HDC printDC);  

    /**
     * getPrintedPageCount 
     * Not Implemented
     */
    //virtual unsigned int getPrintedPageCount(HDC printDC);

    /**
     * spoolPages 
     * Not Implemented
     */
    //virtual void* spoolPages(HDC printDC, UINT startPage, UINT endPage);


    /**
     * test if the frame is frame set 
     */
    virtual bool isFrameSet();

    /**
     * get string representation
     */
    virtual WebCore::String string();

    /**
     * get size 
     */
    virtual WebCore::IntSize size();

    /**
     * test if the frame has scrollBars 
     */
    virtual bool hasScrollBars();

    /**
     * get content bounds 
     */
    virtual WebCore::IntRect contentBounds();

    /**
     * get frame bounds 
     */
    virtual WebCore::IntRect frameBounds();


    /**
     * test if the frame is descendant of frame 
     */
    virtual bool isDescendantOfFrame(WebFrame *ancestor);

    /**
     * set allows scrolling 
     */
    virtual void setAllowsScrolling(bool flag);

    /**
     * get allows scrolling 
     */
    virtual bool allowsScrolling();

    /*!
      @method setIsDisconnected
      @abstract Set whether a frame is disconnected
      @param flag true to mark the frame as disconnected, false keeps it a regular frame
    */
    virtual void setIsDisconnected(bool flag);

    /**
     * setExcludeFromTextSearch
     */
    virtual void setExcludeFromTextSearch(bool flag);

    /**
     * test if the frame supports text encoding 
     */
    virtual bool supportsTextEncoding();

    /**
     * get selected string 
     */
    virtual WebCore::String selectedString();

    /**
     * select all 
     */
    virtual void selectAll();

    /**
     * deselect all 
     */
    virtual void deselectAll();
    
    // FrameLoaderClient

    /**
     * frameLoader destroyed 
     */
    virtual void frameLoaderDestroyed();

    /**
     * make representation 
     */
    virtual void makeRepresentation(WebCore::DocumentLoader*);

    /**
     * force layout for non HTML 
     */
    virtual void forceLayoutForNonHTML();

    /**
     * set copies on scroll 
     */
    virtual void setCopiesOnScroll();

    /**
     * detached from parent2 
     */
    virtual void detachedFromParent2();

    /**
     * detached from parent3 
     */
    virtual void detachedFromParent3();

    /**
     * detached from parent4 
     */
    virtual void detachedFromParent4();

    /**
     * cancel policy check 
     */
    virtual void cancelPolicyCheck();

    /**
     *  dispatchWillSubmitForm 
     */
    virtual void dispatchWillSubmitForm(WebCore::FramePolicyFunction, PassRefPtr<WebCore::FormState>);

    /**
     *  revertToProvisionalState 
     */
    virtual void revertToProvisionalState(WebCore::DocumentLoader*);

    /**
     *  setMainFrameDocumentReady
     */
    virtual void setMainFrameDocumentReady(bool);

    /**
     *  willChangeTitle 
     */
    virtual void willChangeTitle(WebCore::DocumentLoader*);

    /**
     *  didChangeTitle 
     */
    virtual void didChangeTitle(WebCore::DocumentLoader*);
    
    /**
     * canHandleRequest
     */
    virtual bool canHandleRequest(const WebCore::ResourceRequest&) const;
    
    /**
     * canShowMIMEType 
     */
    virtual bool canShowMIMEType(const WebCore::String& MIMEType) const;
    
    /**
     * representationExistsForURLScheme 
     */
    virtual bool representationExistsForURLScheme(const WebCore::String& URLScheme) const;
    
    /**
     * generatedMIMETypeForURLScheme 
     */
    virtual WebCore::String generatedMIMETypeForURLScheme(const WebCore::String& URLScheme) const;

    /**
     *  frameLoadCompleted 
     */
    virtual void frameLoadCompleted();

    /**
     *  restoreViewState 
     * Not Implemented
     */
    virtual void restoreViewState();

    /**
     *  provisionalLoadStarted 
     * Not Implemented
     */
    virtual void provisionalLoadStarted();

    /**
     *  shouldTreatURLAsSameAsCurrent 
     * Not Implemented
     */
    virtual bool shouldTreatURLAsSameAsCurrent(const WebCore::KURL&) const;

    /**
     *  addHistoryItemForFragmentScroll 
     * Not Implemented
     */
    virtual void addHistoryItemForFragmentScroll();

    /**
     *  didFinishLoad 
     * Not Implemented
     */
    virtual void didFinishLoad();

    /**
     *  prepareForDataSourceReplacement 
     * Not Implemented
     */
    virtual void prepareForDataSourceReplacement();

    /**
     *  userAgent 
     */
    virtual WebCore::String userAgent(const WebCore::KURL&);

    /**
     *  transitionToCommittedFromCachedPage 
     * Not Implemented
     */
    virtual void transitionToCommittedFromCachedPage(WebCore::CachedPage*);

    /**
     *  saveViewStateToItem 
     * Not Implemented
     */
    virtual void saveViewStateToItem(WebCore::HistoryItem *);

    /**
     *  cancelledError 
     */
    virtual WebCore::ResourceError cancelledError(const WebCore::ResourceRequest&);

    /**
     *  blockedError 
     */
    virtual WebCore::ResourceError blockedError(const WebCore::ResourceRequest&);

    /**
     *  cannotShowURLError 
     */
    virtual WebCore::ResourceError cannotShowURLError(const WebCore::ResourceRequest&);

    /**
     *  interruptForPolicyChangeError 
     */
    virtual WebCore::ResourceError interruptForPolicyChangeError(const WebCore::ResourceRequest&);

    /**
     *  cannotShowMIMETypeError 
     * Not Implemented
     */
    virtual WebCore::ResourceError cannotShowMIMETypeError(const WebCore::ResourceResponse&);

    /**
     *  fileDoesNotExistError 
     * Not Implemented
     */
    virtual WebCore::ResourceError fileDoesNotExistError(const WebCore::ResourceResponse&);

    /**
     * pluginWillHandleLoadError
     * Not Implemented
     */
    virtual WebCore::ResourceError pluginWillHandleLoadError(const WebCore::ResourceResponse&);

    /**
     *  shouldFallBack 
     */
    virtual bool shouldFallBack(const WebCore::ResourceError&);

    /**
     *  dispatchDecidePolicyForMIMEType 
     */
    virtual void dispatchDecidePolicyForMIMEType(WebCore::FramePolicyFunction, const WebCore::String& MIMEType, const WebCore::ResourceRequest&);

    /**
     *  dispatchDecidePolicyForNewWindowAction 
     */
    virtual void dispatchDecidePolicyForNewWindowAction(WebCore::FramePolicyFunction, const WebCore::NavigationAction&, const WebCore::ResourceRequest&, PassRefPtr<WebCore::FormState>, const WebCore::String& frameName);

    /**
     *  dispatchDecidePolicyForNavigationAction
     */
    virtual void dispatchDecidePolicyForNavigationAction(WebCore::FramePolicyFunction, const WebCore::NavigationAction&, const WebCore::ResourceRequest&, PassRefPtr<WebCore::FormState>);

    /**
     *  dispatchUnableToImplementPolicy 
     */
    virtual void dispatchUnableToImplementPolicy(const WebCore::ResourceError&);

    /**
     *  download 
     */
    virtual void download(WebCore::ResourceHandle*, const WebCore::ResourceRequest&, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&);


    /**
     *  dispatchDidLoadResourceFromMemoryCache 
     */
    virtual bool dispatchDidLoadResourceFromMemoryCache(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&, int length);

    /**
     *  dispatchDidFailProvisionalLoad 
     */
    virtual void dispatchDidFailProvisionalLoad(const WebCore::ResourceError&);

    /**
     *  dispatchDidFailLoad 
     */
    virtual void dispatchDidFailLoad(const WebCore::ResourceError&);

    /**
     *  startDownload 
     */
    virtual void startDownload(const WebCore::ResourceRequest&);
        

    /**
     *  createJavaAppletWidget 
     */
    virtual WebCore::Widget* createJavaAppletWidget(const WebCore::IntSize&, WebCore::Element*, const WebCore::KURL& baseURL, const Vector<WebCore::String>& paramNames, const Vector<WebCore::String>& paramValues);


    /**
     *  objectContentType 
     */
    virtual WebCore::ObjectContentType objectContentType(const WebCore::KURL& url, const WebCore::String& mimeType);

    /**
     *  overrideMediaType 
     */
    virtual WebCore::String overrideMediaType() const;


    /**
     *  windowObjectCleared 
     */
    virtual void windowObjectCleared();

    /**
     *  didPerformFirstNavigation 
     */
    virtual void didPerformFirstNavigation() const;


    /**
     *  registerForIconNotification 
     */
    virtual void registerForIconNotification(bool listen);

    // WebFrame

    /**
     *  init WebFrame
     */
    PassRefPtr<WebCore::Frame> init(WebView*, WebCore::Page*, WebCore::HTMLFrameOwnerElement*);

    /**
     * get frame
     */
    WebCore::Frame* impl();

    /**
     * invalidate webframe
     */
    void invalidate();

    /**
     * unmark all misspellings 
     */
    void unmarkAllMisspellings();

    /**
     * unmark all bad grammar 
     */
    void unmarkAllBadGrammar();

    void updateBackground();

    // WebFrame (matching WebCoreFrameBridge)

    /**
     *  setTextSizeMultiplier 
     * Change the size of the text rendering in views managed by this webView.
        @param multiplier A fractional percentage value, 1.0 is 100%.
     */
    void setTextSizeMultiplier(float multiplier);

    /**
     *  inViewSourceMode 
     * @param[in]: 
     * @param[out]: 
     * @code
     * @endcode
     */
    bool inViewSourceMode();

    /**
     *  setInViewSourceMode 
     * Whether or not the WebView is in source-view mode for HTML.
     */
    void setInViewSourceMode(bool flag);

    /**
     *  elementWithName 
     * Not Implemented
     */
    //HRESULT elementWithName(BSTR name, IDOMElement* form, IDOMElement** element);

    /**
     *  formForElement 
     * Not Implemented
     */
    //HRESULT formForElement(IDOMElement* element, IDOMElement** form);

    /**
     *  elementDoesAutoComplete 
     * Not Implemented
     */
    //HRESULT elementDoesAutoComplete(IDOMElement* element, bool* result);

    /**
     *  controlsInForm 
     * Not Implemented
     */
    //HRESULT controlsInForm(IDOMElement* form, IDOMElement** controls, int* cControls);

    /**
     *  elementIsPassword 
     * Not Implemented
     */
    //HRESULT elementIsPassword(IDOMElement* element, bool* result);

    /**
     *  searchForLabelsBeforeElement 
     * Not Implemented
     */
    //HRESULT searchForLabelsBeforeElement(const BSTR* labels, int cLabels, IDOMElement* beforeElement, BSTR* result);

    /**
     *  matchLabelsAgainstElement 
     * Not Implemented
     */
    //HRESULT matchLabelsAgainstElement(const BSTR* labels, int cLabels, IDOMElement* againstElement, BSTR* result);

    /**
     *  canProvideDocumentSource 
     * Not Implemented
     */
    //HRESULT canProvideDocumentSource(bool* result);


    /**
     * set up policy listener 
     */
    WebFramePolicyListener* setUpPolicyListener(WebCore::FramePolicyFunction function);

    /**
     * received policy decision 
     */
    void receivedPolicyDecision(WebCore::PolicyAction);

    /**
     * get url
     */
    WebCore::KURL url() const;

    /**
     * get webview
     */
    WebView* webView() const;

    /** 
     * add to JSWindow object 
     * add an balObject to extend the javascript 
     */  
    void addToJSWindowObject(const char* name, void *object); 
    
    //BCObserverAddons 
    /** 
     * observe implementation
     */
    virtual void observe(const OWBAL::String &topic, BalObject* obj);
    virtual void observe(const OWBAL::String &topic, OWBAL::Bookmarklet* bookmarklet);

protected:

    /**
     *  loadData 
     */
    void loadData(PassRefPtr<WebCore::SharedBuffer>, WebCore::String mimeType, WebCore::String textEncodingName, WebCore::String baseURL, WebCore::String failingURL);

    /**
     *  loadHTMLString 
     */
    void loadHTMLString(WebCore::String string, WebCore::String baseURL, WebCore::String unreachableURL);

    /**
     *  computePageRects 
     */
    //const Vector<WebCore::IntRect>& computePageRects(HDC printDC);

    /**
     *  setPrinting 
     */
    //void setPrinting(bool printing, float minPageWidth, float maxPageWidth, bool adjustViewSize);

    /**
     *  headerAndFooterHeights 
     */
    //void headerAndFooterHeights(float*, float*);

    /**
     *  printerMarginRect 
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
