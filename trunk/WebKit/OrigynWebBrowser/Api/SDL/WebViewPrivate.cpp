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

using namespace WebCore;

WebViewPrivate::WebViewPrivate(WebView *webView)
    : m_webView(webView)
    , isInitialized(false)
    , m_scrollSurface(0)
{
}

void WebViewPrivate::fireWebKitEvents()
{
    WTF::dispatchFunctionsFromMainThread();
    fireTimerIfNeeded();
    usleep(10000);
}

void WebViewPrivate::onExpose(BalEventExpose event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;

    if(!isInitialized) {
        isInitialized = true;
        frame->view()->resize(m_rect.width(), m_rect.height());
        frame->view()->forceLayout();
        frame->view()->adjustViewSize();
    }

    GraphicsContext ctx(m_webView->viewWindow());
    ctx.setBalExposeEvent(&event);
    IntRect rect(m_webView->dirtyRegion());
    if (frame->contentRenderer() && frame->view() && !rect.isEmpty()) {
        frame->view()->layoutIfNeededRecursive();
        IntRect dirty = m_webView->dirtyRegion();
        IntRect d = dirty;
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

        updateView(ctx.platformContext(), dirty);
        m_webView->clearDirtyRegion();
    }
}

void WebViewPrivate::onKeyDown(BalEventKey event)
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (!frame)
        return;
    PlatformKeyboardEvent keyboardEvent(&event);

    if (frame->eventHandler()->keyEvent(keyboardEvent))
        return;

    FrameView* view = frame->view();
    SelectionController::EAlteration alteration;
    if (event.keysym.sym == SDLK_LSHIFT)
        alteration = SelectionController::EXTEND;
    else
        alteration = SelectionController::MOVE;

    switch (event.keysym.sym) {
    case SDLK_DOWN:
        view->scrollBy(IntSize(0, (int)cScrollbarPixelsPerLineStep));
        return;
    case SDLK_UP:
        view->scrollBy(IntSize(0, (int)-cScrollbarPixelsPerLineStep));
        return;
    case SDLK_RIGHT:
        view->scrollBy(IntSize((int)cScrollbarPixelsPerLineStep, 0));
        return;
    case SDLK_LEFT:
        view->scrollBy(IntSize((int)-cScrollbarPixelsPerLineStep, 0));
        return;
    case SDLK_PAGEDOWN:
        view->scrollBy(IntSize(0, m_rect.height()));
        return;
    case SDLK_PAGEUP:
        view->scrollBy(IntSize(0, -m_rect.height()));
        return;
    case SDLK_HOME:
        frame->selection()->modify(alteration, SelectionController::BACKWARD, DocumentBoundary, true);
        return;
    case SDLK_END:
        frame->selection()->modify(alteration, SelectionController::FORWARD, DocumentBoundary, true);
        return;
    case SDLK_ESCAPE:
        SDL_Event ev;
        ev.type = SDL_QUIT;
        SDL_PushEvent(&ev);
        return;
    case SDLK_F1:
    {
        m_webView->goBack();
        GraphicsContext ctx(m_webView->viewWindow());
        if (frame->contentRenderer() && frame->view()) {
            frame->view()->layoutIfNeededRecursive();
            IntRect dirty(0, 0, m_rect.width(), m_rect.height());
            frame->view()->paint(&ctx, dirty);
	}
        return;
    }
    case SDLK_F2:
    {
        m_webView->goForward();
        GraphicsContext ctx(m_webView->viewWindow());
        if (frame->contentRenderer() && frame->view()) {
            frame->view()->layoutIfNeededRecursive();
            IntRect dirty(0, 0, m_rect.width(), m_rect.height());
            frame->view()->paint(&ctx, dirty);
	}
        return;
    }
    case SDLK_F3:
        // We ignore the return value as we are not interested
        // in whether the zoom occurred.
        m_webView->zoomPageIn();
        return;
    case SDLK_F4:
        // Same comment as above.
        m_webView->zoomPageOut();
        return;
    default:
        break;
    }
}

void WebViewPrivate::onKeyUp(BalEventKey event)
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (!frame)
        return;
    PlatformKeyboardEvent keyboardEvent(&event);

    if (frame->eventHandler()->keyEvent(keyboardEvent))
        return ;
}

void WebViewPrivate::onMouseMotion(BalEventMotion event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;
    frame->eventHandler()->mouseMoved(PlatformMouseEvent(&event));
}

void WebViewPrivate::onMouseButtonDown(BalEventButton event)
{
    Frame* frame = core(m_webView->mainFrame());

    if (!frame)
        return;

    if (event.button == SDL_BUTTON_MIDDLE)
        return ;//webkit_web_view_forward_context_menu_event(webView, PlatformMouseEvent(event));
    frame->eventHandler()->handleMousePressEvent(PlatformMouseEvent(&event));
}

void WebViewPrivate::onMouseButtonUp(BalEventButton event)
{
    Frame* focusedFrame = m_webView->page()->focusController()->focusedFrame();
    if (!focusedFrame)
        return;
//     if (focusedFrame && focusedFrame->editor()->canEdit()) {
//         ;
//     }

    focusedFrame->eventHandler()->handleMouseReleaseEvent(PlatformMouseEvent(&event));
}

void WebViewPrivate::onScroll(BalEventScroll event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;
    PlatformWheelEvent wheelEvent(&event);
    frame->eventHandler()->handleWheelEvent(wheelEvent);
}

void WebViewPrivate::onResize(BalResizeEvent event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;
    m_rect.setWidth(event.w);
    m_rect.setHeight(event.h);
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
}

void WebViewPrivate::updateView(BalWidget *surf, IntRect rect)
{
    if (!surf || rect.isEmpty())
        return;
    rect.intersect(m_rect);
    /* use SDL_Flip only if double buffering is available */
    SDL_Rect sdlRect = {rect.x(), rect.y(), rect.width(), rect.height()};
    //printf("updateView x=%d y=%d w=%d h=%d\n", sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
    sdlRect.x = max(sdlRect.x, (Sint16)0);
    sdlRect.y = max(sdlRect.y, (Sint16)0);
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
#if !PLATFORM(AMIGAOS4) && SDL_BYTEORDER == SDL_BIG_ENDIAN
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

