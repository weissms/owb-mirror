/*
 * Copyright (C) 2010 Pleyo.  All rights reserved.
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

#include "GraphicsContext.h"
#include "WebWindow.h"
#include "WebView.h"
#include "SDL/SDL.h"
#include <signal.h>
#include <unistd.h>

using namespace WebCore;
static bool quit = false;
 
void WebWindow::createPlatformWindow()
{
#if PLATFORM(SDLCAIRO)
    m_surface = cairo_image_surface_create_for_data (
                                    (unsigned char*)m_mainSurface->pixels,
                                    CAIRO_FORMAT_ARGB32,
                                    m_mainSurface->w,
                                    m_mainSurface->h,
                                    m_mainSurface->pitch);
#else
    Uint32 rmask, gmask, bmask, amask;
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
    m_surface = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, m_mainSurface->w, m_mainSurface->h, 32,
                                  rmask, gmask, bmask, amask);
#endif
}

GraphicsContext* WebWindow::createContext()
{
#if PLATFORM(SDLCAIRO)
    cairo_t* cr = cairo_create(m_surface);
    GraphicsContext *ctx = new GraphicsContext(cr);
#else
    GraphicsContext *ctx = new GraphicsContext(m_surface);
#endif
    return ctx;
}

void WebWindow::releaseContext(GraphicsContext* ctx)
{
#if PLATFORM(SDLCAIRO)
    cairo_t* cr = ctx->platformContext();
    cairo_destroy(cr);
#endif
    delete ctx;
}

void WebWindow::updateRect(BalRectangle src, BalRectangle dest)
{
#if !PLATFORM(SDLCAIRO)
    SDL_BlitSurface(m_surface, &src, m_mainSurface, &src);
    SDL_BlitSurface(m_mainSurface, &src, SDL_GetVideoSurface(), &dest);
#else
    if (SDL_GetVideoSurface()->flags & SDL_DOUBLEBUF)
        SDL_Flip(SDL_GetVideoSurface());
    else
        SDL_UpdateRect(SDL_GetVideoSurface(), dest.x, dest.y, dest.w, dest.h);
#endif
}

static void quitCatcher(int)
{
    SDL_Quit();
    exit(1);
}

void WebWindow::runMainLoop()
{
    quit = false;

    signal(SIGKILL, &quitCatcher);
    signal(SIGINT, &quitCatcher);
    signal(SIGTERM, &quitCatcher);

    SDL_Event event;
    while (!quit) {
        if (SDL_PollEvent(&event) != 0)
        {
            switch (event.type) {
                case SDL_ACTIVEEVENT:
                {
                    SDL_ExposeEvent ev;
                    memset(&ev, 0, sizeof(SDL_ExposeEvent));
                    BalRectangle rect = m_webView->frameRect();
                    onExpose(ev, rect);
                    break;
                }
                case SDL_KEYDOWN:
                    onKeyDown(event.key);
                    break;
                case SDL_KEYUP:
                    onKeyUp(event.key);
                    break;
                case SDL_MOUSEMOTION:
                    onMouseMotion(event.motion);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN)
                        onScroll(event.button);
                    else
                        onMouseButtonDown(event.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN) {
                        onScroll(event.button);
                    }
                    else
                        onMouseButtonUp(event.button);
                case SDL_VIDEORESIZE:
                    break;
                case SDL_VIDEOEXPOSE:
                {
                    BalRectangle rect = m_webView->frameRect();
                    onExpose(event.expose, rect);
                    break;
                }
                case SDL_USEREVENT:
                    onUserEvent(event.user);
                    break;
                case SDL_SYSWMEVENT:
                    break;
                default:
                    ;//printf("other event\n");
            }
        } else
            onIdle();
    }
}

void WebWindow::stopMainLoop()
{
    quit = true;
}
