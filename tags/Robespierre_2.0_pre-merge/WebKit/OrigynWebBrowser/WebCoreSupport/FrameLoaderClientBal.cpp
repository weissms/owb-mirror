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
#include "BIWindow.h"
#include "BIWindowEvent.h"
#include "Font.h"
#include "FrameLoaderClientBal.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "DocumentLoader.h"
#include "FrameBal.h"
#include "FrameLoader.h"
#include "Page.h"
#include "ProgressTracker.h"
#include "PlatformString.h"
#include "ResourceRequest.h"

using namespace BAL;

namespace WebCore {

FrameLoaderClientBal::FrameLoaderClientBal()
    : m_frame(0)
    , m_firstData(false)
    , m_loadFailed(false)
{
}

Frame* FrameLoaderClientBal::frame()
{
    return static_cast<Frame*> (m_frame);
}

String FrameLoaderClientBal::userAgent()
{
#ifdef __OWBAL_PLATFORM_MACPORT__
    //We use the user agent from safari to avoid the rejection from google services (google docs, gmail, etc...)
    return "Mozilla/5.0 (Macintosh; U; Intel Mac OS X; fr) AppleWebKit/522.11 (KHTML, like Gecko) Safari/412";
#else
   //NOTE: some pages don't render with this UA
    return "Mozilla/5.0 (X11; U; Linux i686; en-US) AppleWebKit/420+ (KHTML, like Gecko) Safari/412 OWB/Robespierre";
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
    FrameLoader *fl = loader->frameLoader();
    fl->setEncoding(m_response.textEncodingName(), false);
    fl->addData(data, length);
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

Widget* FrameLoaderClientBal::createPlugin(Element*, const KURL&, const Vector<String>&, const Vector<String>&, const String&, bool)
{
    BALNotImplemented();
    return 0;
}

Frame* FrameLoaderClientBal::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement,
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
    FrameLoadType childLoadType = FrameLoadTypeInternal;
    childFrame->loader()->load(url, referrer, childLoadType, name, 0, 0, HashMap<String, String>());
 
    // The frame's onload handler may have removed it from the document.
    if (!childFrame->tree()->parent())
        return 0;
    
    return childFrame.get();
}

void FrameLoaderClientBal::redirectDataToPlugin(Widget* pluginWidget)
{
    BALNotImplemented();
    return;
}

Widget* FrameLoaderClientBal::createJavaAppletWidget(const IntSize&, Element*, const KURL& baseURL,
                                                    const Vector<String>& paramNames, const Vector<String>& paramValues)
{
    BALNotImplemented();
    return 0;
}

ObjectContentType FrameLoaderClientBal::objectContentType(const KURL& url, const String& mimeType)
{
    BALNotImplemented();
    return ObjectContentType();
}

String FrameLoaderClientBal::overrideMediaType() const
{
    BALNotImplemented();
    return String();
}

void FrameLoaderClientBal::windowObjectCleared() const
{
    BALNotImplemented();
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

void FrameLoaderClientBal::setMainDocumentError(DocumentLoader* loader, const ResourceError&)
{
    if(m_firstData) {
        loader->frameLoader()->setEncoding(m_response.textEncodingName(), false);
        m_firstData = false;
    }

    if (m_frame)
        m_frame->loader()->loadDone();
}

void FrameLoaderClientBal::finishedLoading(DocumentLoader* loader)
{
    if (m_firstData) {
        FrameLoader* fl = loader->frameLoader();
        fl->setEncoding(m_response.textEncodingName(), false);
        m_firstData = false;
    }

    
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
//    m_frame->view()->setNeedsLayout();
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
    m_frame = 0;
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

bool FrameLoaderClientBal::canShowMIMEType(const String&) const
{
    BALNotImplemented();
    return true;
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

void FrameLoaderClientBal::setDocumentViewFromPageCache(WebCore::PageCache*)
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

void FrameLoaderClientBal::download(ResourceHandle*, const ResourceRequest&, const ResourceResponse&)
{
    BALNotImplemented();
}

ResourceError FrameLoaderClientBal::cancelledError(const ResourceRequest&)
{
    BALNotImplemented();
    return ResourceError();
}

ResourceError FrameLoaderClientBal::cannotShowURLError(const ResourceRequest&)
{
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

void FrameLoaderClientBal::saveDocumentViewToPageCache(PageCache*)
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

}
