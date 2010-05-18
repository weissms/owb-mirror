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


#include "SDL.h"
#include "SDL_getenv.h"
#include <signal.h>
#include <DumpRenderTree.h>
#include <WebKit.h>

// Choose some default values.
const unsigned int maxViewWidth = 800;
const unsigned int maxViewHeight = 600;
timer_t waitToDumpWatchdog;

void init(int argc, char *argv[])
{
}

BalRectangle clientRect(bool isSVGW3CTest)
{
    SDL_Rect r;
    r.x = r.y = 0;
    r.w = isSVGW3CTest ? 480 : maxViewWidth;
    r.h = isSVGW3CTest ? 360 : maxViewHeight;
    return r; 
}

#if ENABLE(DAE_APPLICATION)
BalWidget* createWindow(WebView **webView, BalRectangle rect)
{
    const SDL_VideoInfo* vi;
    int flags = SDL_RESIZABLE;
    int h = rect.h;
    int w = rect.w;

    putenv((char*)"SDL_VIDEODRIVER=dummy");

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

void waitEvent()
{
    SDL_Event event;
    WebView* webView = getWebView();

    //SDL_Surface* scr;
    while (!done) {
        //printf("SDL_WaitEvent\n");
        //if (SDL_WaitEvent(&event) != 0)
        // We need to use a polling SDL because waitEvent block during a new event are sending, if an event are sending before the waitEvent, the event is lost.
        if (SDL_PollEvent(&event) != 0)
        {
            //printf("event type = %d timer = %d \n", event.type, SDLUserEventCode_Timer);
            switch (event.type) {
                case SDL_ACTIVEEVENT:
                    //printf("gain =%d state = %d\n", event.active.gain, event.active.state);
                    if(event.active.gain == 1 && event.active.state == 2) {
                        SDL_Rect rect = {0, 0, 800, 600};
                        webView->addToDirtyRegion(rect);
                    }
                    SDL_ExposeEvent ev;
                    ev.type = SDL_VIDEOEXPOSE;
                    webAppMgr().onExpose(ev);
                    break;
                case SDL_KEYDOWN:
                    //printf("La touche %s a été préssée!\n", SDL_GetKeyName(event.key.keysym.sym));
                    webAppMgr().onKeyDown(event.key);
                    break;
                case SDL_KEYUP:
                    //printf("La touche %s a été relachée!\n", SDL_GetKeyName(event.key.keysym.sym));
                    webAppMgr().onKeyUp(event.key);
                    break;
                case SDL_MOUSEMOTION:
                    //printf("mouse motion\n");
                    webAppMgr().onMouseMotion(event.motion);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //printf("bouton mouse down\n");
                    if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN)
                        webAppMgr().onScroll(event.button);
                    else
                        webAppMgr().onMouseButtonDown(event.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    //printf("button mouse up\n");
                    if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN) 
                        webAppMgr().onScroll(event.button);
                    else
                        webAppMgr().onMouseButtonUp(event.button);
                    break;
                case SDL_VIDEORESIZE:
                    //printf("video resize\n");
                    /*scr = createSDLWindow(event.resize.w, event.resize.h);
                    webView->setViewWindow(scr);
                    webView->onResize(event.resize);
                    SDL_FreeSurface(s_screen);
                    s_screen = scr;*/
                    break;
                case SDL_VIDEOEXPOSE:
                    //printf("#######################video expose\n");
                    webAppMgr().onExpose(event.expose);
                    break;
                case SDL_USEREVENT:
                    //printf("user event\n");
                    if (event.user.code == SDLUserEventCode_Timer)
                        webAppMgr().fireWebKitTimerEvents();
                    else if (event.user.code == SDLUserEventCode_Thread)
                        webAppMgr().fireWebKitThreadEvents();
                    else
                        webAppMgr().onUserEvent(event.user);
                    break;
                case SDL_SYSWMEVENT:
                    //printf("sys wm event\n");
                    break;
                case SDL_QUIT:
                    //printf("SDL_QUIT\n");
                    webAppMgr().onQuit(event.quit);
                    break;
                default:
                    ;//printf("other event\n");
            }
        } else {
            webView->fireWebKitThreadEvents();
            webView->fireWebKitTimerEvents();
            usleep(1000);
        }
    }
}
#else
BalWidget* createWindow(WebView **webView, BalRectangle rect)
{
    const SDL_VideoInfo* vi;
    int flags = SDL_RESIZABLE;
    int h = rect.h;
    int w = rect.w;

    putenv((char*)"SDL_VIDEODRIVER=dummy");

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

    (*webView)->setViewWindow(screen);
    return screen;
}

void waitEvent()
{
    SDL_Event event;
    WebView *webView = getWebView();

    //SDL_Surface* scr;
    while (!done) {
        //printf("waitEvent \n");
        if (SDL_PollEvent(&event) != 0)
        {
            switch (event.type) {
                case SDL_ACTIVEEVENT:
                    //printf("gain =%d state = %d\n", event.active.gain, event.active.state);
                    if(event.active.gain == 1 && event.active.state == 2) {
                        SDL_Rect rect = {0, 0, 800, 600};
                        webView->addToDirtyRegion(rect);
                    }
                    SDL_ExposeEvent ev;
                    memset(&ev, 0, sizeof(SDL_ExposeEvent));
                    webView->onExpose(ev);
                    break;
                case SDL_KEYDOWN:
                    //printf("La touche %s a été préssée!\n", SDL_GetKeyName(event.key.keysym.sym));
                    webView->onKeyDown(event.key);
                    break;
                case SDL_KEYUP:
                    //printf("La touche %s a été relachée!\n", SDL_GetKeyName(event.key.keysym.sym));
                    webView->onKeyUp(event.key);
                    break;
                case SDL_MOUSEMOTION:
                    //printf("mouse motion\n");
                    webView->onMouseMotion(event.motion);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //printf("bouton mouse down\n");
                    if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN)
                        webView->onScroll(event.button);
                    else
                        webView->onMouseButtonDown(event.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    //printf("button mouse up\n");
                    if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN) {
                        webView->onScroll(event.button);
                    }
                    else
                        webView->onMouseButtonUp(event.button);
                    break;
                case SDL_VIDEORESIZE:
                    //printf("video resize\n");
                    /*scr = createSDLWindow(event.resize.w, event.resize.h);
                    webView->setViewWindow(scr);
                    webView->onResize(event.resize);
                    SDL_FreeSurface(s_screen);
                    s_screen = scr;*/
                    break;
                case SDL_VIDEOEXPOSE:
                    //printf("#######################video expose\n");
                    webView->onExpose(event.expose);
                    break;
                case SDL_USEREVENT:
                    //printf("user event\n");
                    webView->onUserEvent(event.user);
                    break;
                case SDL_SYSWMEVENT:
                    //printf("sys wm event\n");
                    break;
                case SDL_QUIT:
                    //printf("SDL_QUIT\n");
                    done = true;
                    break;
                default:
                    ;//printf("other event\n");
            }
        } else {
            webView->fireWebKitThreadEvents();
            webView->fireWebKitTimerEvents();
            usleep(1000);
        }
    }
}
#endif

void startEventLoop(BalWidget *view)
{
    done = false;

    waitEvent();
}

void stopEventLoop()
{
    done = true;
}

static void waitToDumpWatchdogFired(sigval_t signal)
{
    const char* message = "FAIL: Timed out waiting for notifyDone to be called\n";
    fprintf(stderr, "%s", message);
    fprintf(stdout, "%s", message);
    removeTimer();
    dump();
}


void addTimetoDump(int timeoutSeconds)
{
    //FIXME: check return value from timer_create()
    struct sigevent handler;
    memset(&waitToDumpWatchdog, 0, sizeof(timer_t));
    memset(&handler, 0, sizeof(sigevent));
    handler.sigev_notify = SIGEV_THREAD;
    handler.sigev_notify_function = waitToDumpWatchdogFired;
    timer_create(CLOCK_REALTIME, &handler, &waitToDumpWatchdog);

    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(itimerspec));
    timeout.it_value.tv_sec = timeoutSeconds;
    timer_settime(waitToDumpWatchdog, 0, &timeout, NULL);
}

void removeTimer()
{
    timer_delete(waitToDumpWatchdog);
    memset(&waitToDumpWatchdog, 0, sizeof(timer_t));
}


void getBalPoint(int x, int y, BalPoint* point)
{
    point->x = x;
    point->y = y;
}

void displayWebView()
{
    getWebView()->addToDirtyRegion(getWebView()->frameRect());
    SDL_ExposeEvent ev;
    ev.type = SDL_VIDEOEXPOSE; 
#if ENABLE(DAE_APPLICATION)
    webAppMgr().onExpose(ev);
#else
    getWebView()->onExpose(ev);
#endif
}

int getX(BalPoint pos)
{
    return pos.x;
}

int getY(BalPoint pos)
{
    return pos.y;
}
