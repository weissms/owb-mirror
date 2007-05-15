/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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

#ifndef BIWINDOW_CPP_
#define BIWINDOW_CPP_

#include "config.h"
#include <assert.h>
#include "BALConfiguration.h"
#include "BIEventLoop.h"
#include "BIPainter.h"
#include "BIWindowEvent.h"
#include "BTLogHelper.h"
#include "WindowBal.h"
// XXX FIXME SRO use BI instead
#include "../Graphics/SDL/BCGraphicsContext.h"
#include "../Graphics/SDL/BCNativeImage.h"
#include "SystemTime.h"
// FIXME SRO BAD because it introduces a link with BAL
#include "Frame.h"
#include "FrameView.h"

namespace BAL {

static BIWindow* __win = 0;

BIWindow* getBIWindow()
{
    return __win;
}

BIWindow* createBCWindowBal(int x, int y, int width, int height, int depth)
{
    if (__win)
        return __win;
    else
        return __win = new WindowBal( x, y, width, height, depth );
}

static SDL_Surface* m_screen = 0;

SDL_Surface* WindowBal::m_screen = 0;

Uint32 sdlTimeoutCallback(Uint32 interval, void* param)
{
    BIWindowEvent* event = createBIWindowEvent(BIWindowEvent::REDRAW, true, IntRect(0,0,0,0));
    getBIEventLoop()->PushEvent(event);
    return interval;
}

WindowBal::WindowBal(int x, int y, int width, int height, int depth)
    : m_x(0), m_y(0), m_width(width), m_height(height)
    , m_painter(0), m_data(0), m_gc(0), m_surface(0)
    , m_needsDisplayInRect(IntRect(0,0,0,0))
{
    if (!__win)
        __win = this;

#ifndef NDEBUG
    if (getenv("DISABLE_DISPLAY")) {
        m_layoutTests = true;
        return;
    }
    else
        m_layoutTests = false;
#endif
    const SDL_VideoInfo* vi;
    char line[64];

    snprintf(line, 64, "SDL_VIDEO_WINDOW_POS=%d,%d", x, y);
    SDL_putenv(line);

    vi = SDL_GetVideoInfo();
    if(vi && vi->wm_available) /* Change les titres */
        SDL_WM_SetCaption("Origyn Web Browser", "Icone SDL");

    /* Initialize only once with main frame size */
    if (!m_screen)
        m_screen = SDL_SetVideoMode(width, height, depth, SDL_HWSURFACE|SDL_NOFRAME|SDL_SRCALPHA);

    if (!m_screen) {
        logml(MODULE_GRAPHICS, LEVEL_CRITICAL, "SDL_SetVideoMode failed");
        exit(1);
    } else
        logm(MODULE_GRAPHICS, "SDL_SetVideoMode succeeded");

    m_gc = createBIGraphicsContext();
    if (!m_gc) {
        logml(MODULE_WIDGETS, LEVEL_EMERGENCY, "out of memory");
        return;
    }
    SDL_AddTimer(50, sdlTimeoutCallback, 0);
}

WindowBal::~WindowBal()
{
    if (m_gc)
        delete m_gc;
}

void WindowBal::clear(const IntRect& rect)
{
    if (rect.isEmpty()) {
        logml(MODULE_GRAPHICS, LEVEL_INFO, "clear all called");
        SDL_FillRect(m_screen, 0, 0x00000000);
        // we must update all m_screen because the next page may be smaller, and then some part may
        // still be visible !
        SDL_UpdateRect(m_screen, m_x, m_y, m_width, m_height);
        SDL_FillRect(static_cast<BCNativeImage*>(m_gc->getNativeImage())->getSurface(), 0, 0x00000000);
    }
    else {
        logml(MODULE_GRAPHICS, LEVEL_INFO, make_message("clear rect called for %dx%d+%d+%d", rect.width(), rect.height(), rect.x(), rect.y()));
        SDL_Rect sdlRect;
        sdlRect.x = rect.x();
        sdlRect.y = rect.y();
        sdlRect.w = rect.width();
        sdlRect.h = rect.height();
        SDL_FillRect(m_screen, &sdlRect, 0x00000000);
        // do not do an update or else a black flash appear
        SDL_FillRect(static_cast<BCNativeImage*>(m_gc->getNativeImage())->getSurface(), &sdlRect, 0x00000000);
    }
}

int WindowBal::x()
{
    return m_x;
}

int WindowBal::y()
{
    return m_x;
}

int WindowBal::width()
{
#ifndef NDEBUG
    if (m_layoutTests)
        return 800;
#endif
    return m_width;
}

int WindowBal::height()
{
#ifndef NDEBUG
    if (m_layoutTests)
        return 600;
#endif
    return m_height;
}

int WindowBal::depth()
{
#ifndef NDEBUG
    if (m_layoutTests)
        return 32;
#endif
    return m_screen->format->BitsPerPixel;
}

/**
 * Should be used only after receiving an expose event
 */
void WindowBal::expose(const IntRect& rect)
{
    if (rect.isEmpty())
        return; // nothing to redraw : stop m_timer repetition
    m_needsDisplayInRect.unite(rect);
}

void WindowBal::redraw(const IntRect& rect)
{
#ifndef NDEBUG
    if (m_layoutTests)
        return;
#endif
    if (m_needsDisplayInRect.isEmpty())
        return; // nothing to redraw : stop m_timer repetition
    m_needsDisplayInRect.intersect(IntRect(0,0, width(), height()));
    clear(m_needsDisplayInRect);

    if (m_painter) {
        WebCore::FrameView *view = reinterpret_cast<WebCore::Frame*> (m_painter)->view();
        if (view->layoutPending())
            view->layout(false);
        // paint the needed rect in our m_gc
        m_painter->paint(m_gc, m_needsDisplayInRect);
    }

    SDL_Rect sdlRect, sdlDest;
    sdlRect.x = m_needsDisplayInRect.x();
    sdlRect.y = m_needsDisplayInRect.y();
    sdlRect.w = m_needsDisplayInRect.width();
    sdlRect.h = m_needsDisplayInRect.height();

    sdlDest.x = m_x + m_needsDisplayInRect.x();
    sdlDest.y = m_y + m_needsDisplayInRect.y();

#ifndef NDEBUG
    if (getenv("FLASHING_RECTS")) {
        // flash a red rect
        SDL_FillRect(m_screen, &sdlRect, 0x00ff0000);
        SDL_UpdateRect(m_screen, sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
        SDL_Delay(10); // wait 10ms in order to see flashing rect
    }
#endif

    m_surface = static_cast<BCNativeImage*> (m_gc->getNativeImage())->getSurface();
    // now blit the updated rect
    SDL_BlitSurface(m_surface, &sdlRect, m_screen, &sdlDest);
    SDL_UpdateRect(m_screen, sdlDest.x, sdlDest.y, sdlRect.w, sdlRect.h);

    // reset m_needsDisplayInRect
    m_needsDisplayInRect = IntRect(0, 0, 0, 0);
}

void WindowBal::setPainter(BIPainter* aPainter)
{
    if (aPainter)
        m_painter = aPainter;
}

void WindowBal::scroll(int deltaX, int deltaY)
{
    SDL_Rect sdlRect, sdlDest;
    SDL_Rect updateWidth, updateHeight;
    int x1, x2, y1, y2;

    if (deltaY > 0) {
        y1 = deltaY;
        y2 = 0;
        sdlRect.h = height() - deltaY;
        updateWidth.y = sdlRect.h;
        updateWidth.h = deltaY;
    } else {
        y1 = 0;
        y2 = -deltaY;
        sdlRect.h = height() + deltaY;
        updateWidth.y = 0;
        updateWidth.h = -deltaY;
    }

    if (deltaX > 0) {
        x1 = deltaX;
        x2 = 0;
        sdlRect.w = width() - deltaX;
        updateHeight.x = sdlRect.w;
        updateHeight.w = -deltaX;
    } else {
        x1 = 0;
        x2 = -deltaX;
        sdlRect.w = width() + deltaX;
        updateHeight.x = 0;
        updateHeight.w = deltaX;
    }

    updateWidth.x = 0;
    updateWidth.w = width();
    updateHeight.y = 0,
    updateHeight.h = height();

    IntRect rectForWidth(updateWidth.x, updateWidth.y, updateWidth.w, updateWidth.h);
    IntRect rectForHeight(updateHeight.x, updateHeight.y, updateHeight.w, updateHeight.h);
    sdlRect.x = x1;
    sdlRect.y = y1;

    sdlDest.x = x2;
    sdlDest.y = y2;

    SDL_BlitSurface(m_screen, &sdlRect, m_screen, &sdlDest);

    if (m_painter) {
        // paint the needed rect in our m_gc
        m_painter->paint(m_gc, rectForWidth);
        SDL_BlitSurface(m_surface, &updateWidth, m_screen, &updateWidth);
        m_painter->paint(m_gc, rectForHeight);
        SDL_BlitSurface(m_surface, &updateHeight, m_screen, &updateHeight);
    }
    SDL_UpdateRect(m_screen, 0, 0, width(), height());
}

}

#endif // BIWINDOW_CPP
