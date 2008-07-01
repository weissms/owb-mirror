/*
 * Copyright (C) 2007 Holger Hans Peter Freyther
 * Copyright (C) 2007, 2008 Alp Toker <alp@atoker.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef BROWSERWIDGET_h
#define BROWSERWIDGET_h

#include <gtk/gtk.h>
#include <JavaScriptCore/JSBase.h>

#include "WebView.h"
#include "WebFrame.h"
//#include <webkit/webkitdefines.h>
//#include <webkit/webkitwebbackforwardlist.h>
//#include <webkit/webkitwebhistoryitem.h>
//#include <webkit/webkitwebsettings.h>

#ifdef G_OS_WIN32
    #ifdef BUILDING_WEBKIT
        #define WEBKIT_API __declspec(dllexport)
    #else
        #define WEBKIT_API __declspec(dllimport)
    #endif
    #define WEBKIT_OBSOLETE_API WEBKIT_API
#else
    #define WEBKIT_API __attribute__((visibility("default")))
    #define WEBKIT_OBSOLETE_API WEBKIT_API __attribute__((deprecated))
#endif

#ifndef WEBKIT_API
    #define WEBKIT_API
#endif

G_BEGIN_DECLS

#define WEBKIT_TYPE_WEB_VIEW            (webkit_web_view_get_type())
#define WEBKIT_WEB_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), WEBKIT_TYPE_WEB_VIEW, WebKitWebView))
#define WEBKIT_WEB_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  WEBKIT_TYPE_WEB_VIEW, WebKitWebViewClass))
#define WEBKIT_IS_WEB_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), WEBKIT_TYPE_WEB_VIEW))
#define WEBKIT_IS_WEB_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  WEBKIT_TYPE_WEB_VIEW))
#define WEBKIT_WEB_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  WEBKIT_TYPE_WEB_VIEW, WebKitWebViewClass))

typedef struct _WebKitWebView WebKitWebView;
typedef struct _WebKitWebViewClass WebKitWebViewClass;

typedef enum {
    WEBKIT_NAVIGATION_RESPONSE_ACCEPT,
    WEBKIT_NAVIGATION_RESPONSE_IGNORE,
    WEBKIT_NAVIGATION_RESPONSE_DOWNLOAD
} WebKitNavigationResponse;

typedef enum
{
    WEBKIT_WEB_VIEW_TARGET_INFO_HTML = - 1,
    WEBKIT_WEB_VIEW_TARGET_INFO_TEXT = - 2
} WebKitWebViewTargetInfo;

struct _WebKitWebView {
    GtkContainer parent_instance;

    //WebKitWebViewPrivate* priv;
};

struct _WebKitWebViewClass {
    GtkContainerClass parent_class;

    /*
     * default handler/virtual methods
     * DISCUSS: create_web_view needs a request and should we make this a signal with default handler? this would
     * require someone doing a g_signal_stop_emission_by_name
     * WebUIDelegate has nothing for create_frame, WebPolicyDelegate as well...
     */
    WebKitWebView*  (*create_web_view)  (WebKitWebView* web_view);

    /*
     * TODO: FIXME: Create something like WebPolicyDecisionListener_Protocol instead
     */
    WebKitNavigationResponse (*navigation_requested) (WebKitWebView* web_view, WebFrame* frame, gchar* uri);

    void (*window_object_cleared) (WebKitWebView* web_view, WebFrame* frame, JSGlobalContextRef context, JSObjectRef window_object);
    gchar*   (*choose_file) (WebKitWebView* web_view, WebFrame* frame, const gchar* old_file);
    gboolean (*script_alert) (WebKitWebView* web_view, WebFrame* frame, const gchar* alert_message);
    gboolean (*script_confirm) (WebKitWebView* web_view, WebFrame* frame, const gchar* confirm_message, gboolean* did_confirm);
    gboolean (*script_prompt) (WebKitWebView* web_view, WebFrame* frame, const gchar* message, const gchar* default_value, gchar** value);
    gboolean (*console_message) (WebKitWebView* web_view, const gchar* message, unsigned int line_number, const gchar* source_id);
    void (*select_all) (WebKitWebView* web_view);
    void (*cut_clipboard) (WebKitWebView* web_view);
    void (*copy_clipboard) (WebKitWebView* web_view);
    void (*paste_clipboard) (WebKitWebView* web_view);

    /*
     * internal
     */
    void   (*set_scroll_adjustments) (WebKitWebView*, GtkAdjustment*, GtkAdjustment*);
};

WEBKIT_API GType
webkit_web_view_get_type (void);

WEBKIT_API GtkWidget*
webkit_web_view_new (WebView *view);

G_END_DECLS

#endif
