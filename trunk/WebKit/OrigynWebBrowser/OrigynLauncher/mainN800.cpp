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
 
/**
 * @file  main.cpp
 *
 * @brief bal launcher
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $

 */
#include "config.h"
#include "BALConfiguration.h"
#include "BIEventLoop.h"
#include "BIKeyboardEvent.h"
#include "BIWindow.h"
#include "BIWindowEvent.h"
#include "BIWindowManager.h"
#include "BIGraphicsDevice.h"
#include "BTLogHelper.h"
#include "BTDeviceChannel.h"
#include "BTTextLogFormatter.h"
#include "CookieJar.h"
#include "IntRect.h"
#include "KURL.h"
#include "wtf/RefPtr.h"
#ifdef __OWBAL_PLATFORM_MACPORT__
// SDL on Mac has some tricks with main()
#include <SDL/SDL.h>
#endif
#include <signal.h>

// include for N800
#include <hildon-widgets/hildon-program.h>
#include <gtk/gtkmain.h>
#include <gtk/gtk.h>
#include <SDL/SDL_syswm.h>
#include <X11/Xlib.h>

static HildonWindow *gtk_window = NULL;

static GtkWidget *butt_back = NULL;
static GtkWidget *butt_next = NULL;
static GtkWidget *text_url = NULL;
static GtkWidget *area = NULL;
/**
 * Signal 11 catcher to prevent process from freezing on segfaults.
 */
void signalCatcher(int signum)
{
    printf("signal %d catched: abort.\n", signum);
    abort();
}

using namespace BAL;

static void owbQuit()
{
    getBIEventLoop()->PushEvent(createBIWindowEvent(BAL::BIWindowEvent::QUIT, false, WebCore::IntRect(), getBIWindowManager()->activeWindow()->widget()));
}

static void urlBack()
{
    getBIWindowManager()->activeWindow()->goBackOrForward(-1);;
}

static void urlNext()
{
    getBIWindowManager()->activeWindow()->goBackOrForward(1);
}

static void urlGo()
{
    KURL url(gtk_entry_get_text(GTK_ENTRY(text_url)));
    getBIWindowManager()->activeWindow()->setURL(url);
}

static void handle_event(BIEventLoop& aEventLoop) {
    BIEvent* aEvent = NULL;
    bool isQuitCalled = false;
    bool isEventValid = false;
    while (!isQuitCalled)
    {
        if (aEvent != NULL) {
            delete aEvent;
            aEvent = NULL; // otherwise may be freed twice if WaitEvent returns an invalid event
        }
        if (butt_back && butt_next) {
            gtk_widget_set_sensitive(butt_back, getBIWindowManager()->activeWindow()->canGoBackOrForward(-1));
            gtk_widget_set_sensitive(butt_next, getBIWindowManager()->activeWindow()->canGoBackOrForward(1));
        }

        isEventValid = aEventLoop.WaitEvent(aEvent);
        if (isEventValid) {
            BIKeyboardEvent* aKeyboardEvent = aEvent->queryIsKeyboardEvent();
            if(aKeyboardEvent)
            {
                if(aKeyboardEvent->virtualKeyCode() == BAL::BIKeyboardEvent::VK_ESCAPE ) {
                    break; // stop loop
                }
            }

            BIWindowEvent* aWindowEvent = aEvent->queryIsWindowEvent();
            if( aWindowEvent && aWindowEvent->type() == BAL::BIWindowEvent::QUIT )
            {
                break; // stop loop
            }
            // In other cases, event is handled by frame
            getBIWindowManager()->handleEvent(aEvent);
        }
    }
}

void GtkInitN800(int argc, char *argv[], bool isFullscreen)
{
    /* Create needed variables */
    HildonProgram *program;
    HildonWindow *window;


    gtk_init(&argc, &argv);
    /* Create the hildon program and setup the title */
    program = HILDON_PROGRAM(hildon_program_get_instance());
    g_set_application_name("Origyn Web Browser");

    /* Create HildonWindow and set it to HildonProgram */
    window = HILDON_WINDOW(hildon_window_new());
    hildon_program_add_window(program, window);

    if (!isFullscreen) {
        butt_back = gtk_button_new_with_label("Back");
        GtkWidget* img_back = gtk_image_new_from_file("/usr/share/icons/back.png");
        gtk_button_set_image(GTK_BUTTON(butt_back), img_back);
        gtk_button_set_relief(GTK_BUTTON(butt_back), GTK_RELIEF_NONE);

        butt_next = gtk_button_new_with_label("Next");
        GtkWidget* img_next = gtk_image_new_from_file("/usr/share/icons/next.png");
        gtk_button_set_image(GTK_BUTTON(butt_next), img_next);
        gtk_button_set_relief(GTK_BUTTON(butt_next), GTK_RELIEF_NONE);

        text_url = gtk_entry_new();
        gtk_entry_set_width_chars(GTK_ENTRY(text_url), 20);

        GtkContainer *navbar= GTK_CONTAINER(gtk_toolbar_new());
        gtk_toolbar_append_widget(GTK_TOOLBAR(navbar), butt_back, "go back", "go back");
        gtk_toolbar_append_widget(GTK_TOOLBAR(navbar), butt_next, "go next", "go next");
        gtk_toolbar_append_widget(GTK_TOOLBAR(navbar), text_url , "enter url ", "enter url ");
        hildon_window_add_toolbar(window, GTK_TOOLBAR(navbar));

        g_signal_connect(G_OBJECT(butt_back), "clicked", urlBack, NULL);
        g_signal_connect(G_OBJECT(butt_next), "clicked", urlNext, NULL);
        g_signal_connect(G_OBJECT(text_url), "activate", urlGo, NULL);
        gtk_entry_set_invisible_char(GTK_ENTRY(text_url), 0);
    }

    /* Connect signal to X in the upper corner */
    g_signal_connect(G_OBJECT(window), "delete_event", owbQuit, NULL);

    /* Begin the main application */
    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_window = window;
}


int main(int argc, char *argv[])
{
    uint16_t width = 0, height = 0;
    uint8_t depth = 16;
    bool isFullscreen = false;

    signal(SIGSEGV, &signalCatcher);

#ifdef BAL_LOG // logger is not defined in NDEBUG
    // Setup a nice formatter for log
    RefPtr<BALFacilities::BTDeviceChannel> channel = new BALFacilities::BTDeviceChannel();
    RefPtr<BALFacilities::BTTextLogFormatter> textLogFormatter
            = new BALFacilities::BTTextLogFormatter();
    channel->setFormatter(textLogFormatter.get());
    BALFacilities::logger.addChannel(channel.get());
#endif

    
    char* url = "http://google.com";

    // simple arg parsing
    int argCount = 1;
    char* option;
    while (argCount < argc) {
        bool optionIsUnknown=true;
        option = argv[argCount];
        if ((strcmp(option, "--help")==0) || (strcmp(option,"-h")==0))
        {
            printf("usage:\n%s [--help] [-h] [-f] [any gtk option] [[--url] url]\n",argv[0]);
            return 0;
        }
        else if (strcmp(option,"-f")==0)
        {
            optionIsUnknown=false;
            isFullscreen=true;
        }
        else if (argv[argCount][0]!='-')
        {
            optionIsUnknown=false;
            url=argv[argCount];
        }else if (argCount+1<argc){
            if (strcmp(option, "--url") == 0){
                optionIsUnknown=false;
                url = argv[++argCount];
            }
        }

        
        
        
        if (optionIsUnknown)
            fprintf(stderr, "error: unknown option %s\n", argv[argCount]);

        // next arg
        argCount += 1;
    }


    if (!isFullscreen) {
        width = 690;
        height = 360;
    }
    else{
        width = 800;
        height = 480;
    }
    GtkInitN800(argc, argv, isFullscreen);

    getBIGraphicsDevice()->initialize(width, height, depth);
    BIEventLoop* aEventLoop = getBIEventLoop();
    if( aEventLoop == NULL ) {
      printf("No event loop\n");
      return 1;
    }

#if 1
    BIWindow *window = getBIWindowManager()->openWindow(0, 0, width, height);
#else // code below left as example
    BIWindow *window = getBIWindowManager()->openWindow(10, 10, 385, 
285);
    BIWindow *window2 = getBIWindowManager()->openWindow(10, 305, 385, 285);
    window2->setURL("http://www.pleyo.com");
    BIWindow *window3 = getBIWindowManager()->openWindow(405, 10, 385, 285);
    window3->setURL("http://webkit.org/");
    BIWindow *window4 = getBIWindowManager()->openWindow(405, 305, 385, 285);
    window4->setURL("http://www.ubuntu-fr.org");
#endif

    if(::WebCore::cookiesEnabled())
        getBICookieJar();

    window->setURL(url);

    SDL_SysWMinfo sdl_info;
    memset (&sdl_info, 0, sizeof(sdl_info));
    SDL_VERSION(&sdl_info.version);
    if (SDL_GetWMInfo(&sdl_info) <= 0 || sdl_info.subsystem != SDL_SYSWM_X11) {
        fprintf (stderr, "This is not X11\n");
        memset (&sdl_info, 0, sizeof(sdl_info));
    }

    Display *sdl_display = sdl_info.info.x11.display;
    Window sdl_window = sdl_info.info.x11.wmwindow;
    Window xwin = GDK_WINDOW_XWINDOW(GTK_WIDGET(gtk_window)->window);
    if (isFullscreen) {
        XReparentWindow(sdl_display, sdl_info.info.x11.wmwindow, xwin, 0, 0);
        XReparentWindow(sdl_display, sdl_info.info.x11.window, xwin, 0, 0);
    } else {
        XReparentWindow(sdl_display, sdl_info.info.x11.wmwindow, xwin, 10, 0);
        XReparentWindow(sdl_display, sdl_info.info.x11.window, xwin, 10, 0);
    }
    XSetInputFocus(sdl_display, sdl_info.info.x11.window, RevertToParent, CurrentTime);
    if (isFullscreen)
        gtk_window_fullscreen(GTK_WINDOW(gtk_window));

    handle_event( *aEventLoop );
    getBIWindowManager()->closeWindow(window);
    delete getBIWindowManager();
    delete aEventLoop;

    getBIGraphicsDevice()->finalize();
    delete getBIGraphicsDevice();

    if(::WebCore::cookiesEnabled())
        deleteBICookieJar(getBICookieJar());
	
    return 0;
}
