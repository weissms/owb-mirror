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
#include "WebView.h"

#include "PtrAndFlags.h"
#include "DefaultPolicyDelegate.h"
#include "WebDownloadDelegate.h"
#include "DOMCoreClasses.h"
#include "WebDatabaseManager.h"
#include "WebDocumentLoader.h"
#include "WebEditorClient.h"
#include "WebElementPropertyBag.h"
#include "WebFrame.h"
#include "WebBackForwardList.h"
#include "WebBackForwardList_p.h"
#include "WebChromeClient.h"
#include "WebContextMenuClient.h"
#include "WebDragClient.h"
#include "WebHistoryItem.h"
#include "WebHistoryItem_p.h"
#if ENABLE(ICONDATABASE)
#include "WebIconDatabase.h"
#endif
#if ENABLE(INSPECTOR)
#include "WebInspector.h"
#include "WebInspectorClient.h"
#endif
#include "WebPluginHalterClient.h"
#include "WebMutableURLRequest.h"
#include "WebPreferences.h"
#include "WebViewPrivate.h"

#if ENABLE(OFFLINE_WEB_APPLICATIONS)
#include <ApplicationCacheStorage.h>
#endif
#include <AXObjectCache.h>
#include "ObserverData.h"
#include "ObserverServiceData.h"
#include "KeyboardCodes.h"
#include <PlatformString.h>
#include <Cache.h>
#include <ContextMenu.h>
#include <ContextMenuController.h>
#include <CrossOriginPreflightResultCache.h>
#include <CString.h>
#include <Cursor.h>
#include <DragController.h>
#include <DragData.h>
#include <Editor.h>
#include <EventHandler.h>
#include <EventNames.h>
#include <FileSystem.h>
#include <FocusController.h>
#include <FrameLoader.h>
#include <FrameTree.h>
#include <FrameView.h>
#include <Frame.h>
#include <GraphicsContext.h>
#include <HistoryItem.h>
#include <HitTestResult.h>
#include <IntPoint.h>
#include <IntRect.h>
#include <KeyboardEvent.h>
#include <MIMETypeRegistry.h>
#include <NotImplemented.h>
#include <Page.h>
#include <PageCache.h>
#include <PageGroup.h>
#include <PlatformKeyboardEvent.h>
#include <PlatformMouseEvent.h>
#include <PlatformWheelEvent.h>
#include <PluginDatabase.h>
#include <PluginInfoStore.h>
#include <PluginView.h>
#include <ProgressTracker.h>
#include <RenderTheme.h>
#include <ResourceHandle.h>
#include <ResourceHandleClient.h>
#include <ScriptValue.h> 
#include <ScrollbarTheme.h>
#include <SelectionController.h>
#include <Settings.h>
#include <SimpleFontData.h>
#include <TypingCommand.h>

#include <Collector.h>
#include <JSCell.h>
#include <JSValue.h>
#include <wtf/HashSet.h>
#include "InitializeThreading.h"

#include "owb-config.h"
#include "FileIO.h"

#if PLATFORM(MACPORT)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif PLATFORM(AMIGAOS4)
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/layout.h>
#include <intuition/gadgetclass.h>
#else
#include <sys/sysinfo.h>
#endif

using namespace WebCore;
using std::min;
using std::max;

//static HMODULE accessibilityLib;

WebView* kit(Page* page)
{
    return page ? static_cast<WebChromeClient*>(page->chrome()->client())->webView() : 0;
}


class WebViewObserver : public ObserverData {
public:
    WebViewObserver(WebView* webview) : m_webView(webview) {};
    ~WebViewObserver() {m_webView = 0;}

    virtual void observe(const WebCore::String &topic, const WebCore::String &data, void *userData);
private:
    WebView *m_webView;
};

void WebViewObserver::observe(const WebCore::String &topic, const WebCore::String &data, void *userData)
{
    if (topic == WebPreferences::webPreferencesChangedNotification())
        m_webView->notifyPreferencesChanged(static_cast<WebPreferences*>(userData));
    if (topic == "PopupMenuHide")
        m_webView->popupMenuHide();
    if (topic == "PopupMenuShow")
        m_webView->popupMenuShow(userData);
}


class PreferencesChangedOrRemovedObserver : public ObserverData {
public:
    static PreferencesChangedOrRemovedObserver* sharedInstance();

private:
    PreferencesChangedOrRemovedObserver() {}
    virtual ~PreferencesChangedOrRemovedObserver() {}

public:
    virtual void observe(const WebCore::String &topic, const WebCore::String &data, void *userData);

private:
    void notifyPreferencesChanged(WebCacheModel);
    void notifyPreferencesRemoved(WebCacheModel);
};

PreferencesChangedOrRemovedObserver* PreferencesChangedOrRemovedObserver::sharedInstance()
{
    static PreferencesChangedOrRemovedObserver shared;
    return &shared;
}

void PreferencesChangedOrRemovedObserver::observe(const WebCore::String &topic, const WebCore::String &data, void *userData)
{
    WebPreferences* preferences = static_cast<WebPreferences*>(userData);
    if (!preferences)
        return;

    WebCacheModel cacheModel = preferences->cacheModel();

    if (topic == WebPreferences::webPreferencesChangedNotification())
        notifyPreferencesChanged(cacheModel);

    if (topic == WebPreferences::webPreferencesRemovedNotification())
        notifyPreferencesRemoved(cacheModel);
}

void PreferencesChangedOrRemovedObserver::notifyPreferencesChanged(WebCacheModel cacheModel)
{
    if (!WebView::didSetCacheModel() || cacheModel > WebView::cacheModel())
        WebView::setCacheModel(cacheModel);
    else if (cacheModel < WebView::cacheModel()) {
        WebCacheModel sharedPreferencesCacheModel = WebPreferences::sharedStandardPreferences()->cacheModel();
        WebView::setCacheModel(max(sharedPreferencesCacheModel, WebView::maxCacheModelInAnyInstance()));
    }
}

void PreferencesChangedOrRemovedObserver::notifyPreferencesRemoved(WebCacheModel cacheModel)
{
    if (cacheModel == WebView::cacheModel()) {
        WebCacheModel sharedPreferencesCacheModel = WebPreferences::sharedStandardPreferences()->cacheModel();
        WebView::setCacheModel(max(sharedPreferencesCacheModel, WebView::maxCacheModelInAnyInstance()));
    }
}

static const int maxToolTipWidth = 250;

static const int delayBeforeDeletingBackingStoreMsec = 5000;

static void initializeStaticObservers();

static void updateSharedSettingsFromPreferencesIfNeeded(WebPreferences*);

static bool continuousSpellCheckingEnabled;
static bool grammarCheckingEnabled;

static bool s_didSetCacheModel;
static WebCacheModel s_cacheModel = WebCacheModelDocumentViewer;

enum {
    UpdateActiveStateTimer = 1,
    DeleteBackingStoreTimer = 2,
};

bool WebView::s_allowSiteSpecificHacks = false;

#if ENABLE(OFFLINE_WEB_APPLICATIONS)
static void WebKitSetApplicationCachePathIfNecessary()
{
    static bool initialized = false;
    if (initialized)
        return;

    WebCore::String path = WebCore::pathByAppendingComponent(WebCore::homeDirectoryPath(), "ApplicationCache");

    if (!path.isNull())
        cacheStorage().setCacheDirectory(path);

    initialized = true;
}
#endif

WebView::WebView()
    : /*m_hostWindow(0)
    ,*/ m_viewWindow(0)
    , m_mainFrame(0)
    , m_policyDelegate(0)
    , m_downloadDelegate(0)
    , m_webNotificationDelegate(0)
    , m_webFrameLoadDelegate(0)
    , m_jsActionDelegate(0)
    , m_preferences(0)
    , m_userAgentOverridden(false)
    , m_useBackForwardList(true)
    , m_zoomMultiplier(1.0f)
    , m_mouseActivated(false)
    // , m_dragData(0)
    // , m_currentCharacterCode(0)
    , m_isBeingDestroyed(false)
    , m_paintCount(0)
    , m_hasSpellCheckerDocumentTag(false)
    , m_smartInsertDeleteEnabled(false)
    , m_selectTrailingWhitespaceEnabled(false)
    , m_didClose(false)
    , m_hasCustomDropTarget(false)
    , m_inIMEComposition(0)
    , m_toolTipHwnd(0)
    , m_deleteBackingStoreTimerActive(false)
    , m_transparent(false)
    , m_isInitialized(false)
    // , m_closeWindowTimer(this, &WebView::closeWindowTimerFired)
    , m_topLevelParent(0)
    , d(new WebViewPrivate(this))
    , m_webViewObserver(new WebViewObserver(this))
{
    JSC::initializeThreading();
    WebCore::InitializeLoggingChannelsIfNecessary();

    d->clearDirtyRegion();

    initializeStaticObservers();

#if ENABLE(INSPECTOR)
    m_page = new Page(new WebChromeClient(this), new WebContextMenuClient(this), new WebEditorClient(this), new WebDragClient(this), new WebInspectorClient(this), new WebPluginHalterClient());
#else
    m_page = new Page(new WebChromeClient(this), new WebContextMenuClient(this), new WebEditorClient(this), new WebDragClient(this), 0, new WebPluginHalterClient());
#endif
    m_mainFrame = WebFrame::createInstance();
    m_mainFrame->init(this, m_page, 0);

    WebCore::ObserverServiceData::createObserverService()->registerObserver("PopupMenuShow", m_webViewObserver);
    WebCore::ObserverServiceData::createObserverService()->registerObserver("PopupMenuHide", m_webViewObserver);

    WebPreferences* sharedPreferences = WebPreferences::sharedStandardPreferences();
    continuousSpellCheckingEnabled = sharedPreferences->continuousSpellCheckingEnabled();
    grammarCheckingEnabled = sharedPreferences->grammarCheckingEnabled();
    sharedPreferences->willAddToWebView();
    m_preferences = sharedPreferences;
}

WebView::~WebView()
{
    close();
    
    if (m_preferences)
        if (m_preferences != WebPreferences::sharedStandardPreferences())
            delete m_preferences;
    if (m_policyDelegate)
        m_policyDelegate = 0;
    if (m_downloadDelegate)
        m_downloadDelegate = 0;
    if (m_webNotificationDelegate)
        m_webNotificationDelegate = 0;
    if (m_mainFrame)
        delete m_mainFrame;
    if (d)
        delete d;
    if (m_webViewObserver)
        delete m_webViewObserver;
}

WebView* WebView::createInstance()
{
    WebView* instance = new WebView();
    return instance;
}

void initializeStaticObservers()
{
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;

    WebCore::ObserverServiceData::createObserverService()->registerObserver(WebPreferences::webPreferencesChangedNotification(), PreferencesChangedOrRemovedObserver::sharedInstance());
    WebCore::ObserverServiceData::createObserverService()->registerObserver(WebPreferences::webPreferencesRemovedNotification(), PreferencesChangedOrRemovedObserver::sharedInstance());

}

static HashSet<WebView*>& allWebViewsSet()
{
    static HashSet<WebView*> allWebViewsSet;
    return allWebViewsSet;
}

void WebView::addToAllWebViewsSet()
{
    allWebViewsSet().add(this);
}

void WebView::removeFromAllWebViewsSet()
{
    allWebViewsSet().remove(this);
}

void WebView::setCacheModel(WebCacheModel cacheModel)
{
    if (s_didSetCacheModel && cacheModel == s_cacheModel)
        return;

    //TODO : Calcul the next values
    unsigned long long memSize = 256;
    unsigned long long diskFreeSize = 4096;
    
#if PLATFORM(MACPORT)
    unsigned int physmem;
    size_t len = sizeof physmem;
    static int mib[2] = { CTL_HW, HW_PHYSMEM };
    static size_t miblen = sizeof(mib) / sizeof(mib[0]);
  
    if (sysctl (mib, miblen, &physmem, &len, NULL, 0) == 0 && len == sizeof (physmem))
       memSize=physmem / (1024*1024);
#elif PLATFORM(AMIGAOS4)
    memSize = IExec->AvailMem(MEMF_TOTAL) / (1024 * 1024);
#else
    struct sysinfo info;
    memset(&info, 0, sizeof(info));
    if (sysinfo(&info) == 0)
        memSize = info.totalram * info.mem_unit / (1024 * 1024);
#endif

    unsigned cacheTotalCapacity = 0;
    unsigned cacheMinDeadCapacity = 0;
    unsigned cacheMaxDeadCapacity = 0;
    double deadDecodedDataDeletionInterval = 0;

    unsigned pageCacheCapacity = 0;

    switch (cacheModel) {
    case WebCacheModelDocumentViewer: {
        // Page cache capacity (in pages)
        pageCacheCapacity = 0;

        // Object cache capacities (in bytes)
        if (memSize >= 2048)
            cacheTotalCapacity = 96 * 1024 * 1024;
        else if (memSize >= 1536)
            cacheTotalCapacity = 64 * 1024 * 1024;
        else if (memSize >= 1024)
            cacheTotalCapacity = 32 * 1024 * 1024;
        else if (memSize >= 512)
            cacheTotalCapacity = 16 * 1024 * 1024;

        cacheMinDeadCapacity = 0;
        cacheMaxDeadCapacity = 0;

        break;
    }
    case WebCacheModelDocumentBrowser: {
        // Page cache capacity (in pages)
        if (memSize >= 1024)
            pageCacheCapacity = 3;
        else if (memSize >= 512)
            pageCacheCapacity = 2;
        else if (memSize >= 256)
            pageCacheCapacity = 1;
        else
            pageCacheCapacity = 0;

        // Object cache capacities (in bytes)
        if (memSize >= 2048)
            cacheTotalCapacity = 96 * 1024 * 1024;
        else if (memSize >= 1536)
            cacheTotalCapacity = 64 * 1024 * 1024;
        else if (memSize >= 1024)
            cacheTotalCapacity = 32 * 1024 * 1024;
        else if (memSize >= 512)
            cacheTotalCapacity = 16 * 1024 * 1024;

        cacheMinDeadCapacity = cacheTotalCapacity / 8;
        cacheMaxDeadCapacity = cacheTotalCapacity / 4;

        break;
    }
    case WebCacheModelPrimaryWebBrowser: {
        // Page cache capacity (in pages)
        // (Research indicates that value / page drops substantially after 3 pages.)
        if (memSize >= 2048)
            pageCacheCapacity = 5;
        else if (memSize >= 1024)
            pageCacheCapacity = 4;
        else if (memSize >= 512)
            pageCacheCapacity = 3;
        else if (memSize >= 256)
            pageCacheCapacity = 2;
        else
            pageCacheCapacity = 1;

        // Object cache capacities (in bytes)
        // (Testing indicates that value / MB depends heavily on content and
        // browsing pattern. Even growth above 128MB can have substantial 
        // value / MB for some content / browsing patterns.)
        if (memSize >= 2048)
            cacheTotalCapacity = 128 * 1024 * 1024;
        else if (memSize >= 1536)
            cacheTotalCapacity = 96 * 1024 * 1024;
        else if (memSize >= 1024)
            cacheTotalCapacity = 64 * 1024 * 1024;
        else if (memSize >= 512)
            cacheTotalCapacity = 32 * 1024 * 1024;

        cacheMinDeadCapacity = cacheTotalCapacity / 4;
        cacheMaxDeadCapacity = cacheTotalCapacity / 2;

        // This code is here to avoid a PLT regression. We can remove it if we
        // can prove that the overall system gain would justify the regression.
        cacheMaxDeadCapacity = max(24u, cacheMaxDeadCapacity);

        deadDecodedDataDeletionInterval = 60;

        break;
    }
    default:
        ASSERT_NOT_REACHED();
    };

    cache()->setCapacities(cacheMinDeadCapacity, cacheMaxDeadCapacity, cacheTotalCapacity);
    cache()->setDeadDecodedDataDeletionInterval(deadDecodedDataDeletionInterval);
    pageCache()->setCapacity(pageCacheCapacity);

    s_didSetCacheModel = true;
    s_cacheModel = cacheModel;
    return;
}

WebCacheModel WebView::cacheModel()
{
    return s_cacheModel;
}

bool WebView::didSetCacheModel()
{
    return s_didSetCacheModel;
}

WebCacheModel WebView::maxCacheModelInAnyInstance()
{
    WebCacheModel cacheModel = WebCacheModelDocumentViewer;

    HashSet<WebView*>::iterator end = allWebViewsSet().end();
    for (HashSet<WebView*>::iterator it = allWebViewsSet().begin(); it != end; ++it) {
        WebPreferences* pref = (*it)->preferences();
        if (!pref)
            continue;
        WebCacheModel prefCacheModel = WebCacheModelDocumentViewer;
        prefCacheModel = pref->cacheModel();

        cacheModel = max(cacheModel, prefCacheModel);
    }

    return cacheModel;
}

void WebView::close()
{
    if (m_didClose)
        return;

    m_didClose = true;

#ifndef NDEBUG
    Cache::Statistics stats = cache()->getStatistics();
    printf("stats about cache:\n");
    printf("\timages: count=%d - size=%d - liveSize=%d - decodedSize=%d\n", stats.images.count, stats.images.size, stats.images.liveSize, stats.images.decodedSize);
    printf("\tcssStyleSheets: count=%d - size=%d - liveSize=%d - decodedSize=%d\n", stats.cssStyleSheets.count, stats.cssStyleSheets.size, stats.cssStyleSheets.liveSize, stats.cssStyleSheets.decodedSize);
    printf("\tscripts: count=%d - size=%d - liveSize=%d - decodedSize=%d\n", stats.scripts.count, stats.scripts.size, stats.scripts.liveSize, stats.scripts.decodedSize);
    printf("\tfonts: count=%d - size=%d - liveSize=%d - decodedSize=%d\n", stats.fonts.count, stats.fonts.size, stats.fonts.liveSize, stats.fonts.decodedSize);
#if ENABLE(XSLT)
    printf("\txslStyleSheets: count=%d - size=%d - liveSize=%d - decodedSize=%d\n", stats.xslStyleSheets.count, stats.xslStyleSheets.size, stats.xslStyleSheets.liveSize, stats.xslStyleSheets.decodedSize);
#endif
#if ENABLE(XBL)
    printf("\txblDocs: count=%d - size=%d - liveSize=%d - decodedSize=%d\n", stats.xblDocs.count, stats.xblDocs.size, stats.xblDocs.liveSize, stats.xblDocs.decodedSize);
#endif
#endif
    // Purge page cache
    // The easiest way to do that is to disable page cache
    // The preferences can be null.
    if (m_preferences && m_preferences->usesPageCache())
        m_page->settings()->setUsesPageCache(false);
    
    removeFromAllWebViewsSet();

    if (!WebCore::cache()->disabled()) {
        WebCore::cache()->setDisabled(true);
        WebCore::cache()->setDisabled(false);

#if ENABLE(STORAGE)
        // Empty the application cache.
        WebCore::cacheStorage().empty();
#endif

        // Empty the Cross-Origin Preflight cache
        WebCore::CrossOriginPreflightResultCache::shared().empty();
    }

    Frame* frame = m_page->mainFrame();
    if (frame)
        frame->loader()->detachFromParent();

    delete m_page;
    m_page = 0;

/*#if ENABLE(ICONDATABASE)
    registerForIconNotification(false);
#endif*/
    WebCore::ObserverServiceData::createObserverService()->removeObserver(WebPreferences::webPreferencesChangedNotification(), m_webViewObserver);
    WebCore::ObserverServiceData::createObserverService()->removeObserver("PopupMenuShow", m_webViewObserver);
    WebCore::ObserverServiceData::createObserverService()->removeObserver("PopupMenuHide", m_webViewObserver);
    const String& identifier = m_preferences ? m_preferences->identifier() : String();
    if (identifier != String())
        WebPreferences::removeReferenceForIdentifier(identifier);

    WebPreferences* preferences = m_preferences;
    if (preferences)
        preferences->didRemoveFromWebView();

    deleteBackingStore();
}

void WebView::repaint(const WebCore::IntRect& windowRect, bool contentChanged, bool immediate, bool repaintContentOnly)
{
    if (d)
        d->repaint(windowRect, contentChanged, immediate, repaintContentOnly);
}

void WebView::deleteBackingStore()
{
    if (m_deleteBackingStoreTimerActive) {
        m_deleteBackingStoreTimerActive = false;
    }
    //m_backingStoreBitmap.clear();
    d->clearDirtyRegion();
}

bool WebView::ensureBackingStore()
{
    /*RECT windowRect;
    ::GetClientRect(m_viewWindow, &windowRect);
    LONG width = windowRect.right - windowRect.left;
    LONG height = windowRect.bottom - windowRect.top;
    if (width > 0 && height > 0 && (width != m_backingStoreSize.cx || height != m_backingStoreSize.cy)) {
        deleteBackingStore();

        m_backingStoreSize.cx = width;
        m_backingStoreSize.cy = height;
        BitmapInfo bitmapInfo = BitmapInfo::createBottomUp(IntSize(m_backingStoreSize));

        void* pixels = NULL;
        m_backingStoreBitmap.set(::CreateDIBSection(NULL, &bitmapInfo, DIB_RGB_COLORS, &pixels, NULL, 0));
        return true;
    }*/

    return false;
}

void WebView::addToDirtyRegion(const BalRectangle& dirtyRect)
{
    d->addToDirtyRegion(dirtyRect);
}

BalRectangle WebView::dirtyRegion()
{
    return d->dirtyRegion();
}

void WebView::clearDirtyRegion()
{
    d->clearDirtyRegion();
}

void WebView::scrollBackingStore(FrameView* frameView, int dx, int dy, const BalRectangle& scrollViewRect, const BalRectangle& clipRect)
{
    d->scrollBackingStore(frameView, dx, dy, scrollViewRect, clipRect);
    
}

void WebView::updateBackingStore(FrameView* frameView, bool backingStoreCompletelyDirty)
{
    //frameView->updateBackingStore();
}

void WebView::selectionChanged() 
{
    Frame* targetFrame = m_page->focusController()->focusedOrMainFrame();
    if (!targetFrame || !targetFrame->editor()->hasComposition())
        return;
    
    if (targetFrame->editor()->ignoreCompositionSelectionChange())
        return;

    unsigned start;
    unsigned end;
    if (!targetFrame->editor()->getCompositionSelection(start, end))
        targetFrame->editor()->confirmCompositionWithoutDisturbingSelection();
}


void WebView::onExpose(BalEventExpose ev)
{
    d->onExpose(ev);
}

void WebView::onKeyDown(BalEventKey ev)
{
    d->onKeyDown(ev);
}

void WebView::onKeyUp(BalEventKey ev)
{
    d->onKeyUp(ev);
}

void WebView::onMouseMotion(BalEventMotion ev)
{
    d->onMouseMotion(ev);
}

void WebView::onMouseButtonDown(BalEventButton ev)
{
    d->onMouseButtonDown(ev);
}

void WebView::onMouseButtonUp(BalEventButton ev)
{
    d->onMouseButtonUp(ev);
}

void WebView::onScroll(BalEventScroll ev)
{
    d->onScroll(ev);
}

void WebView::onResize(BalResizeEvent ev)
{
    d->onResize(ev);
}

void WebView::onQuit(BalQuitEvent ev)
{
    d->onQuit(ev);
}

void WebView::onUserEvent(BalUserEvent ev)
{
    d->onUserEvent(ev);
}

void WebView::paint()
{
    /*Frame* coreFrame = core(m_mainFrame);
    if (!coreFrame)
        return;
    FrameView* frameView = coreFrame->view();
    frameView->paint();*/
}

BalRectangle WebView::frameRect()
{
    return d->frameRect();
}

void WebView::closeWindowSoon()
{
//    m_closeWindowTimer.startOneShot(0);
#if PLATFORM(AMIGAOS4)
    closeWindow();
#endif    
}

/*void WebView::closeWindowTimerFired(WebCore::Timer<WebView>*)
{
    closeWindow();
}*/

void WebView::closeWindow()
{
#if PLATFORM(AMIGAOS4)
    extern void closeAmigaWindow(BalWidget *owbwindow);

    closeAmigaWindow(viewWindow());
#endif    
}

bool WebView::canHandleRequest(const WebCore::ResourceRequest& request)
{
    // On the mac there's an about url protocol implementation but CFNetwork doesn't have that.
    if (equalIgnoringCase(String(request.url().protocol()), "about"))
        return true;

    return true;
}

void WebView::reloadFromOrigin()
{
    if (!m_mainFrame)
        return;

    m_mainFrame->reloadFromOrigin();
}

const char* WebView::standardUserAgentWithApplicationName(const char* applicationName)
{
#if PLATFORM(MACPORT)
    // We use the user agent from safari to avoid the rejection from google services (google docs, gmail, etc...)
    return  "Mozilla/5.0 (Macintosh; U; Intel Mac OS X; fr) AppleWebKit/522.11 (KHTML, like Gecko) Safari/412 OWB/Doduo";
#elif PLATFORM(AMIGAOS4)
    if (IExec->Data.LibBase->lib_Version < 53)
        return "Mozilla/5.0 (compatible; Origyn Web Browser; AmigaOS 4.0; ppc; U; en) AppleWebKit/528.5+ (KHTML, like Gecko, Safari/528.5+)";
    else
        return "Mozilla/5.0 (compatible; Origyn Web Browser; AmigaOS 4.1; ppc; U; en) AppleWebKit/528.5+ (KHTML, like Gecko, Safari/528.5+)";
#else
    // NOTE: some pages don't render with this UA.
    // m_userAgentStandard = "Mozilla/5.0 (iPod; U; CPU like Mac OS X; fr) AppleWebKit/420.1 (KHTML, like Gecko) Version/3.0 Mobile/3B48b Safari/419.3";
    return "Mozilla/5.0 (X11; U; Linux i686; en-US) AppleWebKit/525.1+ (KHTML, like Gecko, Safari/525.1+)";
    /*m_userAgentStandard = String::format("Mozilla/5.0 (Windows; U; %s; %s) AppleWebKit/%s (KHTML, like Gecko)%s%s", osVersion().latin1().data(), defaultLanguage().latin1().data(), webKitVersion().latin1().data(), (m_applicationName.length() ? " " : ""), m_applicationName.latin1().data());*/
#endif
}

void WebView::setCookieEnabled(bool enable)
{
    if (!m_page)
        return;

    m_page->setCookieEnabled(enable);
}

bool WebView::cookieEnabled()
{
    if (!m_page)
        return false;

    return m_page->cookieEnabled();
}

Page* WebView::page()
{
    return m_page;
}

static const unsigned CtrlKey = 1 << 0;
static const unsigned AltKey = 1 << 1;
static const unsigned ShiftKey = 1 << 2;
#if PLATFORM(AMIGAOS4)
static const unsigned AmigaKey = 1 << 4;
#endif


struct KeyDownEntry {
    unsigned virtualKey;
    unsigned modifiers;
    const char* name;
};

struct KeyPressEntry {
    unsigned charCode;
    unsigned modifiers;
    const char* name;
};

static const KeyDownEntry keyDownEntries[] = {
    { VK_LEFT,   0,                  "MoveLeft"                                    },
    { VK_LEFT,   ShiftKey,           "MoveLeftAndModifySelection"                  },
    { VK_LEFT,   CtrlKey,            "MoveWordLeft"                                },
    { VK_LEFT,   CtrlKey | ShiftKey, "MoveWordLeftAndModifySelection"              },
    { VK_RIGHT,  0,                  "MoveRight"                                   },
    { VK_RIGHT,  ShiftKey,           "MoveRightAndModifySelection"                 },
    { VK_RIGHT,  CtrlKey,            "MoveWordRight"                               },
    { VK_RIGHT,  CtrlKey | ShiftKey, "MoveWordRightAndModifySelection"             },
    { VK_UP,     0,                  "MoveUp"                                      },
    { VK_UP,     ShiftKey,           "MoveUpAndModifySelection"                    },
    { VK_PRIOR,  ShiftKey,           "MovePageUpAndModifySelection"                },
    { VK_DOWN,   0,                  "MoveDown"                                    },
    { VK_DOWN,   ShiftKey,           "MoveDownAndModifySelection"                  },
    { VK_NEXT,   ShiftKey,           "MovePageDownAndModifySelection"              },
    { VK_PRIOR,  0,                  "MovePageUp"                                  },
    { VK_NEXT,   0,                  "MovePageDown"                                },
    { VK_HOME,   0,                  "MoveToBeginningOfLine"                       },
    { VK_HOME,   ShiftKey,           "MoveToBeginningOfLineAndModifySelection"     },
    { VK_HOME,   CtrlKey,            "MoveToBeginningOfDocument"                   },
    { VK_HOME,   CtrlKey | ShiftKey, "MoveToBeginningOfDocumentAndModifySelection" },

    { VK_END,    0,                  "MoveToEndOfLine"                             },
    { VK_END,    ShiftKey,           "MoveToEndOfLineAndModifySelection"           },
    { VK_END,    CtrlKey,            "MoveToEndOfDocument"                         },
    { VK_END,    CtrlKey | ShiftKey, "MoveToEndOfDocumentAndModifySelection"       },

    { VK_BACK,   0,                  "DeleteBackward"                              },
    { VK_BACK,   ShiftKey,           "DeleteBackward"                              },
    { VK_DELETE, 0,                  "DeleteForward"                               },
    { VK_BACK,   CtrlKey,            "DeleteWordBackward"                          },
    { VK_DELETE, CtrlKey,            "DeleteWordForward"                           },
    
    { 'B',       CtrlKey,            "ToggleBold"                                  },
    { 'I',       CtrlKey,            "ToggleItalic"                                },

    { VK_ESCAPE, 0,                  "Cancel"                                      },
    { VK_OEM_PERIOD, CtrlKey,        "Cancel"                                      },
    { VK_TAB,    0,                  "InsertTab"                                   },
    { VK_TAB,    ShiftKey,           "InsertBacktab"                               },
    { VK_RETURN, 0,                  "InsertNewline"                               },
    { VK_RETURN, CtrlKey,            "InsertNewline"                               },
    { VK_RETURN, AltKey,             "InsertNewline"                               },
    { VK_RETURN, ShiftKey,           "InsertNewline"                               },
    { VK_RETURN, AltKey | ShiftKey,  "InsertNewline"                               },

    // It's not quite clear whether clipboard shortcuts and Undo/Redo should be handled
    // in the application or in WebKit. We chose WebKit.
    { 'C',       CtrlKey,            "Copy"                                        },
    { 'V',       CtrlKey,            "Paste"                                       },
    { 'X',       CtrlKey,            "Cut"                                         },
    { 'A',       CtrlKey,            "SelectAll"                                   },
#if PLATFORM(AMIGAOS4)
    { 'C',       AmigaKey,           "Copy"                                        },
    { 'V',       AmigaKey,           "Paste"                                       },
    { 'X',       AmigaKey,           "Cut"                                         },
    { 'A',       AmigaKey,           "SelectAll"                                   },
#endif
    { VK_INSERT, CtrlKey,            "Copy"                                        },
    { VK_DELETE, ShiftKey,           "Cut"                                         },
    { VK_INSERT, ShiftKey,           "Paste"                                       },
    { 'Z',       CtrlKey,            "Undo"                                        },
    { 'Z',       CtrlKey | ShiftKey, "Redo"                                        },
    { 9999999,   0,                  ""                                            },
};

static const KeyPressEntry keyPressEntries[] = {
    { '\t',   0,                  "InsertTab"                                   },
    { '\t',   ShiftKey,           "InsertBacktab"                               },
    { '\r',   0,                  "InsertNewline"                               },
    { '\r',   CtrlKey,            "InsertNewline"                               },
    { '\r',   AltKey,             "InsertNewline"                               },
    { '\r',   ShiftKey,           "InsertNewline"                               },
    { '\r',   AltKey | ShiftKey,  "InsertNewline"                               },
    { 9999999,   0,               ""                                         },
};

const char* WebView::interpretKeyEvent(const KeyboardEvent* evt)
{
    ASSERT(evt->type() == eventNames().keydownEvent || evt->type() == eventNames().keypressEvent);

    static HashMap<int, const char*>* keyDownCommandsMap = 0;
    static HashMap<int, const char*>* keyPressCommandsMap = 0;

    if (!keyDownCommandsMap) {
        keyDownCommandsMap = new HashMap<int, const char*>;
        keyPressCommandsMap = new HashMap<int, const char*>;

        int i = 0;
        while(keyDownEntries[i].virtualKey != 9999999) {
            keyDownCommandsMap->set(keyDownEntries[i].modifiers << 16 | keyDownEntries[i].virtualKey, keyDownEntries[i].name);
            i++;
        }

        i = 0;
        while(keyPressEntries[i].charCode != 9999999) {
            keyPressCommandsMap->set(keyPressEntries[i].modifiers << 16 | keyPressEntries[i].charCode, keyPressEntries[i].name);
            i++;
        }
    }

    unsigned modifiers = 0;
    if (evt->shiftKey())
        modifiers |= ShiftKey;
    if (evt->altKey())
        modifiers |= AltKey;
    if (evt->ctrlKey())
        modifiers |= CtrlKey;
#if PLATFORM (AMIGAOS4)
    if (evt->metaKey())
        modifiers |= AmigaKey;
#endif

    if (evt->type() == eventNames().keydownEvent) {
        int mapKey = modifiers << 16 | evt->keyCode();
        return mapKey ? keyDownCommandsMap->get(mapKey) : 0;
    }

    int mapKey = modifiers << 16 | evt->charCode();
    return mapKey ? keyPressCommandsMap->get(mapKey) : 0;
}

bool WebView::handleEditingKeyboardEvent(KeyboardEvent* evt)
{
    WebCore::Node* node = evt->target()->toNode();
    ASSERT(node);
    Frame* frame = node->document()->frame();
    ASSERT(frame);

    const PlatformKeyboardEvent* keyEvent = evt->keyEvent();
    if (!keyEvent)  // do not treat this as text input if it's a system key event
        return false;

    Editor::Command command = frame->editor()->command(interpretKeyEvent(evt));

    if (keyEvent->type() == PlatformKeyboardEvent::RawKeyDown) {
        // WebKit doesn't have enough information about mode to decide how commands that just insert text if executed via Editor should be treated,
        // so we leave it upon WebCore to either handle them immediately (e.g. Tab that changes focus) or let a keypress event be generated
        // (e.g. Tab that inserts a Tab character, or Enter).
        return !command.isTextInsertion() && command.execute(evt);
    }

     if (command.execute(evt))
        return true;

    // Don't insert null or control characters as they can result in unexpected behaviour
    if (evt->charCode() < ' ')
        return false;

    return frame->editor()->insertText(evt->keyEvent()->text(), evt);
}



bool WebView::developerExtrasEnabled() const
{
    if (m_preferences->developerExtrasDisabledByOverride())
        return false;

#ifdef NDEBUG
    bool enabled = m_preferences->developerExtrasEnabled();
    return enabled;
#else
    return true;
#endif
}

const char* WebView::userAgentForKURL(const char*)
{
    if (m_userAgentOverridden)
        return m_userAgentCustom.c_str();

    char* userAgent = getenv("OWB_USER_AGENT");
    if (userAgent)
        m_userAgentStandard = userAgent;

    if (!m_userAgentStandard.length())
        m_userAgentStandard = WebView::standardUserAgentWithApplicationName(m_applicationName.c_str());

    return m_userAgentStandard.c_str();
}

bool WebView::canShowMIMEType(const char* mimeType)
{
    return MIMETypeRegistry::isSupportedImageMIMEType(mimeType) ||
        MIMETypeRegistry::isSupportedNonImageMIMEType(mimeType) ||
        PluginInfoStore::supportsMIMEType(mimeType);
}

bool WebView::canShowMIMETypeAsHTML(const char* /*mimeType*/)
{
    // FIXME
    return true;
}

void WebView::setMIMETypesShownAsHTML(const char* /*mimeTypes*/, int /*cMimeTypes*/)
{
}

void WebView::initWithFrame(BalRectangle& frame, const char* frameName, const char* groupName)
{
    if (m_isInitialized)
        return;

    m_isInitialized = true;

    if (!m_viewWindow)
        m_viewWindow = d->createWindow(frame);
    else
        d->setFrameRect(frame);


    RefPtr<Frame> coreFrame = core(m_mainFrame);
    m_mainFrame->setName(frameName);
    coreFrame->init();
    setGroupName(groupName);
    WebCore::FrameView* frameView = coreFrame->view();
    d->initWithFrameView(frameView);

    addToAllWebViewsSet();

    initializeToolTipWindow();
    windowAncestryDidChange();

#if ENABLE(DATABASE)
    WebKitSetWebDatabasesPathIfNecessary();
#if ENABLE(OFFLINE_WEB_APPLICATIONS)
    WebKitSetApplicationCachePathIfNecessary();
#endif
#endif

    WebCore::ObserverServiceData::createObserverService()->registerObserver(WebPreferences::webPreferencesChangedNotification(), m_webViewObserver);
    
    m_preferences->postPreferencesChangesNotification();

    setSmartInsertDeleteEnabled(true);
    PageGroup::setShouldTrackVisitedLinks(true);
}

/*static bool initCommonControls()
{
    static bool haveInitialized = false;
    if (haveInitialized)
        return true;

    INITCOMMONCONTROLSEX init;
    init.dwSize = sizeof(init);
    init.dwICC = ICC_TREEVIEW_CLASSES;
    haveInitialized = !!::InitCommonControlsEx(&init);
    return haveInitialized;
}*/

void WebView::initializeToolTipWindow()
{
    /*if (!initCommonControls())
        return;

    m_toolTipHwnd = CreateWindowEx(0, TOOLTIPS_CLASS, 0, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                   m_viewWindow, 0, 0, 0);
    if (!m_toolTipHwnd)
        return;

    TOOLINFO info = {0};
    info.cbSize = sizeof(info);
    info.uFlags = TTF_IDISHWND | TTF_SUBCLASS ;
    info.uId = reinterpret_cast<UINT_PTR>(m_viewWindow);

    ::SendMessage(m_toolTipHwnd, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&info));
    ::SendMessage(m_toolTipHwnd, TTM_SETMAXTIPWIDTH, 0, maxToolTipWidth);
dsf
    ::SetWindowPos(m_toolTipHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);*/
}

void WebView::setToolTip(const char* toolTip)
{
#if PLATFORM(AMIGAOS4)
    extern char* utf8ToAmiga(const char* utf8);

    if (toolTip == m_toolTip)
        return;

    m_toolTip = toolTip;

    BalWidget *widget = m_viewWindow;
    if (widget && widget->gad_status) {
        char* toolTipAmiga = utf8ToAmiga(toolTip);
        snprintf(widget->toolTipText, sizeof(widget->toolTipText), "%s", toolTipAmiga);
        free(toolTipAmiga);
        if (widget->statusBarText[0] && widget->toolTipText[0])
            snprintf(widget->statusToolTipText, sizeof(widget->statusToolTipText), "%s | %s", widget->statusBarText, widget->toolTipText);
        else
            snprintf(widget->statusToolTipText, sizeof(widget->statusToolTipText), "%s", widget->statusBarText[0] ? widget->statusBarText : widget->toolTipText);
        if (ILayout->SetPageGadgetAttrs(widget->gad_status, widget->page,
                                        widget->window, NULL,
                                        GA_Text, widget->statusToolTipText,
                                        TAG_DONE))
            ILayout->RefreshPageGadget(widget->gad_status, widget->page, widget->window, NULL);
    }
#endif
    /*if (!m_toolTipHwnd)
        return;

    if (toolTip == m_toolTip)
        return;

    m_toolTip = toolTip;

    if (!m_toolTip.isEmpty()) {
        TOOLINFO info = {0};
        info.cbSize = sizeof(info);
        info.uFlags = TTF_IDISHWND;
        info.uId = reinterpret_cast<UINT_PTR>(m_viewWindow);
        info.lpszText = const_cast<UChar*>(m_toolTip.charactersWithNullTermination());
        ::SendMessage(m_toolTipHwnd, TTM_UPDATETIPTEXT, 0, reinterpret_cast<LPARAM>(&info));
    }

    ::SendMessage(m_toolTipHwnd, TTM_ACTIVATE, !m_toolTip.isEmpty(), 0);*/
}

/*#if ENABLE(ICONDATABASE)
void WebView::notifyDidAddIcon()
{
    dispatchDidReceiveIconFromWebFrame(m_mainFrame);
}

void WebView::registerForIconNotification(bool listen)
{
    if (listen)
        WebCore::ObserverServiceData::createObserverService()->registerObserver(WebIconDatabase::iconDatabaseDidAddIconNotification(), this);
    else
        WebCore::ObserverServiceData::createObserverService()->removeObserver(WebIconDatabase::iconDatabaseDidAddIconNotification(), this);
}

void WebView::dispatchDidReceiveIconFromWebFrame(WebFrame* frame)
{
    registerForIconNotification(false);
}
#endif*/

void WebView::setDownloadDelegate(WebDownloadDelegate* d)
{
    m_downloadDelegate = d;
}

WebDownloadDelegate* WebView::downloadDelegate()
{
    return m_downloadDelegate;
}

void WebView::setPolicyDelegate(DefaultPolicyDelegate* d)
{
    m_policyDelegate = d;
}

DefaultPolicyDelegate* WebView::policyDelegate()
{
    return m_policyDelegate;
}

void WebView::setWebNotificationDelegate(WebNotificationDelegate* notificationDelegate)
{
    m_webNotificationDelegate = notificationDelegate;
}

WebNotificationDelegate* WebView::webNotificationDelegate()
{
    return m_webNotificationDelegate;
}

void WebView::setWebFrameLoadDelegate(WebFrameLoadDelegate* webFrameLoadDelegate)
{
    m_webFrameLoadDelegate = webFrameLoadDelegate;

}

WebFrameLoadDelegate* WebView::webFrameLoadDelegate()
{
    return m_webFrameLoadDelegate;
}

void WebView::setJSActionDelegate(JSActionDelegate* newJSActionDelegate)
{
    m_jsActionDelegate = newJSActionDelegate;
}

JSActionDelegate* WebView::jsActionDelegate()
{
    return m_jsActionDelegate;
}

WebFrame* WebView::mainFrame()
{
    return m_mainFrame;
}

WebFrame* WebView::focusedFrame()
{
    Frame* f = m_page->focusController()->focusedFrame();
    if (!f)
        return 0;

    WebFrame* webFrame = kit(f);
    return webFrame;
}
WebBackForwardList* WebView::backForwardList()
{
    WebBackForwardListPrivate *p = new WebBackForwardListPrivate(m_page->backForwardList());
    return WebBackForwardList::createInstance(p);
}

void WebView::setMaintainsBackForwardList(bool flag)
{
    m_useBackForwardList = !!flag;
}

bool WebView::goBack()
{
    return m_page->goBack();
}

bool WebView::goForward()
{
    return m_page->goForward();
}

void WebView::goBackOrForward(int steps)
{
    m_page->goBackOrForward(steps);
}

bool WebView::goToBackForwardItem(WebHistoryItem* item)
{
    WebHistoryItemPrivate *priv = item->getPrivateItem(); 
    m_page->goToItem(priv->m_historyItem.get(), FrameLoadTypeIndexedBackForward);
    return true;
}

bool WebView::setTextSizeMultiplier(float multiplier)
{
    return setZoomMultiplier(multiplier, true);
}

bool WebView::setPageSizeMultiplier(float multiplier)
{
    return setZoomMultiplier(multiplier, false);
}

bool WebView::setZoomMultiplier(float multiplier, bool isTextOnly)
{
    // Check if we need to round the value.
    bool hasRounded = false;
    if (multiplier < cMinimumZoomMultiplier) {
        multiplier = cMinimumZoomMultiplier;
        hasRounded = true;
    } else if (multiplier > cMaximumZoomMultiplier) {
        multiplier = cMaximumZoomMultiplier;
        hasRounded = true;
    }

    m_zoomMultiplier = multiplier;
    m_page->settings()->setZoomsTextOnly(isTextOnly);
    Frame* coreFrame = core(m_mainFrame);
    if (coreFrame && coreFrame->document())
        coreFrame->setZoomFactor(multiplier, isTextOnly);

    return !hasRounded;
}

float WebView::textSizeMultiplier()
{
    return zoomMultiplier(true);
}

float WebView::pageSizeMultiplier()
{
    return zoomMultiplier(false);
}

float WebView::zoomMultiplier(bool isTextOnly)
{
    if (isTextOnly != m_page->settings()->zoomsTextOnly())
        return 1.0f;
    return m_zoomMultiplier;
}

bool WebView::zoomIn(bool isTextOnly)
{
    return setZoomMultiplier(zoomMultiplier(isTextOnly) * cZoomMultiplierRatio, isTextOnly);
}

bool WebView::zoomOut(bool isTextOnly)
{
    return setZoomMultiplier(zoomMultiplier(isTextOnly) / cZoomMultiplierRatio, isTextOnly);
}

bool WebView::zoomPageIn()
{
    return zoomIn(false);
}

bool WebView::zoomPageOut()
{
    return zoomOut(false);
}

bool WebView::canResetZoom(bool isTextOnly)
{
    return zoomMultiplier(isTextOnly) != 1.0f;
}

bool WebView::canResetPageZoom()
{
    return canResetZoom(false);
}

void WebView::resetPageZoom()
{
    return resetZoom(false);
}

void WebView::resetZoom(bool isTextOnly)
{
    if (!canResetZoom(isTextOnly))
        return ;
    setZoomMultiplier(1.0f, isTextOnly);
}

bool WebView::makeTextLarger()
{
    return zoomIn(m_page->settings()->zoomsTextOnly());
}

bool WebView::makeTextSmaller()
{
    return zoomOut(m_page->settings()->zoomsTextOnly());
}

bool WebView::canMakeTextStandardSize()
{
    // Since we always reset text zoom and page zoom together, this should continue to return an answer about text zoom even if its not enabled.
    return canResetZoom(true);
}

void WebView::makeTextStandardSize()
{
    resetZoom(true);
}



void WebView::setApplicationNameForUserAgent(const char* applicationName)
{
    m_applicationName = applicationName;
    m_userAgentStandard = "";
}

const char* WebView::applicationNameForUserAgent()
{
    return m_applicationName.c_str();
}

void WebView::setCustomUserAgent(const char* userAgentString)
{
    m_userAgentOverridden = true;
    m_userAgentCustom = string(userAgentString);
}

const char* WebView::customUserAgent()
{
    return m_userAgentCustom.c_str();
}

const char* WebView::userAgentForURL(const char* url)
{
    String ua = this->userAgentForKURL(url);
    printf("UA: %s", ua.latin1().data());
    return ua.utf8().data();
}

bool WebView::supportsTextEncoding()
{
    return true;
}

void WebView::setCustomTextEncodingName(const char* encodingName)
{
    if (!m_mainFrame)
        return ;

    String oldEncoding = customTextEncodingName();

    if (oldEncoding != encodingName) {
        if (Frame* coreFrame = core(m_mainFrame))
            coreFrame->loader()->reloadWithOverrideEncoding(encodingName);
    }
}

const char* WebView::customTextEncodingName()
{
    WebDataSource* dataSource;

    if (!m_mainFrame)
        return NULL;

    dataSource = m_mainFrame->provisionalDataSource();
    if (!dataSource) {
        dataSource = m_mainFrame->dataSource();
        if (!dataSource)
            return NULL;
    }

    return dataSource->documentLoader()->overrideEncoding().utf8().data();
}

void WebView::setMediaStyle(const char* /*media*/)
{
}

const char* WebView::mediaStyle()
{
    return NULL;
}

const char* WebView::stringByEvaluatingJavaScriptFromString(const char* script)
{
    Frame* coreFrame = core(m_mainFrame);
    if (!coreFrame)
        return NULL; 

    JSC::JSValue scriptExecutionResult = coreFrame->loader()->executeScript(script, false).jsValue();
    if(!scriptExecutionResult)
        return NULL;
    else if (scriptExecutionResult.isString())
        return scriptExecutionResult.getString().ascii();
    return NULL;
}

void WebView::executeScript(const char* script)
{
    if (!script)
        return;

    Frame* coreFrame = core(m_mainFrame);
    if (!coreFrame)
        return ;

    if (FrameLoader* loader = coreFrame->loader())
        loader->executeScript(String::fromUTF8(script), true);
}

WebScriptObject* WebView::windowScriptObject()
{
    return 0;
}

void WebView::setPreferences(WebPreferences* prefs)
{
    if (!prefs)
        prefs = WebPreferences::sharedStandardPreferences();

    if (m_preferences == prefs)
        return ;

    prefs->willAddToWebView();

    WebPreferences *oldPrefs = m_preferences;

    WebCore::ObserverServiceData::createObserverService()->removeObserver(WebPreferences::webPreferencesChangedNotification(), m_webViewObserver);

    String identifier = oldPrefs->identifier();
    oldPrefs->didRemoveFromWebView();
    oldPrefs = 0; // Make sure we release the reference, since WebPreferences::removeReferenceForIdentifier will check for last reference to WebPreferences
    
    WebPreferences::removeReferenceForIdentifier(identifier);

    m_preferences = prefs;

    WebCore::ObserverServiceData::createObserverService()->registerObserver(WebPreferences::webPreferencesChangedNotification(), m_webViewObserver);

    m_preferences->postPreferencesChangesNotification();
}

WebPreferences* WebView::preferences()
{
    return m_preferences;
}

void WebView::setPreferencesIdentifier(const char* /*anIdentifier*/)
{
}

const char* WebView::preferencesIdentifier()
{
    return "";
}

// void WebView::windowReceivedMessage(HWND, UINT message, WPARAM wParam, LPARAM)
// {
//     switch (message) {
//     case WM_NCACTIVATE:
//         updateActiveStateSoon();
//         if (!wParam)
//             deleteBackingStoreSoon();
//         break;
//     }
// }

void WebView::updateActiveStateSoon() const
{
//    SetTimer(m_viewWindow, UpdateActiveStateTimer, 0, 0);
}

void WebView::deleteBackingStoreSoon()
{
    m_deleteBackingStoreTimerActive = true;
//    SetTimer(m_viewWindow, DeleteBackingStoreTimer, delayBeforeDeletingBackingStoreMsec, 0);
}

void WebView::cancelDeleteBackingStoreSoon()
{
    if (!m_deleteBackingStoreTimerActive)
        return;
    m_deleteBackingStoreTimerActive = false;
//    KillTimer(m_viewWindow, DeleteBackingStoreTimer);
}

/*void WebView::setHostWindow(BalWidget *window)
{
    m_hostWindow = window;

    if (m_viewWindow)
        windowAncestryDidChange();
}

BalWidget* WebView::hostWindow()
{
    return m_hostWindow;
}*/


static Frame *incrementFrame(Frame *curr, bool forward, bool wrapFlag)
{
    return forward
        ? curr->tree()->traverseNextWithWrap(wrapFlag)
        : curr->tree()->traversePreviousWithWrap(wrapFlag);
}

bool WebView::searchFor(const char* str, bool forward, bool caseFlag, bool wrapFlag)
{
    if (!m_page || !m_page->mainFrame())
        return false;

    return m_page->findString(str, caseFlag ? TextCaseSensitive : TextCaseInsensitive, forward ? FindDirectionForward : FindDirectionBackward, wrapFlag);
}

bool WebView::active()
{
    /*HWND activeWindow = GetActiveWindow();
    return (activeWindow && m_topLevelParent == findTopLevelParent(activeWindow));*/
    return true;
}

static PassOwnPtr<Vector<String> > toStringVector(unsigned patternsCount, const char** patterns)
{
    // Convert the patterns into a Vector.
    if (patternsCount == 0)
        return 0;
    Vector<String>* patternsVector = new Vector<String>;
    for (unsigned i = 0; i < patternsCount; ++i)
        patternsVector->append(String(patterns[i]));

    return patternsVector;
}

bool WebView::addUserScriptToGroup(const char* groupName, unsigned worldID, const char* source, const char* url, unsigned whitelistCount, const char** whitelist, unsigned blacklistCount, const char** blacklist, WebUserScriptInjectionTime injectionTime)
{
    String group(groupName);
    if (group.isEmpty() || !worldID || worldID == numeric_limits<unsigned>::max())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;
    
    pageGroup->addUserScript(String(source),
                             KURL(KURL(), String(url)),
                             toStringVector(whitelistCount, whitelist),
                             toStringVector(blacklistCount, blacklist),
                             worldID,
                             injectionTime == WebUserScriptInjectAtDocumentStart ? InjectAtDocumentStart : InjectAtDocumentEnd);
    return true;
}

bool WebView::addUserStyleSheetToGroup(const char* groupName, unsigned worldID, const char* source, const char* url, unsigned whitelistCount, const char** whitelist, unsigned blacklistCount, const char** blacklist)
{
    String group(groupName);
    if (group.isEmpty() || !worldID || worldID == numeric_limits<unsigned>::max())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;
 
    pageGroup->addUserStyleSheet(String(source),
                                 KURL(KURL(), String(url)),
                                 toStringVector(whitelistCount, whitelist),
                                 toStringVector(blacklistCount, blacklist),
                                 worldID);    
    return true;
}

bool WebView::removeUserContentWithURLFromGroup(const char* groupName, unsigned worldID, const char* url)
{
    String group(groupName);
    if (group.isEmpty() || !worldID || worldID == numeric_limits<unsigned>::max())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;

    pageGroup->removeUserContentWithURLForWorld(KURL(KURL(), url), worldID);
    return true;
}

bool WebView::removeUserContentFromGroup(const char* groupName, unsigned worldID)
{
    String group(groupName);
    if (group.isEmpty() || !worldID || worldID == numeric_limits<unsigned>::max())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;

    pageGroup->removeUserContentForWorld(worldID);
    return true;
}

bool WebView::removeAllUserContentFromGroup(const char* groupName)
{
    String group(groupName);
    if (group.isEmpty())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;

    pageGroup->removeAllUserContent();
    return true;
}

void WebView::updateActiveState()
{
    m_page->focusController()->setActive(active());
}

void WebView::updateFocusedAndActiveState()
{
    updateActiveState();

    bool active = m_page->focusController()->isActive();
    Frame* mainFrame = m_page->mainFrame();
    Frame* focusedFrame = m_page->focusController()->focusedOrMainFrame();
    mainFrame->selection()->setFocused(mainFrame);
}

String buffer(const char* url)
{
    String path;
    if (!url) {
        path = OWB_DATA;
        path +=  "owbrc";
    } else
        path = url;

    File *configFile = new File(path);
    if (!configFile)
        return String();
    if (configFile->open('r') == -1) {
        delete configFile;
        return String();
    }
    String buffer(configFile->read(configFile->getSize()));
    configFile->close();
    delete configFile;

    return buffer;
}

void WebView::parseConfigFile(const char* url)
{
    int width = 0, height = 0;
    String fileBuffer = buffer(url);
    while (!fileBuffer.isEmpty()) {
        int eol = fileBuffer.find("\n");
        int delimiter = fileBuffer.find("=");

        String keyword = fileBuffer.substring(0, delimiter).stripWhiteSpace();
        String key = fileBuffer.substring(delimiter +  1, eol - delimiter).stripWhiteSpace();

        if (keyword == "width")
            width = key.toInt();
        if (keyword == "height")
            height = key.toInt();

        //Remove processed line from the buffer
        String truncatedBuffer = fileBuffer.substring(eol + 1, fileBuffer.length() - eol - 1);
        fileBuffer = truncatedBuffer;
    }

    if (width > 0 && height > 0)
        d->setFrameRect(IntRect(0, 0, width, height));
}


void WebView::executeCoreCommandByName(const char* name, const char* value)
{
    m_page->focusController()->focusedOrMainFrame()->editor()->command(name).execute(value);
}

unsigned int WebView::markAllMatchesForText(const char* str, bool caseSensitive, bool highlight, unsigned int limit)
{
    if (!m_page || !m_page->mainFrame())
        return 0;

    return m_page->markAllMatchesForText(str, caseSensitive ? TextCaseSensitive : TextCaseInsensitive, highlight, limit);
}

void WebView::unmarkAllTextMatches()
{
    if (!m_page || !m_page->mainFrame())
        return ;

    m_page->unmarkAllTextMatches();
}

/*Vector<IntRect> WebView::rectsForTextMatches()
{
    Vector<IntRect> allRects;
    WebCore::Frame* frame = m_page->mainFrame();
    do {
        if (Document* document = frame->document()) {
            IntRect visibleRect = enclosingIntRect(frame->view()->visibleContentRect());
            Vector<IntRect> frameRects = document->renderedRectsForMarkers(DocumentMarker::TextMatch);
            IntPoint frameOffset(-frame->view()->scrollOffset().width(), -frame->view()->scrollOffset().height());
            frameOffset = frame->view()->convertToContainingWindow(frameOffset);

            Vector<IntRect>::iterator end = frameRects.end();
            for (Vector<IntRect>::iterator it = frameRects.begin(); it < end; it++) {
                it->intersect(visibleRect);
                it->move(frameOffset.x(), frameOffset.y());
                allRects.append(*it);
            }
        }
        frame = incrementFrame(frame, true, false);
    } while (frame);

    return allRects;
}*/

/*WebCore::Image* WebView::generateSelectionImage(bool forceWhiteText)
{
    WebCore::Frame* frame = m_page->focusController()->focusedOrMainFrame();

    if (frame) {*/
        /*HBITMAP bitmap = imageFromSelection(frame, forceWhiteText ? TRUE : FALSE);
        *hBitmap = (OLE_HANDLE)(ULONG64)bitmap;*/
/*    }

    return 0;
}*/

BalRectangle WebView::selectionRect()
{
    WebCore::Frame* frame = m_page->focusController()->focusedOrMainFrame();

    if (frame) {
        IntRect ir = enclosingIntRect(frame->selectionBounds());
        ir = frame->view()->convertToContainingWindow(ir);
        ir.move(-frame->view()->scrollOffset().width(), -frame->view()->scrollOffset().height());
        return ir;
    }

    return IntRect();
}

void WebView::setGroupName(const char* groupName)
{
    m_page->setGroupName(groupName);
}
    
const char* WebView::groupName()
{
    // We need to duplicate the translated string.
    CString groupName = m_page->groupName().utf8();
    return strdup(groupName.data());
}
    
double WebView::estimatedProgress()
{
    return m_page->progress()->estimatedProgress();
}
    
bool WebView::isLoading()
{
    WebDataSource* dataSource = m_mainFrame->dataSource();

    bool isLoading = false;
    if (dataSource)
        isLoading = dataSource->isLoading();

    if (isLoading)
        return true;

    WebDataSource* provisionalDataSource = m_mainFrame->provisionalDataSource();
    if (provisionalDataSource)
        isLoading = provisionalDataSource->isLoading();
    return isLoading;
}

WebElementPropertyBag* WebView::elementAtPoint(BalPoint& point)
{
    Frame* frame = core(m_mainFrame);
    if (!frame)
        return 0;

    IntPoint webCorePoint(point);
    HitTestResult result = HitTestResult(webCorePoint);
    if (frame->contentRenderer())
        result = frame->eventHandler()->hitTestResultAtPoint(webCorePoint, false);
    return WebElementPropertyBag::createInstance(result);
}

const char* WebView::selectedText()
{
    Frame* focusedFrame = (m_page && m_page->focusController()) ? m_page->focusController()->focusedOrMainFrame() : 0;
    if (!focusedFrame)
        return NULL;

    return focusedFrame->selectedText().utf8().data();
}

void WebView::centerSelectionInVisibleArea()
{
    Frame* coreFrame = core(m_mainFrame);
    if (!coreFrame)
        return ;

    coreFrame->revealSelection(ScrollAlignment::alignCenterAlways);
}


void WebView::moveDragCaretToPoint(BalPoint& /*point*/)
{
}

void WebView::removeDragCaret()
{
}

void WebView::setDrawsBackground(bool /*drawsBackground*/)
{
}

bool WebView::drawsBackground()
{
    return false;
}

void WebView::setMainFrameURL(const char* /*urlString*/)
{
}

const char* WebView::mainFrameURL()
{
    return m_mainFrame->url();
}

DOMDocument* WebView::mainFrameDocument()
{
    return m_mainFrame->domDocument();
}

const char* WebView::mainFrameTitle()
{
    return m_mainFrame->name();
}

void WebView::registerURLSchemeAsLocal(const char* scheme)
{
    SecurityOrigin::registerURLSchemeAsLocal(scheme);
}

void WebView::setSmartInsertDeleteEnabled(bool flag)
{
    m_smartInsertDeleteEnabled = !!flag;
    if (m_smartInsertDeleteEnabled)
        setSelectTrailingWhitespaceEnabled(false);
}
    
bool WebView::smartInsertDeleteEnabled()
{
    return m_smartInsertDeleteEnabled ? true : false;
}

void WebView::setSelectTrailingWhitespaceEnabled(bool flag)
{
    m_selectTrailingWhitespaceEnabled = !!flag;
    if (m_selectTrailingWhitespaceEnabled)
        setSmartInsertDeleteEnabled(false);
}

bool WebView::isSelectTrailingWhitespaceEnabled()
{
    return m_selectTrailingWhitespaceEnabled;
}

void WebView::setContinuousSpellCheckingEnabled(bool flag)
{
    if (continuousSpellCheckingEnabled != !!flag) {
        continuousSpellCheckingEnabled = !!flag;
        WebPreferences* prefs = preferences();
        if (prefs)
            prefs->setContinuousSpellCheckingEnabled(flag);
    }
    
    bool spellCheckingEnabled = isContinuousSpellCheckingEnabled();
    if (!spellCheckingEnabled)
        m_mainFrame->unmarkAllMisspellings();
    /*else
        preflightSpellChecker();*/
}

bool WebView::isContinuousSpellCheckingEnabled()
{
    return (continuousSpellCheckingEnabled && continuousCheckingAllowed()) ? true : false;
}

int WebView::spellCheckerDocumentTag()
{
    // we just use this as a flag to indicate that we've spell checked the document
    // and need to close the spell checker out when the view closes.
    m_hasSpellCheckerDocumentTag = true;
    return 0;
}

bool WebView::continuousCheckingAllowed()
{
    static bool allowContinuousSpellChecking = true;
    static bool readAllowContinuousSpellCheckingDefault = false;
    if (!readAllowContinuousSpellCheckingDefault) {
        WebPreferences* prefs = preferences();
        if (prefs) {
            bool allowed = prefs->allowContinuousSpellChecking();
            allowContinuousSpellChecking = !!allowed;
        }
        readAllowContinuousSpellCheckingDefault = true;
    }
    return allowContinuousSpellChecking;
}

bool WebView::hasSelectedRange()
{
    return m_page->mainFrame()->selection()->isRange();
}

bool WebView::cutEnabled()
{
    Editor* editor = m_page->focusController()->focusedOrMainFrame()->editor();
    return editor->canCut() || editor->canDHTMLCut();
}

bool WebView::copyEnabled()
{
    Editor* editor = m_page->focusController()->focusedOrMainFrame()->editor();
    return editor->canCopy() || editor->canDHTMLCopy();
}

bool WebView::pasteEnabled()
{
    Editor* editor = m_page->focusController()->focusedOrMainFrame()->editor();
    return editor->canPaste() || editor->canDHTMLPaste();
}

bool WebView::deleteEnabled()
{
    return m_page->focusController()->focusedOrMainFrame()->editor()->canDelete();
}

bool WebView::editingEnabled()
{
    return m_page->focusController()->focusedOrMainFrame()->editor()->canEdit();
}

bool WebView::isGrammarCheckingEnabled()
{
    return grammarCheckingEnabled ? true : false;
}

void WebView::setGrammarCheckingEnabled(bool enabled)
{
    if (grammarCheckingEnabled == !!enabled)
        return ;

    grammarCheckingEnabled = !!enabled;
    WebPreferences* prefs = preferences();
    if (prefs)
        prefs->setGrammarCheckingEnabled(enabled);

    // We call _preflightSpellChecker when turning continuous spell checking on, but we don't need to do that here
    // because grammar checking only occurs on code paths that already preflight spell checking appropriately.

    bool grammarEnabled = isGrammarCheckingEnabled();
    if (!grammarEnabled)
        m_mainFrame->unmarkAllBadGrammar();
}

void WebView::replaceSelectionWithText(const char* text)
{
    Position start = m_page->mainFrame()->selection()->selection().start();
    m_page->focusController()->focusedOrMainFrame()->editor()->insertText(text, 0);
    m_page->mainFrame()->selection()->setBase(start);
}
    
void WebView::replaceSelectionWithMarkupString(const char* /*markupString*/)
{
}
    
void WebView::replaceSelectionWithArchive(WebArchive* /*archive*/)
{
}
    
void WebView::deleteSelection()
{
    Editor* editor = m_page->focusController()->focusedOrMainFrame()->editor();
    editor->deleteSelectionWithSmartDelete(editor->canSmartCopyOrDelete());
}

void WebView::clearSelection()
{
    m_page->focusController()->focusedOrMainFrame()->selection()->clear();
}

void WebView::clearMainFrameName()
{
    m_page->mainFrame()->tree()->clearName();
}

void WebView::copy()
{
    m_page->focusController()->focusedOrMainFrame()->editor()->command("Copy").execute();
}

void WebView::cut()
{
    m_page->focusController()->focusedOrMainFrame()->editor()->command("Cut").execute();
}

void WebView::paste()
{
    m_page->focusController()->focusedOrMainFrame()->editor()->command("Paste").execute();
}

void WebView::copyURL(const char* url)
{
    m_page->focusController()->focusedOrMainFrame()->editor()->copyURL(KURL(ParsedURLString, url), "");
}


void WebView::copyFont()
{
}
    
void WebView::pasteFont()
{
}
    
void WebView::delete_()
{
    m_page->focusController()->focusedOrMainFrame()->editor()->command("Delete").execute();
}
    
void WebView::pasteAsPlainText()
{
}
    
void WebView::pasteAsRichText()
{
}
    
void WebView::changeFont()
{
}
    
void WebView::changeAttributes()
{
}
    
void WebView::changeDocumentBackgroundColor()
{
}
    
void WebView::changeColor()
{
}
    
void WebView::alignCenter()
{
}
    
void WebView::alignJustified()
{
}
    
void WebView::alignLeft()
{
}
    
void WebView::alignRight()
{
}
    
void WebView::checkSpelling()
{
    core(m_mainFrame)->editor()->advanceToNextMisspelling();
}
    
void WebView::showGuessPanel()
{
    core(m_mainFrame)->editor()->advanceToNextMisspelling(true);
    //m_editingDelegate->showSpellingUI(TRUE);
}
    
void WebView::performFindPanelAction()
{
}
    
void WebView::startSpeaking()
{
}
    
void WebView::stopSpeaking()
{
}

/*void WebView::observe(const WebCore::String &topic, const WebCore::String &data, void *userData)
{
#if ENABLE(ICONDATABASE)
    if (topic == WebIconDatabase::iconDatabaseDidAddIconNotification())
        notifyDidAddIcon();
#endif

    if (topic == WebPreferences::webPreferencesChangedNotification())
        notifyPreferencesChanged(static_cast<WebPreferences*>(userData));

    if (topic == "PopupMenuHide")
        d->popupMenuHide();
    if (topic == "PopupMenuShow") {
        d->popupMenuShow(userData);
    }

}*/

void WebView::notifyPreferencesChanged(WebPreferences* preferences)
{
    ASSERT(preferences == m_preferences);

    String str;
    int size;
    bool enabled;

    Settings* settings = m_page->settings();

    str = preferences->cursiveFontFamily();
    settings->setCursiveFontFamily(str);

    size = preferences->defaultFixedFontSize();
    settings->setDefaultFixedFontSize(size);

    size = preferences->defaultFontSize();
    settings->setDefaultFontSize(size);

    str = preferences->defaultTextEncodingName();
    settings->setDefaultTextEncodingName(str);

    str = preferences->fantasyFontFamily();
    settings->setFantasyFontFamily(str);

    str = preferences->fixedFontFamily();
    settings->setFixedFontFamily(str);

    enabled = preferences->isJavaEnabled();
    settings->setJavaEnabled(!!enabled);

    enabled = preferences->isJavaScriptEnabled();
    settings->setJavaScriptEnabled(!!enabled);

    enabled = preferences->javaScriptCanOpenWindowsAutomatically();
    settings->setJavaScriptCanOpenWindowsAutomatically(!!enabled);

    size = preferences->minimumFontSize();
    settings->setMinimumFontSize(size);

    size = preferences->minimumLogicalFontSize();
    settings->setMinimumLogicalFontSize(size);

    enabled = preferences->arePlugInsEnabled();
    settings->setPluginsEnabled(!!enabled);

    enabled = preferences->privateBrowsingEnabled();
    settings->setPrivateBrowsingEnabled(!!enabled);

    str = preferences->sansSerifFontFamily();
    settings->setSansSerifFontFamily(str);

    str = preferences->serifFontFamily();
    settings->setSerifFontFamily(str);

    str = preferences->standardFontFamily();
    settings->setStandardFontFamily(str);

    enabled = preferences->loadsImagesAutomatically();
    settings->setLoadsImagesAutomatically(!!enabled);

    enabled = preferences->userStyleSheetEnabled();
    if (enabled) {
        str = preferences->userStyleSheetLocation();
        settings->setUserStyleSheetLocation(KURL(ParsedURLString, str));
    } else {
        settings->setUserStyleSheetLocation(KURL());
    }

    enabled = preferences->shouldPrintBackgrounds();
    settings->setShouldPrintBackgrounds(!!enabled);

    enabled = preferences->textAreasAreResizable();
    settings->setTextAreasAreResizable(!!enabled);

    WebKitEditableLinkBehavior behavior = preferences->editableLinkBehavior();
    settings->setEditableLinkBehavior((EditableLinkBehavior)behavior);

    enabled = preferences->usesPageCache();
    settings->setUsesPageCache(!!enabled);

    enabled = preferences->isDOMPasteAllowed();
    settings->setDOMPasteAllowed(!!enabled);

    enabled = preferences->shouldPaintCustomScrollbars();
    settings->setShouldPaintCustomScrollbars(!!enabled);

    enabled = preferences->zoomsTextOnly();
    settings->setZoomsTextOnly(!!enabled);

#if PLATFORM(AMIGAOS4)
    settings->setShowsURLsInToolTips(true);
#else
    settings->setShowsURLsInToolTips(false);
#endif
    settings->setForceFTPDirectoryListings(true);
    settings->setDeveloperExtrasEnabled(developerExtrasEnabled());
    settings->setNeedsSiteSpecificQuirks(s_allowSiteSpecificHacks);

    FontSmoothingType smoothingType = preferences->fontSmoothing();
    settings->setFontRenderingMode(smoothingType != FontSmoothingTypeWindows ? NormalRenderingMode : AlternateRenderingMode);

    enabled = preferences->authorAndUserStylesEnabled();
    settings->setAuthorAndUserStylesEnabled(enabled);

    enabled = preferences->inApplicationChromeMode();
    settings->setApplicationChromeMode(!!enabled);

    enabled = preferences->offlineWebApplicationCacheEnabled();
    settings->setOfflineWebApplicationCacheEnabled(enabled);

    enabled = preferences->databasesEnabled();
    settings->setDatabasesEnabled(enabled);

    enabled = preferences->experimentalNotificationsEnabled();
    settings->setExperimentalNotificationsEnabled(enabled);

#if ENABLE(WEB_SOCKETS)
    enabled = preferences->experimentalWebSocketsEnabled();
    settings->setExperimentalWebSocketsEnabled(enabled);
#endif

    enabled = preferences->isWebSecurityEnabled();
    settings->setWebSecurityEnabled(enabled);

    enabled = preferences->allowUniversalAccessFromFileURLs();
    settings->setAllowUniversalAccessFromFileURLs(!!enabled);

    enabled = preferences->isXSSAuditorEnabled();
    settings->setXSSAuditorEnabled(!!enabled);

    // Only use on windows
/*    enabled = preferences->shouldUseHighResolutionTimers();
    settings->setShouldUseHighResolutionTimers(enabled);*/

    enabled = preferences->pluginHalterEnabled();
    settings->setPluginHalterEnabled(enabled);

    enabled = preferences->pluginAllowedRunTime();
    settings->setPluginAllowedRunTime(enabled);

#if ENABLE(3D_CANVAS)
    settings->setExperimentalWebGLEnabled(true);
#endif

/*    if (!m_closeWindowTimer.isActive())
        m_mainFrame->invalidate(); // FIXME*/

    updateSharedSettingsFromPreferencesIfNeeded(preferences);
}

void updateSharedSettingsFromPreferencesIfNeeded(WebPreferences* preferences)
{
    if (preferences != WebPreferences::sharedStandardPreferences())
        return ;

    WebKitCookieStorageAcceptPolicy acceptPolicy = preferences->cookieStorageAcceptPolicy();
}

/*HRESULT STDMETHODCALLTYPE WebView::removeCustomDropTarget()
{
    if (!m_hasCustomDropTarget)
        return S_OK;
    m_hasCustomDropTarget = false;
    revokeDragDrop();
    return registerDragDrop();
}*/

void WebView::setInViewSourceMode(bool flag)
{
    if (!m_mainFrame)
        return ;

    return m_mainFrame->setInViewSourceMode(flag);
}
    
bool WebView::inViewSourceMode()
{
    if (!m_mainFrame)
        return false;

    return m_mainFrame->inViewSourceMode();
}

BalWidget* WebView::viewWindow()
{
    return m_viewWindow;
}

void WebView::setViewWindow(BalWidget* view)
{
    m_viewWindow = view;
}

BalPoint WebView::scrollOffset()
{
    IntSize offsetIntSize = m_page->mainFrame()->view()->scrollOffset();
    return IntPoint(offsetIntSize.width(), offsetIntSize.height());
}

void WebView::scrollBy(BalPoint offset)
{
    //FIXME: implement BalPoint
    IntPoint p(offset);
    m_page->mainFrame()->view()->scrollBy(IntSize(p.x(), p.y()));
}

BalRectangle WebView::visibleContentRect()
{
    FloatRect visibleContent = m_page->mainFrame()->view()->visibleContentRect();
    return IntRect((int)visibleContent.x(), (int)visibleContent.y(), (int)visibleContent.right(), (int)visibleContent.bottom());
}


bool WebView::canHandleRequest(WebMutableURLRequest *request)
{
    return !!canHandleRequest(request->resourceRequest());
}

void WebView::clearFocusNode()
{
    if (m_page && m_page->focusController())
        m_page->focusController()->setFocusedNode(0, 0);
}

void WebView::setInitialFocus(bool forward)
{
    if (m_page && m_page->focusController()) {
        Frame* frame = m_page->focusController()->focusedOrMainFrame();
        frame->document()->setFocusedNode(0);
        m_page->focusController()->setInitialFocus(forward ? FocusDirectionForward : FocusDirectionBackward, 0);
    }
}

void WebView::setTabKeyCyclesThroughElements(bool cycles)
{
    if (m_page)
        m_page->setTabKeyCyclesThroughElements(!!cycles);
}

bool WebView::tabKeyCyclesThroughElements()
{
    return m_page && m_page->tabKeyCyclesThroughElements() ? true : false;
}

void WebView::setAllowSiteSpecificHacks(bool allow)
{
    s_allowSiteSpecificHacks = !!allow;
    // FIXME: This sets a global so it needs to call notifyPreferencesChanged
    // on all WebView objects (not just itself).
}

void WebView::addAdditionalPluginDirectory(const char* directory)
{
    PluginDatabase::installedPlugins()->addExtraPluginDirectory(directory);
}

void WebView::loadBackForwardListFromOtherView(WebView* otherView)
{
    if (!m_page)
        return ;
    
    // It turns out the right combination of behavior is done with the back/forward load
    // type.  (See behavior matrix at the top of WebFramePrivate.)  So we copy all the items
    // in the back forward list, and go to the current one.
    BackForwardList* backForwardList = m_page->backForwardList();
    ASSERT(!backForwardList->currentItem()); // destination list should be empty

    BackForwardList* otherBackForwardList = otherView->m_page->backForwardList();
    if (!otherBackForwardList->currentItem())
        return ; // empty back forward list, bail
    
    HistoryItem* newItemToGoTo = 0;

    int lastItemIndex = otherBackForwardList->forwardListCount();
    for (int i = -otherBackForwardList->backListCount(); i <= lastItemIndex; ++i) {
        if (!i) {
            // If this item is showing , save away its current scroll and form state,
            // since that might have changed since loading and it is normally not saved
            // until we leave that page.
            otherView->m_page->mainFrame()->loader()->saveDocumentAndScrollState();
        }
        RefPtr<HistoryItem> newItem = otherBackForwardList->itemAtIndex(i)->copy();
        if (!i) 
            newItemToGoTo = newItem.get();
        backForwardList->addItem(newItem.release());
    }
    
    ASSERT(newItemToGoTo);
    m_page->goToItem(newItemToGoTo, FrameLoadTypeIndexedBackForward);
}

void WebView::clearUndoRedoOperations()
{
    if (Frame* frame = m_page->focusController()->focusedOrMainFrame())
        frame->editor()->clearUndoRedoOperations();
}

bool WebView::shouldClose()
{
    if (Frame* frame = m_page->focusController()->focusedOrMainFrame())
        return frame->shouldClose() ? true : false;
    return true;
}

void WebView::setProhibitsMainFrameScrolling(bool b)
{
    if (!m_page)
        return ;

    m_page->mainFrame()->view()->setProhibitsScrolling(b);
}

void WebView::setShouldApplyMacFontAscentHack(bool b)
{
    //SimpleFontData::setShouldApplyMacAscentHack(b);
}


void WebView::windowAncestryDidChange()
{
    updateActiveState();
}

/*void WebView::paintDocumentRectToContext(IntRect rect, PlatformGraphicsContext *pgc)
{
    Frame* frame = m_page->mainFrame();
    if (!frame)
        return ;

    FrameView* view = frame->view();
    if (!view)
        return ;

    // We can't paint with a layout still pending.
    view->layoutIfNeededRecursive();

    GraphicsContext gc(pgc);
    gc.save();
    int width = rect.width();
    int height = rect.height();
    FloatRect dirtyRect;
    dirtyRect.setWidth(width);
    dirtyRect.setHeight(height);
    gc.clip(dirtyRect);
    gc.translate(-rect.x(), -rect.y());
    view->paint(&gc, rect);
    gc.restore();
}*/

void WebView::setCustomHTMLTokenizerTimeDelay(double timeDelay)
{
    if (!m_page)
        return;

    m_page->setCustomHTMLTokenizerTimeDelay(timeDelay);
}

void WebView::setCustomHTMLTokenizerChunkSize(int chunkSize)
{
    if (!m_page)
        return;

    m_page->setCustomHTMLTokenizerChunkSize(chunkSize);
}

/*WebCore::Image* WebView::backingStore()
{
    WebCore::Image* hBitmap = m_backingStoreBitmap.get();
    return hBitmap;
}*/

void WebView::setTransparent(bool transparent)
{
    if (m_transparent == !!transparent)
        return;

    m_transparent = transparent;
    m_mainFrame->updateBackground();
}

void WebView::transparent(bool* transparent)
{
    if (!transparent)
        return;

    *transparent = this->transparent() ? true : false;
}

void WebView::setMediaVolume(float volume)                                                        
{                                                                                                                      
    if (!m_page)                                                                                                       
        return ; 
                                                                                                                
    m_page->setMediaVolume(volume);                                                                                    
}                                                                                                                      
                                                                                                                       
float WebView::mediaVolume()
{                                                                                                                      
    if (!m_page)                                                                                                       
        return 0.0;
                                                                                                                       
    return m_page->mediaVolume();                                                                                   
}  

void WebView::setMemoryCacheDelegateCallsEnabled(bool enabled)
{
    m_page->setMemoryCacheClientCallsEnabled(enabled);
}

void WebView::setDefersCallbacks(bool defersCallbacks)
{
    if (!m_page)
        return;

    m_page->setDefersLoading(defersCallbacks);
}

bool WebView::defersCallbacks()
{
    if (!m_page)
        return false;

    return m_page->defersLoading();
}

void WebView::popupMenuHide()
{
    d->popupMenuHide();
}

void WebView::popupMenuShow(void* userData)
{
    d->popupMenuShow(userData);
}

void WebView::fireWebKitEvents()
{
    d->fireWebKitEvents();
}

void WebView::setJavaScriptURLsAreAllowed(bool areAllowed)
{
    m_page->setJavaScriptURLsAreAllowed(areAllowed);
}

Page* core(WebView* webView)
{
    Page* page = 0;

    page = webView->page();

    return page;
}

bool WebView::invalidateBackingStore(const WebCore::IntRect* rect)
{
    notImplemented();
    return false;
/*
    if (!IsWindow(m_viewWindow))
        return S_OK;

    RECT clientRect;
    if (!GetClientRect(m_viewWindow, &clientRect))
        return E_FAIL;

    RECT rectToInvalidate;
    if (!rect)
        rectToInvalidate = clientRect;
    else if (!IntersectRect(&rectToInvalidate, &clientRect, rect))
        return S_OK;

    repaint(rectToInvalidate, true);
    return S_OK;
*/
}
