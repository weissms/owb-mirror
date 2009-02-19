#include "SDL.h"
#include "SDL_getenv.h"
#include <WebKit.h>
#include <DumpRenderTree.h>


// Choose some default values.
const unsigned int maxViewWidth = 800;
const unsigned int maxViewHeight = 600;
SDL_TimerID waitToDumpWatchdog = 0;

static WebView* webView;

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

BalWidget* createWindow(WebView *webView, BalRectangle rect)
{
    const SDL_VideoInfo* vi;
    int flags = SDL_RESIZABLE;
    int h = rect.h;
    int w = rect.w;

    putenv("SDL_VIDEODRIVER=dummy");

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

    webView->setViewWindow(screen);
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
            webView->fireWebKitEvents();
        }
    }
}

void startEventLoop(BalWidget *view)
{
    done = false;

    waitEvent();
}

void stopEventLoop()
{
    done = true;
}

static Uint32 waitToDumpWatchdogFired(Uint32 interval, void* param)
{
    const char* message = "FAIL: Timed out waiting for notifyDone to be called\n";
    fprintf(stderr, "%s", message);
    fprintf(stdout, "%s", message);
    waitToDumpWatchdog = 0;
    dump();
    return false;
}


void addTimetoDump(int timeoutSeconds)
{
    waitToDumpWatchdog = SDL_AddTimer(timeoutSeconds * 1000, waitToDumpWatchdogFired, 0);
}

void removeTimer()
{
    SDL_RemoveTimer(waitToDumpWatchdog);
}
