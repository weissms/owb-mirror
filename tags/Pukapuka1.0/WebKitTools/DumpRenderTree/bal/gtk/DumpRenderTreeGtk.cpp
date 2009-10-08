#include <gtk/gtk.h>
#include "cairo.h"
#include <WebKit.h>
#include <DumpRenderTree.h>

// Choose some default values.
const unsigned int maxViewWidth = 800;
const unsigned int maxViewHeight = 600;
unsigned int waitToDumpWatchdog = 0;

static GtkWidget* main_window;

static void destroy_cb (GtkWidget* widget, gpointer data)
{
    gtk_main_quit ();
}

BalRectangle clientRect(bool isSVGW3CTest)
{
    GdkRectangle clientRect = {0, 0, isSVGW3CTest ? 480 : maxViewWidth, isSVGW3CTest ? 360 : maxViewHeight};
    return clientRect;
}

static GtkWidget* create_window (BalRectangle rect)
{
    GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), rect.width, rect.height);
    gtk_widget_set_name (window, "GtkLauncher");
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_cb), NULL);

    return window;
}

void init(int argc, char *argv[])
{
    gtk_init (&argc, &argv);
}

BalWidget* createWindow(WebView *webView, BalRectangle rect)
{
    main_window = create_window(rect);

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

static gboolean waitToDumpWatchdogFired(void*)
{
    const char* message = "FAIL: Timed out waiting for notifyDone to be called\n";
    fprintf(stderr, "%s", message);
    fprintf(stdout, "%s", message);
    waitToDumpWatchdog = 0;
    dump();
    return FALSE;
}


void addTimetoDump(int timeoutSeconds)
{
#if GLIB_CHECK_VERSION(2,14,0)
    waitToDumpWatchdog = g_timeout_add_seconds(timeoutSeconds, waitToDumpWatchdogFired, 0);
#else
    waitToDumpWatchdog = g_timeout_add(timeoutSeconds * 1000, waitToDumpWatchdogFired, 0);
#endif
}

void removeTimer()
{
    g_source_remove(waitToDumpWatchdog);
}
