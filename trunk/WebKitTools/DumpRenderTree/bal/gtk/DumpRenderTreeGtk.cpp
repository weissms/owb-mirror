#include "config.h"
#include <gtk/gtk.h>
#include "cairo.h"
#include <Api/WebFrame.h>
#include <Api/WebView.h>
#include "Page.h"
#include "Frame.h"
#include "FrameView.h"

static GtkWidget* main_window;
volatile bool done;

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

//     GtkWidget* window = gtk_window_new(GTK_WINDOW_POPUP);
//     gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
//     GtkContainer* container = GTK_CONTAINER(gtk_fixed_new());
//     gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(container));
//     gtk_widget_realize(window);
// 
//     return window;
}
void init(int argc, char *argv[])
{
    gtk_init (&argc, &argv);
}

BalWidget* createWindow(WebView *webView)
{
    main_window = create_window ();

    GtkWidget* scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    BalWidget *view = webView->viewWindow();
    gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET (view));
    gtk_container_add (GTK_CONTAINER (main_window), scrolled_window);
    gtk_widget_realize(main_window);
    return view;
}

void startEventLoop(BalWidget *view)
{
    done = false;
    gtk_widget_grab_focus (view);
    gtk_widget_realize(view);
    //gtk_widget_show_all (main_window);

    while (!done)
        g_main_context_iteration(NULL, true);

}

void stopEventLoop()
{
    done = true;
}
