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
#include "WebFrameLoaderClient.h"

#include "DefaultPolicyDelegate.h"
#include "WebActionPropertyBag.h"
#include "WebCachedFramePlatformData.h"
#include "WebChromeClient.h"
#include "WebDocumentLoader.h"
#include "WebDownload.h"
#include "WebError.h"
#include "WebFrame.h"
#include "WebFrameLoadDelegate.h"
#include "WebFramePolicyListener.h"
#include "WebHistory.h"
#include "WebHistoryItem.h"
#include "WebHistoryItem_p.h"
#include "WebMutableURLRequest.h"
#include "WebNotificationDelegate.h"
#include "WebPreferences.h"
#include "WebURLAuthenticationChallenge.h"
#include "WebURLResponse.h"
#include "WebView.h"

#if ENABLE(JS_ADDONS)
#include "BindingJS.h"
#endif
#include "ObserverServiceData.h"
#if ENABLE(ORIGYNSUITE)
#include "OrigynServer.h"
#endif

#include <CachedFrame.h>
#include <DocumentLoader.h>
#include <FormState.h>
#include <Frame.h>
#include <FrameLoader.h>
#include <FrameTree.h>
#include <FrameView.h>
#include <HTMLAppletElement.h>
#include <HTMLFormElement.h>
#include <HTMLFrameElement.h>
#include <HTMLFrameOwnerElement.h>
#include <HTMLNames.h>
#include <HTMLPlugInElement.h>
#include <HistoryItem.h>
#include <Logging.h>
#include <MIMETypeRegistry.h>
#include <Page.h>
#include <PluginDatabase.h>
#include <PluginPackage.h>
#include <PluginView.h>
#include <RenderPart.h>
#include <ResourceHandle.h>
#include <ScriptString.h> 
#include <Settings.h>
#include <APICast.h>
#if PLATFORM(AMIGAOS4)
#include <proto/clicktab.h>
#include <proto/intuition.h>
#include <proto/layout.h>
#include <gadgets/clicktab.h>
#include <intuition/gadgetclass.h>
#endif

#include <cstdio>

using namespace WebCore;
using namespace HTMLNames;

static WebDataSource* getWebDataSource(DocumentLoader* loader)
{
    return loader ? static_cast<WebDocumentLoader*>(loader)->dataSource() : 0;
}

WebFrameLoaderClient::WebFrameLoaderClient(WebFrame* webFrame)
    : m_webFrame(webFrame)
    , m_pluginView(0) 
    , m_hasSentResponseToPlugin(false)
    , m_policyFunction(0)
    , m_policyListener(0)
{
    ASSERT_ARG(webFrame, webFrame);
}

WebFrameLoaderClient::~WebFrameLoaderClient()
{
    if (m_webFrame)
        m_webFrame = 0;
    if (m_pluginView)
        delete m_pluginView;
    if (m_policyListener)
        delete m_policyListener;
}

bool WebFrameLoaderClient::hasWebView() const
{
    return m_webFrame->webView();
}

void WebFrameLoaderClient::forceLayout()
{
    FrameView* view = core(m_webFrame)->view();
    if (view)
        view->forceLayout(true);
}

void WebFrameLoaderClient::assignIdentifierToInitialRequest(unsigned long identifier, DocumentLoader* loader, const ResourceRequest& request)
{
/*    WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return;

    COMPtr<WebMutableURLRequest> webURLRequest(AdoptCOM, WebMutableURLRequest::createInstance(request));
    resourceLoadDelegate->identifierForInitialRequest(webView, webURLRequest.get(), getWebDataSource(loader), identifier);*/
}

void WebFrameLoaderClient::dispatchDidReceiveAuthenticationChallenge(DocumentLoader* loader, unsigned long identifier, const AuthenticationChallenge& challenge)
{
    //ASSERT(challenge.sourceHandle());

    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (SUCCEEDED(webView->resourceLoadDelegate(&resourceLoadDelegate))) {
        COMPtr<WebURLAuthenticationChallenge> webChallenge(AdoptCOM, WebURLAuthenticationChallenge::createInstance(challenge));
        if (SUCCEEDED(resourceLoadDelegate->didReceiveAuthenticationChallenge(webView, identifier, webChallenge.get(), getWebDataSource(loader))))
            return;
    }*/

    // If the ResourceLoadDelegate doesn't exist or fails to handle the call, we tell the ResourceHandle
    // to continue without credential - this is the best approximation of Mac behavior
    //challenge.sourceHandle()->receivedRequestToContinueWithoutCredential(challenge);
}

void WebFrameLoaderClient::dispatchDidFirstVisuallyNonEmptyLayout()
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebFrameLoadDelegatePrivate> frameLoadDelegatePrivate;
    if (SUCCEEDED(webView->frameLoadDelegatePrivate(&frameLoadDelegatePrivate)) && frameLoadDelegatePrivate) {
        COMPtr<IWebFrameLoadDelegatePrivate2> frameLoadDelegatePrivate2(Query, frameLoadDelegatePrivate);
        if (frameLoadDelegatePrivate2)
            frameLoadDelegatePrivate2->didFirstVisuallyNonEmptyLayoutInFrame(webView, m_webFrame);
    }*/
}


void WebFrameLoaderClient::dispatchDidCancelAuthenticationChallenge(DocumentLoader* loader, unsigned long identifier, const AuthenticationChallenge& challenge)
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return;

    COMPtr<WebURLAuthenticationChallenge> webChallenge(AdoptCOM, WebURLAuthenticationChallenge::createInstance(challenge));
    resourceLoadDelegate->didCancelAuthenticationChallenge(webView, identifier, webChallenge.get(), getWebDataSource(loader));*/
}

bool WebFrameLoaderClient::shouldUseCredentialStorage(DocumentLoader* loader, unsigned long identifier)
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return true;

    COMPtr<IWebResourceLoadDelegatePrivate2> resourceLoadDelegatePrivate;
    if (FAILED(resourceLoadDelegate->QueryInterface(IID_IWebResourceLoadDelegatePrivate2, reinterpret_cast<void**>(&resourceLoadDelegatePrivate))))
        return true;

    BOOL shouldUse;
    if (SUCCEEDED(resourceLoadDelegatePrivate->shouldUseCredentialStorage(webView, identifier, getWebDataSource(loader), &shouldUse)))
        return shouldUse;*/
    
    return true;
}

void WebFrameLoaderClient::dispatchWillSendRequest(DocumentLoader* loader, unsigned long identifier, ResourceRequest& request, const ResourceResponse& redirectResponse)
{
#if ENABLE (ORIGYNSUITE)
    OrigynServer::get()->redirectRequest(request);
#else
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return;

    COMPtr<WebMutableURLRequest> webURLRequest(AdoptCOM, WebMutableURLRequest::createInstance(request));
    COMPtr<WebURLResponse> webURLRedirectResponse(AdoptCOM, WebURLResponse::createInstance(redirectResponse));

    COMPtr<IWebURLRequest> newWebURLRequest;
    if (FAILED(resourceLoadDelegate->willSendRequest(webView, identifier, webURLRequest.get(), webURLRedirectResponse.get(), getWebDataSource(loader), &newWebURLRequest)))
        return;

    if (webURLRequest == newWebURLRequest)
        return;

    COMPtr<WebMutableURLRequest> newWebURLRequestImpl(Query, newWebURLRequest);
    if (!newWebURLRequestImpl)
        return;

    request = newWebURLRequestImpl->resourceRequest();*/
#endif
}

void WebFrameLoaderClient::dispatchDidReceiveResponse(DocumentLoader* loader, unsigned long identifier, const ResourceResponse& response)
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return;

    COMPtr<WebURLResponse> webURLResponse(AdoptCOM, WebURLResponse::createInstance(response));
    resourceLoadDelegate->didReceiveResponse(webView, identifier, webURLResponse.get(), getWebDataSource(loader));*/
}

void WebFrameLoaderClient::dispatchDidReceiveContentLength(DocumentLoader* loader, unsigned long identifier, int length)
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return;

    resourceLoadDelegate->didReceiveContentLength(webView, identifier, length, getWebDataSource(loader));*/
}

void WebFrameLoaderClient::dispatchDidFinishLoading(DocumentLoader* loader, unsigned long identifier)
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return;

    resourceLoadDelegate->didFinishLoadingFromDataSource(webView, identifier, getWebDataSource(loader));*/
}

void WebFrameLoaderClient::dispatchDidFailLoading(DocumentLoader* loader, unsigned long identifier, const ResourceError& error)
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return;

    COMPtr<WebError> webError(AdoptCOM, WebError::createInstance(error));
    resourceLoadDelegate->didFailLoadingWithError(webView, identifier, webError.get(), getWebDataSource(loader));*/
}

void WebFrameLoaderClient::dispatchDidLoadResourceByXMLHttpRequest(unsigned long, const ScriptString&)                                                     
{                                                                                                                                                          
} 

bool WebFrameLoaderClient::shouldCacheResponse(DocumentLoader* loader, unsigned long identifier, const ResourceResponse& response, const unsigned char* data, const unsigned long long length)                                                                                                                                                           
{
/*
    WebView* webView = m_webFrame->webView();
    COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return true;

    COMPtr<IWebResourceLoadDelegatePrivate3> resourceLoadDelegatePrivate(Query, resourceLoadDelegate);
    if (!resourceLoadDelegatePrivate)
        return true;

    COMPtr<IWebURLResponse> urlResponse(WebURLResponse::createInstance(response));
    BOOL shouldCache;
    if (SUCCEEDED(resourceLoadDelegatePrivate->shouldCacheResponse(webView, identifier, urlResponse.get(), data, length, getWebDataSource(loader), &shouldCache)))
        return shouldCache;

    return true;
*/
    return false;
}

void WebFrameLoaderClient::dispatchDidHandleOnloadEvents()
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebFrameLoadDelegatePrivate> frameLoadDelegatePriv;
    if (SUCCEEDED(webView->frameLoadDelegatePrivate(&frameLoadDelegatePriv)) && frameLoadDelegatePriv)
        frameLoadDelegatePriv->didHandleOnloadEventsForFrame(webView, m_webFrame);*/
}

void WebFrameLoaderClient::dispatchDidReceiveServerRedirectForProvisionalLoad()
{
    WebFrameLoadDelegate* frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->didReceiveServerRedirectForProvisionalLoadForFrame(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidCancelClientRedirect()
{
    WebFrameLoadDelegate* frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->didCancelClientRedirectForFrame(m_webFrame);
}

void WebFrameLoaderClient::dispatchWillPerformClientRedirect(const KURL& url, double delay, double fireDate)
{
    WebFrameLoadDelegate* frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->willPerformClientRedirectToURL(m_webFrame, url.string().utf8().data(), delay, fireDate);
}

void WebFrameLoaderClient::dispatchDidChangeLocationWithinPage()
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebFrameLoadDelegate> frameLoadDelegate;
    if (SUCCEEDED(webView->frameLoadDelegate(&frameLoadDelegate)))
        frameLoadDelegate->didChangeLocationWithinPageForFrame(webView, m_webFrame);*/
}

void WebFrameLoaderClient::dispatchWillClose()
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebFrameLoadDelegate> frameLoadDelegate;
    if (SUCCEEDED(webView->frameLoadDelegate(&frameLoadDelegate)))
        frameLoadDelegate->willCloseFrame(webView, m_webFrame);*/
}

void WebFrameLoaderClient::dispatchDidReceiveIcon()
{
#if ENABLE(ICON_DATABASE)
    m_webFrame->webView()->dispatchDidReceiveIconFromWebFrame(m_webFrame);
#endif
}

void WebFrameLoaderClient::dispatchDidStartProvisionalLoad()
{
    WebFrameLoadDelegate* webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->didStartProvisionalLoad(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidReceiveTitle(const String& title)
{
    WebFrameLoadDelegate* webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->titleChange(m_webFrame, title.utf8().data());
}

void WebFrameLoaderClient::dispatchDidCommitLoad()
{
    WebFrameLoadDelegate* webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->didCommitLoad(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidFinishDocumentLoad()
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebFrameLoadDelegatePrivate> frameLoadDelegatePriv;
    if (SUCCEEDED(webView->frameLoadDelegatePrivate(&frameLoadDelegatePriv)) && frameLoadDelegatePriv)
        frameLoadDelegatePriv->didFinishDocumentLoadForFrame(webView, m_webFrame);*/
}

void WebFrameLoaderClient::dispatchDidFinishLoad()
{
    WebFrameLoadDelegate* webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->didFinishLoad(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidFirstLayout()
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebFrameLoadDelegatePrivate> frameLoadDelegatePriv;
    if (SUCCEEDED(webView->frameLoadDelegatePrivate(&frameLoadDelegatePriv)) && frameLoadDelegatePriv)
        frameLoadDelegatePriv->didFirstLayoutInFrame(webView, m_webFrame);*/
}

Frame* WebFrameLoaderClient::dispatchCreatePage()
{
#if PLATFORM(AMIGAOS4)
    extern BalWidget *createAmigaWindow(WebView *);
    extern void closeAmigaWindow(BalWidget *);

    WebView* newWebView = WebView::createInstance();
    if (newWebView) {
        BalWidget *newowbwindow = createAmigaWindow(newWebView);
        if (newowbwindow) {
            BalRectangle clientRect = {0, 0, newowbwindow->webViewWidth, newowbwindow->webViewHeight};
            newWebView->initWithFrame(clientRect, "", "");
            newWebView->setViewWindow(newowbwindow);

            WebFrame *mainFrame = newWebView->mainFrame();
            if (mainFrame && mainFrame->impl())
                return mainFrame->impl();

            closeAmigaWindow(newowbwindow);
        }
        delete newWebView;
    }

    return 0;
#else
    /*WebView* webView = m_webFrame->webView();

    COMPtr<IWebUIDelegate> ui;
    if (FAILED(webView->uiDelegate(&ui)))
        return 0;

    COMPtr<IWebView> newWebView;
    if (FAILED(ui->createWebViewWithRequest(webView, 0, &newWebView)))
        return 0;

    COMPtr<IWebFrame> mainFrame;
    if (FAILED(newWebView->mainFrame(&mainFrame)))
        return 0;

    COMPtr<WebFrame> mainFrameImpl(Query, mainFrame);
    return core(mainFrameImpl.get());*/
    return 0;
#endif
}

void WebFrameLoaderClient::dispatchShow()
{
    /*WebView* webView = m_webFrame->webView();
    COMPtr<IWebUIDelegate> ui;
    if (SUCCEEDED(webView->uiDelegate(&ui)))
        ui->webViewShow(webView);*/
}

void WebFrameLoaderClient::dispatchDidLoadMainResource(DocumentLoader* loader)
{
}

void WebFrameLoaderClient::setMainDocumentError(DocumentLoader*, const ResourceError& error)
{
    if (!m_pluginView)
        return;

    if (m_pluginView->status() == PluginStatusLoadedSuccessfully)
        m_pluginView->didFail(error);
    m_pluginView = 0;
    m_hasSentResponseToPlugin = false;
}

void WebFrameLoaderClient::postProgressStartedNotification()
{
#ifdef BENCH_LOAD_TIME
    gettimeofday(&m_timerStart, NULL);
#endif

#if ENABLE(ORIGYNSUITE)
    ResourceRequest request = core(m_webFrame)->loader()->originalRequest();
    // before each clic or new page, check if we have new infos from servers
    OrigynServer::get()->redirectRequest(request);
    // TODO give new request to loader
    if (request.url() != core(m_webFrame)->loader()->originalRequest().url())
        core(m_webFrame)->loader()->load(request);
#endif


    WebCore::ObserverServiceData::createObserverService()->notifyObserver(WebViewProgressStartedNotification, "", m_webFrame->webView());
    WebNotificationDelegate* webNotificationDelegate = m_webFrame->webView()->webNotificationDelegate();
    if (webNotificationDelegate)
        webNotificationDelegate->startLoadNotification(m_webFrame);
}

void WebFrameLoaderClient::postProgressEstimateChangedNotification()
{
    WebCore::ObserverServiceData::createObserverService()->notifyObserver(WebViewProgressEstimateChangedNotification, "", m_webFrame->webView());
    WebNotificationDelegate* webNotificationDelegate = m_webFrame->webView()->webNotificationDelegate();
    if (webNotificationDelegate)
        webNotificationDelegate->progressNotification(m_webFrame);
}

void WebFrameLoaderClient::postProgressFinishedNotification()
{
    WebCore::ObserverServiceData::createObserverService()->notifyObserver(WebViewProgressFinishedNotification, "", m_webFrame->webView());
    WebNotificationDelegate* webNotificationDelegate = m_webFrame->webView()->webNotificationDelegate();
    if (webNotificationDelegate)
        webNotificationDelegate->finishedLoadNotification(m_webFrame);

#if ENABLE(ORIGYNSUITE)
    if (OrigynServer::get()->syncNeeded())
        OrigynServer::get()->synchronize(core(m_webFrame)); // sync if they are some
#endif

    //FIXME : remove this notification
    WebCore::ObserverServiceData::createObserverService()->notifyObserver("layoutTestController", "loadDone", NULL);

#ifdef BENCH_LOAD_TIME
    gettimeofday(&m_timerStop, NULL);
    if (m_timerStart.tv_sec == m_timerStop.tv_sec)
        printf("load time: %06d us\n", static_cast<uint32_t> (m_timerStop.tv_usec - m_timerStart.tv_usec));
    else {
        int seconds = m_timerStop.tv_sec - m_timerStart.tv_sec;
        int microseconds = m_timerStop.tv_usec - m_timerStart.tv_usec;
        if (microseconds < 0) {
            seconds -= 1;
            microseconds = 1000000 + microseconds;
        }
        printf("load time: %d s %06d us\n", seconds, microseconds);
    }
    // This is meant to help script watching OWB activity so that they can kill it and
    // dump the statistics.
    fflush(stdout);
#endif
}

void WebFrameLoaderClient::committedLoad(DocumentLoader* loader, const char* data, int length)
{
    // FIXME: This should probably go through the data source.
    const String& textEncoding = loader->response().textEncodingName();

    if (!m_pluginView)
        receivedData(data, length, textEncoding);

    if (!m_pluginView || m_pluginView->status() != PluginStatusLoadedSuccessfully)
        return;

    if (!m_hasSentResponseToPlugin) {
        m_pluginView->didReceiveResponse(core(m_webFrame)->loader()->documentLoader()->response());
        // didReceiveResponse sets up a new stream to the plug-in. on a full-page plug-in, a failure in
        // setting up this stream can cause the main document load to be cancelled, setting m_pluginView
        // to null
        if (!m_pluginView)
            return;
        m_hasSentResponseToPlugin = true;
    }
    m_pluginView->didReceiveData(data, length);
}

void WebFrameLoaderClient::receivedData(const char* data, int length, const String& textEncoding)
{
    Frame* coreFrame = core(m_webFrame);
    if (!coreFrame)
        return;

    // Set the encoding. This only needs to be done once, but it's harmless to do it again later.
    String encoding = coreFrame->loader()->documentLoader()->overrideEncoding();
    bool userChosen = !encoding.isNull();
    if (encoding.isNull())
        encoding = textEncoding;
    coreFrame->loader()->setEncoding(encoding, userChosen);

    coreFrame->loader()->addData(data, length);
}

void WebFrameLoaderClient::finishedLoading(DocumentLoader* loader)
{
    // Telling the frame we received some data and passing 0 as the data is our
    // way to get work done that is normally done when the first bit of data is
    // received, even for the case of a document with no data (like about:blank)
    if (!m_pluginView) {
        committedLoad(loader, 0, 0);
        return;
    }

    if (m_pluginView->status() == PluginStatusLoadedSuccessfully)
        m_pluginView->didFinishLoading();
    m_pluginView = 0;
    m_hasSentResponseToPlugin = false;
}

void WebFrameLoaderClient::updateGlobalHistory()
{
    WebHistory* history = WebHistory::sharedHistory();
    if (!history)
        return;
    DocumentLoader* loader = core(m_webFrame)->loader()->documentLoader();

    history->visitedURL(loader->urlForHistory(), loader->title(), loader->originalRequestCopy().httpMethod(), loader->urlForHistoryReflectsFailure());
}

void WebFrameLoaderClient::updateGlobalHistoryRedirectLinks()
{                                                                                                                                                          
    WebHistory* history = WebHistory::sharedHistory();                                                                                                     
    if (!history)                                                                                                                                          
        return;                                                                                                                                            
    DocumentLoader* loader = core(m_webFrame)->loader()->documentLoader();
    ASSERT(loader->unreachableURL().isEmpty());

    if (!loader->clientRedirectSourceForHistory().isNull()) {
        if (WebHistoryItem *webHistoryItem = history->itemForURLString(loader->clientRedirectSourceForHistory())) {
            webHistoryItem->getPrivateItem()->m_historyItem.get()->addRedirectURL(loader->clientRedirectDestinationForHistory());
        }
    }
                                                                                              
    if (!loader->serverRedirectSourceForHistory().isNull()) {                               
        if (WebHistoryItem *webHistoryItem = history->itemForURLString(loader->serverRedirectSourceForHistory())) {
            webHistoryItem->getPrivateItem()->m_historyItem.get()->addRedirectURL(loader->serverRedirectDestinationForHistory());
        }
    }
}

bool WebFrameLoaderClient::shouldGoToHistoryItem(HistoryItem*) const
{
    return true;
}

PassRefPtr<DocumentLoader> WebFrameLoaderClient::createDocumentLoader(const ResourceRequest& request, const SubstituteData& substituteData)
{
    RefPtr<WebDocumentLoader> loader = WebDocumentLoader::create(request, substituteData);

    WebDataSource* dataSource = WebDataSource::createInstance(loader.get());
    loader->setDataSource(dataSource);
    return loader.release();
}

void WebFrameLoaderClient::setTitle(const String& title, const KURL& url)
{
#if PLATFORM(AMIGAOS4)
    if (!m_webFrame->parentFrame()) {
        BalWidget* viewWindow = m_webFrame->webView()->viewWindow();
        if (viewWindow && viewWindow->window) {
            extern char* utf8ToAmiga(const char* utf8);

            char *titlestr = utf8ToAmiga(title.utf8().data());
            if (titlestr && titlestr[0])
                snprintf(viewWindow->title, sizeof(viewWindow->title), viewWindow->clickTabNode ? "%s" : "OWB: %s", titlestr);
            else
                strcpy(viewWindow->title, "Origyn Web Browser");
            free(titlestr);

            if (amigaConfig.tabs) {
                IIntuition->SetGadgetAttrs(viewWindow->gad_clicktab, viewWindow->window, NULL,
                                           CLICKTAB_Labels, ~0,
                                           TAG_DONE);

                IClickTab->SetClickTabNodeAttrs(viewWindow->clickTabNode, TNA_Text, viewWindow->title, TAG_DONE);

                IIntuition->RefreshSetGadgetAttrs(viewWindow->gad_clicktab, viewWindow->window, NULL,
                                                  CLICKTAB_Labels, viewWindow->clickTabList,
                                                  TAG_DONE);
            }
            else
                IIntuition->SetWindowTitles(viewWindow->window, viewWindow->title, (STRPTR)~0UL);

            CString urlLatin1 = url.prettyURL().latin1();
            const char *urlstr = urlLatin1.data();
            if (urlstr && urlstr[0] && viewWindow->gad_url) {
                snprintf(viewWindow->url, sizeof(viewWindow->url), "%s", urlstr);
                if (ILayout->SetPageGadgetAttrs(viewWindow->gad_url, viewWindow->page,
                                                viewWindow->window, NULL,
                                                STRINGA_TextVal, viewWindow->url,
                                                TAG_DONE))
                    ILayout->RefreshPageGadget(viewWindow->gad_url, viewWindow->page, viewWindow->window, NULL);
            }
        }
    }
#endif
    bool privateBrowsingEnabled = false;
    WebPreferences* preferences = m_webFrame->webView()->preferences();
    if (preferences)
        privateBrowsingEnabled = preferences->privateBrowsingEnabled();
    if (privateBrowsingEnabled)
        return;

    // update title in global history
    WebHistory* history = webHistory();
    if (!history)
        return;

    WebHistoryItem* item = history->itemForURL(url.string());
    if (!item)
        return;

    item->setTitle(title.utf8().data());
}

void WebFrameLoaderClient::savePlatformDataToCachedFrame(WebCore::CachedFrame* cachedFrame)
{
    Frame* coreFrame = core(m_webFrame);
    if (!coreFrame)
        return;

    ASSERT(coreFrame->loader()->documentLoader() == cachedFrame->documentLoader());

    WebCachedFramePlatformData* webPlatformData = new WebCachedFramePlatformData(getWebDataSource(coreFrame->loader()->documentLoader()));
    cachedFrame->setCachedFramePlatformData(webPlatformData);
}

void WebFrameLoaderClient::transitionToCommittedFromCachedFrame(CachedFrame*)                                                                              
{                                                                                                                                                          
}

void WebFrameLoaderClient::transitionToCommittedForNewPage()
{
    WebView* view = m_webFrame->webView();

    BalRectangle rect = view->frameRect();
    bool transparent = view->transparent();
    Color backgroundColor = transparent ? Color::transparent : Color::white;
    core(m_webFrame)->createView(IntRect(rect).size(), backgroundColor, transparent, IntSize(), false);
}

bool WebFrameLoaderClient::canCachePage() const
{
    return true;
}

PassRefPtr<Frame> WebFrameLoaderClient::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement, const String& referrer, bool /*allowsScrolling*/, int /*marginWidth*/, int /*marginHeight*/)
{
    RefPtr<Frame> result = createFrame(url, name, ownerElement, referrer);
    if (!result)
        return 0;

    return result.release();
}

PassRefPtr<Frame> WebFrameLoaderClient::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement, const String& referrer)
{
    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    WebFrame* webFrame = WebFrame::createInstance();
    //webFrame->ref();

    RefPtr<Frame> childFrame = webFrame->init(m_webFrame->webView(), coreFrame->page(), ownerElement);
    childFrame->ref();

    coreFrame->tree()->appendChild(childFrame);
    childFrame->tree()->setName(name);
    childFrame->init();

    // The creation of the frame may have run arbitrary JavaScript that removed it from the page already.
    if (!childFrame->page()) {
        delete webFrame;
        return 0;
    }

    childFrame->loader()->loadURLIntoChildFrame(url, referrer, childFrame.get());

    // The frame's onload handler may have removed it from the document.
    if (!childFrame->tree()->parent()) {
        delete webFrame;
        return 0;
    }

    return childFrame.release();
}

Widget* WebFrameLoaderClient::createPlugin(const IntSize& pluginSize, HTMLPlugInElement* element, const KURL& url, const Vector<String>& paramNames, const Vector<String>& paramValues, const String& mimeType, bool loadManually)
{
    Frame* frame = core(m_webFrame);
    PluginView* pluginView = PluginView::create(frame, pluginSize, element, url, paramNames, paramValues, mimeType, loadManually);

    if (pluginView->status() == PluginStatusLoadedSuccessfully)
        return pluginView;

    /*COMPtr<IWebResourceLoadDelegate> resourceLoadDelegate;

    WebView* webView = m_webFrame->webView();
    if (FAILED(webView->resourceLoadDelegate(&resourceLoadDelegate)))
        return pluginView;

    RetainPtr<CFMutableDictionaryRef> userInfo(AdoptCF, CFDictionaryCreateMutable(0, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));

    unsigned count = (unsigned)paramNames.size();
    for (unsigned i = 0; i < count; i++) {
        if (paramNames[i] == "pluginspage") {
            static CFStringRef key = MarshallingHelpers::LPCOLESTRToCFStringRef(WebKitErrorPlugInPageURLStringKey);
            RetainPtr<CFStringRef> str(AdoptCF, paramValues[i].createCFString());
            CFDictionarySetValue(userInfo.get(), key, str.get());
            break;
        }
    }

    if (!mimeType.isNull()) {
        static CFStringRef key = MarshallingHelpers::LPCOLESTRToCFStringRef(WebKitErrorMIMETypeKey);

        RetainPtr<CFStringRef> str(AdoptCF, mimeType.createCFString());
        CFDictionarySetValue(userInfo.get(), key, str.get());
    }

    String pluginName;
    if (pluginView->plugin())
        pluginName = pluginView->plugin()->name();
    if (!pluginName.isNull()) {
        static CFStringRef key = MarshallingHelpers::LPCOLESTRToCFStringRef(WebKitErrorPlugInNameKey);
        RetainPtr<CFStringRef> str(AdoptCF, pluginName.createCFString());
        CFDictionarySetValue(userInfo.get(), key, str.get());
    }

    COMPtr<CFDictionaryPropertyBag> userInfoBag(AdoptCOM, CFDictionaryPropertyBag::createInstance());
    userInfoBag->setDictionary(userInfo.get());
 
    int errorCode = 0;
    switch (pluginView->status()) {
        case PluginStatusCanNotFindPlugin:
            errorCode = WebKitErrorCannotFindPlugIn;
            break;
        case PluginStatusCanNotLoadPlugin:
            errorCode = WebKitErrorCannotLoadPlugIn;
            break;
        default:
            ASSERT_NOT_REACHED();
    }

    ResourceError resourceError(String(WebKitErrorDomain), errorCode, url.string(), String());
    COMPtr<IWebError> error(AdoptCOM, WebError::createInstance(resourceError, userInfoBag.get()));
     
    resourceLoadDelegate->plugInFailedWithError(webView, error.get(), getWebDataSource(frame->loader()->documentLoader()));*/

    return pluginView;
}

void WebFrameLoaderClient::redirectDataToPlugin(Widget* pluginWidget)
{
    // Ideally, this function shouldn't be necessary, see <rdar://problem/4852889>

    m_pluginView = static_cast<PluginView*>(pluginWidget);
}

WebHistory* WebFrameLoaderClient::webHistory() const
{
    if (m_webFrame != m_webFrame->webView()->topLevelFrame())
        return 0;

    return WebHistory::sharedHistory();
}

void WebFrameLoaderClient::forceLayoutForNonHTML()
{
    BalNotImplemented();
}

void WebFrameLoaderClient::setCopiesOnScroll()
{
    BalNotImplemented();
}

void WebFrameLoaderClient::detachedFromParent2()
{
    BalNotImplemented();
}

void WebFrameLoaderClient::detachedFromParent3()
{
    BalNotImplemented();
}

void WebFrameLoaderClient::cancelPolicyCheck()
{
    if (m_policyListener) {
        m_policyListener->invalidate();
        m_policyListener = 0;
    }

    m_policyFunction = 0;
}

void WebFrameLoaderClient::dispatchWillSubmitForm(FramePolicyFunction function, PassRefPtr<FormState> formState)
{
    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    (coreFrame->loader()->*function)(PolicyUse);
}

void WebFrameLoaderClient::revertToProvisionalState(DocumentLoader*)
{
    BalNotImplemented();
}

void WebFrameLoaderClient::setMainFrameDocumentReady(bool)
{
    BalNotImplemented();
}

void WebFrameLoaderClient::willChangeTitle(DocumentLoader*)
{
    BalNotImplemented();
}

void WebFrameLoaderClient::didChangeTitle(DocumentLoader*)
{
    BalNotImplemented();
}

bool WebFrameLoaderClient::canHandleRequest(const ResourceRequest& request) const
{
    return WebView::canHandleRequest(request);
}

bool WebFrameLoaderClient::canShowMIMEType(const String& type) const
{
    return MIMETypeRegistry::isSupportedImageMIMEType(type) || MIMETypeRegistry::isSupportedNonImageMIMEType(type) ||
            PluginDatabase::installedPlugins()->isMIMETypeRegistered(type);
}

bool WebFrameLoaderClient::representationExistsForURLScheme(const String& /*URLScheme*/) const
{
    BalNotImplemented();
    return false;
}

String WebFrameLoaderClient::generatedMIMETypeForURLScheme(const String& /*URLScheme*/) const
{
    BalNotImplemented();
    ASSERT_NOT_REACHED();
    return String();
}

void WebFrameLoaderClient::frameLoadCompleted()
{
}

void WebFrameLoaderClient::restoreViewState()
{
}

void WebFrameLoaderClient::provisionalLoadStarted()
{
    BalNotImplemented();
}

void WebFrameLoaderClient::didFinishLoad()
{
    BalNotImplemented();
}

void WebFrameLoaderClient::prepareForDataSourceReplacement()
{
    BalNotImplemented();
}

String WebFrameLoaderClient::userAgent(const KURL& url)
{
    return m_webFrame->webView()->userAgentForKURL(url.string().utf8().data());
}

void WebFrameLoaderClient::transitionToCommittedFromCachedPage(CachedPage*)
{
}

void WebFrameLoaderClient::saveViewStateToItem(HistoryItem*)
{
}

ResourceError WebFrameLoaderClient::cancelledError(const ResourceRequest& request)
{
    // FIXME: Need ChickenCat to include CFNetwork/CFURLError.h to get these values
    // Alternatively, we could create our own error domain/codes.
    return ResourceError(String(WebURLErrorDomain), -999, request.url().string(), String());
}

ResourceError WebFrameLoaderClient::blockedError(const ResourceRequest& request)
{
    // FIXME: Need to implement the String descriptions for errors in the WebKitErrorDomain and have them localized
    return ResourceError(String(WebKitErrorDomain), WebKitErrorCannotUseRestrictedPort, request.url().string(), String());
}

ResourceError WebFrameLoaderClient::cannotShowURLError(const ResourceRequest&)
{
    BalNotImplemented();
    return ResourceError();
}

ResourceError WebFrameLoaderClient::interruptForPolicyChangeError(const ResourceRequest& request)
{
    // FIXME: Need to implement the String descriptions for errors in the WebKitErrorDomain and have them localized
    return ResourceError(String(WebKitErrorDomain), WebKitErrorFrameLoadInterruptedByPolicyChange, request.url().string(), String());
}

ResourceError WebFrameLoaderClient::cannotShowMIMETypeError(const ResourceResponse&)
{
    BalNotImplemented();
    return ResourceError();
}

ResourceError WebFrameLoaderClient::fileDoesNotExistError(const ResourceResponse&)
{
    BalNotImplemented();
    return ResourceError();
}

ResourceError WebFrameLoaderClient::pluginWillHandleLoadError(const ResourceResponse& response)
{
    return ResourceError(String(WebKitErrorDomain), WebKitErrorPlugInWillHandleLoad, response.url().string(), String());
}


bool WebFrameLoaderClient::shouldFallBack(const ResourceError& error)
{
    return error.errorCode() != WebURLErrorCancelled;
}

WebFramePolicyListener* WebFrameLoaderClient::setUpPolicyListener(WebCore::FramePolicyFunction function)
{
    // FIXME: <rdar://5634381> We need to support multiple active policy listeners.

    if (m_policyListener)
        m_policyListener->invalidate();

    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    m_policyListener = WebFramePolicyListener::createInstance(coreFrame);
    m_policyFunction = function;

    return m_policyListener;
}

void WebFrameLoaderClient::receivedPolicyDecision(PolicyAction action)
{
    ASSERT(m_policyListener);
    ASSERT(m_policyFunction);

    FramePolicyFunction function = m_policyFunction;

    m_policyListener = 0;
    m_policyFunction = 0;

    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    (coreFrame->loader()->*function)(action);
}

void WebFrameLoaderClient::dispatchDecidePolicyForMIMEType(FramePolicyFunction function, const String& mimeType, const ResourceRequest& request)
{
    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    DefaultPolicyDelegate* policyDelegate = m_webFrame->webView()->policyDelegate();
    if (!policyDelegate)
        policyDelegate = DefaultPolicyDelegate::sharedInstance();

    WebMutableURLRequest* urlRequest = WebMutableURLRequest::createInstance(request);

    policyDelegate->decidePolicyForMIMEType(m_webFrame->webView(), mimeType.utf8().data(), urlRequest, m_webFrame, setUpPolicyListener(function));

    delete urlRequest;
}

void WebFrameLoaderClient::dispatchDecidePolicyForNewWindowAction(FramePolicyFunction function, const NavigationAction& action, const ResourceRequest& request, PassRefPtr<FormState> formState, const String& frameName)
{
    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    DefaultPolicyDelegate* policyDelegate = m_webFrame->webView()->policyDelegate();
    if (!policyDelegate)
        policyDelegate = DefaultPolicyDelegate::sharedInstance();

    WebMutableURLRequest* urlRequest =  WebMutableURLRequest::createInstance(request);
    WebActionPropertyBag* actionInformation = WebActionPropertyBag::createInstance(action, coreFrame);

    //policyDelegate->decidePolicyForNewWindowAction(d->webView, actionInformation, urlRequest, frameName, setUpPolicyListener(function));

    (coreFrame->loader()->*function)(PolicyUse);
    delete urlRequest;
    delete actionInformation;
}

void WebFrameLoaderClient::dispatchDecidePolicyForNavigationAction(FramePolicyFunction function, const NavigationAction& action, const ResourceRequest& request, PassRefPtr<FormState> formState)
{
    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    DefaultPolicyDelegate* policyDelegate = m_webFrame->webView()->policyDelegate();
    if (!policyDelegate) {
        m_webFrame->webView()->setPolicyDelegate(DefaultPolicyDelegate::sharedInstance());
        policyDelegate = m_webFrame->webView()->policyDelegate();
    }

    WebMutableURLRequest* urlRequest =  WebMutableURLRequest::createInstance(request);
    WebActionPropertyBag* actionInformation = WebActionPropertyBag::createInstance(action, coreFrame);

    policyDelegate->decidePolicyForNavigationAction(m_webFrame->webView(), actionInformation, urlRequest, m_webFrame, setUpPolicyListener(function));

    delete urlRequest;
    delete actionInformation;
}

void WebFrameLoaderClient::dispatchUnableToImplementPolicy(const ResourceError& error)
{
    DefaultPolicyDelegate* policyDelegate = m_webFrame->webView()->policyDelegate();
    if (!policyDelegate)
        policyDelegate = DefaultPolicyDelegate::sharedInstance();

    WebError* webError = WebError::createInstance(error);
    policyDelegate->unableToImplementPolicyWithError(m_webFrame->webView(), webError, m_webFrame);

    delete webError;
}

void WebFrameLoaderClient::download(ResourceHandle* handle, const ResourceRequest& request, const ResourceRequest&, const ResourceResponse& response)
{
    WebDownloadDelegate* downloadDelegate;
    WebView* webView = m_webFrame->webView();
    downloadDelegate = webView->downloadDelegate();
    if(downloadDelegate)
    {
        WebDownload* download = WebDownload::createInstance(handle, &request, &response, downloadDelegate);
        download->start();
    }
}

bool WebFrameLoaderClient::dispatchDidLoadResourceFromMemoryCache(DocumentLoader*, const ResourceRequest&, const ResourceResponse&, int /*length*/)
{
    BalNotImplemented();
    return false;
}

void WebFrameLoaderClient::dispatchDidFailProvisionalLoad(const ResourceError& error)
{
    WebFrameLoadDelegate* webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->didFailLoad(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidFailLoad(const ResourceError& error)
{
    WebFrameLoadDelegate* webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->didFailLoad(m_webFrame);
}

void WebFrameLoaderClient::startDownload(const ResourceRequest&)
{
    BalNotImplemented();
}

Widget* WebFrameLoaderClient::createJavaAppletWidget(const IntSize& pluginSize, HTMLAppletElement* element, const KURL& /*baseURL*/, const Vector<String>& paramNames, const Vector<String>& paramValues)
{
    PluginView* pluginView = PluginView::create(core(m_webFrame), pluginSize, element, KURL(), paramNames, paramValues, "application/x-java-applet", false);

    // Check if the plugin can be loaded successfully
    if (pluginView->plugin() && pluginView->plugin()->load())
        return pluginView;

    return pluginView;
}

ObjectContentType WebFrameLoaderClient::objectContentType(const KURL& url, const String& mimeTypeIn)
{
    String mimeType = mimeTypeIn;
    if (mimeType.isEmpty())
        mimeType = MIMETypeRegistry::getMIMETypeForExtension(url.path().substring(url.path().reverseFind('.') + 1));

    if (mimeType.isEmpty())
        return ObjectContentFrame; // Go ahead and hope that we can display the content.

    if (MIMETypeRegistry::isSupportedImageMIMEType(mimeType))
        return WebCore::ObjectContentImage;

    if (PluginDatabase::installedPlugins()->isMIMETypeRegistered(mimeType))
        return WebCore::ObjectContentNetscapePlugin;

    if (MIMETypeRegistry::isSupportedNonImageMIMEType(mimeType))
        return WebCore::ObjectContentFrame;

    return WebCore::ObjectContentNone;
}

String WebFrameLoaderClient::overrideMediaType() const
{
    BalNotImplemented();
    return String();
}

void WebFrameLoaderClient::windowObjectCleared()
{
    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    Settings* settings = coreFrame->settings();
    if (!settings || !settings->isJavaScriptEnabled())
        return;

//     COMPtr<IWebFrameLoadDelegate> frameLoadDelegate;
//     if (SUCCEEDED(d->webView->frameLoadDelegate(&frameLoadDelegate))) {
//         COMPtr<IWebFrameLoadDelegate2> frameLoadDelegate2(Query, frameLoadDelegate);
//
//         JSContextRef context = toRef(coreFrame->scriptProxy()->globalObject()->globalExec());
//         JSObjectRef windowObject = toRef(coreFrame->scriptProxy()->globalObject());
//         ASSERT(windowObject);
//
//         if (!frameLoadDelegate2 ||
//             FAILED(frameLoadDelegate2->didClearWindowObject(d->webView, context, windowObject, this)))
//             frameLoadDelegate->windowScriptObjectAvailable(d->webView, context, windowObject);
//     }
#if ENABLE(JS_ADDONS)
    if (m_webFrame->bindingJS())
        m_webFrame->bindingJS()->registerBinding();
#endif
   

    JSGlobalContextRef context = toGlobalRef(coreFrame->script()->globalObject()->globalExec());
    JSObjectRef windowObject = toRef(coreFrame->script()->globalObject());
    ASSERT(windowObject);

    WebFrameLoadDelegate* webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->windowObjectClearNotification(m_webFrame, (void*)context, (void*)windowObject);
}

void WebFrameLoaderClient::documentElementAvailable()
{                                             
}

void WebFrameLoaderClient::didPerformFirstNavigation() const
{
    WebPreferences* preferences = m_webFrame->webView()->preferences();
    if(!preferences)
        return;

    bool automaticallyDetectsCacheModel = preferences->automaticallyDetectsCacheModel();

    WebCacheModel cacheModel = preferences->cacheModel();

    if (automaticallyDetectsCacheModel && cacheModel < WebCacheModelDocumentBrowser)
        preferences->setCacheModel(WebCacheModelDocumentBrowser);
}

void WebFrameLoaderClient::frameLoaderDestroyed()
{
    // The FrameLoader going away is equivalent to the Frame going away,
    // so we now need to clear our frame pointer.
    //deref();
}

void WebFrameLoaderClient::registerForIconNotification(bool listen)
{
/*#if ENABLE(ICONDATABASE)
    d->webView->registerForIconNotification(listen);
#endif*/
}

void WebFrameLoaderClient::makeRepresentation(DocumentLoader*)
{
    BalNotImplemented();
}

