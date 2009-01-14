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

#include <WebKit.h>
#include "SDL.h"
#include "signal.h"
#include <unistd.h>

using namespace WebCore;
static WebView* webView;
static SDL_Surface* s_screen = NULL;
static bool quit = false;

void signalCatcher(int signum)
{
    // BEFORE YOU ABORT QUIT SDL.
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
    abort();
}

// Used for quitting OWB when receiving a signal.
void signalQuitCatcher(int /*signum*/)
{
    quit = true;
}

SDL_Surface* createSDLWindow(int w, int h)
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

int eventFilter(const SDL_Event* event)
{
    //printf("eventFilter\n");
    switch (event->type) {
        case SDL_VIDEOEXPOSE:
            //if(!isExposed) {
                //printf("--------------------video expose\n");
              //  webView->onExpose(event->expose);
              //  isExposed = true;
            //}
            break;
        default:
            ;
    }
    return 1;
}

void waitEvent()
{
    SDL_Event event;

    SDL_GetEventFilter();
    SDL_Surface* scr;
    while (!quit) {
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
                    scr = createSDLWindow(event.resize.w, event.resize.h);
                    webView->setViewWindow(scr);
                    webView->onResize(event.resize);
                    SDL_FreeSurface(s_screen);
                    s_screen = scr;
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
                    quit = true;
                    break;
                default:
                    ;//printf("other event\n");
            }
        } else {
            webView->fireWebKitEvents();
        }
    }
}

void usage()
{
    printf("owb [-c tokenizerChunkSize -d tokenizerDelay -f configFile] [url_to_load]\n");
}

void progressNotification()
{
    printf("progress : %d \n", int(webView->estimatedProgress() * 100));
}


int main (int argc, char* argv[])
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to init SDL: %s", SDL_GetError());
        exit(1);
    }

    signal(SIGSEGV, &signalCatcher);

    // Signals that should shutdown OWB properly.
    signal(SIGKILL, &signalQuitCatcher);
    signal(SIGINT, &signalQuitCatcher);

//    SDL_EventState(SDL_VIDEOEXPOSE, SDL_ENABLE);

    //SDL_SetEventFilter(eventFilter);

    webView = WebView::createInstance();

    int optionFirstCharacter;
    bool hasParsedConfigFile = false;
    while ((optionFirstCharacter = getopt(argc, argv, "c:f:d:")) != -1) {
        switch(optionFirstCharacter) {
        case 'c': {
            int chunkSize = (int)strtol(optarg, (char **)NULL, 10);
            if (chunkSize)
                webView->setCustomHTMLTokenizerChunkSize(chunkSize);
            break;
        }
        case 'd': {
            int delay = (int)strtol(optarg, (char **)NULL, 10);
            if (delay)
                webView->setCustomHTMLTokenizerTimeDelay(delay);
            break;
        }
        case 'f':
            hasParsedConfigFile = true;
            webView->parseConfigFile(optarg);
            break;
        default:
            usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (!hasParsedConfigFile)
        webView->parseConfigFile();

    SDL_Rect rect = webView->frameRect();
    if (rect.w == 0 || rect.h == 0) {
        // The rectangle was not initialized so
        // default to a 800 x 600 rect.
        rect.w = 800;
        rect.h = 600;
    }

    s_screen = createSDLWindow(rect.w, rect.h);
    webView->setViewWindow(s_screen);
    webView->initWithFrame(rect, NULL, NULL);

    webView->registerOnNotifyProgress(progressNotification);
    char* uri = (char*) (argc > 0 ? argv[0] : "http://www.google.com/");
    webView->mainFrame()->loadURL(uri);

    /*if(webView->canZoomPageIn()) {
        printf("canZoomPageIn\n");
        webView->zoomPageIn();
    }*/
    /*if(webView->canMakeTextLarger()) {
        printf("canMakeTextLarger\n");
        webView->makeTextLarger();
    }*/

    waitEvent();

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();

    delete webView;
    return 0;
}
