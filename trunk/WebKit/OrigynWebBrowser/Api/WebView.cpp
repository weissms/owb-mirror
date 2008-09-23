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

#include "DefaultPolicyDelegate.h"
#include "DefaultDownloadDelegate.h"
#include "DOMCoreClasses.h"
#include "WebDatabaseManager.h"
#include "WebDocumentLoader.h"
#include "WebEditorClient.h"
#include "WebElementPropertyBag.h"
#include "WebFrame.h"
#include "WebBackForwardList.h"
#include "WebChromeClient.h"
#include "WebContextMenuClient.h"
#include "WebDragClient.h"
#include "WebIconDatabase.h"
#if ENABLE(INSPECTOR)
#include "WebInspector.h"
#endif
#include "WebInspectorClient.h"
#include "WebMutableURLRequest.h"
#include "WebPreferences.h"
#include "WebViewPrivate.h"

#include <AXObjectCache.h>
#include "ObserverData.h"
#include "ObserverServiceData.h"
#include "KeyboardCodes.h"
#include <PlatformString.h>
#include <Cache.h>
#include <ContextMenu.h>
#include <ContextMenuController.h>
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
#include <IntRect.h>
#include <KeyboardEvent.h>
#include <Language.h>
#include <MIMETypeRegistry.h>
#include <NotImplemented.h>
#include <Page.h>
#include <PageCache.h>
#include <PlatformKeyboardEvent.h>
#include <PlatformMouseEvent.h>
#include <PlatformWheelEvent.h>
#include <PluginDatabase.h>
#include <PluginInfoStore.h>
#include <PluginView.h>
#include <ProgressTracker.h>
#include <ResourceHandle.h>
#include <ResourceHandleClient.h>
#include <SelectionController.h>
#include <Settings.h>
#include <SimpleFontData.h>
#include <TypingCommand.h>

#include <collector.h>
#include <JSCell.h>
#include <JSValue.h>
#include <wtf/HashSet.h>
#include "InitializeThreading.h"

#include "owb-config.h"
#include "FileIO.h"
#include DEEPSEE_INCLUDE

#if PLATFORM(MACPORT)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif PLATFORM(AMIGAOS4)
#include <proto/exec.h>
#include <intuition/intuition.h>
#else
#include <sys/sysinfo.h>
#endif

using namespace WebCore;
using namespace WebCore::EventNames;
using std::min;
using std::max;

//static HMODULE accessibilityLib;

WebView* kit(Page* page)
{
    return page ? static_cast<WebChromeClient*>(page->chrome()->client())->webView() : 0;
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

WebView::WebView()
: /*m_hostWindow(0)
,*/ m_viewWindow(0)
, m_mainFrame(0)
, m_page(0)
, m_policyDelegate(0)
, m_downloadDelegate(0)
, m_preferences(0)
#if ENABLE(INSPECTOR)
, m_webInspector(0)
#endif
, m_userAgentOverridden(false)
, m_useBackForwardList(true)
, m_zoomMultiplier(1.0f)
, m_zoomMultiplierIsTextOnly(true)
, m_mouseActivated(false)
//, m_dragData(0)
//, m_currentCharacterCode(0)
, m_isBeingDestroyed(false)
, m_paintCount(0)
, m_hasSpellCheckerDocumentTag(false)
, m_smartInsertDeleteEnabled(false)
, m_didClose(false)
, m_hasCustomDropTarget(false)
, m_inIMEComposition(0)
, m_toolTipHwnd(0)
, m_deleteBackingStoreTimerActive(false)
, m_transparent(false)
, m_closeWindowTimer(this, &WebView::closeWindowTimerFired)
, m_topLevelParent(0)
, d(new WebViewPrivate(this))
{
    JSC::initializeThreading();
    WebCore::InitializeLoggingChannelsIfNecessary();

    m_backingStoreSize.setX(0);
    m_backingStoreSize.setY(0);

    initializeStaticObservers();

    WebPreferences* sharedPreferences = WebPreferences::sharedStandardPreferences();
    continuousSpellCheckingEnabled = sharedPreferences->continuousSpellCheckingEnabled();
    grammarCheckingEnabled = sharedPreferences->grammarCheckingEnabled();

    OWBAL::ObserverServiceData::createObserverService()->registerObserver("PopupMenuShow", this);
    OWBAL::ObserverServiceData::createObserverService()->registerObserver("PopupMenuHide", this);
    DS_CONSTRUCT();
}

WebView::~WebView()
{
    close();
    
    if (m_mainFrame)
        delete m_mainFrame;
    if (m_preferences)
#if PLATFORM(AMIGAOS4)
      if (m_preferences != WebPreferences::sharedStandardPreferences())
#endif
        delete m_preferences;
    if (m_policyDelegate)
        m_policyDelegate = 0;
    if (m_downloadDelegate)
        delete m_downloadDelegate;
#if ENABLE(INSPECTOR)
    if (m_webInspector)
        delete m_webInspector;
#endif
    if (d)
        delete d;
    DS_DESTRUCT();
}

WebView* WebView::createInstance()
{
    WebView* instance = new WebView();
    return instance;
}

void initializeStaticObservers()
{
    static bool initialized;
    if (initialized)
        return;
    initialized = true;

    OWBAL::ObserverServiceData::createObserverService()->registerObserver(WebPreferences::webPreferencesChangedNotification(), PreferencesChangedOrRemovedObserver::sharedInstance());
    OWBAL::ObserverServiceData::createObserverService()->registerObserver(WebPreferences::webPreferencesRemovedNotification(), PreferencesChangedOrRemovedObserver::sharedInstance());
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
#elif !PLATFORM(AMIGAOS4)
    struct sysinfo info;
    memset(&info, 0, sizeof(info));
    if (sysinfo(&info) == 0)
        memSize = info.totalram * info.mem_unit / (1024 * 1024);
#endif

    unsigned cacheTotalCapacity = 0;
    unsigned cacheMinDeadCapacity = 0;
    unsigned cacheMaxDeadCapacity = 0;

    unsigned pageCacheCapacity = 0;

    switch (cacheModel) {
    case WebCacheModelDocumentViewer: {
        // Page cache capacity (in pages)
        pageCacheCapacity = 0;

        // Object cache capacities (in bytes)
        if (memSize >= 4096)
            cacheTotalCapacity = 256 * 1024 * 1024;
        else if (memSize >= 3072)
            cacheTotalCapacity = 192 * 1024 * 1024;
        else if (memSize >= 2048)
            cacheTotalCapacity = 128 * 1024 * 1024;
        else if (memSize >= 1536)
            cacheTotalCapacity = 86 * 1024 * 1024;
        else if (memSize >= 1024)
            cacheTotalCapacity = 64 * 1024 * 1024;
        else if (memSize >= 512)
            cacheTotalCapacity = 32 * 1024 * 1024;
        else if (memSize >= 256)
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
        if (memSize >= 4096)
            cacheTotalCapacity = 256 * 1024 * 1024;
        else if (memSize >= 3072)
            cacheTotalCapacity = 192 * 1024 * 1024;
        else if (memSize >= 2048)
            cacheTotalCapacity = 128 * 1024 * 1024;
        else if (memSize >= 1536)
            cacheTotalCapacity = 86 * 1024 * 1024;
        else if (memSize >= 1024)
            cacheTotalCapacity = 64 * 1024 * 1024;
        else if (memSize >= 512)
            cacheTotalCapacity = 32 * 1024 * 1024;
        else if (memSize >= 256)
            cacheTotalCapacity = 16 * 1024 * 1024; 

        cacheMinDeadCapacity = cacheTotalCapacity / 8;
        cacheMaxDeadCapacity = cacheTotalCapacity / 4;

        break;
    }
    case WebCacheModelPrimaryWebBrowser: {
        // Page cache capacity (in pages)
        // (Research indicates that value / page drops substantially after 3 pages.)
        if (memSize >= 8192)
            pageCacheCapacity = 7;
        if (memSize >= 4096)
            pageCacheCapacity = 6;
        else if (memSize >= 2048)
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
        if (memSize >= 4096)
            cacheTotalCapacity = 512 * 1024 * 1024;
        else if (memSize >= 3072)
            cacheTotalCapacity = 384 * 1024 * 1024;
        else if (memSize >= 2048)
            cacheTotalCapacity = 256 * 1024 * 1024;
        else if (memSize >= 1536)
            cacheTotalCapacity = 172 * 1024 * 1024;
        else if (memSize >= 1024)
            cacheTotalCapacity = 128 * 1024 * 1024;
        else if (memSize >= 512)
            cacheTotalCapacity = 64 * 1024 * 1024;
        else if (memSize >= 256)
            cacheTotalCapacity = 32 * 1024 * 1024; 

        cacheMinDeadCapacity = cacheTotalCapacity / 4;
        cacheMaxDeadCapacity = cacheTotalCapacity / 2;

        // This code is here to avoid a PLT regression. We can remove it if we
        // can prove that the overall system gain would justify the regression.
        cacheMaxDeadCapacity = max(24u, cacheMaxDeadCapacity);

        break;
    }
    default:
        ASSERT_NOT_REACHED();
    };

    cache()->setCapacities(cacheMinDeadCapacity, cacheMaxDeadCapacity, cacheTotalCapacity);
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
    //Purge page cache
    //The easiest way to do that is to disable page cache
    if (m_preferences->usesPageCache())
        m_page->settings()->setUsesPageCache(false);
    
    removeFromAllWebViewsSet();

    Frame* frame = m_page->mainFrame();
    if (frame)
        frame->loader()->detachFromParent();

#if ENABLE(INSPECTOR)
    if (m_webInspector)
        m_webInspector->webViewClosed();
#endif

    delete m_page;
    m_page = 0;

#if ENABLE(ICON_DATABASE)
    registerForIconNotification(false);
#endif
    OWBAL::ObserverServiceData::createObserverService()->removeObserver(WebPreferences::webPreferencesChangedNotification(), this);
    OWBAL::ObserverServiceData::createObserverService()->removeObserver("PopupMenuShow", this);
    OWBAL::ObserverServiceData::createObserverService()->removeObserver("PopupMenuHide", this);
    String identifier = m_preferences->identifier();
    if (identifier != String())
        WebPreferences::removeReferenceForIdentifier(identifier);

    WebPreferences* preferences = m_preferences;
    preferences->didRemoveFromWebView();

    deleteBackingStore();
}

void WebView::deleteBackingStore()
{
    if (m_deleteBackingStoreTimerActive) {
        m_deleteBackingStoreTimerActive = false;
    }
    m_backingStoreBitmap.clear();
    m_backingStoreDirtyRegion.setX(0);
    m_backingStoreDirtyRegion.setY(0);
    m_backingStoreDirtyRegion.setWidth(0);
    m_backingStoreDirtyRegion.setHeight(0);

    m_backingStoreSize.setX(0);
    m_backingStoreSize.setY(0);
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
        BITMAPINFO bitmapInfo;
        bitmapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
        bitmapInfo.bmiHeader.biWidth         = width; 
        bitmapInfo.bmiHeader.biHeight        = -height;
        bitmapInfo.bmiHeader.biPlanes        = 1;
        bitmapInfo.bmiHeader.biBitCount      = 32;
        bitmapInfo.bmiHeader.biCompression   = BI_RGB;
        bitmapInfo.bmiHeader.biSizeImage     = 0;
        bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
        bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
        bitmapInfo.bmiHeader.biClrUsed       = 0;
        bitmapInfo.bmiHeader.biClrImportant  = 0;

        void* pixels = NULL;
        m_backingStoreBitmap.set(::CreateDIBSection(NULL, &bitmapInfo, DIB_RGB_COLORS, &pixels, NULL, 0));
        return true;
    }*/

    return false;
}

void WebView::addToDirtyRegion(const IntRect& dirtyRect)
{
    m_backingStoreDirtyRegion.unite(dirtyRect);
}

IntRect WebView::dirtyRegion()
{
    return m_backingStoreDirtyRegion;
}

void WebView::clearDirtyRegion()
{
    m_backingStoreDirtyRegion.setX(0);
    m_backingStoreDirtyRegion.setY(0);
    m_backingStoreDirtyRegion.setWidth(0);
    m_backingStoreDirtyRegion.setHeight(0);
}

void WebView::scrollBackingStore(FrameView* frameView, int dx, int dy, const IntRect& scrollViewRect, const IntRect& clipRect)
{
#if PLATFORM(AMIGAOS4)
    m_backingStoreDirtyRegion.move(dx, dy);
#endif
    /*printf("scrollViewRect dx=%d dy=%d scrollview x=%d y=%d w=%d h=%d clip x=%d y=%d w=%d h=%d\n", dx, dy, scrollViewRect.x(), scrollViewRect.y(), scrollViewRect.width(), scrollViewRect.height(), clipRect.x(), clipRect.y(), clipRect.width(), clipRect.height());
    
    IntSize offsetIntSize = m_page->mainFrame()->view()->scrollOffset();
    printf("offsetIntSize w=%d h=%d\n", offsetIntSize.width(), offsetIntSize.height());*/
    //frameView->scrollBackingStore(dx, dy, scrollViewRect, clipRect);
}

void WebView::updateBackingStore(FrameView* frameView)
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

IntRect WebView::frameRect()
{
    IntRect r(d->frameRect());
    return d->frameRect();
}

void WebView::closeWindowSoon()
{
    m_closeWindowTimer.startOneShot(0);
}

void WebView::closeWindowTimerFired(WebCore::Timer<WebView>*)
{
    closeWindow();
}

void WebView::closeWindow()
{
}

bool WebView::canHandleRequest(const WebCore::ResourceRequest& request)
{
    // On the mac there's an about url protocol implementation but CFNetwork doesn't have that.
    if (equalIgnoringCase(String(request.url().protocol()), "about"))
        return true;

    return true;
}

Page* WebView::page()
{
    return m_page;
}

static const unsigned CtrlKey = 1 << 0;
static const unsigned AltKey = 1 << 1;
static const unsigned ShiftKey = 1 << 2;


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
    { VK_RETURN, AltKey | ShiftKey,  "InsertNewline"                               },

    // It's not quite clear whether clipboard shortcuts and Undo/Redo should be handled
    // in the application or in WebKit. We chose WebKit.
    { 'C',       CtrlKey,            "Copy"                                        },
    { 'V',       CtrlKey,            "Paste"                                       },
    { 'X',       CtrlKey,            "Cut"                                         },
    { 'A',       CtrlKey,            "SelectAll"                                   },
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
    { '\r',   AltKey | ShiftKey,  "InsertNewline"                               },
    { 9999999,   0,               ""                                         },
};

const char* WebView::interpretKeyEvent(const KeyboardEvent* evt)
{
    ASSERT(evt->type() == keydownEvent || evt->type() == keypressEvent);

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

    if (evt->type() == keydownEvent) {
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

const String& WebView::userAgentForKURL(const KURL&)
{
    if (m_userAgentOverridden)
        return m_userAgentCustom;

    m_userAgentStandard = getenv("OWB_USER_AGENT");

    if (!m_userAgentStandard.length())
#if PLATFORM(MACPORT)
        //We use the user agent from safari to avoid the rejection from google services (google docs, gmail, etc...)
        m_userAgentStandard =  "Mozilla/5.0 (Macintosh; U; Intel Mac OS X; fr) AppleWebKit/522.11 (KHTML, like Gecko) Safari/412 OWB/Doduo";
#elif PLATFORM(AMIGAOS4)
//        m_userAgentStandard =  "Mozilla/5.0 (AMIGA; U; AmigaOS4 ppc; en-US) AppleWebKit/420+ (KHTML, like Gecko) Safari/412 OWB/Doduo";
        if (IExec->Data.LibBase->lib_Version < 53)
            m_userAgentStandard = "Mozilla/5.0 (compatible; Origyn Web Browser; AmigaOS 4.0; PPC; U) AppleWebKit/525.1+ (KHTML, like Gecko, Safari/525.1+)";
        else
            m_userAgentStandard = "Mozilla/5.0 (compatible; Origyn Web Browser; AmigaOS 4.1; PPC; U) AppleWebKit/525.1+ (KHTML, like Gecko, Safari/525.1+)";
#else
        //NOTE: some pages don't render with this UA
        //m_userAgentStandard = "Mozilla/5.0 (iPod; U; CPU like Mac OS X; fr) AppleWebKit/420.1 (KHTML, like Gecko) Version/3.0 Mobile/3B48b Safari/419.3";
        m_userAgentStandard = "Mozilla/5.0 (X11; U; Linux i686; en-US) AppleWebKit/525.1+ (KHTML, like Gecko, Safari/525.1+)";
        /*m_userAgentStandard = String::format("Mozilla/5.0 (Windows; U; %s; %s) AppleWebKit/%s (KHTML, like Gecko)%s%s", osVersion().latin1().data(), defaultLanguage().latin1().data(), webKitVersion().latin1().data(), (m_applicationName.length() ? " " : ""), m_applicationName.latin1().data());*/
#endif

    return m_userAgentStandard;
}

bool WebView::canShowMIMEType(String mimeType)
{
    return MIMETypeRegistry::isSupportedImageMIMEType(mimeType) ||
        MIMETypeRegistry::isSupportedNonImageMIMEType(mimeType) ||
        PluginInfoStore::supportsMIMEType(mimeType);
}

bool WebView::canShowMIMETypeAsHTML(String /*mimeType*/)
{
    // FIXME
    return true;
}

void WebView::setMIMETypesShownAsHTML(String /*mimeTypes*/, int /*cMimeTypes*/)
{
}

void WebView::initWithFrame(IntRect frame, String frameName, String groupName)
{
    if (m_page)
        return;

    if(!m_viewWindow)
        m_viewWindow = d->createWindow(frame.x(), frame.y(), frame.width(), frame.height());
    else
        d->setFrameRect(frame);

    WebPreferences* sharedPreferences = WebPreferences::sharedStandardPreferences();
    sharedPreferences->willAddToWebView();
    m_preferences = sharedPreferences;

    m_page = new Page(new WebChromeClient(this), new WebContextMenuClient(this), new WebEditorClient(this), new WebDragClient(this), new WebInspectorClient(this));

    WebFrame* webFrame = WebFrame::createInstance();
    RefPtr<Frame> coreFrame = webFrame->init(this, m_page, 0);
    m_mainFrame = webFrame;
    //webFrame->Release(); // The WebFrame is owned by the Frame, so release our reference to it.

    coreFrame->tree()->setName(frameName);
    coreFrame->init();
    setGroupName(groupName);

    WebCore::FrameView * frameView = core(webFrame)->view();
    frameView->setContainingWindow(m_viewWindow);
    d->initWithFrameView(frameView);

#if ENABLE(DATABASE)
    WebKitSetWebDatabasesPathIfNecessary();
#endif

    addToAllWebViewsSet();

    initializeToolTipWindow();
    windowAncestryDidChange();

    OWBAL::ObserverServiceData::createObserverService()->registerObserver(WebPreferences::webPreferencesChangedNotification(), this);
    
    m_preferences->postPreferencesChangesNotification();

    setSmartInsertDeleteEnabled(true);

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

    ::SetWindowPos(m_toolTipHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);*/
}

void WebView::setToolTip(const String& toolTip)
{
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

#if ENABLE(ICON_DATABASE)
void WebView::notifyDidAddIcon()
{
    dispatchDidReceiveIconFromWebFrame(m_mainFrame);
}

void WebView::registerForIconNotification(bool listen)
{
    if (listen)
        OWBAL::ObserverServiceData::createObserverService()->registerObserver(WebIconDatabase::iconDatabaseDidAddIconNotification(), this);
    else
        OWBAL::ObserverServiceData::createObserverService()->removeObserver(WebIconDatabase::iconDatabaseDidAddIconNotification(), this);
}

void WebView::dispatchDidReceiveIconFromWebFrame(WebFrame* frame)
{
    registerForIconNotification(false);
}
#endif

void WebView::setDownloadDelegate(DefaultDownloadDelegate* d)
{
    m_downloadDelegate = d;
}

DefaultDownloadDelegate* WebView::downloadDelegate()
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

WebFrame* WebView::mainFrame()
{
    return m_mainFrame;
}

WebFrame *WebView::focusedFrame()
{
    Frame* f = m_page->focusController()->focusedFrame();
    if (!f)
        return 0;

    WebFrame* webFrame = kit(f);
    return webFrame;
}
WebBackForwardList* WebView::backForwardList()
{
    return WebBackForwardList::createInstance(m_page->backForwardList());
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

bool WebView::goToBackForwardItem(WebHistoryItem* item)
{
    m_page->goToItem(item->historyItem(), FrameLoadTypeIndexedBackForward);
    return true;
}

void WebView::setTextSizeMultiplier(float multiplier)
{
    setZoomMultiplier(multiplier, true);
}

void WebView::setPageSizeMultiplier(float multiplier)
{
    setZoomMultiplier(multiplier, false);
}

void WebView::setZoomMultiplier(float multiplier, bool isTextOnly)
{
    m_zoomMultiplier = multiplier;
    m_zoomMultiplierIsTextOnly = isTextOnly;
    if (Frame* coreFrame = core(m_mainFrame))
        coreFrame->setZoomFactor(multiplier, isTextOnly);
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
    if (isTextOnly != m_zoomMultiplierIsTextOnly)
        return 1.0f;
    return m_zoomMultiplier;
}

// FIXME: This code should move into WebCore so it can be shared by all the WebKits.
#define MinimumZoomMultiplier   0.5f
#define MaximumZoomMultiplier   3.0f
#define ZoomMultiplierRatio     1.2f

bool WebView::canZoomIn(bool isTextOnly)
{
    return zoomMultiplier(isTextOnly) * ZoomMultiplierRatio < MaximumZoomMultiplier;
}

bool WebView::canZoomOut(bool isTextOnly)
{
    return zoomMultiplier(isTextOnly) / ZoomMultiplierRatio > MinimumZoomMultiplier;
}

void WebView::zoomIn(bool isTextOnly)
{
    if (!canZoomIn(isTextOnly))
        return ;
    setZoomMultiplier(zoomMultiplier(isTextOnly) * ZoomMultiplierRatio, isTextOnly);
}

void WebView::zoomOut(bool isTextOnly)
{
    if (!canZoomOut(isTextOnly))
        return ;
    setZoomMultiplier(zoomMultiplier(isTextOnly) / ZoomMultiplierRatio, isTextOnly);
}

bool WebView::canZoomPageIn()
{
    return canZoomIn(false);
}

void WebView::zoomPageIn()
{
    zoomIn(false);
}

bool WebView::canZoomPageOut()
{
    return canZoomOut(false);
}

void WebView::zoomPageOut()
{
    zoomOut(false);
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

bool WebView::canMakeTextLarger()
{
    return canZoomIn(true);
}

void WebView::makeTextLarger()
{
    zoomIn(true);
}

bool WebView::canMakeTextSmaller()
{
    return canZoomOut(true);
}

void WebView::makeTextSmaller()
{
    zoomOut(true);
}

bool WebView::canMakeTextStandardSize()
{
    return canResetZoom(true);
}

void WebView::makeTextStandardSize()
{
    resetZoom(true);
}



void WebView::setApplicationNameForUserAgent(String applicationName)
{
    m_applicationName = applicationName;
}

String WebView::applicationNameForUserAgent()
{
    return m_applicationName;
}

void WebView::setCustomUserAgent(String userAgentString)
{
    m_userAgentOverridden = true;
    m_userAgentCustom = userAgentString;
}

String WebView::customUserAgent()
{
    return m_userAgentCustom;
}

String WebView::userAgentForURL(String url)
{
    return this->userAgentForKURL(KURL(url));
}

bool WebView::supportsTextEncoding()
{
    return true;
}

void WebView::setCustomTextEncodingName(String encodingName)
{
    if (!m_mainFrame)
        return ;

    String oldEncoding = customTextEncodingName();

    if (oldEncoding != encodingName) {
        if (Frame* coreFrame = core(m_mainFrame))
            coreFrame->loader()->reloadAllowingStaleData(encodingName);
    }
}

String WebView::customTextEncodingName()
{
    WebDataSource* dataSource;

    if (!m_mainFrame)
        return String();

    dataSource = m_mainFrame->provisionalDataSource();
    if (!dataSource) {
        dataSource = m_mainFrame->dataSource();
        if (!dataSource)
            return String();
    }

    return dataSource->documentLoader()->overrideEncoding();
}

void WebView::setMediaStyle(String /*media*/)
{
}

String WebView::mediaStyle()
{
    return String();
}

String WebView::stringByEvaluatingJavaScriptFromString(String script)
{
    Frame* coreFrame = core(m_mainFrame);
    if (!coreFrame)
        return String();

    JSC::JSCell* scriptExecutionResult = static_cast<JSC::JSCell*> (coreFrame->loader()->executeScript(script, false));
    if(!scriptExecutionResult)
        return String();
    else if (scriptExecutionResult->isString())
        return scriptExecutionResult->getString();
    return String();
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

    OWBAL::ObserverServiceData::createObserverService()->removeObserver(WebPreferences::webPreferencesChangedNotification(), this);

    String identifier = oldPrefs->identifier();
    oldPrefs->didRemoveFromWebView();
    oldPrefs = 0; // Make sure we release the reference, since WebPreferences::removeReferenceForIdentifier will check for last reference to WebPreferences
    
    WebPreferences::removeReferenceForIdentifier(identifier);

    m_preferences = prefs;

    OWBAL::ObserverServiceData::createObserverService()->registerObserver(WebPreferences::webPreferencesChangedNotification(), this);

    m_preferences->postPreferencesChangesNotification();
}

WebPreferences* WebView::preferences()
{
    return m_preferences;
}

void WebView::setPreferencesIdentifier(String /*anIdentifier*/)
{
}

String WebView::preferencesIdentifier()
{
    return String();
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

bool WebView::searchFor(String str, bool forward, bool caseFlag, bool wrapFlag)
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
    mainFrame->selection()->setFocused(active && mainFrame == focusedFrame);
}

String buffer(String url)
{
    String path;
    if (url.isEmpty()) {
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

void WebView::parseConfigFile(String url)
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


void WebView::executeCoreCommandByName(String name, String value)
{
    m_page->focusController()->focusedOrMainFrame()->editor()->command(name).execute(value);
}

unsigned int WebView::markAllMatchesForText(String str, bool caseSensitive, bool highlight, unsigned int limit)
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

Vector<IntRect> WebView::rectsForTextMatches()
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
}

WebCore::Image* WebView::generateSelectionImage(bool forceWhiteText)
{
    WebCore::Frame* frame = m_page->focusController()->focusedOrMainFrame();

    if (frame) {
        /*HBITMAP bitmap = imageFromSelection(frame, forceWhiteText ? TRUE : FALSE);
        *hBitmap = (OLE_HANDLE)(ULONG64)bitmap;*/
    }

    return 0;
}

IntRect WebView::selectionRect()
{
    WebCore::Frame* frame = m_page->focusController()->focusedOrMainFrame();

    if (frame) {
        IntRect ir = enclosingIntRect(frame->selectionRect());
        ir = frame->view()->convertToContainingWindow(ir);
        ir.move(-frame->view()->scrollOffset().width(), -frame->view()->scrollOffset().height());
        return ir;
    }

    return IntRect();
}

void WebView::setGroupName(String groupName)
{
    if (!m_page)
        return ;
    m_page->setGroupName(groupName);
}
    
String WebView::groupName()
{
    if (!m_page)
        return String();
    return m_page->groupName();
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

WebElementPropertyBag* WebView::elementAtPoint(WebCore::IntPoint point)
{
    Frame* frame = core(m_mainFrame);
    if (!frame)
        return 0;

    IntPoint webCorePoint(point.x(), point.y());
    HitTestResult result = HitTestResult(webCorePoint);
    if (frame->contentRenderer())
        result = frame->eventHandler()->hitTestResultAtPoint(webCorePoint, false);
    return WebElementPropertyBag::createInstance(result);
}

String WebView::selectedText()
{
    Frame* focusedFrame = (m_page && m_page->focusController()) ? m_page->focusController()->focusedOrMainFrame() : 0;
    if (!focusedFrame)
        return String();

    return focusedFrame->selectedText();
}

void WebView::centerSelectionInVisibleArea()
{
    Frame* coreFrame = core(m_mainFrame);
    if (!coreFrame)
        return ;

    coreFrame->revealSelection(RenderLayer::gAlignCenterAlways);
}


void WebView::moveDragCaretToPoint(IntPoint /*point*/)
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

void WebView::setMainFrameURL(String /*urlString*/)
{
}

String WebView::mainFrameURL()
{
    return m_mainFrame->url();
}

DOMDocument* WebView::mainFrameDocument()
{
    return m_mainFrame->domDocument();
}

String WebView::mainFrameTitle()
{
    return m_mainFrame->name();
}

void WebView::registerURLSchemeAsLocal(String scheme)
{
    FrameLoader::registerURLSchemeAsLocal(scheme);
}

void WebView::setSmartInsertDeleteEnabled(bool flag)
{
    m_smartInsertDeleteEnabled = !!flag;
}
    
bool WebView::smartInsertDeleteEnabled()
{
    return m_smartInsertDeleteEnabled ? true : false;
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

void WebView::replaceSelectionWithText(String text)
{
    Position start = m_page->mainFrame()->selection()->selection().start();
    m_page->focusController()->focusedOrMainFrame()->editor()->insertText(text, 0);
    m_page->mainFrame()->selection()->setBase(start);
}
    
void WebView::replaceSelectionWithMarkupString(String /*markupString*/)
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

void WebView::copyURL(String url)
{
    m_page->focusController()->focusedOrMainFrame()->editor()->copyURL(KURL(url), "");
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

void WebView::observe(const WebCore::String &topic, const WebCore::String &data, void *userData)
{
#if ENABLE(ICON_DATABASE)
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

}

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
        settings->setUserStyleSheetLocation(KURL(str));
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

    settings->setShowsURLsInToolTips(false);
    settings->setForceFTPDirectoryListings(true);
    settings->setDeveloperExtrasEnabled(developerExtrasEnabled());
    settings->setNeedsSiteSpecificQuirks(s_allowSiteSpecificHacks);

    FontSmoothingType smoothingType = preferences->fontSmoothing();
    settings->setFontRenderingMode(smoothingType != FontSmoothingTypeWindows ? NormalRenderingMode : AlternateRenderingMode);

    enabled = preferences->authorAndUserStylesEnabled();
    settings->setAuthorAndUserStylesEnabled(enabled);

    if (!m_closeWindowTimer.isActive())
        m_mainFrame->invalidate(); // FIXME

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

IntPoint WebView::scrollOffset()
{
    IntSize offsetIntSize = m_page->mainFrame()->view()->scrollOffset();
    return IntPoint(offsetIntSize.width(), offsetIntSize.height());
}

void WebView::scrollBy(IntPoint offset)
{
    m_page->mainFrame()->view()->scrollBy(offset.x(), offset.y());
}

IntRect WebView::visibleContentRect()
{
    FloatRect visibleContent = m_page->mainFrame()->view()->visibleContentRect();
    return IntRect((int)visibleContent.x(), (int)visibleContent.y(), (int)visibleContent.right(), (int)visibleContent.bottom());
}


bool WebView::canHandleRequest(WebMutableURLRequest *request)
{
    WebMutableURLRequest* requestImpl;

    return !!canHandleRequest(requestImpl->resourceRequest());
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

void WebView::addAdditionalPluginDirectory(String directory)
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

    m_page->mainFrame()->setProhibitsScrolling(b);
}

void WebView::setShouldApplyMacFontAscentHack(bool b)
{
    //SimpleFontData::setShouldApplyMacAscentHack(b);
}


#if ENABLE(INSPECTOR)
WebInspector* WebView::inspector()
{
    if (!m_webInspector)
        m_webInspector = WebInspector::createInstance(this);

    return m_webInspector;
}
#endif

void WebView::windowAncestryDidChange()
{
    updateActiveState();
}

void WebView::paintDocumentRectToContext(IntRect rect, PlatformGraphicsContext *pgc)
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
    frame->paint(&gc, rect);
    gc.restore();
}

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

WebCore::Image* WebView::backingStore()
{
    WebCore::Image* hBitmap = m_backingStoreBitmap.get();
    return hBitmap;
}

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

Page* core(WebView* webView)
{
    Page* page = 0;

    page = webView->page();

    return page;
}

