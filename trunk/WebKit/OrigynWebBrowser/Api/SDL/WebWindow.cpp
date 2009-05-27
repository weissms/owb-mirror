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
#include "WebWindow.h"
#include "SDL/SDL.h"
#include <unistd.h>
#include "GraphicsContext.h"
#include "Frame.h"
#include "WebView.h"
#include "FrameView.h"
#include "WebFrame.h"
  
using namespace WebCore;

void WebWindow::show()
{
    quit = false;
    bool isInitialized = false;

    SDL_Event event;
    while (!quit) {
        if (!isInitialized) {
            SDL_ExposeEvent ev;
            onExpose(ev);
            isInitialized = true;
        }
        
        if (SDL_WaitEvent(&event) != 0)
        {
            switch (event.type) {
                case SDL_ACTIVEEVENT:
                    //printf("gain =%d state = %d\n", event.active.gain, event.active.state);
                    /*if(event.active.gain == 1 && event.active.state == 2) {
                        m_view->addToDirtyRegion(IntRect(0, 0, 800, 480));
                    }*/
                    SDL_ExposeEvent ev;
                    onExpose(ev);
                    break;
                case SDL_KEYDOWN:
                    //printf("La touche %s a Ã©tÃ© prÃ©ssÃ©e!\n", SDL_GetKeyName(event.key.keysym.sym));
                    onKeyDown(event.key);
                    break;
                case SDL_KEYUP:
                    //printf("La touche %s a Ã©tÃ© relachÃ©e!\n", SDL_GetKeyName(event.key.keysym.sym));
                    onKeyUp(event.key);
                    break;
                case SDL_MOUSEMOTION:
                    //printf("mouse motion\n");
                    onMouseMotion(event.motion);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //printf("bouton mouse down\n");
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
                    //printf("video resize\n");
                    /*scr = createSDLWindow(event.resize.w, event.resize.h);
                    m_view->setViewWindow(scr);
                    m_view->onResize(event.resize);
                    SDL_FreeSurface(s_screen);
                    s_screen = scr;*/
                    break;
                case SDL_VIDEOEXPOSE:
                    //printf("#######################video expose\n");
                    onExpose(event.expose);
                    break;
                case SDL_USEREVENT:
                    //printf("user event\n");
                    onUserEvent(event.user);
                    break;
                case SDL_SYSWMEVENT:
                    //printf("sys wm event\n");
                    break;
                case SDL_QUIT:
                    //printf("SDL_QUIT\n");
                    break;
                default:
                    ;//printf("other event\n");
            }
        }
    }
}

void WebWindow::hide()
{
    quit = true;
}

void WebWindow::setMainWindow(WebView *view)
{
    Uint32 rmask, gmask, bmask, amask;
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
    m_surface = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, m_mainSurface->w, m_mainSurface->h, 32,
                                  rmask, gmask, bmask, amask);
    SDL_Rect sdlSrc, sdlDest;
    sdlSrc.x = sdlDest.x = 0;
    sdlSrc.y = sdlDest.y = 0;
    sdlSrc.w = sdlDest.w = m_surface->w;
    sdlSrc.h = sdlDest.h = m_surface->h;

    // FIXME : find a better solution to show web page
    GraphicsContext ctx(m_mainSurface);
    Frame* frame = core(view->mainFrame());
    if (frame->contentRenderer() && frame->view()) {
        frame->view()->layoutIfNeededRecursive();
        IntRect dirty(0, 0, m_surface->w, m_surface->h);
        frame->view()->paint(&ctx, dirty);
    }
    if (m_mainSurface->flags & SDL_DOUBLEBUF)
        SDL_Flip(m_mainSurface);
    else
        SDL_UpdateRect(m_mainSurface, sdlDest.x, sdlDest.y, sdlDest.w, sdlDest.h);
    SDL_BlitSurface(m_mainSurface, &sdlSrc, m_surface, &sdlDest);
}

