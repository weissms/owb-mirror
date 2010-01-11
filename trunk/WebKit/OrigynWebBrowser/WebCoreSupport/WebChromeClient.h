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

#include <ChromeClient.h>
#include <FocusDirection.h>
#include <GraphicsContext.h>
#include <ScrollTypes.h>
#include <wtf/Forward.h>

class WebDesktopNotificationsDelegate;
class WebView;

#if USE(ACCELERATED_COMPOSITING)
namespace WebCore {
    class GraphicsLayer;
}
#endif

class WebChromeClient : public WebCore::ChromeClient {
public:
    WebChromeClient(WebView*);
    ~WebChromeClient();

    virtual void chromeDestroyed();

    virtual void setWindowRect(const WebCore::FloatRect&);
    virtual WebCore::FloatRect windowRect();
    
    virtual WebCore::FloatRect pageRect();
    
    virtual float scaleFactor();

    virtual void focus();
    virtual void unfocus();

    virtual bool canTakeFocus(WebCore::FocusDirection);
    virtual void takeFocus(WebCore::FocusDirection);

    void focusedNodeChanged(WebCore::Node*);

    virtual WebCore::Page* createWindow(WebCore::Frame*, const WebCore::FrameLoadRequest&, const WebCore::WindowFeatures&);
    virtual void show();

    virtual bool canRunModal();
    virtual void runModal();

    virtual void setToolbarsVisible(bool);
    virtual bool toolbarsVisible();
    
    virtual void setStatusbarVisible(bool);
    virtual bool statusbarVisible();
    
    virtual void setScrollbarsVisible(bool);
    virtual bool scrollbarsVisible();
    
    virtual void setMenubarVisible(bool);
    virtual bool menubarVisible();

    virtual void setResizable(bool);

    virtual void addMessageToConsole(WebCore::MessageSource source, WebCore::MessageType type, WebCore::MessageLevel level, const WebCore::String& message, unsigned line, const WebCore::String& url);

    virtual bool canRunBeforeUnloadConfirmPanel();
    virtual bool runBeforeUnloadConfirmPanel(const WebCore::String& message, WebCore::Frame* frame);

    virtual void closeWindowSoon();

    virtual void runJavaScriptAlert(WebCore::Frame*, const WebCore::String&);
    virtual bool runJavaScriptConfirm(WebCore::Frame*, const WebCore::String&);
    virtual bool runJavaScriptPrompt(WebCore::Frame*, const WebCore::String& message, const WebCore::String& defaultValue, WebCore::String& result);
    virtual void setStatusbarText(const WebCore::String&);
    virtual bool shouldInterruptJavaScript();

    virtual bool tabsToLinks() const;
    virtual WebCore::IntRect windowResizerRect() const;

    virtual void repaint(const WebCore::IntRect&, bool contentChanged, bool immediate = false, bool repaintContentOnly = false);
    virtual void scroll(const WebCore::IntSize& scrollDelta, const WebCore::IntRect& rectToScroll, const WebCore::IntRect& clipRect);
    virtual WebCore::IntPoint screenToWindow(const WebCore::IntPoint& p) const ;
    virtual WebCore::IntRect windowToScreen(const WebCore::IntRect& r) const;
    virtual PlatformPageClient platformPageClient() const;
    virtual void contentsSizeChanged(WebCore::Frame*, const WebCore::IntSize&) const;

    virtual void scrollbarsModeDidChange() const;
    virtual void mouseDidMoveOverElement(const WebCore::HitTestResult&, unsigned modifierFlags);

    virtual void setToolTip(const WebCore::String&, WebCore::TextDirection);

    virtual void print(WebCore::Frame*);

    virtual void exceededDatabaseQuota(WebCore::Frame*, const WebCore::String&);

#if ENABLE(OFFLINE_WEB_APPLICATIONS)
    virtual void reachedMaxAppCacheSize(int64_t spaceNeeded);
#endif

#if ENABLE(DASHBOARD_SUPPORT)
    virtual void dashboardRegionsChanged();
#endif

    virtual void populateVisitedLinks();

    virtual bool paintCustomScrollbar(WebCore::GraphicsContext*, const WebCore::FloatRect&, WebCore::ScrollbarControlSize,
                                        WebCore::ScrollbarControlState, WebCore::ScrollbarPart pressedPart, bool vertical,
                                        float value, float proportion, WebCore::ScrollbarControlPartMask);
    virtual bool paintCustomScrollCorner(WebCore::GraphicsContext*, const WebCore::FloatRect&);
      
    virtual void runOpenPanel(WebCore::Frame*, PassRefPtr<WebCore::FileChooser>);

    virtual bool setCursor(WebCore::PlatformCursorHandle cursor);

    WebView* webView() const { return m_webView; }

    virtual void formStateDidChange(const WebCore::Node*) { }

    virtual PassOwnPtr<WebCore::HTMLParserQuirks> createHTMLParserQuirks() { return 0; }

    virtual void scrollRectIntoView(const WebCore::IntRect&, const WebCore::ScrollView*) const {}

    virtual void requestGeolocationPermissionForFrame(WebCore::Frame*, WebCore::Geolocation*);
    
#if ENABLE(VIDEO)
    virtual bool supportsFullscreenForNode(const WebCore::Node*);
    virtual void enterFullscreenForNode(WebCore::Node*);
    virtual void exitFullscreenForNode(WebCore::Node*);
#endif

#if ENABLE(NOTIFICATIONS)
    virtual WebCore::NotificationPresenter* notificationPresenter() const { return reinterpret_cast<WebCore::NotificationPresenter*>(m_notificationsDelegate.get()); }
#endif

#if USE(ACCELERATED_COMPOSITING)
    // Pass 0 as the GraphicsLayer to detatch the root layer.
    virtual void attachRootGraphicsLayer(WebCore::Frame*, WebCore::GraphicsLayer*);
    // Sets a flag to specify that the next time content is drawn to the window,
    // the changes appear on the screen in synchrony with updates to GraphicsLayers.
    virtual void setNeedsOneShotDrawingSynchronization();
    // Sets a flag to specify that the view needs to be updated, so we need
    // to do an eager layout before the drawing.
    virtual void scheduleCompositingLayerSync();
#endif

private:
    WebView* m_webView;
#if ENABLE(NOTIFICATIONS)
    OwnPtr<WebDesktopNotificationsDelegate> m_notificationsDelegate;
#endif
};
