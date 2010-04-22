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
#include "DOMCoreClasses.h"
#include "NotImplemented.h"
#include "WebNavigationAction.h"
#include "WebCachedFramePlatformData.h"
#include "WebChromeClient.h"
#include "WebDocumentLoader.h"
#include "WebDownload.h"
#include "WebDownloadDelegate.h"
#include "WebError.h"
#include "WebFrame.h"
#include "WebFrameLoadDelegate.h"
#include "WebFramePolicyListener.h"
#include "WebHistory.h"
#include "WebHistoryDelegate.h"
#include "WebHistoryItem.h"
#include "WebHistoryItem_p.h"
#include "WebMutableURLRequest.h"
#include "WebNavigationData.h"
#include "WebNotificationDelegate.h"
#include "WebPreferences.h"
#include "WebResourceLoadDelegate.h"
#include "WebSecurityOrigin.h"
#include "WebURLAuthenticationChallenge.h"
#include "WebURLResponse.h"
#include "WebView.h"

#if ENABLE(JS_ADDONS)
#include "WebBindingJSDelegate.h"
#endif
#if ENABLE(WIDGET_ENGINE)
#include "WebWidgetEngineDelegate.h"
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
#include <HTMLObjectElement.h>
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
#if OS(AMIGAOS4)
#include <proto/clicktab.h>
#include <proto/intuition.h>
#include <proto/layout.h>
#include <gadgets/clicktab.h>
#include <intuition/gadgetclass.h>
#endif

#include <cstdio>

#if ENABLE(CEHTML)
#include "CEHTMLJSWindowExtensions.h"
#endif

#if ENABLE(DAE)
#include "WebApplication.h"
#include "WebApplicationManager.h"
#endif

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
    Frame* frame = core(m_webFrame);
    if (!frame)
        return;

    if (frame->document() && frame->document()->inPageCache())
        return;

    FrameView* view = frame->view();
    if (!view)
        return;

    view->setNeedsLayout();
    view->forceLayout(true);
}

void WebFrameLoaderClient::assignIdentifierToInitialRequest(unsigned long identifier, DocumentLoader* loader, const ResourceRequest& request)
{
    WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (!resourceLoadDelegate)
        return;

    WebMutableURLRequest* webURLRequest = WebMutableURLRequest::createInstance(request);
    resourceLoadDelegate->identifierForInitialRequest(webView, webURLRequest, getWebDataSource(loader), identifier);
    delete webURLRequest;
}

void WebFrameLoaderClient::dispatchDidReceiveAuthenticationChallenge(DocumentLoader* loader, unsigned long identifier, const AuthenticationChallenge& challenge)
{
#if USE(CURL)
    ASSERT(challenge.authenticationClient());
#endif

    WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (resourceLoadDelegate) {
        WebURLAuthenticationChallenge* webChallenge = WebURLAuthenticationChallenge::createInstance(challenge);
        resourceLoadDelegate->didReceiveAuthenticationChallenge(webView, identifier, webChallenge, getWebDataSource(loader));
        delete webChallenge;
    }

#if USE(CURL)
    // If the ResourceLoadDelegate doesn't exist or fails to handle the call, we tell the ResourceHandle
    // to continue without credential - this is the best approximation of Mac behavior
    challenge.authenticationClient()->receivedRequestToContinueWithoutCredential(challenge);
#endif
}

void WebFrameLoaderClient::dispatchDidFirstVisuallyNonEmptyLayout()
{
    SharedPtr<WebFrameLoadDelegate> frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->dispatchDidFirstVisuallyNonEmptyLayout(m_webFrame);
    
#if ENABLE(HBBTV_0_8)
    Frame* coreFrame = core(m_webFrame);
    if (coreFrame->document()) {
        HTMLElement* body = coreFrame->document()->body();
        
        // When HBBTV_0_8 is enabled, windows are created opaque by default.
        // Fixup the window transparency based on the page's background color
        // - necessary to support displaying multiple applications on the same
        // native window.
        if (body && body->renderer()->style()->hasBackground()) {
            Color backgroundColor = body->renderer()->style()->backgroundColor();
            coreFrame->view()->updateBackgroundRecursively(backgroundColor, backgroundColor.hasAlpha());
        }
    }
#endif
}


void WebFrameLoaderClient::dispatchDidCancelAuthenticationChallenge(DocumentLoader* loader, unsigned long identifier, const AuthenticationChallenge& challenge)
{
    WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (!resourceLoadDelegate)
        return;

    WebURLAuthenticationChallenge* webChallenge = WebURLAuthenticationChallenge::createInstance(challenge);
    resourceLoadDelegate->didCancelAuthenticationChallenge(webView, identifier, webChallenge, getWebDataSource(loader));
    delete webChallenge;
}

bool WebFrameLoaderClient::shouldUseCredentialStorage(DocumentLoader* loader, unsigned long identifier)
{
    /*WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (!resourceLoadDelegate)
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
    WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (!resourceLoadDelegate)
        return;

    WebMutableURLRequest* webURLRequest = WebMutableURLRequest::createInstance(request);
    WebURLResponse* webURLRedirectResponse = WebURLResponse::createInstance(redirectResponse);

    WebMutableURLRequest* newWebURLRequest = resourceLoadDelegate->willSendRequest(webView, identifier, webURLRequest, webURLRedirectResponse, getWebDataSource(loader));
    delete webURLRedirectResponse; 

    if (webURLRequest == newWebURLRequest) {
        delete webURLRequest;
        return;
    }

    if (!newWebURLRequest) {
        request = ResourceRequest();
        delete webURLRequest;
        return;
    }

    delete webURLRequest;

    request = newWebURLRequest->resourceRequest();
#endif
}

void WebFrameLoaderClient::dispatchDidReceiveResponse(DocumentLoader* loader, unsigned long identifier, const ResourceResponse& response)
{
#if ENABLE(WIDGET_ENGINE)
    SharedPtr<WebWidgetEngineDelegate> widgetEngineDelegate = m_webFrame->webView()->webWidgetEngineDelegate();
    if (widgetEngineDelegate && loader->responseMIMEType() == "application/widget") {
        const char* url = widgetEngineDelegate->receiveWidget(strdup(loader->responseURL().string().utf8().data()), m_webFrame);
        loader->stopLoading();
        if (url)
            m_webFrame->loadURL(url);

        return;
    }
#endif

    WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (!resourceLoadDelegate)
        return;

    WebURLResponse* webURLResponse = WebURLResponse::createInstance(response);
    resourceLoadDelegate->didReceiveResponse(webView, identifier, webURLResponse, getWebDataSource(loader));
    delete webURLResponse;
}

void WebFrameLoaderClient::dispatchDidReceiveContentLength(DocumentLoader* loader, unsigned long identifier, int length)
{
    WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (!resourceLoadDelegate)
        return;

    resourceLoadDelegate->didReceiveContentLength(webView, identifier, length, getWebDataSource(loader));
}

void WebFrameLoaderClient::dispatchDidFinishLoading(DocumentLoader* loader, unsigned long identifier)
{
    WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (!resourceLoadDelegate)
        return;

    resourceLoadDelegate->didFinishLoadingFromDataSource(webView, identifier, getWebDataSource(loader));
}

void WebFrameLoaderClient::dispatchDidFailLoading(DocumentLoader* loader, unsigned long identifier, const ResourceError& error)
{
    WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (!resourceLoadDelegate)
        return;

    WebError* webError = WebError::createInstance(error);
    resourceLoadDelegate->didFailLoadingWithError(webView, identifier, webError, getWebDataSource(loader));
    delete webError;
}

bool WebFrameLoaderClient::shouldCacheResponse(DocumentLoader* loader, unsigned long identifier, const ResourceResponse& response, const unsigned char* data, const unsigned long long length)
{
    /*WebView* webView = m_webFrame->webView();
    SharedPtr<WebResourceLoadDelegate> resourceLoadDelegate = webView->webResourceLoadDelegate();
    if (!resourceLoadDelegate)
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
    SharedPtr<WebFrameLoadDelegate> frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->didHandleOnloadEventsForFrame(m_webFrame);
    
#if ENABLE(DAE_APPLICATION)
    SharedPtr<WebApplication> app = webAppMgr().application(m_webFrame->webView());
    // FIXME: Not conformant to spec.: event gets dispatched *after* the document onload event
    if (app)
        app->dispatchApplicationEvent(eventNames().ApplicationLoadedEvent);
#endif
}

void WebFrameLoaderClient::dispatchDidReceiveServerRedirectForProvisionalLoad()
{
    SharedPtr<WebFrameLoadDelegate> frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->didReceiveServerRedirectForProvisionalLoadForFrame(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidCancelClientRedirect()
{
    SharedPtr<WebFrameLoadDelegate> frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->didCancelClientRedirectForFrame(m_webFrame);
}

void WebFrameLoaderClient::dispatchWillPerformClientRedirect(const KURL& url, double delay, double fireDate)
{
    SharedPtr<WebFrameLoadDelegate> frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->willPerformClientRedirectToURL(m_webFrame, url.string().utf8().data(), delay, fireDate);
}

void WebFrameLoaderClient::dispatchDidChangeLocationWithinPage()
{
    SharedPtr<WebFrameLoadDelegate> frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->didChangeLocationWithinPageForFrame(m_webFrame);
}
void WebFrameLoaderClient::dispatchDidPushStateWithinPage()
{
    balNotImplemented();
}

void WebFrameLoaderClient::dispatchDidReplaceStateWithinPage()
{
    balNotImplemented();
}

void WebFrameLoaderClient::dispatchDidPopStateWithinPage()
{
    balNotImplemented();
}

void WebFrameLoaderClient::dispatchWillClose()
{
    SharedPtr<WebFrameLoadDelegate> frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->willCloseFrame(m_webFrame);

#if ENABLE(DAE_APPLICATION)
    SharedPtr<WebApplication> app = webAppMgr().application(m_webFrame->webView());
    if (app)
        app->dispatchApplicationEvent(eventNames().ApplicationUnloadedEvent);
#endif
}

void WebFrameLoaderClient::dispatchDidReceiveIcon()
{
#if ENABLE(ICON_DATABASE)
    m_webFrame->webView()->dispatchDidReceiveIconFromWebFrame(m_webFrame);
#endif
}

void WebFrameLoaderClient::dispatchDidStartProvisionalLoad()
{
    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->didStartProvisionalLoad(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidReceiveTitle(const String& title)
{
    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->titleChange(m_webFrame, title.utf8().data());
}

void WebFrameLoaderClient::dispatchDidCommitLoad()
{
    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->didCommitLoad(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidFinishDocumentLoad()
{
    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->didFinishDocumentLoadForFrame(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidFinishLoad()
{
    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate)
        webFrameLoadDelegate->didFinishLoad(m_webFrame);
}

void WebFrameLoaderClient::dispatchDidFirstLayout()
{
    SharedPtr<WebFrameLoadDelegate> frameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (frameLoadDelegate)
        frameLoadDelegate->dispatchDidFirstLayout(m_webFrame);
}

Frame* WebFrameLoaderClient::dispatchCreatePage()
{
#if OS(AMIGAOS4)
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

    m_webFrame->webView()->stopLoading(false);
        

#if ENABLE(ORIGYNSUITE)
    ResourceRequest request = core(m_webFrame)->loader()->originalRequest();
    // before each clic or new page, check if we have new infos from servers
    OrigynServer::get()->redirectRequest(request);
    // TODO give new request to loader
    if (request.url() != core(m_webFrame)->loader()->originalRequest().url())
        core(m_webFrame)->loader()->load(request);
#endif

    SharedPtr<WebNotificationDelegate> webNotificationDelegate = m_webFrame->webView()->webNotificationDelegate();
    if (webNotificationDelegate)
        webNotificationDelegate->startLoadNotification(m_webFrame);
}

void WebFrameLoaderClient::postProgressEstimateChangedNotification()
{
    SharedPtr<WebNotificationDelegate> webNotificationDelegate = m_webFrame->webView()->webNotificationDelegate();
    if (webNotificationDelegate)
        webNotificationDelegate->progressNotification(m_webFrame);
}

void WebFrameLoaderClient::postProgressFinishedNotification()
{
    SharedPtr<WebNotificationDelegate> webNotificationDelegate = m_webFrame->webView()->webNotificationDelegate();
    if (webNotificationDelegate)
        webNotificationDelegate->finishedLoadNotification(m_webFrame);

#if ENABLE(ORIGYNSUITE)
    if (OrigynServer::get()->syncNeeded())
        OrigynServer::get()->synchronize(core(m_webFrame)); // sync if they are some
#endif

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
    coreFrame->loader()->writer()->setEncoding(encoding, userChosen);

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
    DocumentLoader* loader = core(m_webFrame)->loader()->documentLoader();

    WebView* webView = m_webFrame->webView();
    SharedPtr<WebHistoryDelegate> historyDelegate = webView->historyDelegate();
    if (historyDelegate) {
        String url(loader->urlForHistory().string());
        String title(loader->title());
        String redirectSource(loader->clientRedirectSourceForHistory());
        OwnPtr<WebURLResponse> urlResponse(WebURLResponse::createInstance(loader->response()));
        OwnPtr<WebMutableURLRequest> urlRequest(WebMutableURLRequest::createInstance(loader->originalRequestCopy()));
        OwnPtr<WebNavigationData> navigationData(WebNavigationData::createInstance(url.utf8().data(), title.utf8().data(), urlRequest.get(), urlResponse.get(), loader->substituteData().isValid(), redirectSource.utf8().data()));

        historyDelegate->didNavigateWithNavigationData(webView, navigationData.get(), m_webFrame);
        return;
    }


    WebHistory* history = WebHistory::sharedHistory();
    if (!history)
        return;

    history->visitedURL(strdup(loader->urlForHistory().string().utf8().data()), strdup(loader->title().utf8().data()), strdup(loader->originalRequestCopy().httpMethod().utf8().data()), loader->urlForHistoryReflectsFailure());
}

void WebFrameLoaderClient::updateGlobalHistoryRedirectLinks()
{
    WebView* webView = m_webFrame->webView();
    SharedPtr<WebHistoryDelegate> historyDelegate = webView->historyDelegate();

    WebHistory* history = WebHistory::sharedHistory();

    DocumentLoader* loader = core(m_webFrame)->loader()->documentLoader();
    ASSERT(loader->unreachableURL().isEmpty());

    if (!loader->clientRedirectSourceForHistory().isNull()) {
        if (historyDelegate) {
            String sourceURL(loader->clientRedirectSourceForHistory());
            String destinationURL(loader->clientRedirectDestinationForHistory());
            historyDelegate->didPerformClientRedirectFromURL(webView, sourceURL.utf8().data(), destinationURL.utf8().data(), m_webFrame);
        } else {
            if (history) {
                if (WebHistoryItem* webHistoryItem = history->itemForURLString(strdup(loader->clientRedirectSourceForHistory().utf8().data())))
                    webHistoryItem->getPrivateItem()->m_historyItem.get()->addRedirectURL(loader->clientRedirectDestinationForHistory());
            }
        }
    }

    if (!loader->serverRedirectSourceForHistory().isNull()) {
        if (historyDelegate) {
            String sourceURL(loader->serverRedirectSourceForHistory());
            String destinationURL(loader->serverRedirectDestinationForHistory());
            historyDelegate->didPerformServerRedirectFromURL(webView, sourceURL.utf8().data(), destinationURL.utf8().data(), m_webFrame);
        } else {
            if (history) {
                if (WebHistoryItem *webHistoryItem = history->itemForURLString(strdup(loader->serverRedirectSourceForHistory().utf8().data())))
                    webHistoryItem->getPrivateItem()->m_historyItem.get()->addRedirectURL(loader->serverRedirectDestinationForHistory());
            }
        }
    }
}

bool WebFrameLoaderClient::shouldGoToHistoryItem(HistoryItem *item) const
{
    // FIXME: This is a very simple implementation. More sophisticated
    // implementation would delegate the decision to a PolicyDelegate.
    // See mac implementation for example.
    return item != 0;
}

void WebFrameLoaderClient::dispatchDidAddBackForwardItem(HistoryItem*) const
{
    balNotImplemented();
}

void WebFrameLoaderClient::dispatchDidRemoveBackForwardItem(HistoryItem*) const
{
    balNotImplemented();
}

void WebFrameLoaderClient::dispatchDidChangeBackForwardIndex() const
{
    balNotImplemented();
}


void WebFrameLoaderClient::didDisplayInsecureContent()
{
    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate) {
        webFrameLoadDelegate->didDisplayInsecureContent(m_webFrame);
    }
}

void WebFrameLoaderClient::didRunInsecureContent(SecurityOrigin* origin)
{
    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate) {
        WebSecurityOrigin* webSecurityOrigin = WebSecurityOrigin::createInstance(origin);
        webFrameLoadDelegate->didRunInsecureContent(m_webFrame, webSecurityOrigin);
    }
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
#if OS(AMIGAOS4)
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
    WebView* webView = m_webFrame->webView();
    SharedPtr<WebHistoryDelegate> historyDelegate = webView->historyDelegate();
    if (historyDelegate) {
        historyDelegate->updateHistoryTitle(webView, title.utf8().data(), url.string().utf8().data());
        return;
    }
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

    WebHistoryItem* item = history->itemForURL(strdup(url.string().utf8().data()));
    if (!item)
        return;

    item->setTitle(title.utf8().data());
}

void WebFrameLoaderClient::savePlatformDataToCachedFrame(CachedFrame* cachedFrame)
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
    if (url.string().isEmpty())
        return 0;

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

void WebFrameLoaderClient::didTransferChildFrameToNewDocument()
{
}

PassRefPtr<Widget> WebFrameLoaderClient::createPlugin(const IntSize& pluginSize, HTMLPlugInElement* element, const KURL& url, const Vector<String>& paramNames, const Vector<String>& paramValues, const String& mimeType, bool loadManually)
{
#if ENABLE(CEHTML)
    if (isCEHTMLSupportedMIMEType(mimeType))
        return createCEHTMLPlugin(pluginSize, element, url, paramNames, paramValues, mimeType, loadManually);
#endif

#if ENABLE(DAE)
    if (isDAESupportedMIMEType(mimeType))
        return createDAEPlugin(pluginSize, element, url, paramNames, paramValues, mimeType, loadManually);
#endif

    Frame* frame = core(m_webFrame);
    PassRefPtr<PluginView> pluginView = PluginView::create(frame, pluginSize, element, url, paramNames, paramValues, mimeType, loadManually);

    if (pluginView->status() == PluginStatusLoadedSuccessfully)
        return pluginView;

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
    notImplemented();
}

void WebFrameLoaderClient::setCopiesOnScroll()
{
    notImplemented();
}

void WebFrameLoaderClient::detachedFromParent2()
{
    notImplemented();
}

void WebFrameLoaderClient::detachedFromParent3()
{
    notImplemented();
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

    (coreFrame->loader()->policyChecker()->*function)(PolicyUse);
}

void WebFrameLoaderClient::revertToProvisionalState(DocumentLoader*)
{
    notImplemented();
}

void WebFrameLoaderClient::setMainFrameDocumentReady(bool)
{
    notImplemented();
}

void WebFrameLoaderClient::willChangeTitle(DocumentLoader*)
{
    notImplemented();
}

void WebFrameLoaderClient::didChangeTitle(DocumentLoader*)
{
    notImplemented();
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
    notImplemented();
    return false;
}

String WebFrameLoaderClient::generatedMIMETypeForURLScheme(const String& /*URLScheme*/) const
{
    notImplemented();
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
    notImplemented();
}

void WebFrameLoaderClient::didFinishLoad()
{
    notImplemented();
}

void WebFrameLoaderClient::prepareForDataSourceReplacement()
{
    notImplemented();
}

String WebFrameLoaderClient::userAgent(const KURL& url)
{
    return m_webFrame->webView()->userAgentForKURL(url.string().utf8().data()).c_str();
}

void WebFrameLoaderClient::transitionToCommittedFromCachedPage(CachedPage*)
{
}

void WebFrameLoaderClient::saveViewStateToItem(HistoryItem*)
{
}

// FIXME: We need to have better names for the 7 next *Error methods and have a localized description for each.
ResourceError WebFrameLoaderClient::cancelledError(const ResourceRequest& request)
{
    return ResourceError(String(WebURLErrorDomain), WebURLErrorCancelled, request.url().string(), String());
}

ResourceError WebFrameLoaderClient::blockedError(const ResourceRequest& request)
{
    return ResourceError(String(WebKitErrorDomain), WebKitErrorCannotUseRestrictedPort, request.url().string(), String());
}

ResourceError WebFrameLoaderClient::cannotShowURLError(const ResourceRequest& request)
{
    return ResourceError(String(WebKitCannotShowURL), WebURLErrorBadURL, request.url().string(), String());
}

ResourceError WebFrameLoaderClient::interruptForPolicyChangeError(const ResourceRequest& request)
{
    return ResourceError(String(WebKitErrorDomain), WebKitErrorFrameLoadInterruptedByPolicyChange, request.url().string(), String());
}

ResourceError WebFrameLoaderClient::cannotShowMIMETypeError(const ResourceResponse& response)
{
    return ResourceError(String(WebKitErrorMIMETypeKey), WebKitErrorCannotShowMIMEType, response.url().string(), String());
}

ResourceError WebFrameLoaderClient::fileDoesNotExistError(const ResourceResponse& response)
{
    return ResourceError(String(WebKitFileDoesNotExist), WebURLErrorFileDoesNotExist, response.url().string(), String());
}

ResourceError WebFrameLoaderClient::pluginWillHandleLoadError(const ResourceResponse& response)
{
    return ResourceError(String(WebKitErrorDomain), WebKitErrorPlugInWillHandleLoad, response.url().string(), String());
}


bool WebFrameLoaderClient::shouldFallBack(const ResourceError& error)
{
    return error.errorCode() != WebURLErrorCancelled;
}

WebFramePolicyListener* WebFrameLoaderClient::setUpPolicyListener(FramePolicyFunction function)
{
    // FIXME: <rdar://5634381> We need to support multiple active policy listeners.

    if (m_policyListener)
        m_policyListener->invalidate();

    m_policyListener = WebFramePolicyListener::createInstance(m_webFrame);
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

    (coreFrame->loader()->policyChecker()->*function)(action);
}

void WebFrameLoaderClient::dispatchDecidePolicyForMIMEType(FramePolicyFunction function, const String& mimeType, const ResourceRequest& request)
{
    SharedPtr<WebPolicyDelegate> policyDelegate = m_webFrame->webView()->policyDelegate();
    if (!policyDelegate)
        policyDelegate = DefaultPolicyDelegate::sharedInstance();

    WebMutableURLRequest* urlRequest = WebMutableURLRequest::createInstance(request);

    if (policyDelegate->decidePolicyForMIMEType(m_webFrame->webView(), mimeType.utf8().data(), urlRequest, m_webFrame, setUpPolicyListener(function))) {
        delete urlRequest;
        return;
    }

    Frame* coreFrame = core(m_webFrame);
    (coreFrame->loader()->policyChecker()->*function)(PolicyUse);

}

void WebFrameLoaderClient::dispatchDecidePolicyForNewWindowAction(FramePolicyFunction function, const NavigationAction& action, const ResourceRequest& request, PassRefPtr<FormState> formState, const String& frameName)
{
    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    SharedPtr<WebPolicyDelegate> policyDelegate = m_webFrame->webView()->policyDelegate();
    if (!policyDelegate)
        policyDelegate = DefaultPolicyDelegate::sharedInstance();

    WebMutableURLRequest* urlRequest =  WebMutableURLRequest::createInstance(request);
    WebNavigationAction* actionInformation = WebNavigationAction::createInstance(&action, formState ? formState->form() : 0, m_webFrame);

    //policyDelegate->decidePolicyForNewWindowAction(d->webView, actionInformation, urlRequest, frameName, setUpPolicyListener(function));

    (coreFrame->loader()->policyChecker()->*function)(PolicyUse);
    delete urlRequest;
    delete actionInformation;
}

void WebFrameLoaderClient::dispatchDecidePolicyForNavigationAction(FramePolicyFunction function, const NavigationAction& action, const ResourceRequest& request, PassRefPtr<FormState> formState)
{
    SharedPtr<WebPolicyDelegate> policyDelegate = m_webFrame->webView()->policyDelegate();
    if (!policyDelegate) {
        policyDelegate = DefaultPolicyDelegate::sharedInstance();
        m_webFrame->webView()->setPolicyDelegate(policyDelegate);
    }

    WebMutableURLRequest* urlRequest =  WebMutableURLRequest::createInstance(request);
    WebNavigationAction* actionInformation = WebNavigationAction::createInstance(&action, formState ? formState->form() : 0, m_webFrame);

    policyDelegate->decidePolicyForNavigationAction(m_webFrame->webView(), actionInformation, urlRequest, m_webFrame, setUpPolicyListener(function));

    delete urlRequest;
    delete actionInformation;
}

void WebFrameLoaderClient::dispatchUnableToImplementPolicy(const ResourceError& error)
{
    SharedPtr<WebPolicyDelegate> policyDelegate = m_webFrame->webView()->policyDelegate();
    if (!policyDelegate)
        policyDelegate = DefaultPolicyDelegate::sharedInstance();

    WebError* webError = WebError::createInstance(error);
    policyDelegate->unableToImplementPolicyWithError(m_webFrame->webView(), webError, m_webFrame);

    delete webError;
}

void WebFrameLoaderClient::download(ResourceHandle* handle, const ResourceRequest& request, const ResourceRequest&, const ResourceResponse& response)
{
    SharedPtr<WebDownloadDelegate> downloadDelegate;
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
    notImplemented();
    return false;
}

void WebFrameLoaderClient::dispatchDidFailProvisionalLoad(const ResourceError& error)
{
    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate) {
        WebError* err = WebError::createInstance(error);
        webFrameLoadDelegate->didFailProvisionalLoad(m_webFrame, err);
    }
}

void WebFrameLoaderClient::dispatchDidFailLoad(const ResourceError& error)
{
    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
    if (webFrameLoadDelegate) {
        WebError* err = WebError::createInstance(error);
        webFrameLoadDelegate->didFailLoad(m_webFrame, err);
    }
}

void WebFrameLoaderClient::startDownload(const ResourceRequest&)
{
    notImplemented();
}

PassRefPtr<Widget> WebFrameLoaderClient::createJavaAppletWidget(const IntSize& pluginSize, HTMLAppletElement* element, const KURL& /*baseURL*/, const Vector<String>& paramNames, const Vector<String>& paramValues)
{
    RefPtr<PluginView> pluginView = PluginView::create(core(m_webFrame), pluginSize, element, KURL(), paramNames, paramValues, "application/x-java-applet", false);

    // Check if the plugin can be loaded successfully
    if (pluginView->plugin() && pluginView->plugin()->load())
        return pluginView;

    return pluginView;
}

ObjectContentType WebFrameLoaderClient::objectContentType(const KURL& url, const String& mimeType)
{
    return FrameLoader::defaultObjectContentType(url, mimeType);
}

String WebFrameLoaderClient::overrideMediaType() const
{
    notImplemented();
    return String();
}

void WebFrameLoaderClient::dispatchDidClearWindowObjectInWorld(DOMWrapperWorld* world)
{
    Frame* coreFrame = core(m_webFrame);
    ASSERT(coreFrame);

    Settings* settings = coreFrame->settings();
    if (!settings || !settings->isJavaScriptEnabled())
        return;

#if ENABLE(JS_ADDONS)
    SharedPtr<WebBindingJSDelegate> webBindingJSDelegate = m_webFrame->webView()->webBindingJSDelegate();
    if (webBindingJSDelegate)
        webBindingJSDelegate->registerBinding(m_webFrame);
#endif
#if ENABLE(CEHTML)
    addCEHTMLJSWindowExtensions(coreFrame->script()->globalObject(world));
#endif
#if ENABLE(DAE)
    addDAEJSWindowExtensions();
#endif
 
    JSGlobalContextRef context = toGlobalRef(coreFrame->script()->globalObject(world)->globalExec());
    JSObjectRef windowObject = toRef(coreFrame->script()->globalObject(world));
    ASSERT(windowObject);

    SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webFrame->webView()->webFrameLoadDelegate();
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
    delete this;
}

void WebFrameLoaderClient::registerForIconNotification(bool listen)
{
/*#if ENABLE(ICONDATABASE)
    d->webView->registerForIconNotification(listen);
#endif*/
}

void WebFrameLoaderClient::makeRepresentation(DocumentLoader*)
{
    notImplemented();
}

bool WebFrameLoaderClient::shouldUsePluginDocument(const String& mimeType) const
{
#if ENABLE(DAE)
    if (mimeType == "application/oipfapplicationmanager" || mimeType == "video/mpeg" 
        || mimeType == "video/mpeg4" || mimeType == "video/mp4" || mimeType == "video/broadcast"
        || mimeType == "application/oipfconfiguration" || mimeType == "audio/mpeg")
        return true;
#endif
    return false;
}

#if USE(CURL_OPENSSL)
void WebFrameLoaderClient::didReceiveSSLSecurityExtension(const ResourceRequest& request, const char* securityExtension)
{
    // FIXME: This sanity check is failing because m_webFrame->url() is returning null, so disable it for now.
    //ASSERT(request.url().string() == m_webFrame->url());
    UNUSED_PARAM(request); // For release build.
#if ENABLE(DAE_PERMISSION)
    SharedPtr<WebApplication> webApp = webAppMgr().application(m_webFrame->webView());
    if (webApp)
        webApp->setPermissions(securityExtension);
#endif
}
#endif
