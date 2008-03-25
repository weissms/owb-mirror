/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2007 Collabora, Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "gtk2xtbin.h"
#include "PluginViewGtk.h"
#include <gdk/gdkx.h>

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

namespace WebCore {

void PluginViewPrivate::updateWindow(const IntRect &windowRect, bool needsXEmbed) const
{
    GtkAllocation allocation = { windowRect.x(), windowRect.y(), windowRect.width(), windowRect.height() };
    if (m_widget) {
        gtk_widget_size_allocate(m_widget, &allocation);
        if (!needsXEmbed) {
            gtk_xtbin_resize(m_widget, windowRect.width(), windowRect.height());
        }
    }
}

void PluginViewPrivate::setNPWindowRect(const IntRect &rect, bool needsXEmbed)
{
    GtkAllocation allocation = { rect.x(), rect.y(), rect.width(), rect.height() };
    gtk_widget_size_allocate(m_widget, &allocation);
    if (!needsXEmbed) {
        gtk_xtbin_resize(m_widget, allocation.width, allocation.height);
    }
}

PluginViewPrivate::~PluginViewPrivate()
{
    if (!m_needsXEmbed && m_widget)
        gtk_widget_destroy(m_widget);
    if( m_gtkWindow )
        gtk_widget_destroy(m_gtkWindow);
    m_container = 0;
    m_widget = 0;
    m_gtkWindow = 0;
}


PluginViewPrivate::PluginViewPrivate()
    : m_container(0)
    , m_gtkWindow(0)
    , m_widget(0)
    , m_needsXEmbed(false)
{
}

GtkWidget* PluginViewPrivate::containingWindow(const IntRect &rect)
{
    if (m_container)
        return m_container;

    //Get OWB X display and window:
    SDL_SysWMinfo sdl_info;
    memset (&sdl_info, 0, sizeof(sdl_info));
    SDL_VERSION(&sdl_info.version);
    if (SDL_GetWMInfo(&sdl_info) <= 0 || sdl_info.subsystem != SDL_SYSWM_X11) {
        fprintf (stderr, "This is not X11\n");
        memset (&sdl_info, 0, sizeof(sdl_info));
    }
    Display *owbDisplay = sdl_info.info.x11.display;
    Window owbWindow = sdl_info.info.x11.wmwindow;

    //create the plugin Gtk widget
    m_gtkWindow = gtk_window_new(GTK_WINDOW_POPUP);
    //    gtk_window_set_decorated (GTK_WINDOW(m_gtkWindow),false);
    m_container = gtk_hbox_new(false, 0);
    gtk_container_add(GTK_CONTAINER(m_gtkWindow), m_container);
    //we must realize it to really create the Xwindow in order to reparent it just after.
    gtk_widget_realize(m_gtkWindow);
    gtk_window_set_default_size(GTK_WINDOW(m_gtkWindow), rect.width(), rect.height());


    //get the GdkWindow, and reparent it to the OWB window, at good position
    Window gtkWindow = GDK_DRAWABLE_XID((GTK_WIDGET(m_gtkWindow))->window);
    XReparentWindow(owbDisplay,gtkWindow, owbWindow, rect.x(), rect.y());


    gtk_widget_show_all(m_gtkWindow);

    return m_container;
}
void PluginViewPrivate::init(bool needsXEmbed, NPWindow *npWindow, bool isWindowed, XID *window, const IntRect &rect)
{
    if (needsXEmbed) {
        GtkWidget *win = GTK_WIDGET(containingWindow(rect));
        m_widget = gtk_socket_new();
        gtk_container_add(GTK_CONTAINER(win), m_widget);
    } else {
        GtkWidget *win = GTK_WIDGET(containingWindow(rect));
        //FIXME : remove this printf, if i remove this the gtk_xtbin_realize is not call
//        printf("container = %p\n", win );
        gtk_widget_realize(win);
        m_widget = gtk_xtbin_new(win, NULL);
    }

    GtkAllocation allocation = { rect.x(), rect.y(), rect.width(), rect.height() };
    gtk_widget_size_allocate(m_widget, &allocation);
    if (!needsXEmbed) {

        gtk_xtbin_resize(m_widget, allocation.width, allocation.height);
    }
    gtk_widget_show(m_widget);

    NPSetWindowCallbackStruct* ws = g_new0(NPSetWindowCallbackStruct, 1);

    ws->type = 0;

    if (needsXEmbed) {
        ws->display = GDK_WINDOW_XDISPLAY(m_widget->window);
        ws->visual = GDK_VISUAL_XVISUAL(gdk_window_get_visual(m_widget->window));
        ws->depth = gdk_window_get_visual(m_widget->window)->depth;
        ws->colormap = GDK_COLORMAP_XCOLORMAP(gdk_window_get_colormap(m_widget->window));
        *window = GDK_WINDOW_XWINDOW(m_widget->window);
    } else {
        ws->display = GTK_XTBIN(m_widget)->xtdisplay;
        ws->visual = GTK_XTBIN(m_widget)->xtclient.xtvisual;
        ws->depth = GTK_XTBIN(m_widget)->xtclient.xtdepth;
        ws->colormap = GTK_XTBIN(m_widget)->xtclient.xtcolormap;
        *window = GTK_XTBIN(m_widget)->xtwindow;

        XFlush (ws->display);
    }

    npWindow->ws_info = ws;

    if (isWindowed) {
        npWindow->type = NPWindowTypeWindow;
        npWindow->window = (void*)*window;
    } else {
        npWindow->type = NPWindowTypeDrawable;
        npWindow->window = 0;;
    }
}

XtAppContext PluginViewPrivate::getContext()
{
    return XtDisplayToApplicationContext(GTK_XTBIN(m_widget)->xtclient.xtdisplay);
}

int PluginViewPrivate::nvToolkit()
{
    return 2;
}

bool PluginViewPrivate::supportsXEmbed()
{
    return true;
}

XID PluginViewPrivate::nvNetscapeWindow(IntRect rect)
{
    return GDK_WINDOW_XWINDOW(GTK_WIDGET(containingWindow(rect))->window);
}

} // namespace WebCore
