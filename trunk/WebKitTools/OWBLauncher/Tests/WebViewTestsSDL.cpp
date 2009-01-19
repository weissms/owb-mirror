/*
 * Copyright (C) 2009 Pleyo.  All rights reserved.
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

#include <iostream>

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "SDL.h"
#include <WebKit.h>

// FIXME: Most of this code is copied from SDL/main.cpp
// we should try to share the code.
static WebView* webView;
static SDL_Surface* s_screen = NULL;

static SDL_Surface* createSDLWindow(int w, int h)
{
    const SDL_VideoInfo* vi;
    int flags = SDL_RESIZABLE;

    vi = SDL_GetVideoInfo();
    if (vi && vi->wm_available) /* Change title */
        SDL_WM_SetCaption("Origyn Web Browser", "Origyn Web Browser");
    if (vi && vi->hw_available) /* Double buffering will not be set by SDL if not supported */
        flags |= SDL_HWSURFACE | SDL_DOUBLEBUF;

    SDL_Surface *screen;
    screen = SDL_SetVideoMode(w, h, 32, flags);
    if ( screen == NULL ) {
        fprintf(stderr, "Unable to create screen : %s", SDL_GetError());
        exit(1);
    }
    if (SDL_InitSubSystem(SDL_INIT_EVENTTHREAD) < 0) {
        fprintf(stderr, "Unable to init SDL: %s", SDL_GetError());
        exit(1);
    } else {
        SDL_EnableUNICODE(1);
        if (0 != SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL))
            fprintf(stderr, "SDL can't enable key repeat\n");
    }
    if (SDL_InitSubSystem(SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "Unable to init SDL: %s", SDL_GetError());
        exit(1);
    }

    if (getenv("SDL_NOMOUSE"))
        SDL_ShowCursor(SDL_DISABLE);

    return screen;
}

static void waitEventUntilLoaded()
{
    SDL_Event event;

    SDL_GetEventFilter();
    SDL_Surface* scr;
    bool quit = false;
    while (!quit) {
        if (SDL_PollEvent(&event) != 0)
        {
            switch (event.type) {
                case SDL_ACTIVEEVENT:
                    if(event.active.gain == 1 && event.active.state == 2) {
                        SDL_Rect rect = {0, 0, 800, 600};
                        webView->addToDirtyRegion(rect);
                    }
                    SDL_ExposeEvent ev;
                    webView->onExpose(ev);
                    break;
                case SDL_KEYDOWN:
                    webView->onKeyDown(event.key);
                    break;
                case SDL_KEYUP:
                    webView->onKeyUp(event.key);
                    break;
                case SDL_MOUSEMOTION:
                    webView->onMouseMotion(event.motion);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN)
                        webView->onScroll(event.button);
                    else
                        webView->onMouseButtonDown(event.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN) {
                        webView->onScroll(event.button);
                    }
                    else
                        webView->onMouseButtonUp(event.button);
                    break;
                case SDL_VIDEORESIZE:
                    scr = createSDLWindow(event.resize.w, event.resize.h);
                    webView->setViewWindow(scr);
                    webView->onResize(event.resize);
                    SDL_FreeSurface(s_screen);
                    s_screen = scr;
                    break;
                case SDL_VIDEOEXPOSE:
                    webView->onExpose(event.expose);
                    break;
                case SDL_USEREVENT:
                    webView->onUserEvent(event.user);
                    break;
                case SDL_SYSWMEVENT:
                    break;
                case SDL_QUIT:
                    quit = true;
                    break;
                default:
                    ;
            }
        } else {
            webView->fireWebKitEvents();
        }
        quit = !webView->isLoading();
    }
}

class WebViewTestSDL : public CPPUNIT_NS::TestCase 
{
    CPPUNIT_TEST_SUITE(WebViewTestSDL);
    CPPUNIT_TEST(WebViewLoadURL);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void)
    {
        webView = WebView::createInstance();
        SDL_Rect rect = { 0, 0, 800, 600 };
        s_screen = createSDLWindow(rect.w, rect.h);
        webView->setViewWindow(s_screen);
        webView->initWithFrame(rect, NULL, NULL);
    }

    void tearDown(void)
    {
        delete webView;
    }

protected:
   void WebViewLoadURL()
   {
       const char* googleFr = "http://www.google.fr";
       webView->mainFrame()->loadURL(googleFr);
       waitEventUntilLoaded();
       const char* url = webView->mainFrame()->url();
       CPPUNIT_ASSERT(!strncmp(googleFr, url, strlen(googleFr)));
   }

};

CPPUNIT_TEST_SUITE_REGISTRATION(WebViewTestSDL);

int main( int ac, char **av )
{
    //--- Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    //--- Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener( &result );        

    //--- Add a listener that print dots as test run.
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener( &progress );      

    //--- Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( controller );

    return result.wasSuccessful() ? 0 : 1;
}
