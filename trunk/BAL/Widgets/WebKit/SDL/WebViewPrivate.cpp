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
#include "WebViewPrivate.h"
#include "IntRect.h"
#include "FrameView.h"
#include "Frame.h"
#include "SDL.h"
#include "GraphicsContext.h"
#include "Page.h"
#include "EventHandler.h"
#include "FocusController.h"
#include "HitTestResult.h"
#include "HitTestRequest.h"
#include <MainThread.h>
#include <PlatformKeyboardEvent.h>
#include <PlatformMouseEvent.h>
#include <PlatformWheelEvent.h>
#include "SelectionController.h"
#include <SharedTimer.h>
#include "Editor.h"
#include "owb-config.h"
#include "PopupMenu.h"
#include "CString.h"
#include "FileIO.h"
#include "WebFrame.h"
#include "WebView.h"
#include "WebWindowAlert.h"

#if ENABLE(DAE_APPLICATION)
#include "WebApplication.h"
#include "WebApplicationManager.h"
#endif

#if PLATFORM(SDL)
#include "WebPopup.h"
#endif

using namespace WebCore;

WebViewPrivate::WebViewPrivate(WebView *webView)
    : m_webView(webView)
    , isInitialized(false)
    , m_shouldDestroyViewWindow(false)
    , m_scrollSurface(0)
    , m_clickCount(0)
    , m_closeWindowTimer(this, &WebViewPrivate::closeWindowTimerFired)
{
    gettimeofday(&m_timerClick, NULL);
}

WebViewPrivate::~WebViewPrivate() 
{
    if (m_shouldDestroyViewWindow)
        SDL_FreeSurface(m_webView->viewWindow());
}

void WebViewPrivate::fireWebKitTimerEvents() 
{
    while (fireTimerCount())
        fireTimerIfNeeded();
#if USE(VIDEO_GSTREAMER)
    GMainContext* glibContext = g_main_context_default();
    if (glibContext)
        g_main_context_iteration(glibContext, FALSE);
#endif
    usleep(10000);
}

void WebViewPrivate::fireWebKitThreadEvents()
{
    WTF::dispatchFunctionsFromMainThread();
}


#if PLATFORM(SDL)
BalRectangle WebViewPrivate::onExpose(BalEventExpose event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return IntRect(); 

    if(!isInitialized) {
        isInitialized = true;
        frame->view()->resize(m_rect.width(), m_rect.height());
        frame->view()->forceLayout();
        frame->view()->adjustViewSize();
    }

    if (frame->contentRenderer() && frame->view()) {
        frame->view()->layoutIfNeededRecursive();
        IntRect dirty = m_webView->dirtyRegion();
        IntRect d = dirty;

        if (dirty.isEmpty())
            return IntRect();

        m_webView->clearDirtyRegion();

        GraphicsContext ctx(m_webView->viewWindow());
        ctx.save();

        if (!m_hUpdateRect.isEmpty()) {
            frame->view()->paint(&ctx, m_hUpdateRect);
            dirty.unite(m_hUpdateRect);
            m_hUpdateRect = IntRect();
        }
        if (!m_vUpdateRect.isEmpty()) {
            frame->view()->paint(&ctx, m_vUpdateRect);
            dirty.unite(m_vUpdateRect);
            m_vUpdateRect = IntRect();
        }

        if (m_webView->transparent())
            ctx.clearRect(d);

        ctx.clip(d);
        if (m_webView->transparent())
            ctx.clearRect(d);

        frame->view()->paint(&ctx, d);

        if (m_scrollSurface) {
            SDL_Rect src = {0, 0, m_dstRect.w, m_dstRect.h};
            SDL_BlitSurface(m_scrollSurface, &src, m_webView->viewWindow(), &m_dstRect);
            dirty.unite(IntRect(m_dstRect.x, m_dstRect.y, m_dstRect.w, m_dstRect.h));
            SDL_FreeSurface(m_scrollSurface);
            m_scrollSurface = 0;
            m_dstRect.x = 0;
            m_dstRect.y = 0;
            m_dstRect.w = 0;
            m_dstRect.h = 0;
            m_srcRect.x = 0;
            m_srcRect.y = 0;
            m_srcRect.w = 0;
            m_srcRect.h = 0;
            m_scrollUpdateRect = IntRect();
        }

        ctx.restore();  
        updateView(ctx.platformContext(), dirty);
        return dirty;
    }
    return IntRect();
}
#elif PLATFORM(SDLCAIRO)
BalRectangle WebViewPrivate::onExpose(BalEventExpose event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return IntRect();

    if(!isInitialized) {
        isInitialized = true;
        frame->view()->resize(m_rect.width(), m_rect.height());
        frame->view()->forceLayout();
        frame->view()->adjustViewSize();
        SDL_Surface *sdlsurf = m_webView->viewWindow();
        cairo_surface_t* cairosurf = cairo_image_surface_create_for_data (
                                    (unsigned char*)sdlsurf->pixels,
                                    CAIRO_FORMAT_ARGB32,
                                    sdlsurf->w,
                                    sdlsurf->h,
                                    sdlsurf->pitch);
        m_cr = cairo_create(cairosurf);
    }

    if (frame->contentRenderer() && frame->view()) {
        frame->view()->layoutIfNeededRecursive();
        IntRect dirty = m_webView->dirtyRegion();
        
        m_webView->clearDirtyRegion();

        if (dirty.isEmpty())
            return IntRect();

        while (SDL_LockSurface (m_webView->viewWindow()) != 0)
            SDL_Delay (1);

        GraphicsContext ctx(m_cr);
        ctx.save();

#if ENABLE(DAE_APPLICATION)
        SharedPtr<WebApplication> app = webAppMgr().application(m_webView);
        ctx.translate(app->pos().x(), app->pos().y());
#endif

        ctx.clip(dirty);
                
#if !ENABLE(DAE_APPLICATION)
        if (m_webView->transparent())
            ctx.clearRect(dirty);
#endif

        frame->view()->paint(&ctx, dirty);

        SDL_UnlockSurface (m_webView->viewWindow());

#if !ENABLE(DAE_APPLICATION)
        updateView(m_webView->viewWindow(), dirty);
#endif

        ctx.restore();
        return dirty;
    }
    return IntRect();
}
#else
#error "platform not supported"
#endif

#include "CString.h"

bool WebViewPrivate::onKeyDown(BalEventKey event)
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (!frame)
        return false;

    PlatformKeyboardEvent keyboardEvent(&event);
    bool eventFlowAltered = false;
    bool handled = frame->eventHandler()->keyEvent(keyboardEvent, &eventFlowAltered);

#if ENABLE(DAE)
    return (handled || eventFlowAltered);
#else
    return handled;
#endif
}

bool WebViewPrivate::onKeyUp(BalEventKey event)
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (!frame)
        return false;
    
    PlatformKeyboardEvent keyboardEvent(&event);
    bool eventFlowAltered = false;
    bool handled = frame->eventHandler()->keyEvent(keyboardEvent, &eventFlowAltered);

#if ENABLE(DAE)
    return (handled || eventFlowAltered);
#else
    return handled;
#endif
}

bool WebViewPrivate::onMouseMotion(BalEventMotion event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return false;

    m_clickCount = 0;
    return frame->eventHandler()->mouseMoved(PlatformMouseEvent(&event));
}

bool WebViewPrivate::onMouseButtonDown(BalEventButton event)
{
    Frame* frame = core(m_webView->mainFrame());

    if (!frame)
        return false;

    if (event.button == SDL_BUTTON_MIDDLE)
        return false;//webkit_web_view_forward_context_menu_event(webView, PlatformMouseEvent(event));
    struct timeval timerClick;
    gettimeofday(&timerClick, NULL);
    if (m_timerClick.tv_sec == timerClick.tv_sec)
        m_clickCount++;
    else
        m_clickCount = 1;

    m_timerClick = timerClick;
    return frame->eventHandler()->handleMousePressEvent(PlatformMouseEvent(&event, m_clickCount));
}

bool WebViewPrivate::onMouseButtonUp(BalEventButton event)
{
    Frame* frame = core(m_webView->mainFrame());

    if (!frame)
        return false;

    return frame->eventHandler()->handleMouseReleaseEvent(PlatformMouseEvent(&event, 0));
}

bool WebViewPrivate::onScroll(BalEventScroll event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return false;
    PlatformWheelEvent wheelEvent(&event);
    return frame->eventHandler()->handleWheelEvent(wheelEvent);
}

void WebViewPrivate::onResize(BalResizeEvent event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;
    m_rect.setWidth(event.w);
    m_rect.setHeight(event.h);

#if PLATFORM(SDLCAIRO)
    if (m_cr)
       cairo_destroy(m_cr); 
    SDL_Surface *sdlsurf = m_webView->viewWindow();
    cairo_surface_t* cairosurf = cairo_image_surface_create_for_data (
                                (unsigned char*)sdlsurf->pixels,
                                CAIRO_FORMAT_ARGB32,
                                sdlsurf->w,
                                sdlsurf->h,
                                sdlsurf->pitch);
    m_cr = cairo_create(cairosurf);
#endif

    frame->view()->resize(event.w, event.h);
    frame->view()->forceLayout();
    frame->view()->adjustViewSize();
}

void WebViewPrivate::onQuit(BalQuitEvent)
{
}

void WebViewPrivate::onUserEvent(BalUserEvent)
{
}

void WebViewPrivate::popupMenuHide()
{
    //BOOKMARKLET_INSTALL_PATH
}


void WebViewPrivate::popupMenuShow(void *popupInfo)
{
#if PLATFORM(SDL)
    PopupMenu *pop = static_cast<PopupMenu *>(popupInfo);
    WebPopup* webPopup = new WebPopup(pop, m_webView);
    webPopup->show();
    delete webPopup;
#endif
}

void WebViewPrivate::updateView(BalWidget *surf, IntRect rect)
{
    if (!surf || rect.isEmpty())
        return;

    rect.intersect(m_rect);
    /* use SDL_Flip only if double buffering is available */
#if ENABLE(DAE_APPLICATION)
    SharedPtr<WebApplication> app = webAppMgr().application(m_webView);
    rect.move(app->pos().x(), app->pos().y());
#endif
    SDL_Rect sdlRect = {rect.x(), rect.y(), rect.width(), rect.height()};
    sdlRect.x = max(sdlRect.x, (Sint16)0);
    sdlRect.y = max(sdlRect.y, (Sint16)0);
    //printf("update rect %d %d %d %d\n", sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
    if (surf->flags & SDL_DOUBLEBUF)
        SDL_Flip(surf);
    else
        SDL_UpdateRect(surf, sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
}

void WebViewPrivate::sendExposeEvent(IntRect)
{
    SDL_Event ev;
    ev.type = SDL_VIDEOEXPOSE;
    SDL_PushEvent(&ev);
}

void WebViewPrivate::repaint(const WebCore::IntRect& windowRect, bool contentChanged, bool immediate, bool repaintContentOnly)
{
    if (windowRect.isEmpty())
        return;
    IntRect rect = windowRect;
    rect.intersect(m_rect);
    
    if (rect.isEmpty())
        return;
    
    if (contentChanged) {
        m_webView->addToDirtyRegion(rect);
#if 0
        Frame* focusedFrame = m_webView->page()->focusController()->focusedFrame();
        if (focusedFrame) {
            Scrollbar* hBar = focusedFrame->view()->horizontalScrollbar();
            Scrollbar* vBar = focusedFrame->view()->verticalScrollbar();
            
            // TODO : caculate the scroll delta and test this.
            //if (dx && hBar)
            if (hBar)
                m_webView->addToDirtyRegion(IntRect(focusedFrame->view()->windowClipRect().x() + hBar->x(), focusedFrame->view()->windowClipRect().y() + hBar->y(), hBar->width(), hBar->height()));
            //if (dy && vBar)
            if (vBar)
                m_webView->addToDirtyRegion(IntRect(focusedFrame->view()->windowClipRect().x() + vBar->x(), focusedFrame->view()->windowClipRect().y() + vBar->y(), vBar->width(), vBar->height()));
        }
#endif
    }
    if (!repaintContentOnly)
        sendExposeEvent(rect);
    if (immediate) {
        if (repaintContentOnly)
            m_webView->updateBackingStore(core(m_webView->topLevelFrame())->view());
        else
            sendExposeEvent(rect);
    }
}

void WebViewPrivate::scrollBackingStore(WebCore::FrameView* view, int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect)
{
    IntRect updateRect = clipRect;
    updateRect.intersect(scrollViewRect);
    
#if 0
    dy = -dy;
    dx = -dx;
    
    int svWidth = scrollViewRect.width();
    int svHeight = scrollViewRect.height();
    int dirtyX = 0, dirtyY = 0, dirtyW = 0, dirtyH = 0;

    if (dy == 0 && dx < 0 && -dx < svWidth) {
        dirtyW = -dx;
        dirtyH = svHeight;
    }
    else if (dy == 0 && dx > 0 && dx < svWidth) {
        dirtyX = svWidth - dx;
        dirtyW = dx;
        dirtyH = svHeight;
    }
    else if (dx == 0 && dy < 0 && -dy < svHeight) {
        dirtyW = svWidth;
        dirtyH = -dy;
    }
    else if (dx == 0 && dy > 0 && dy < svHeight) {
        dirtyY = svHeight - dy;
        dirtyW = svWidth;
        dirtyH = dy;
    }

    if (m_webView->viewWindow() && dirtyW) {
        m_srcRect.x = dx > 0 ? dx + scrollViewRect.x() : scrollViewRect.x();
        m_srcRect.y = dy > 0 ? dy + scrollViewRect.y() : scrollViewRect.y();
        m_srcRect.w = dx > 0 ? scrollViewRect.width() - dx : scrollViewRect.width() + dx;
        m_srcRect.h = dy > 0 ? scrollViewRect.height() - dy : scrollViewRect.height() + dy;

        m_dstRect.x = dx > 0 ? scrollViewRect.x() : -dx + scrollViewRect.x();
        m_dstRect.y = dy > 0 ? scrollViewRect.y() : -dy + scrollViewRect.y();
        m_dstRect.w = dx > 0 ? scrollViewRect.width() - dx : scrollViewRect.width() + dx;
        m_dstRect.h = dy > 0 ? scrollViewRect.height() -dy : scrollViewRect.height() + dy;

        int x, y, w, h = 0;
        if (dx > 0) {
            m_srcRect.x += m_scrollUpdateRect.width();
            m_srcRect.w -= m_scrollUpdateRect.width();
            x = scrollViewRect.x() + dirtyX - m_scrollUpdateRect.width();
            w = dirtyW + m_scrollUpdateRect.width();
        } else if (dx < 0) {
            m_dstRect.x += m_scrollUpdateRect.width();
            m_srcRect.w -= m_scrollUpdateRect.width();
            x = scrollViewRect.x() + dirtyX + m_scrollUpdateRect.width();
            w = dirtyW + m_scrollUpdateRect.width();
        } else {
            x = scrollViewRect.x() + dirtyX;
            w = dirtyW;
        }

        if (dy > 0) {
            m_srcRect.y += m_scrollUpdateRect.height();
            m_srcRect.h -= m_scrollUpdateRect.height();
            y = scrollViewRect.y() + dirtyY - m_scrollUpdateRect.height();
            h = dirtyH + m_scrollUpdateRect.height();
        } else if (dy < 0) {
            m_dstRect.y += m_scrollUpdateRect.height();
            m_srcRect.h -= m_scrollUpdateRect.height();
            y = scrollViewRect.y() + dirtyY + m_scrollUpdateRect.height();
            h = dirtyH + m_scrollUpdateRect.height();
        } else {
            y = scrollViewRect.y() + dirtyY;
            h = dirtyH;
        }

        Uint32 rmask, gmask, bmask, amask;
#if !OS(AMIGAOS4) && SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
#else
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
        amask = 0xff000000;
#endif

        if (m_scrollSurface)
            SDL_FreeSurface(m_scrollSurface);
        m_scrollSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, m_dstRect.w, m_dstRect.h , 32, rmask, gmask, bmask, amask);

        SDL_Rect dRect = {0, 0, m_dstRect.w, m_dstRect.h};
        SDL_BlitSurface(m_webView->viewWindow(), &m_srcRect, m_scrollSurface, &dRect);

        m_scrollUpdateRect = IntRect(x, y, w, h);
        m_scrollUpdateRect.intersect(scrollViewRect);

        Scrollbar* hBar = view->horizontalScrollbar();
        Scrollbar* vBar = view->verticalScrollbar();
        if (dx && hBar)
            m_hUpdateRect = IntRect(scrollViewRect.x() + hBar->x(), scrollViewRect.y() + hBar->y(), hBar->width(), hBar->height());
        if (dy && vBar)
            m_vUpdateRect = IntRect(scrollViewRect.x() + vBar->x(), scrollViewRect.y() + vBar->y(), vBar->width(), vBar->height());

        m_webView->addToDirtyRegion(m_scrollUpdateRect);
        sendExposeEvent(m_scrollUpdateRect);
    }
#else
    m_webView->addToDirtyRegion(updateRect);
    sendExposeEvent(updateRect);
#endif
}

BalWidget *WebViewPrivate::createWindow(BalRectangle r)
{
    m_shouldDestroyViewWindow = true;
    WebCore::IntRect rect(r);
    if(rect != m_rect)
        m_rect = rect;

    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
    on the endianness (byte order) of the machine */
#if !OS(AMIGAOS4) && SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
#endif    
    return SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, r.w, r.h, 32,
                                rmask, gmask, bmask, amask);
}

void WebViewPrivate::resize(BalRectangle r)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;
    m_rect.setWidth(r.w);
    m_rect.setHeight(r.h);

#if !ENABLE(DAE_APPLICATION)
    SDL_Surface *surf = m_webView->viewWindow();
    m_webView->setViewWindow(createWindow(r));
    SDL_FreeSurface(surf);
#endif
    
    frame->view()->resize(r.w, r.h);
    frame->view()->forceLayout();
    frame->view()->adjustViewSize();
}

void WebViewPrivate::move(BalPoint lastPos, BalPoint newPos)
{
    IntRect lastRect(lastPos, m_rect.size());
    IntRect newRect(newPos, m_rect.size());

    m_webView->addToDirtyRegion(lastRect);
    m_webView->addToDirtyRegion(newRect);

    sendExposeEvent(lastRect);
}

void WebViewPrivate::repaintAfterNavigationIfNeeded()
{
#if PLATFORM(SDL)
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;
    GraphicsContext ctx(m_webView->viewWindow());
    if (frame->contentRenderer() && frame->view()) {
        frame->view()->layoutIfNeededRecursive();
        IntRect dirty(0, 0, m_rect.width(), m_rect.height());
        frame->view()->paint(&ctx, dirty);
    }
#endif
}

void WebViewPrivate::closeWindowSoon()
{
    m_closeWindowTimer.startOneShot(0); 
}

void WebViewPrivate::closeWindowTimerFired(WebCore::Timer<WebViewPrivate>*)
{
    closeWindow();
}


void WebViewPrivate::closeWindow()
{
#if ENABLE(DAE_APPLICATION)
    webAppMgr().destroyApplication(webAppMgr().application(m_webView));
#else
    SDL_Event ev;
    ev.type = SDL_QUIT;
    SDL_PushEvent(&ev);
#endif
}

#if ENABLE(DAE_APPLICATION)
bool eventInRect(int x, int y, const WebCore::IntRect& r)
{
    if (!r.contains(x, y))
        return false;
    return true;
}

bool WebView::eventMotionInRect(BalEventMotion ev, const BalRectangle& r)
{
    return eventInRect(ev.x, ev.y, r);
}

bool WebView::eventButtonInRect(BalEventButton ev, const BalRectangle& r)
{
    return eventInRect(ev.x, ev.y, r);
}

bool WebView::eventScrollInRect(BalEventScroll ev, const BalRectangle& r)
{
    return eventInRect(ev.x, ev.y, r);
}

void WebView::eventMotionToRelativeCoords(BalEventMotion& ev, const BalPoint& p)
{
    ev.x -= p.x;
    ev.y -= p.y;
}

void WebView::eventButtonToRelativeCoords(BalEventButton& ev, const BalPoint& p)
{
    ev.x -= p.x;
    ev.y -= p.y;
}

void WebView::eventScrollToRelativeCoords(BalEventScroll& ev, const BalPoint& p)
{
    ev.x -= p.x;
    ev.y -= p.y;
}

void WebView::addWidgetOnParentWidget(BalRectangle dirty)
{
    SDL_Rect sdlRect = dirty;
    if (sdlRect.w == 0 || sdlRect.h ==0)
        return;
    SDL_Rect sdlDest = dirty;

    SDL_Surface* view = viewWindow();

    sdlDest.x = max(sdlDest.x, (Sint16)0);
    sdlDest.y = max(sdlDest.y, (Sint16)0);
    if ((sdlDest.x + sdlDest.w) > view->w)
        sdlDest.w = view->w - sdlDest.x;
    if ((sdlDest.y + sdlDest.h) > view->h)
        sdlDest.h = view->h - sdlDest.y;

    if (view->flags & SDL_DOUBLEBUF)
        SDL_Flip(view);
    else
        SDL_UpdateRect(view, sdlDest.x, sdlDest.y, sdlDest.w, sdlDest.h);
}

void WebView::cleanBackground(BalRectangle dirty, bool refresh)
{
    SDL_Rect sdlRect = dirty;
    if (sdlRect.w == 0 || sdlRect.h ==0)
        return;

    SDL_Surface *view = viewWindow();

    SDL_FillRect(view, &sdlRect, 0x00000000);

    if (!refresh)
        return;

    if (view->flags & SDL_DOUBLEBUF)
        SDL_Flip(view);
    else
        SDL_UpdateRect(view, sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
}
#endif // ENABLE(DAE_APPLICATION)
