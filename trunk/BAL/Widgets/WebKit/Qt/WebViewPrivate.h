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

#include "WebView.h"
#include "IntRect.h"
#include "FrameView.h"
#include "Frame.h"
#include "BALBase.h"

class QPainter;
class QRegion;

class WebViewPrivate {
public:
    WebViewPrivate(WebView *webView);
    ~WebViewPrivate() 
    {
    }

    void show()
    {
    }
    
    void hide()
    {
    }

    void setFrameRect(WebCore::IntRect r)
    {
        m_rect = r;
    }

    BalRectangle frameRect()
    {
        return m_rect;
    }
    
    BalWidget *createWindow(BalRectangle r)
    {
        WebCore::IntRect rect(r);
        if(rect != m_rect)
            m_rect = rect;
    

        return 0;
    }

    void initWithFrameView(WebCore::FrameView *frameView)
    {
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
        return m_backingStoreDirtyRegion;
    }

    void addToDirtyRegion(const BalRectangle& dirtyRect)
    {
        m_backingStoreDirtyRegion.unite(dirtyRect);
    }

    BalRectangle onExpose(BalEventExpose event);
    bool onKeyDown(BalEventKey event);
    bool onKeyUp(BalEventKey event);
    bool onMouseMotion(BalEventMotion event);
    bool onMouseButtonDown(BalEventButton event);
    bool onMouseButtonUp(BalEventButton event);
    bool onScroll(BalEventScroll event);
    void onResize(BalResizeEvent event);
    void onQuit(BalQuitEvent);
    void onUserEvent(BalUserEvent);
    void popupMenuHide();
    void popupMenuShow(void *popupInfo);

    void sendExposeEvent(WebCore::IntRect);
    
    void repaint(const WebCore::IntRect&, bool contentChanged, bool immediate = false, bool repaintContentOnly = false);
    
    void scrollBackingStore(WebCore::FrameView*, int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect);

    void fireWebKitTimerEvents() {}
    void fireWebKitThreadEvents() {}
    
    void repaintAfterNavigationIfNeeded();

    void resize(BalRectangle r);

    void move(BalPoint lastPos, BalPoint newPos);

    void closeWindowSoon() {};

    void setViewWindow(BalWidget*) {}

private:
    void updateView(BalWidget* widget, WebCore::IntRect rect);
    BalRectangle paint(QPainter *painter, const QRegion &clip);
    WebCore::IntRect m_rect;
    WebView* m_webView;
    bool isInitialized;
//    SDL_Surface* m_scrollSurface;
//    SDL_Rect m_dstRect;
//    SDL_Rect m_srcRect;
    WebCore::IntRect m_scrollUpdateRect;
    WebCore::IntRect m_hUpdateRect;
    WebCore::IntRect m_vUpdateRect;

    WebCore::IntPoint m_backingStoreSize;
    WebCore::IntRect m_backingStoreDirtyRegion;
};


#endif
