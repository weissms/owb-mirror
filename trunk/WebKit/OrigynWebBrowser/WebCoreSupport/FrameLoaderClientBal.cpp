/*
 * Copyright (C) 2006 Don Gibson <dgibson77@gmail.com>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Trolltech ASA
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "BALConfiguration.h"
#include "BIEventLoop.h"
#include "BIObserverService.h"
#include "BIWindow.h"
#include "BIWindowEvent.h"
#include "Font.h"
#include "FrameLoaderClientBal.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "DocumentLoader.h"
#include "FrameBal.h"
#include "FrameLoader.h"
#include "MIMETypeRegistry.h"
#include "Page.h"
#include "ProgressTracker.h"
#include "PlatformString.h"
#ifdef __OWB_JS__
#include "bindings/runtime.h"
#include "bindings/runtime_root.h"
#include "ExecState.h"
#include "kjs/JSGlobalObject.h"
#include "object.h"
#endif //__OWB_JS__

#ifdef __OWB_NPAPI__
#include "PlugInInfoStore.h"
#include "PluginDatabase.h"
#include "PluginView.h"
#endif

#ifdef __TVCORE__
#include "TvCore.h"
#endif
#ifdef __DVBCORE__
#include "DvbCore.h"
#endif
using namespace BAL;

namespace WebCore {

FrameLoaderClientBal::FrameLoaderClientBal()
    : m_frame(0)
#ifdef __OWB_NPAPI__
    , m_pluginView(0)
#endif
    , m_firstData(false)
    , m_loadFailed(false)
#ifdef __TVCORE__
    , m_tvCore(0)
#endif
#ifdef __DVBCORE__
    , m_dvbCore(0)
#endif
{
}
FrameLoaderClientBal::~FrameLoaderClientBal()
{
#ifdef __TVCORE__
    if (m_tvCore)
        delete m_tvCore;
#endif
#ifdef __DVBCORE__
    if (m_dvbCore)
        delete m_dvbCore;
#endif
}

Frame* FrameLoaderClientBal::frame()
{
    return static_cast<Frame*> (m_frame.get());
}

String FrameLoaderClientBal::userAgent(const KURL&)
{
#ifdef __OWBAL_PLATFORM_MACPORT__
    //We use the user agent from safari to avoid the rejection from google services (google docs, gmail, etc...)
    return "Mozilla/5.0 (Macintosh; U; Intel Mac OS X; fr) AppleWebKit/522.11 (KHTML, like Gecko) Safari/412 OWB/Blastoise";
#elif PLATFORM(AMIGAOS4)
    return "Mozilla/5.0 (AMIGA; U; AmigaOS4 ppc; en-US) AppleWebKit/420+ (KHTML, like Gecko) Safari/412 OWB/Blastoise";
#else
   //NOTE: some pages don't render with this UA
    return "Mozilla/5.0 (X11; U; Linux i686; en-US) AppleWebKit/420+ (KHTML, like Gecko) Safari/412 OWB/Blastoise";
#endif
}

WTF::PassRefPtr<WebCore::DocumentLoader> FrameLoaderClientBal::createDocumentLoader(const WebCore::ResourceRequest& request, const SubstituteData& substituteData)
{
    RefPtr<DocumentLoader> loader = new DocumentLoader(request, substituteData);
    return loader.release();
}

void FrameLoaderClientBal::dispatchWillSubmitForm(FramePolicyFunction policyFunction,  PassRefPtr<FormState>)
{
    // FIXME: This is surely too simple
    ASSERT(frame() && policyFunction);
    if (!frame() || !policyFunction)
        return;
    (frame()->loader()->*policyFunction)(PolicyUse);
}


void FrameLoaderClientBal::committedLoad(DocumentLoader* loader, const char* data, int length)
{
    if (!frame())
        return;

    const String& textEncoding = loader->response().textEncodingName();

#ifdef __OWB_NPAPI__
    if (!m_pluginView) {
        ASSERT(loader->frame());
        // Setting the encoding on the frame loader is our way to get work done that is normally done
        // when the first bit of data is received, even for the case of a document with no data (like about:blank).
        String encoding = loader->overrideEncoding();
        bool userChosen = !encoding.isNull();
        if (!userChosen)
            encoding = loader->response().textEncodingName();

        FrameLoader* frameLoader = loader->frameLoader();
        frameLoader->setEncoding(encoding, userChosen);
        if (data)
            frameLoader->addData(data, length);
    }

    if (m_pluginView) {
        if (!m_hasSentResponseToPlugin) {
             m_pluginView->didReceiveResponse(loader->response());
             m_hasSentResponseToPlugin = true;
        }
        m_pluginView->didReceiveData(data, length);
    }
#else
    FrameLoader *fl = loader->frameLoader();
    fl->setEncoding(m_response.textEncodingName(), false);
    fl->addData(data, length);
#endif
}

void FrameLoaderClientBal::dispatchDidReceiveAuthenticationChallenge(DocumentLoader*, unsigned long  identifier, const AuthenticationChallenge&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidCancelAuthenticationChallenge(DocumentLoader*, unsigned long  identifier, const AuthenticationChallenge&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchWillSendRequest(DocumentLoader*, unsigned long , ResourceRequest&, const ResourceResponse&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::assignIdentifierToInitialRequest(unsigned long identifier, DocumentLoader*, const ResourceRequest&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::postProgressStartedNotification()
{
    m_loadFailed = false;
    // no progress notification for now
#ifndef NDEBUG
    if (!getenv("LAYOUT_TEST"))
        printf("Document download has started for %s.\n", m_frame->loader()->URL().url().ascii());
#endif
    //m_frame->view()->setDirtyRect(IntRect(0, 0, m_frame->view()->width(), m_frame->view()->height()));
}

void FrameLoaderClientBal::postProgressEstimateChangedNotification()
{
    IntRect rect(0, 0, m_frame->view()->width(), m_frame->view()->height());
    rect.unite(*m_frame->view()->dirtyRect());
    //m_frame->view()->setDirtyRect(rect);
}

void FrameLoaderClientBal::postProgressFinishedNotification()
{
    if (!m_loadFailed) {
#ifndef NDEBUG
    if (!getenv("LAYOUT_TEST"))
            printf("Document progress ended for %s.\n", m_frame->loader()->URL().url().ascii());
#endif
        IntRect rect(0, 0, m_frame->view()->width(), m_frame->view()->height());
        rect.unite(*m_frame->view()->dirtyRect());
//         m_frame->view()->setDirtyRect(IntRect(0, 0, m_frame->view()->contentsWidth(), m_frame->view()->contentsHeight()));

    }
#ifndef NDEBUG
    if (getenv("LAYOUT_TEST"))
            getBIObserverService()->notifyObserver("layoutTestController", "loadDone");
#endif
}

void FrameLoaderClientBal::frameLoaderDestroyed()
{
    m_frame = 0;
    delete this;
}

void FrameLoaderClientBal::dispatchDidReceiveResponse(DocumentLoader*, unsigned long, const ResourceResponse& response)
{
    m_response = response;
    m_firstData = true;
}

void FrameLoaderClientBal::dispatchDecidePolicyForMIMEType(FramePolicyFunction policyFunction, const String&, const ResourceRequest&)
{
    // FIXME: we need to call directly here (comment copied from Qt version)
    ASSERT(frame() && policyFunction);
    if (!frame() || !policyFunction)
        return;
    (frame()->loader()->*policyFunction)(PolicyUse);
}

void FrameLoaderClientBal::dispatchDecidePolicyForNewWindowAction(FramePolicyFunction policyFunction, const NavigationAction&, const ResourceRequest&, const String&)
{
    ASSERT(frame() && policyFunction);
    if (!frame() || !policyFunction)
        return;
    // FIXME: I think Qt version marshals this to another thread so when we
    // have multi-threaded download, we might need to do the same
    (frame()->loader()->*policyFunction)(PolicyIgnore);
}

void FrameLoaderClientBal::dispatchDecidePolicyForNavigationAction(FramePolicyFunction policyFunction, const NavigationAction&, const ResourceRequest&)
{
    ASSERT(frame() && policyFunction);
    if (!frame() || !policyFunction)
        return;
    (frame()->loader()->*policyFunction)(PolicyUse);
}

Widget* FrameLoaderClientBal::createPlugin(const IntSize& pluginSize, Element* element, const KURL& url, const Vector<String>& paramNames, const Vector<String>& paramValues, const String& mimeType, bool loadManually)
{
#ifdef __OWB_NPAPI__
    PluginView* pluginView = PluginDatabase::installedPlugins()->createPluginView(frame(), pluginSize, element, url, paramNames, paramValues, mimeType, loadManually);

    if (pluginView->status() == PluginStatusLoadedSuccessfully)
        return pluginView;
#endif

    BALNotImplemented();
    return 0;
}

WTF::PassRefPtr<WebCore::Frame> FrameLoaderClientBal::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement,
                                        const String& referrer, bool allowsScrolling, int marginWidth, int marginHeight)
{
    // new frame must have its frameloaderclient
    FrameLoaderClientBal* frameLoader = new FrameLoaderClientBal();
    // we create here our frame
    RefPtr<Frame> childFrame = new FrameBal(frame()->page(), ownerElement, frameLoader);
    frameLoader->setFrame(static_cast<FrameBal*>(childFrame.get()));
 
    // do not attach new child to parent frame here, or else WindowBal positionning and frame paints will be wrong
    //childFrame->view()->setParent(frame()->view());
    // FIXME: All of the below should probably be moved over into WebCore
    childFrame->tree()->setName(name);
    frame()->tree()->appendChild(childFrame);

 
    // load url in child
    FrameLoadType loadType = frame()->loader()->loadType();
    FrameLoadType childLoadType = FrameLoadTypeRedirectWithLockedHistory;
    childFrame->loader()->load(url, referrer, childLoadType, name, 0, 0);
 
    // The frame's onload handler may have removed it from the document.
    if (!childFrame->tree()->parent())
        return 0;
    
    return childFrame.get();
}

void FrameLoaderClientBal::redirectDataToPlugin(Widget* pluginWidget)
{
#ifdef __OWB_NPAPI__
    m_pluginView = static_cast<PluginView*>(pluginWidget);
#endif
}

Widget* FrameLoaderClientBal::createJavaAppletWidget(const IntSize&, Element*, const KURL& baseURL,
                                                    const Vector<String>& paramNames, const Vector<String>& paramValues)
{
    BALNotImplemented();
    return 0;
}

ObjectContentType FrameLoaderClientBal::objectContentType(const KURL& url, const String& mimeType)
{
    String type = mimeType;
    if (type.isEmpty())
        type = MIMETypeRegistry::getMIMETypeForExtension(url.path().mid(url.path().findRev('.') + 1));

    if (type.isEmpty())
        return WebCore::ObjectContentFrame;

    if (MIMETypeRegistry::isSupportedImageMIMEType(type))
        return WebCore::ObjectContentImage;

#ifdef __OWB_NPAPI__
    if (PluginDatabase::installedPlugins()->isMIMETypeRegistered(mimeType))
        return WebCore::ObjectContentNetscapePlugin;
#endif

    if (MIMETypeRegistry::isSupportedNonImageMIMEType(type))
        return WebCore::ObjectContentFrame;

    return WebCore::ObjectContentNone;
}

String FrameLoaderClientBal::overrideMediaType() const
{
    BALNotImplemented();
    return String();
}

void FrameLoaderClientBal::setFrame(FrameBal* frame)
{
    m_frame = frame;
#ifdef __TVCORE__
    if (!m_tvCore)
        m_tvCore = new KJS::TvCore(m_frame.get());
#endif
#ifdef __DVBCORE__
    if (!m_dvbCore)
        m_dvbCore = new KJS::DvbCore(m_frame.get());
#endif

}

void FrameLoaderClientBal::windowObjectCleared() const
{
    BALNotImplemented();
#ifdef __TVCORE__
    m_frame->addToJSWindowObject("tvCore", m_tvCore);
#endif
#ifdef __DVBCORE__
     m_frame->addToJSWindowObject("dvbCore", m_dvbCore);
#endif
}

void FrameLoaderClientBal::setMainFrameDocumentReady(bool ready)
{
    // this is only interesting once we provide an external API for the DOM
    BALNotImplemented();
}

bool FrameLoaderClientBal::hasWebView() const
{
    BALNotImplemented();
    return true;
}

bool FrameLoaderClientBal::hasFrameView() const
{
    BALNotImplemented();
    return true;
}

/**
 * called when document is completely loaded
 */
void FrameLoaderClientBal::dispatchDidFinishLoad()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::frameLoadCompleted()
{
    BALNotImplemented();
/*    if (m_frame->renderer()) {
        if (m_frame->view()->width() < m_frame->view()->contentsWidth())
            m_frame->renderer()->layer()->setHasHorizontalScrollbar(true);
        if (m_frame->view()->height() < m_frame->view()->contentsHeight())
            m_frame->renderer()->layer()->setHasVerticalScrollbar(true);
        m_frame->renderer()->layer()->updateScrollInfoAfterLayout();
    }*/
}

void FrameLoaderClientBal::saveViewStateToItem(HistoryItem*)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::restoreViewState()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::setMainDocumentError(DocumentLoader* loader, const ResourceError& error)
{
#ifdef __OWB_NPAPI__
    if (m_pluginView) {
        m_pluginView->didFail(error);
        m_pluginView = 0;
        m_hasSentResponseToPlugin = false;
    }
#endif
    if(m_firstData) {
        loader->frameLoader()->setEncoding(m_response.textEncodingName(), false);
        m_firstData = false;
    }

    if (m_frame)
        m_frame->loader()->loadDone();
}

void FrameLoaderClientBal::finishedLoading(DocumentLoader* loader)
{
#ifdef __OWB_NPAPI__
    if (!m_pluginView)
         committedLoad(loader, 0, 0);
    else {
         m_pluginView->didFinishLoading();
         m_pluginView = 0;
         m_hasSentResponseToPlugin = false;
    }
#endif
    if (m_firstData) {
        FrameLoader* fl = loader->frameLoader();
        fl->setEncoding(m_response.textEncodingName(), false);
        m_firstData = false;
    }
    if (frame())
        frame()->view()->setContentsPos(0, 0);
    
}

bool FrameLoaderClientBal::shouldGoToHistoryItem(HistoryItem* item) const
{
    //FIXME: probably too simple
    if (item)
        return true;
    return false;
}

bool FrameLoaderClientBal::privateBrowsingEnabled() const
{
    BALNotImplemented();
    return true;
}

void FrameLoaderClientBal::makeDocumentView()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::makeRepresentation(DocumentLoader*)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::forceLayout()
{
    BALNotImplemented();
    m_frame->view()->setNeedsLayout();
    m_frame->view()->layout();
}

void FrameLoaderClientBal::forceLayoutForNonHTML()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::setCopiesOnScroll()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::detachedFromParent1()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::detachedFromParent2()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::detachedFromParent3()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::detachedFromParent4()
{
    //BALNotImplemented();
    if( m_frame ){
        Frame *f=m_frame.get();
        f->deref();
        m_frame = 0;
        f=0;
    }
}

void FrameLoaderClientBal::loadedFromPageCache()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidHandleOnloadEvents()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidReceiveServerRedirectForProvisionalLoad()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidCancelClientRedirect()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchWillPerformClientRedirect(const KURL&, double, double)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidChangeLocationWithinPage()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchWillClose()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidReceiveIcon()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidStartProvisionalLoad()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidReceiveTitle(const String&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidCommitLoad()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidFinishDocumentLoad()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidFirstLayout()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchShow()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::cancelPolicyCheck()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidLoadMainResource(DocumentLoader*)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::revertToProvisionalState(DocumentLoader*)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::clearUnarchivingState(DocumentLoader*)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::willChangeTitle(DocumentLoader*)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::didChangeTitle(DocumentLoader* l)
{
    setTitle(l->title(), l->URL());
}

void FrameLoaderClientBal::finalSetupForReplace(DocumentLoader*)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::setDefersLoading(bool)
{
    BALNotImplemented();
}

bool FrameLoaderClientBal::isArchiveLoadPending(ResourceLoader*) const
{
    BALNotImplemented();
    return false;
}

void FrameLoaderClientBal::cancelPendingArchiveLoad(ResourceLoader*)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::clearArchivedResources()
{
    BALNotImplemented();
}

bool FrameLoaderClientBal::canHandleRequest(const ResourceRequest&) const
{
    BALNotImplemented();
    return true;
}

bool FrameLoaderClientBal::canShowMIMEType(const String& MIMEType) const
{
    BALNotImplemented();
    return true;
/*
    if (MIMETypeRegistry::isSupportedImageMIMEType(MIMEType))
        return true;

    if (MIMETypeRegistry::isSupportedNonImageMIMEType(MIMEType))
        return true;

    return false;
*/
}

bool FrameLoaderClientBal::representationExistsForURLScheme(const String&) const
{
    BALNotImplemented();
    return false;
}

String FrameLoaderClientBal::generatedMIMETypeForURLScheme(const String&) const
{
    BALNotImplemented();
    return String();
}

void FrameLoaderClientBal::provisionalLoadStarted()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::didFinishLoad()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::prepareForDataSourceReplacement()
{
    BALNotImplemented();
}

void FrameLoaderClientBal::setTitle(const String&, const KURL&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::setDocumentViewFromCachedPage(WebCore::CachedPage*)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidReceiveContentLength(DocumentLoader*, unsigned long  identifier, int lengthReceived)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidFinishLoading(DocumentLoader*, unsigned long  identifier)
{
}

void FrameLoaderClientBal::dispatchDidFailLoading(DocumentLoader*, unsigned long  identifier, const ResourceError&)
{
    BALNotImplemented();
}

bool FrameLoaderClientBal::dispatchDidLoadResourceFromMemoryCache(DocumentLoader*, const ResourceRequest&, const ResourceResponse&, int length)
{
    BALNotImplemented();
    return false;
}

void FrameLoaderClientBal::dispatchDidFailProvisionalLoad(const ResourceError& error)
{
#ifndef NDEBUG
    if (!getenv("LAYOUT_TEST"))
        printf("Error for '%s': %s\n", error.failingURL().deprecatedString().ascii(),
            error.localizedDescription().deprecatedString().ascii());
#endif
    m_loadFailed = true;
    BALNotImplemented();
}

void FrameLoaderClientBal::dispatchDidFailLoad(const ResourceError&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::download(ResourceHandle*, const ResourceRequest&, const ResourceRequest&, const ResourceResponse&)
{
    BALNotImplemented();
}

ResourceError FrameLoaderClientBal::cancelledError(const ResourceRequest&)
{
    BALNotImplemented();
    return ResourceError();
}

ResourceError FrameLoaderClientBal::blockedError(const ResourceRequest&)
{
    BALNotImplemented();
    return ResourceError();
}

ResourceError FrameLoaderClientBal::cannotShowURLError(const ResourceRequest&)
{
#ifndef NDEBUG
    if (!getenv("LAYOUT_TEST"))
        printf("Cannot show url.\n");
#endif
    m_loadFailed = true;
    BALNotImplemented();
    return ResourceError();
}

ResourceError FrameLoaderClientBal::interruptForPolicyChangeError(const ResourceRequest&)
{
    BALNotImplemented();
    return ResourceError();
}

ResourceError FrameLoaderClientBal::cannotShowMIMETypeError(const ResourceResponse&)
{
#ifndef NDEBUG
    if (!getenv("LAYOUT_TEST"))
        printf("Cannot show MIME type.\n");
#endif
    m_loadFailed = true;
    BALNotImplemented();
    return ResourceError();
}

ResourceError FrameLoaderClientBal::fileDoesNotExistError(const ResourceResponse&)
{
    BALNotImplemented();
    return ResourceError();
}

bool FrameLoaderClientBal::shouldFallBack(const ResourceError&)
{
    BALNotImplemented();
    return false;
}

bool FrameLoaderClientBal::willUseArchive(ResourceLoader*, const ResourceRequest&, const KURL& originalURL) const
{
    BALNotImplemented();
    return false;
}

void FrameLoaderClientBal::saveDocumentViewToCachedPage(CachedPage*)
{
    BALNotImplemented();
}

bool FrameLoaderClientBal::canCachePage() const
{
    BALNotImplemented();
    return false;
}

Frame* FrameLoaderClientBal::dispatchCreatePage()
{
    BALNotImplemented();
    return 0;
}

void FrameLoaderClientBal::dispatchUnableToImplementPolicy(const ResourceError&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::startDownload(const ResourceRequest&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::updateGlobalHistoryForStandardLoad(const KURL&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::updateGlobalHistoryForReload(const KURL&)
{
    BALNotImplemented();
}

void FrameLoaderClientBal::didPerformFirstNavigation() const
{
    BALNotImplemented();
}

#ifdef OWB_ICON_SUPPORT
void registerForIconNotification(bool)
{
    BALNotImplemented();
}
#endif //OWB_ICON_SUPPORT

}
