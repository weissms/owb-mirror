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
#ifndef WebViewPrivate_H
#define WebViewPrivate_H

#include "browserWidget.h"
#include "WebView.h"
#include "Frame.h"
#include "IntRect.h"
#include "gtk/gtk.h"
#include "FrameView.h"

class WebViewPrivate {

public:
    WebViewPrivate(WebView *webView):m_webView(webView) {}
    ~WebViewPrivate() 
    {
//         if (m_view)
//             gtk_widget_destroy(m_view); 
    }

    void show()
    {
        gtk_widget_show(m_view);
    }
    
    void hide()
    {
        gtk_widget_hide(m_view);
    }

    void setFrameRect(WebCore::IntRect r)
    {
        m_rect = r;
    }

    WebCore::IntRect frameRect()
    { 
        return m_rect; 
    }
    
    BalWidget *createWindow(BalRectangle r)
    {
        return createWindow(r.x, r.y, r.width, r.height);
    }
  
    GtkWidget *createWindow(int x, int y, int width, int height)
    {
        WebCore::IntRect rect(x, y, width, height);
        if(rect != m_rect)
            m_rect = rect;
    
        m_view = webkit_web_view_new (m_webView);

        return m_view;
    }

    void initWithFrameView(WebCore::FrameView *frameView)
    {
        frameView->setGtkAdjustments(GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0)),
                                 GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0)));
    }

    void onExpose(BalEventExpose){}
    void onKeyDown(BalEventKey){}
    void onKeyUp(BalEventKey){}
    void onMouseMotion(BalEventMotion){}
    void onMouseButtonDown(BalEventButton){}
    void onMouseButtonUp(BalEventButton){}
    void onScroll(BalEventScroll){}
    void onResize(BalResizeEvent){}
    void onQuit(BalQuitEvent){}
    void onUserEvent(BalUserEvent){}
    void popupMenuHide() {}
    void popupMenuShow(void *popupInfo) {}
    
    void sendExposeEvent(WebCore::IntRect r) { 
        GdkRectangle rect = r;
        GdkWindow* window = GTK_WIDGET(m_webView->viewWindow())->window;
        gdk_window_invalidate_rect(window, &rect, true);
        //gdk_window_process_updates(window, true);
    }
    
    
    void repaint(const WebCore::IntRect& windowRect, bool contentChanged, bool immediate = false, bool repaintContentOnly = false)
    {
        if (!repaintContentOnly) {
            m_webView->addToDirtyRegion(windowRect);
            sendExposeEvent(windowRect);
        }
        if (contentChanged)
            m_webView->addToDirtyRegion(windowRect);
        if (immediate) {
            if (repaintContentOnly)
                m_webView->updateBackingStore(core(m_webView->topLevelFrame())->view());
            else
                sendExposeEvent(windowRect);
        }
    }
    
    void scrollBackingStore(WebCore::FrameView*, int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect) 
    { 
        sendExposeEvent(scrollViewRect);
    
        WebCore::IntRect s;
        if (dy != 0) {
            s = WebCore::IntRect(0, scrollViewRect.height(), frameRect().width(), frameRect().height() - scrollViewRect.height());
        } else
            if (dx != 0) {
                s = WebCore::IntRect(scrollViewRect.width(), 0, frameRect().width() - scrollViewRect.width(), frameRect().height());
            }
        m_webView->addToDirtyRegion(s);
        sendExposeEvent(s);
    }

    void clearDirtyRegion()
    {
        m_backingStoreDirtyRegion.setX(0);
        m_backingStoreDirtyRegion.setY(0);
        m_backingStoreDirtyRegion.setWidth(0);
        m_backingStoreDirtyRegion.setHeight(0);
    }

    BalRectangle dirtyRegion()
    {
        GdkRectangle rect = {m_backingStoreDirtyRegion.x(), m_backingStoreDirtyRegion.y(), m_backingStoreDirtyRegion.width(), m_backingStoreDirtyRegion.height()};
        return rect;
    }

    void addToDirtyRegion(const BalRectangle& dirtyRect)
    {
        m_backingStoreDirtyRegion.unite(dirtyRect);
    }

    void fireWebKitEvents() {}

private:
    WebCore::IntRect m_rect;
    GtkWidget *m_view;
    WebView *m_webView;
    WebCore::IntPoint m_backingStoreSize;
    WebCore::IntRect m_backingStoreDirtyRegion;
};


#endif
