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

#include <gtk/gtk.h>
#include <cairo.h>
#include <WebKit.h>

static GtkWidget* main_window;
static WebView* webView = NULL;

using namespace WebCore;

static void destroy_cb (GtkWidget* widget, gpointer data)
{
    gtk_main_quit ();
}

static GtkWidget* create_window ()
{
    GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
    gtk_widget_set_name (window, "GtkLauncher");
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_cb), NULL);

    return window;
}

void progressNotification()
{
    printf("progress : %d \n", int(webView->estimatedProgress() * 100));
}

int main (int argc, char* argv[])
{
    gtk_init (&argc, &argv);

    webView = WebView::createInstance();

    main_window = create_window ();

    GdkRectangle clientRect= {0, 0, 800, 600};
   
    webView->initWithFrame(clientRect, "", "");

    GtkWidget* scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget *view = webView->viewWindow();
    gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET (view));
    gtk_container_add (GTK_CONTAINER (main_window), scrolled_window);

    webView->registerOnNotifyProgress(progressNotification);
    gchar* uri = (gchar*) (argc > 1 ? argv[1] : "http://www.google.com/");
    webView->mainFrame()->loadURL(uri);

    /*if(webView->canZoomPageIn()) {
        printf("canZoomPageIn\n");
        webView->zoomPageIn();
    }*/
    /*if(webView->canMakeTextLarger()) {
        printf("canMakeTextLarger\n");
        webView->makeTextLarger();
    }*/


    gtk_widget_grab_focus (view);
    gtk_widget_show_all (main_window);
    gtk_main ();

    delete webView;
    return 0;
}
