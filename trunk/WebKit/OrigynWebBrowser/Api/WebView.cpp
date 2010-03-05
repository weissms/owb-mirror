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

#include "BALBase.h"
#include "PtrAndFlags.h"
#include "DefaultPolicyDelegate.h"
#include "DOMCoreClasses.h"
#include "JSActionDelegate.h"
#include "WebBackForwardList.h"
#include "WebBackForwardList_p.h"
#include "WebBindingJSDelegate.h"
#include "WebChromeClient.h"
#include "WebContextMenuClient.h"
#include "WebDatabaseManager.h"
#include "WebDocumentLoader.h"
#include "WebDownloadDelegate.h"
#include "WebDragClient.h"
#include "WebDragData.h"
#include "WebDragData_p.h"
#include "WebEditingDelegate.h"
#include "WebEditorClient.h"
#include "WebFrame.h"
#include "WebFrameLoadDelegate.h"
#include "WebGeolocationControllerClient.h"
#include "WebGeolocationPosition.h"
#include "WebGeolocationProvider.h"
#include "WebHistoryDelegate.h"
#include "WebHitTestResults.h"
#include "WebHistoryItem.h"
#include "WebHistoryItem_p.h"
#if ENABLE(ICONDATABASE)
#include "WebIconDatabase.h"
#endif
#include "WebKitVersion.h"
#if ENABLE(INSPECTOR)
#include "WebInspector.h"
#include "WebInspectorClient.h"
#endif
#include "WebMutableURLRequest.h"
#include "WebNotificationDelegate.h"
#include "WebPluginHalterClient.h"
#include "WebPluginHalterDelegate.h"
#include "WebPreferences.h"
#include "WebResourceLoadDelegate.h"
#include "WebScriptWorld.h"
#include "WebViewPrivate.h"
#include "WebWidgetEngineDelegate.h"
#include "WebWindow.h"

#if ENABLE(OFFLINE_WEB_APPLICATIONS)
#include <ApplicationCacheStorage.h>
#endif
#if HAVE(ACCESSIBILITY)
#include <AXObjectCache.h>
#endif
#include "ObserverData.h"
#include "ObserverServiceData.h"
#include "KeyboardCodes.h"
#include <PlatformString.h>
#include <Cache.h>
#include <Chrome.h>
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
#if ENABLE(CLIENT_BASED_GEOLOCATION)
#include <GeolocationController.h>
#include <GeolocationError.h>
#endif
#include <GraphicsContext.h>
#include <HistoryItem.h>
#include <HitTestResult.h>
#include <IntPoint.h>
#include <IntRect.h>
#include <KeyboardEvent.h>
#include <Logging.h>
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
#include <RenderWidget.h>
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
#include <JSLock.h>
#include <JSValue.h>
#include <wtf/unicode/Encoding.h>
#include <wtf/HashSet.h>
#include "InitializeThreading.h"

#include "owb-config.h"
#include "FileIO.h"

#if OS(MACPORT)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif OS(AMIGAOS4)
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/layout.h>
#include <intuition/gadgetclass.h>
#else
#include <sys/sysinfo.h>
#endif

#if ENABLE(CEHTML)
#include "CEHTMLUserAgent.h"
#include "HTMLInputElement.h"
#include "HTMLNames.h"
#endif

#if ENABLE(DAE)
#include "DAEUserAgent.h"
#endif

#if ENABLE(DAE_APPLICATION)
#include "Application.h"
#include "ApplicationManager.h"
#include "ApplicationPrivateData.h"
#include "Keyset.h"
#include "Page.h"
#endif

using namespace WebCore;
using std::min;
using std::max;

#define HOST_NAME_BUFFER_LENGTH 2048

#if OS(MACPORT)
#define UA_PLATFORM     "Macintosh"
#elif OS(AMIGAOS4)
#define UA_PLATFORM     "compatible"
#else
#define UA_PLATFORM     "X11"
#endif

#if OS(MACPORT)
#define UA_OS           "Intel Mac OS X"
#elif OS(AMIGAOS4)
#define UA_OS           ((IExec->Data.LibBase->lib_Version < 53) ? "AmigaOS 4.0 ppc" : "AmigaOS 4.1 ppc")
#else
#define UA_OS           "Linux i686"
#endif

#define UA_SECURITY     "U"
#define UA_LOCALE       "en-US"

WebView* kit(Page* page)
{
    return page ? static_cast<WebChromeClient*>(page->chrome()->client())->webView() : 0;
}

class MemoryEvent : public WTF::MemoryNotification {
public:
    MemoryEvent(WebView* webView) { m_webView = webView; }

    virtual void call()
    {
        if (!m_webView->isStopped()) {
            m_webView->stopLoading(true);
#if ENABLE(DAE_APPLICATION)
            if (webAppMgr().application(m_webView))
                webAppMgr().application(m_webView)->dispatchApplicationEvent(eventNames().LowMemoryEvent);
#endif
            SharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate = m_webView->webFrameLoadDelegate();
            if (webFrameLoadDelegate)
                webFrameLoadDelegate->dispatchNotEnoughtMemory(m_webView->mainFrame());
        }
    }

private:
    WebView* m_webView;
};

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
        WebView::setCacheModel(sharedPreferencesCacheModel);
    }
}

void PreferencesChangedOrRemovedObserver::notifyPreferencesRemoved(WebCacheModel cacheModel)
{
    if (cacheModel == WebView::cacheModel()) {
        WebCacheModel sharedPreferencesCacheModel = WebPreferences::sharedStandardPreferences()->cacheModel();
        WebView::setCacheModel(sharedPreferencesCacheModel);
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
    , m_webEditingDelegate(0)
    , m_webResourceLoadDelegate(0)
    , m_webBindingJSDelegate(0)
    , m_webWidgetEngineDelegate(0)
    , m_pluginHalterDelegate(0)
    , m_historyDelegate(0)
    , m_geolocationProvider(0)
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
    , m_topLevelParent(0)
    , d(new WebViewPrivate(this))
    , m_webViewObserver(new WebViewObserver(this))
#if ENABLE(CEHTML)
    , m_previousDownEventCalledDefaultHandler(false)
#endif
#if ENABLE(INSPECTOR)
    , m_webInspector(0)
#endif
    , m_toolbarsVisible(true)
    , m_statusbarVisible(true)
    , m_menubarVisible(true)
    , m_locationbarVisible(true)
    , m_inputState(false)
    , m_dragTargetDispatch(false)
    , m_dragIdentity(0)
    , m_dropEffect(DropEffectDefault)
    , m_operationsAllowed(WebDragOperationNone)
    , m_dragOperation(WebDragOperationNone)
    , m_currentDragData(0)
{
    JSC::initializeThreading();
    WebCore::InitializeLoggingChannelsIfNecessary();

    d->clearDirtyRegion();

    initializeStaticObservers();

    WebCore::ObserverServiceData::createObserverService()->registerObserver("PopupMenuShow", m_webViewObserver);
    WebCore::ObserverServiceData::createObserverService()->registerObserver("PopupMenuHide", m_webViewObserver);

    WebPreferences* sharedPreferences = WebPreferences::sharedStandardPreferences();
    continuousSpellCheckingEnabled = sharedPreferences->continuousSpellCheckingEnabled();
    grammarCheckingEnabled = sharedPreferences->grammarCheckingEnabled();
    sharedPreferences->willAddToWebView();
    m_preferences = sharedPreferences;

#if ENABLE(CLIENT_BASED_GEOLOCATION)
    WebGeolocationControllerClient* geolocationControllerClient = new WebGeolocationControllerClient(this);
#else
    WebGeolocationControllerClient* geolocationControllerClient = 0;
#endif

    m_page = new Page(new WebChromeClient(this),
                      new WebContextMenuClient(this),
                      new WebEditorClient(this),
                      new WebDragClient(this),
#if ENABLE(INSPECTOR)
                      new WebInspectorClient(this),
#else
                      0,
#endif
                      new WebPluginHalterClient(this),
                      geolocationControllerClient);

    m_mainFrame = WebFrame::createInstance();
    m_mainFrame->init(this, m_page, 0);
    m_memoryEvent = new MemoryEvent(this);
    WTF::setMemoryNotificationCallback(m_memoryEvent);
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
    if (m_pluginHalterDelegate)
        m_pluginHalterDelegate = 0;
    if (m_webFrameLoadDelegate)
        m_webFrameLoadDelegate = 0;
    if (m_jsActionDelegate)
        m_jsActionDelegate = 0;
    if (m_historyDelegate)
        m_historyDelegate = 0;
    if (m_mainFrame)
        delete m_mainFrame;
    if (d)
        delete d;
    if (m_webViewObserver)
        delete m_webViewObserver;
    if (m_memoryEvent)
        delete m_memoryEvent;
    m_children.clear();
}

WebView* WebView::createInstance()
{
    return new WebView();
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

void WebView::setCacheModel(WebCacheModel cacheModel)
{
    if (s_didSetCacheModel && cacheModel == s_cacheModel)
        return;

    //TODO : Calcul the next values
    unsigned long long memSize = 256;
    //unsigned long long diskFreeSize = 4096;
    
#if OS(MACPORT)
    unsigned int physmem;
    size_t len = sizeof physmem;
    static int mib[2] = { CTL_HW, HW_PHYSMEM };
    static size_t miblen = sizeof(mib) / sizeof(mib[0]);
  
    if (sysctl (mib, miblen, &physmem, &len, NULL, 0) == 0 && len == sizeof (physmem))
       memSize=physmem / (1024*1024);
#elif OS(AMIGAOS4)
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

void WebView::close()
{
    if (m_didClose)
        return;

    m_didClose = true;

    // Purge page cache
    // The easiest way to do that is to disable page cache
    // The preferences can be null.
    if (m_preferences && m_preferences->usesPageCache())
        m_page->settings()->setUsesPageCache(false);
    
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
        WebPreferences::removeReferenceForIdentifier(identifier.utf8().data());

    WebPreferences* preferences = m_preferences;
    if (preferences)
        preferences->didRemoveFromWebView();

    deleteBackingStore();
}

void WebView::repaint(const WebCore::IntRect& windowRect, bool contentChanged, bool immediate, bool repaintContentOnly)
{
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


BalRectangle WebView::onExpose(BalEventExpose ev)
{
    BalRectangle rect = d->onExpose(ev);
    for (size_t i = 0; i < m_children.size(); ++i)
        m_children[i]->onExpose(ev, rect);
    return rect;
}

bool WebView::onKeyDown(BalEventKey ev)
{
#if ENABLE(CEHTML)
    PlatformKeyboardEvent keyboardEvent(&ev);
    int virtualKey = keyboardEvent.windowsVirtualKeyCode();
    if (shouldCallDefaultHandlerForVKKey(virtualKey)) {
        m_previousDownEventCalledDefaultHandler = true;
        return defaultActionOnFocusedNode(ev);
    }
    m_previousDownEventCalledDefaultHandler = false;
#endif
    
    for (int i = m_children.size() - 1; i >= 0; --i)
        if(m_children[i]->onKeyDown(ev))
            return true;
    return d->onKeyDown(ev);
}

bool WebView::onKeyUp(BalEventKey ev)
{
#if ENABLE(CEHTML)
        if (m_previousDownEventCalledDefaultHandler) {
            m_previousDownEventCalledDefaultHandler = false;
            return defaultActionOnFocusedNode(ev);
        }
#endif
    for (int i = m_children.size() - 1; i >= 0; --i)
        if(m_children[i]->onKeyUp(ev))
            return true;
    return d->onKeyUp(ev);
}

bool WebView::onMouseMotion(BalEventMotion ev)
{
    for (int i = m_children.size() - 1; i >= 0; --i)
        if(m_children[i]->onMouseMotion(ev))
            return true;
    return d->onMouseMotion(ev);
}

bool WebView::onMouseButtonDown(BalEventButton ev)
{
    for (int i = m_children.size() - 1; i >= 0; --i)
        if(m_children[i]->onMouseButtonDown(ev))
            return true;
    return d->onMouseButtonDown(ev);
}

bool WebView::onMouseButtonUp(BalEventButton ev)
{
    for (int i = m_children.size() - 1; i >= 0; --i)
        if(m_children[i]->onMouseButtonUp(ev))
            return true;
    return d->onMouseButtonUp(ev);
}

bool WebView::onScroll(BalEventScroll ev)
{
    for (int i = m_children.size() - 1; i >= 0; --i)
        if(m_children[i]->onScroll(ev))
            return true;
    return d->onScroll(ev);
}

bool WebView::onResize(BalResizeEvent ev)
{
    for (int i = m_children.size() - 1; i >= 0; --i)
        if(m_children[i]->onResize(ev))
            return true;
    d->onResize(ev);
    return true;
}

bool WebView::onQuit(BalQuitEvent ev)
{
    for (int i = m_children.size() - 1; i >= 0; --i)
        if(m_children[i]->onQuit(ev))
            return false;
    d->onQuit(ev);
    return true;
}

void WebView::onUserEvent(BalUserEvent ev)
{
    for (int i = m_children.size() - 1; i >= 0; --i)
        if(m_children[i]->onUserEvent(ev))
            return;
    d->onUserEvent(ev);
}

bool WebView::defaultActionOnFocusedNode(BalEventKey event)
{
    Frame* frame = page()->focusController()->focusedOrMainFrame();
    if (!frame)
        return false;
    Node* focusedNode = frame->document()->focusedNode();
    if (!focusedNode)
        return false;

    PlatformKeyboardEvent keyboardEvent(&event);
    if (keyboardEvent.type() == PlatformKeyboardEvent::KeyDown) {
        // FIXME: We should tweak the parameters instead of hard coding them.
        keyboardEvent.disambiguateKeyDownEvent(PlatformKeyboardEvent::RawKeyDown, false);
    }
    RefPtr<KeyboardEvent> keyEvent = KeyboardEvent::create(keyboardEvent, frame->document()->defaultView());
    keyEvent->setTarget(focusedNode);
    focusedNode->defaultEventHandler(keyEvent.get());
    
    return (keyEvent->defaultHandled() || keyEvent->defaultPrevented());
}

#if ENABLE(CEHTML)
// This method enforces requirement 5.4.1.f & g according to VK keys dispatch with regard to focused node.
bool WebView::shouldCallDefaultHandlerForVKKey(int virtualKey)
{
    if (virtualKey < VK_LEFT || virtualKey > VK_DOWN)
        return false;

    Frame* frame = core(this)->focusController()->focusedOrMainFrame();
    if (!frame)
        return false;
    WebCore::Node* focusedNode = frame->document()->focusedNode();
    if (!focusedNode)
        return false;

    SelectionController* selectionController = frame->selection();
    if (selectionController->isNone())
        return false;

    if (focusedNode->hasTagName(HTMLNames::textareaTag) || (focusedNode->hasTagName(HTMLNames::inputTag) && static_cast<HTMLInputElement*>(focusedNode)->isTextField())) {
        switch(virtualKey) {
        case VK_LEFT:
        case VK_UP:
        {
            Position base = selectionController->isCaret() ? selectionController->selection().base() : selectionController->selection().end();
            // If we are at the first position, we should dispatch the event so that navigation is possible.
            if (base.atFirstEditingPositionForNode())
                return false;

            return true;
        }
        case VK_RIGHT:
        case VK_DOWN:
        {                                                                                                                                                                                        
            Position base = selectionController->isCaret() ? selectionController->selection().base() : selectionController->selection().start();                                                 
            // If we are at the last position, we should dispatch the event so that navigation is possible.
            if (base.atLastEditingPositionForNode())
                return false;

            return true;
        }
        default:
            return false;
        }
        ASSERT_NOT_REACHED();
    }

    // Other tags.
    return false;
}
#endif


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
#if OS(AMIGAOS4)
    closeWindow();
#else
    d->closeWindowSoon();
#endif
}

void WebView::closeWindow()
{
#if OS(AMIGAOS4)
    extern void closeAmigaWindow(BalWidget *owbwindow);

    closeAmigaWindow(viewWindow());
#endif    
}

bool WebView::canHandleRequest(const WebCore::ResourceRequest& request)
{
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

static string standardUserAgentWithApplicationName(const string& applicationName)
{
    String userAgent = String::format("Mozilla/5.0 (%s; %s; %s; %s) AppleWebKit/%d.%d (KHTML, like Gecko)%s%s",
                                      UA_PLATFORM,
                                      UA_SECURITY,
                                      UA_OS,
                                      UA_LOCALE,
                                      WEBKIT_MAJOR_VERSION,
                                      WEBKIT_MINOR_VERSION,
                                      applicationName.empty() ? "" : " ",
                                      applicationName.empty() ? "" : applicationName.c_str());

    
#if ENABLE(DAE)
    userAgent = String::format("%s %s", userAgent.utf8().data(), HBBTVUserAgent::userAgent().utf8().data());
#elif ENABLE(CEHTML)
    userAgent = String::format("%s %s", userAgent.utf8().data(), CEHTMLUserAgent::userAgent().utf8().data());
#endif

    return userAgent.utf8().data();
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
#if OS(AMIGAOS4)
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
#if ENABLE(CEHTML)
    { VK_PAGE_UP,  ShiftKey,         "MovePageUpAndModifySelection"                },
    { VK_PAGE_UP,  0,                "MovePageUp"                                  },
#else
    { VK_PRIOR,  ShiftKey,           "MovePageUpAndModifySelection"                },
    { VK_PRIOR,  0,                  "MovePageUp"                                  },
#endif
    { VK_DOWN,   0,                  "MoveDown"                                    },
    { VK_DOWN,   ShiftKey,           "MoveDownAndModifySelection"                  },
    { VK_NEXT,   ShiftKey,           "MovePageDownAndModifySelection"              },
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
#if ENABLE(CEHTML)
    { VK_PERIOD, CtrlKey,            "Cancel"                                      },
#else
    { VK_OEM_PERIOD, CtrlKey,        "Cancel"                                      },
#endif
    { VK_TAB,    0,                  "InsertTab"                                   },
    { VK_TAB,    ShiftKey,           "InsertBacktab"                               },
#if ENABLE(CEHTML)
    { VK_ENTER, 0,                   "InsertNewline"                               },
    { VK_ENTER, CtrlKey,             "InsertNewline"                               },
    { VK_ENTER, AltKey,              "InsertNewline"                               },
    { VK_ENTER, AltKey | ShiftKey,   "InsertNewline"                               },
#else
    { VK_RETURN, 0,                  "InsertNewline"                               },
    { VK_RETURN, CtrlKey,            "InsertNewline"                               },
    { VK_RETURN, AltKey,             "InsertNewline"                               },
    { VK_RETURN, ShiftKey,           "InsertNewline"                               },
    { VK_RETURN, AltKey | ShiftKey,  "InsertNewline"                               },
#endif

    // It's not quite clear whether clipboard shortcuts and Undo/Redo should be handled
    // in the application or in WebKit. We chose WebKit.
    { 'C',       CtrlKey,            "Copy"                                        },
    { 'V',       CtrlKey,            "Paste"                                       },
    { 'X',       CtrlKey,            "Cut"                                         },
    { 'A',       CtrlKey,            "SelectAll"                                   },
#if OS(AMIGAOS4)
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
#if OS(AMIGAOS4)
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
    //if (m_preferences->developerExtrasDisabledByOverride())
    //    return false;

#if ENABLE(INSPECTOR)
    return true;
#else
    return false;
#endif
}

const string& WebView::userAgentForKURL(const string&)
{
    if (m_userAgentOverridden)
        return m_userAgentCustom;

    char* userAgent = getenv("OWB_USER_AGENT");
    if (userAgent)
        m_userAgentStandard = userAgent;

    if (!m_userAgentStandard.length())
        m_userAgentStandard = standardUserAgentWithApplicationName(m_applicationName);

    return m_userAgentStandard;
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

void WebView::createWindow(BalRectangle frame)
{
    m_viewWindow = d->createWindow(frame);
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

    m_page->focusController()->setFocusedFrame(m_page->mainFrame());
    m_page->focusController()->setActive(true); 
    m_page->focusController()->setFocused(true); 

    Frame* mainFrame = m_page->mainFrame();
    Frame* focusedFrame = m_page->focusController()->focusedOrMainFrame();
    mainFrame->selection()->setFocused(mainFrame == focusedFrame);
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
#if OS(AMIGAOS4)
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

void WebView::setDownloadDelegate(TransferSharedPtr<WebDownloadDelegate> d)
{
    m_downloadDelegate = d;
}

TransferSharedPtr<WebDownloadDelegate> WebView::downloadDelegate()
{
    return m_downloadDelegate;
}

void WebView::setPolicyDelegate(TransferSharedPtr<WebPolicyDelegate> d)
{
    m_policyDelegate = d;
}

TransferSharedPtr<WebPolicyDelegate> WebView::policyDelegate()
{
    return m_policyDelegate;
}

void WebView::setWebNotificationDelegate(TransferSharedPtr<WebNotificationDelegate> notificationDelegate)
{
    m_webNotificationDelegate = notificationDelegate;
}

TransferSharedPtr<WebNotificationDelegate> WebView::webNotificationDelegate()
{
    return m_webNotificationDelegate;
}

void WebView::setWebFrameLoadDelegate(TransferSharedPtr<WebFrameLoadDelegate> webFrameLoadDelegate)
{
    m_webFrameLoadDelegate = webFrameLoadDelegate;

}

TransferSharedPtr<WebFrameLoadDelegate> WebView::webFrameLoadDelegate()
{
    return m_webFrameLoadDelegate;
}

void WebView::setJSActionDelegate(TransferSharedPtr<JSActionDelegate> newJSActionDelegate)
{
    m_jsActionDelegate = newJSActionDelegate;
}

TransferSharedPtr<JSActionDelegate> WebView::jsActionDelegate()
{
    return m_jsActionDelegate;
}

void WebView::setWebEditingDelegate(TransferSharedPtr<WebEditingDelegate> editing)
{
    m_webEditingDelegate = editing;
}

TransferSharedPtr<WebEditingDelegate> WebView::webEditingDelegate()
{
    return m_webEditingDelegate;
}

void WebView::setWebResourceLoadDelegate(TransferSharedPtr<WebResourceLoadDelegate> resourceLoad)
{
    m_webResourceLoadDelegate = resourceLoad;
}

TransferSharedPtr<WebResourceLoadDelegate> WebView::webResourceLoadDelegate()
{
    return m_webResourceLoadDelegate;
}

void WebView::setWebBindingJSDelegate(TransferSharedPtr<WebBindingJSDelegate> webBindingJSDelegate)
{
    m_webBindingJSDelegate = webBindingJSDelegate;
}

TransferSharedPtr<WebBindingJSDelegate> WebView::webBindingJSDelegate()
{
    return m_webBindingJSDelegate;
}

void WebView::setWebWidgetEngineDelegate(TransferSharedPtr<WebWidgetEngineDelegate> webWidgetEngineDelegate)
{
    m_webWidgetEngineDelegate = webWidgetEngineDelegate;
}

TransferSharedPtr<WebWidgetEngineDelegate> WebView::webWidgetEngineDelegate()
{
    return m_webWidgetEngineDelegate;
}

void WebView::setPluginHalterDelegate(TransferSharedPtr<WebPluginHalterDelegate> d)
{
    m_pluginHalterDelegate = d;
}

TransferSharedPtr<WebPluginHalterDelegate> WebView::pluginHalterDelegate()
{
    return m_pluginHalterDelegate;
}


void WebView::setGeolocationProvider(WebGeolocationProvider* locationProvider)
{
    m_geolocationProvider = locationProvider;
}

WebGeolocationProvider* WebView::geolocationProvider()
{
    return m_geolocationProvider;
}

void WebView::geolocationDidChangePosition(WebGeolocationPosition* position)
{
#if ENABLE(CLIENT_BASED_GEOLOCATION)
    if (!m_page)
        return;
    m_page->geolocationController()->positionChanged(core(position));
#endif
}

void WebView::geolocationDidFailWithError(WebError* error)
{
#if ENABLE(CLIENT_BASED_GEOLOCATION)
    if (!m_page)
        return;
    if (!error)
        return;

    const char* description = error->localizedDescription();
    if (!description)
        return;

    RefPtr<GeolocationError> geolocationError = GeolocationError::create(GeolocationError::PositionUnavailable, description);
    m_page->geolocationController()->errorOccurred(geolocationError.get());
#endif
}

/*void WebView::setDomainRelaxationForbiddenForURLScheme(bool forbidden, const char* scheme)
{
    SecurityOrigin::setDomainRelaxationForbiddenForURLScheme(forbidden, String(scheme, SysStringLen(scheme)));
}

void WebView::registerURLSchemeAsSecure(const char* scheme)
{
    SecurityOrigin::registerURLSchemeAsSecure(toString(scheme));
}*/

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
    bool hasGoneBack= m_page->goBack();
    d->repaintAfterNavigationIfNeeded();
    return hasGoneBack;
}

bool WebView::goForward()
{
    bool hasGoneForward = m_page->goForward();
    d->repaintAfterNavigationIfNeeded();
    return hasGoneForward;
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
    m_page->settings()->setZoomMode(isTextOnly ? ZoomTextOnly : ZoomPage);
    Frame* coreFrame = core(m_mainFrame);
    if (coreFrame && coreFrame->document())
        coreFrame->setZoomFactor(multiplier, isTextOnly ? ZoomTextOnly : ZoomPage);

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
    ZoomMode zoomMode = isTextOnly ? ZoomTextOnly : ZoomPage;
    if (zoomMode != m_page->settings()->zoomMode())
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
    return zoomIn(m_page->settings()->zoomMode() == ZoomTextOnly);
}

bool WebView::makeTextSmaller()
{
    return zoomOut(m_page->settings()->zoomMode() == ZoomTextOnly);
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



void WebView::setApplicationNameForUserAgent(const string& applicationName)
{
    m_applicationName = applicationName;
    m_userAgentStandard = "";
}

const string& WebView::applicationNameForUserAgent()
{
    return m_applicationName;
}

void WebView::setCustomUserAgent(const string& userAgentString)
{
    m_userAgentOverridden = true;
    m_userAgentCustom = userAgentString;
}

const string& WebView::customUserAgent()
{
    return m_userAgentCustom;
}

const string& WebView::userAgentForURL(const string& url)
{
    return userAgentForKURL(url);
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

    JSC::JSValue scriptExecutionResult = coreFrame->script()->executeScript(script, false).jsValue();
    if(!scriptExecutionResult)
        return NULL;
    else if (scriptExecutionResult.isString()) {
    	JSC::JSLock lock(JSC::SilenceAssertionsOnly);
        JSC::ExecState* exec = coreFrame->script()->globalObject(mainThreadNormalWorld())->globalExec();
        return scriptExecutionResult.getString(exec).ascii();
    }
    return NULL;
}

void WebView::executeScript(const char* script)
{
    if (!script)
        return;

    Frame* coreFrame = core(m_mainFrame);
    if (!coreFrame)
        return ;

    coreFrame->script()->executeScript(String::fromUTF8(script), true);
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
    
    WebPreferences::removeReferenceForIdentifier(identifier.utf8().data());

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


/*static Frame *incrementFrame(Frame *curr, bool forward, bool wrapFlag)
{
    return forward
        ? curr->tree()->traverseNextWithWrap(wrapFlag)
        : curr->tree()->traversePreviousWithWrap(wrapFlag);
}*/

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

bool WebView::addUserScriptToGroup(const char* groupName, WebScriptWorld* world, const char* source, const char* url, unsigned whitelistCount, const char** whitelist, unsigned blacklistCount, const char** blacklist, WebUserScriptInjectionTime injectionTime)
{
    String group(groupName);
    if (group.isEmpty())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;
    
    pageGroup->addUserScriptToWorld(world->world(),
                                    String(source),
                                    KURL(KURL(), String(url)),
                                    toStringVector(whitelistCount, whitelist),
                                    toStringVector(blacklistCount, blacklist),
                                    injectionTime == WebUserScriptInjectAtDocumentStart ? InjectAtDocumentStart : InjectAtDocumentEnd);
    return true;
}

bool WebView::addUserStyleSheetToGroup(const char* groupName, WebScriptWorld* world, const char* source, const char* url, unsigned whitelistCount, const char** whitelist, unsigned blacklistCount, const char** blacklist)
{
    String group(groupName);
    if (group.isEmpty())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;
 
    pageGroup->addUserStyleSheetToWorld(world->world(),
                                       String(source),
                                       KURL(KURL(), String(url)),
                                       toStringVector(whitelistCount, whitelist),
                                       toStringVector(blacklistCount, blacklist));
    return true;
}

bool WebView::removeUserScriptFromGroup(const char* groupName, WebScriptWorld* world, const char* url)
{
    String group(groupName);
    if (group.isEmpty())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;

    pageGroup->removeUserScriptFromWorld(world->world(), KURL(KURL(), url));
    return true;
}

bool WebView::removeUserStyleSheetFromWorld(const char* groupName, WebScriptWorld* world, const char* url)
{
    String group(groupName);
    if (group.isEmpty())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;

    pageGroup->removeUserStyleSheetFromWorld(world->world(), KURL(KURL(), url));
    return true;
}

bool WebView::removeUserScriptsFromGroup(const char* groupName, WebScriptWorld* world)
{
    String group(groupName);
    if (group.isEmpty())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;

    pageGroup->removeUserScriptsFromWorld(world->world());
    return true;
}

bool WebView::removeUserStyleSheetsFromGroup(const char* groupName, WebScriptWorld* world)
{
    String group(groupName);
    if (group.isEmpty())
        return false;

    PageGroup* pageGroup = PageGroup::pageGroup(group);
    ASSERT(pageGroup);
    if (!pageGroup)
        return false;

    pageGroup->removeUserStyleSheetsFromWorld(world->world());
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

void WebView::setFocus()
{
    FocusController* focusController = m_page->focusController();
    focusController->setFocused(true);
    if(!focusController->focusedFrame())
        focusController->setFocusedFrame(m_page->mainFrame());
}

bool WebView::focused() const
{
    return m_page->focusController()->isFocused();
}

void WebView::clearFocus()
{
    FocusController* focusController = m_page->focusController();
    focusController->setFocused(false);
}

void WebView::updateActiveState()
{
    FocusController* focusController = m_page->focusController();
    focusController->setActive(active());
    focusController->setFocused(focused());
}

void WebView::updateFocusedAndActiveState()
{
    updateActiveState();

    bool active = m_page->focusController()->isActive();
    Frame* mainFrame = m_page->mainFrame();
    Frame* focusedFrame = m_page->focusController()->focusedOrMainFrame();
    mainFrame->selection()->setFocused(active && mainFrame == focusedFrame);
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

bool WebView::commandEnabled(const char* commandName)
{
    if (!commandName)
        return false;

    Editor::Command command = m_page->focusController()->focusedOrMainFrame()->editor()->command(commandName);
    return command.isEnabled();
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

WebHitTestResults* WebView::elementAtPoint(BalPoint& point)
{
    Frame* frame = core(m_mainFrame);
    if (!frame)
        return 0;

    IntPoint webCorePoint(point);
    HitTestResult result = HitTestResult(webCorePoint);
    if (frame->contentRenderer())
        result = frame->eventHandler()->hitTestResultAtPoint(webCorePoint, false);
    return WebHitTestResults::createInstance(result);
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

WebDragOperation WebView::dragEnter(WebDragData* webDragData, int identity,
                                    const BalPoint& clientPoint, const BalPoint& screenPoint, 
                                    WebDragOperation operationsAllowed)
{
    ASSERT(!m_currentDragData);

    m_currentDragData = webDragData;
    m_dragIdentity = identity;
    m_operationsAllowed = operationsAllowed;

    DragData dragData(
        m_currentDragData->platformDragData()->m_dragDataRef,
        clientPoint,
        screenPoint,
        static_cast<DragOperation>(operationsAllowed));

    m_dropEffect = DropEffectDefault;
    m_dragTargetDispatch = true;
    DragOperation effect = m_page->dragController()->dragEntered(&dragData);
    // Mask the operation against the drag source's allowed operations.
    if ((effect & dragData.draggingSourceOperationMask()) != effect)
        effect = DragOperationNone;
    m_dragTargetDispatch = false;

    if (m_dropEffect != DropEffectDefault) {
        m_dragOperation = (m_dropEffect != DropEffectNone) ? WebDragOperationCopy
                                                           : WebDragOperationNone;
    } else
        m_dragOperation = static_cast<WebDragOperation>(effect);
    return m_dragOperation;
}

WebDragOperation WebView::dragOver(const BalPoint& clientPoint, const BalPoint& screenPoint, WebDragOperation operationsAllowed)
{
    ASSERT(m_currentDragData);

    m_operationsAllowed = operationsAllowed;
    DragData dragData(
        m_currentDragData->platformDragData()->m_dragDataRef,
        clientPoint,
        screenPoint,
        static_cast<DragOperation>(operationsAllowed));

    m_dropEffect = DropEffectDefault;
    m_dragTargetDispatch = true;
    DragOperation effect = m_page->dragController()->dragUpdated(&dragData);
    // Mask the operation against the drag source's allowed operations.
    if ((effect & dragData.draggingSourceOperationMask()) != effect)
        effect = DragOperationNone;
    m_dragTargetDispatch = false;

    if (m_dropEffect != DropEffectDefault) {
        m_dragOperation = (m_dropEffect != DropEffectNone) ? WebDragOperationCopy
                                                           : WebDragOperationNone;
    } else
        m_dragOperation = static_cast<WebDragOperation>(effect);
    return m_dragOperation;
}

void WebView::dragTargetDragLeave()
{
    ASSERT(m_currentDragData);

    DragData dragData(
        m_currentDragData->platformDragData()->m_dragDataRef,
        IntPoint(),
        IntPoint(),
        static_cast<DragOperation>(m_operationsAllowed));

    m_dragTargetDispatch = true;
    m_page->dragController()->dragExited(&dragData);
    m_dragTargetDispatch = false;

    m_currentDragData = 0;
    m_dropEffect = DropEffectDefault;
    m_dragOperation = WebDragOperationNone;
    m_dragIdentity = 0;
}

void WebView::dragTargetDrop(const BalPoint& clientPoint,
                                 const BalPoint& screenPoint)
{
    ASSERT(m_currentDragData);

    // If this webview transitions from the "drop accepting" state to the "not
    // accepting" state, then our IPC message reply indicating that may be in-
    // flight, or else delayed by javascript processing in this webview.  If a
    // drop happens before our IPC reply has reached the browser process, then
    // the browser forwards the drop to this webview.  So only allow a drop to
    // proceed if our webview m_dragOperation state is not DragOperationNone.

    if (m_dragOperation == WebDragOperationNone) { // IPC RACE CONDITION: do not allow this drop.
        dragTargetDragLeave();
        return;
    }

    DragData dragData(
        m_currentDragData->platformDragData()->m_dragDataRef,
        clientPoint,
        screenPoint,
        static_cast<DragOperation>(m_operationsAllowed));

    m_dragTargetDispatch = true;
    m_page->dragController()->performDrag(&dragData);
    m_dragTargetDispatch = false;

    m_currentDragData = 0;
    m_dropEffect = DropEffectDefault;
    m_dragOperation = WebDragOperationNone;
    m_dragIdentity = 0;
}

int WebView::dragIdentity()
{
    if (m_dragTargetDispatch)
        return m_dragIdentity;
    return 0;
}

void WebView::dragSourceEndedAt(const BalPoint& clientPoint, const BalPoint& screenPoint, WebDragOperation operation)
{
    PlatformMouseEvent pme(clientPoint,
                           screenPoint,
                           LeftButton, MouseEventMoved, 0, false, false, false,
                           false, 0);
    m_page->mainFrame()->eventHandler()->dragSourceEndedAt(pme,
        static_cast<DragOperation>(operation));
}

void WebView::dragSourceSystemDragEnded()
{
    // It's possible for us to get this callback while not doing a drag if
    // it's from a previous page that got unloaded.
    //if (m_doingDragAndDrop) {
        m_page->dragController()->dragEnded();
        //m_doingDragAndDrop = false;
    //}
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
    WebDataSource* dataSource(m_mainFrame->provisionalDataSource());
    if (!dataSource) {
        dataSource = m_mainFrame->dataSource();
        if (!dataSource)
            return 0;
    }

    WebMutableURLRequest* request(dataSource->request());
    if (!request)
        return 0;

    return request->URL();
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
    settings->setJavaEnabled(enabled);

    enabled = preferences->isJavaScriptEnabled();
    settings->setJavaScriptEnabled(enabled);

    enabled = preferences->isXSSAuditorEnabled();
    settings->setXSSAuditorEnabled(!!enabled);

    enabled = preferences->javaScriptCanOpenWindowsAutomatically();
    settings->setJavaScriptCanOpenWindowsAutomatically(enabled);

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
    settings->setZoomMode(enabled ? ZoomTextOnly : ZoomPage);

#if OS(AMIGAOS4)
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

    enabled = preferences->inApplicationChromeMode();
    settings->setApplicationChromeMode(enabled);

    enabled = preferences->offlineWebApplicationCacheEnabled();
    settings->setOfflineWebApplicationCacheEnabled(enabled);

    enabled = preferences->databasesEnabled();
    settings->setDatabasesEnabled(enabled);

    enabled = preferences->experimentalNotificationsEnabled();
    settings->setExperimentalNotificationsEnabled(enabled);

    enabled = preferences->isWebSecurityEnabled();
    settings->setWebSecurityEnabled(enabled);

    enabled = preferences->allowUniversalAccessFromFileURLs();
    settings->setAllowUniversalAccessFromFileURLs(!!enabled);

    enabled = preferences->allowFileAccessFromFileURLs();
    settings->setAllowFileAccessFromFileURLs(!!enabled);

    enabled = preferences->isXSSAuditorEnabled();
    settings->setXSSAuditorEnabled(!!enabled);

    // Only use on windows
/*    enabled = preferences->shouldUseHighResolutionTimers();
    settings->setShouldUseHighResolutionTimers(enabled);*/

    enabled = preferences->pluginAllowedRunTime();
    settings->setPluginAllowedRunTime(enabled);

    enabled = preferences->isFrameSetFlatteningEnabled();
    settings->setFrameSetFlatteningEnabled(enabled);
    
    enabled = preferences->webGLEnabled();
    settings->setWebGLEnabled(enabled);

    enabled = preferences->acceleratedCompositingEnabled();
    settings->setAcceleratedCompositingEnabled(enabled);

    enabled = preferences->localStorageEnabled();
    settings->setLocalStorageEnabled(enabled);

    int limit = preferences->memoryLimit();  
    WTF::setMemoryLimit(limit);

    enabled = preferences->allowScriptsToCloseWindows();
    settings->setAllowScriptsToCloseWindows(enabled);

    updateSharedSettingsFromPreferencesIfNeeded(preferences);
}

void updateSharedSettingsFromPreferencesIfNeeded(WebPreferences* preferences)
{
    if (preferences != WebPreferences::sharedStandardPreferences())
        return ;

    //WebKitCookieStorageAcceptPolicy acceptPolicy = preferences->cookieStorageAcceptPolicy();
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
    d->setViewWindow(view);
}

BalPoint WebView::scrollOffset()
{
    if (m_page->mainFrame() && m_page->mainFrame()->view()) {
        IntSize offsetIntSize = m_page->mainFrame()->view()->scrollOffset();
        return IntPoint(offsetIntSize.width(), offsetIntSize.height());
    } else
        return IntPoint();
}

void WebView::scrollBy(BalPoint offset)
{
    if (m_page->mainFrame() && m_page->mainFrame()->view()) {
        IntPoint p(offset);
        m_page->mainFrame()->view()->scrollBy(IntSize(p.x(), p.y()));
    }
}

void WebView::scrollWithDirection(::ScrollDirection direction)
{
    switch (direction) {
    case Up:
        m_page->mainFrame()->view()->scrollBy(IntSize(0, (int)-Scrollbar::pixelsPerLineStep()));
        break;
    case Down:
        m_page->mainFrame()->view()->scrollBy(IntSize(0, (int)Scrollbar::pixelsPerLineStep()));
        break;
    case Left:
        m_page->mainFrame()->view()->scrollBy(IntSize((int)-Scrollbar::pixelsPerLineStep(), 0));
        break;
    case Right:
        m_page->mainFrame()->view()->scrollBy(IntSize((int)Scrollbar::pixelsPerLineStep(), 0));
        break;
    default:
        ASSERT_NOT_REACHED();
    }
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
            otherView->m_page->mainFrame()->loader()->history()->saveDocumentAndScrollState();
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
    if (Frame* frame = m_page->mainFrame())
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
    if (m_mainFrame)
        m_mainFrame->updateBackground();
}

bool WebView::transparent()
{
    return m_transparent;
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

void WebView::fireWebKitTimerEvents()
{
    d->fireWebKitTimerEvents();
}

void WebView::fireWebKitThreadEvents()
{
    d->fireWebKitThreadEvents();
}


void WebView::setJavaScriptURLsAreAllowed(bool areAllowed)
{
    m_page->setJavaScriptURLsAreAllowed(areAllowed);
}

void WebView::resize(BalRectangle r)
{
    d->resize(r);
}


void WebView::move(BalPoint lastPos, BalPoint newPos)
{
    d->move(lastPos, newPos);
}

unsigned WebView::backgroundColor()
{
    if (!m_mainFrame)
        return 0;
    Frame* coreFrame = core(m_mainFrame);
    return coreFrame->view()->baseBackgroundColor().rgb();
}

void WebView::allowLocalLoadsForAll()
{
    SecurityOrigin::setLocalLoadPolicy(SecurityOrigin::AllowLocalLoadsForAll);
}

WebInspector* WebView::inspector() {
#if ENABLE(INSPECTOR)
   if (!m_webInspector)
       m_webInspector = WebInspector::createInstance(this);

    return m_webInspector;
#endif
    return 0;
}


Page* core(WebView* webView)
{
    Page* page = 0;

    page = webView->page();

    return page;
}

bool WebView::invalidateBackingStore(const WebCore::IntRect* rect)
{
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

void WebView::whiteListAccessFromOrigin(const char* sourceOrigin, const char* destinationProtocol, const char* destinationHost, bool allowDestinationSubDomains) const
{
    SecurityOrigin::whiteListAccessFromOrigin(*SecurityOrigin::createFromString(sourceOrigin), destinationProtocol, destinationHost, allowDestinationSubDomains);
}

void WebView::resetOriginAccessWhiteLists() const
{
    SecurityOrigin::resetOriginAccessWhiteLists();
}

void WebView::setHistoryDelegate(TransferSharedPtr<WebHistoryDelegate> historyDelegate)
{
    m_historyDelegate = historyDelegate;
}

TransferSharedPtr<WebHistoryDelegate> WebView::historyDelegate() const
{
    return m_historyDelegate;
}

void WebView::addVisitedLinks(const char** visitedURLs, unsigned visitedURLCount)
{
    PageGroup& group = core(this)->group();

    for (unsigned i = 0; i < visitedURLCount; ++i)
        group.addVisitedLink(KURL(KURL(), visitedURLs[i]));
}

static PluginView* pluginViewForNode(DOMNode* domNode)
{
    if (!domNode)
        return false;

    Node* node = domNode->node();
    if (!node)
        return false;

    RenderObject* renderer = node->renderer();
    if (!renderer || !renderer->isWidget())
        return false;

    Widget* widget = toRenderWidget(renderer)->widget();
    if (!widget || !widget->isPluginView())
        return false;

    return static_cast<PluginView*>(widget);
}

bool WebView::isNodeHaltedPlugin(DOMNode* domNode)
{
    if (!domNode)
        return false;

    PluginView* view = pluginViewForNode(domNode);
    if (!view)
        return false;

    return view->isHalted();
}

bool WebView::restartHaltedPluginForNode(DOMNode* domNode)
{
    if (!domNode)
        return false;

    PluginView* view = pluginViewForNode(domNode);
    if (!view)
        return false;

    view->restart();
    return true;
}

bool WebView::hasPluginForNodeBeenHalted(DOMNode* domNode)
{
    if (!domNode)
        return false;

    PluginView* view = pluginViewForNode(domNode);
    if (!view)
        return false;

    return view->hasBeenHalted();
}


void WebView::stopLoading(bool stop)
{
    m_isStopped = stop;
    if (stop)
        core(mainFrame())->loader()->stopLoading(UnloadEventPolicyUnloadAndPageHide);
}

void WebView::enterFullscreenForNode(WebCore::Node*)
{
    notImplemented();
}

void WebView::exitFullscreen()
{
    notImplemented();
}

const char* WebView::inspectorSettings() const
{
    return m_inspectorSettings.c_str();
}

void WebView::setInspectorSettings(const char* settings)
{
    m_inspectorSettings = settings;
}


const char* WebView::encodeHostName(const char* source)
{
    UChar destinationBuffer[HOST_NAME_BUFFER_LENGTH];

    String src = String::fromUTF8(source);
    int length = src.length();

    if (src.find("%") != -1)
        src = decodeURLEscapeSequences(src);

    const UChar* sourceBuffer = src.characters();

    bool error = false;
    int32_t numCharactersConverted = WTF::Unicode::IDNToASCII(sourceBuffer, length, destinationBuffer, HOST_NAME_BUFFER_LENGTH, &error);

    if (error)
        return source;
    if (numCharactersConverted == length && memcmp(sourceBuffer, destinationBuffer, length * sizeof(UChar)) == 0) {
        return source;
    }
   
    if (!numCharactersConverted)
        return source;

    String result(destinationBuffer, numCharactersConverted);
    return strdup(result.utf8().data()); 
}

const char* WebView::decodeHostName(const char* source)
{
    UChar destinationBuffer[HOST_NAME_BUFFER_LENGTH];

    String src = String::fromUTF8(source);


    const UChar* sourceBuffer = src.characters();
    int length = src.length();

    bool error = false;
    int32_t numCharactersConverted = WTF::Unicode::IDNToUnicode(sourceBuffer, length, destinationBuffer, HOST_NAME_BUFFER_LENGTH, &error);

    if (error)
        return source;

    if (numCharactersConverted == length && memcmp(sourceBuffer, destinationBuffer, length * sizeof(UChar)) == 0) {
        return source;
    }
    if (!WTF::Unicode::allCharactersInIDNScriptWhiteList(destinationBuffer, numCharactersConverted)) {
        return source; 
    }

    if (!numCharactersConverted)
        return source;

    String result(destinationBuffer, numCharactersConverted);
    return strdup(result.utf8().data()); 
}

void WebView::addChildren(WebWindow* webWindow)
{
    if (webWindow)
        m_children.push_back(webWindow);
}

void WebView::removeChildren(WebWindow* webWindow)
{
    vector<WebWindow*>::iterator it = m_children.begin();
    for(; it != m_children.end(); ++it) {
        if ((*it) == webWindow) {
            m_children.erase(it);
            break;
        }
    }
}

void WebView::sendExposeEvent(BalRectangle rect)
{
    addToDirtyRegion(rect);
    d->sendExposeEvent(rect);
}
