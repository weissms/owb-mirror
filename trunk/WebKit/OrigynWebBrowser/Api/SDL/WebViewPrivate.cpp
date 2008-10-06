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

#include "WebViewPrivate.h"
#include "IntRect.h"
#include "FrameView.h"
#include "Frame.h"
#include "SDL.h"
#include "GraphicsContext.h"
#include "Page.h"
#include "EventHandler.h"
#include "FocusController.h"
#include <PlatformKeyboardEvent.h>
#include <PlatformMouseEvent.h>
#include <PlatformWheelEvent.h>
#include "SelectionController.h"
#include "Editor.h"
#include "owb-config.h"
#include "PopupMenu.h"
#include "CString.h"
#include "FileIO.h"
#include "WebFrame.h"
#include "WebView.h"
#include DEEPSEE_INCLUDE

using namespace WebCore;

void WebViewPrivate::onExpose(BalEventExpose event)
{
    //printf("WebViewPrivate::onExpose\n");
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;

    if(!isInitialized) {
        isInitialized = true;
        frame->view()->resize(m_rect.width(), m_rect.height());
        frame->forceLayout();
        frame->view()->adjustViewSize();
    }

    GraphicsContext ctx(m_webView->viewWindow());
    ctx.setBalExposeEvent(&event);
    if (frame->contentRenderer() && frame->view() && !m_webView->dirtyRegion().isEmpty()) {
        frame->view()->layoutIfNeededRecursive();
        IntRect dirty = m_webView->dirtyRegion();
        frame->view()->paint(&ctx, dirty);
	updateView(ctx.platformContext(), dirty);
        m_webView->clearDirtyRegion();
    }
}

void WebViewPrivate::onKeyDown(BalEventKey event)
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (!frame)
        return;
    PlatformKeyboardEvent keyboardEvent(&event);

    if (frame->eventHandler()->keyEvent(keyboardEvent))
        return;

    FrameView* view = frame->view();
    SelectionController::EAlteration alteration;
    if (event.keysym.sym == SDLK_LSHIFT)
        alteration = SelectionController::EXTEND;
    else
        alteration = SelectionController::MOVE;

    switch (event.keysym.sym) {
    case SDLK_DOWN:
        view->scrollBy(IntSize(0, cMouseWheelPixelsPerLineStep));
        return;
    case SDLK_UP:
        view->scrollBy(IntSize(0, -cMouseWheelPixelsPerLineStep));
        return;
    case SDLK_RIGHT:
        view->scrollBy(IntSize(cMouseWheelPixelsPerLineStep, 0));
        return;
    case SDLK_LEFT:
        view->scrollBy(IntSize(-cMouseWheelPixelsPerLineStep, 0));
        return;
    case SDLK_HOME:
        frame->selection()->modify(alteration, SelectionController::BACKWARD, DocumentBoundary, true);
        return;
    case SDLK_END:
        frame->selection()->modify(alteration, SelectionController::FORWARD, DocumentBoundary, true);
        return;
    case SDLK_ESCAPE:
        SDL_Event ev;
        ev.type = SDL_QUIT;
        SDL_PushEvent(&ev);
        return;
    case SDLK_F1:
    {
        m_webView->goBack();
        GraphicsContext ctx(m_webView->viewWindow());
        if (frame->contentRenderer() && frame->view()) {
            frame->view()->layoutIfNeededRecursive();
            IntRect dirty(0, 0, m_rect.width(), m_rect.height());
            frame->view()->paint(&ctx, dirty);
	}
        return;
    }
    case SDLK_F2:
    {
        m_webView->goForward();
        GraphicsContext ctx(m_webView->viewWindow());
        if (frame->contentRenderer() && frame->view()) {
            frame->view()->layoutIfNeededRecursive();
            IntRect dirty(0, 0, m_rect.width(), m_rect.height());
            frame->view()->paint(&ctx, dirty);
	}
        return;
    }
    case SDLK_F3:
        if (m_webView->canZoomPageIn())
            m_webView->zoomPageIn();
        return;
    case SDLK_F4:
        if (m_webView->canZoomPageOut())
            m_webView->zoomPageOut();
        return;
    default:
        break;
    }
}

void WebViewPrivate::onKeyUp(BalEventKey event)
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (!frame)
        return;
    PlatformKeyboardEvent keyboardEvent(&event);

    if (frame->eventHandler()->keyEvent(keyboardEvent))
        return ;
}

void WebViewPrivate::onMouseMotion(BalEventMotion event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;
    frame->eventHandler()->mouseMoved(PlatformMouseEvent(&event));
}

void WebViewPrivate::onMouseButtonDown(BalEventButton event)
{
    Frame* frame = core(m_webView->mainFrame());

    if (!frame)
        return;

    if (event.button == SDL_BUTTON_MIDDLE)
        return ;//webkit_web_view_forward_context_menu_event(webView, PlatformMouseEvent(event));
    frame->eventHandler()->handleMousePressEvent(PlatformMouseEvent(&event));
}

void WebViewPrivate::onMouseButtonUp(BalEventButton event)
{
    Frame* focusedFrame = m_webView->page()->focusController()->focusedFrame();
    if (!focusedFrame)
        return;
//     if (focusedFrame && focusedFrame->editor()->canEdit()) {
//         ;
//     }

    focusedFrame->eventHandler()->handleMouseReleaseEvent(PlatformMouseEvent(&event));
}

void WebViewPrivate::onScroll(BalEventScroll event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;
    PlatformWheelEvent wheelEvent(&event);
    frame->eventHandler()->handleWheelEvent(wheelEvent);
}

void WebViewPrivate::onResize(BalResizeEvent event)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;
    m_rect.setWidth(event.w);
    m_rect.setHeight(event.h);
    frame->view()->resize(event.w, event.h);
    frame->forceLayout();
    frame->view()->adjustViewSize();
}

void WebViewPrivate::onQuit(BalQuitEvent)
{
}

void WebViewPrivate::onUserEvent(BalUserEvent)
{
}

void WebViewPrivate::popupMenuHide()
{
    //BOOKMARKLET_INSTALL_PATH
}

void WebViewPrivate::popupMenuShow(void *popupInfo)
{
    PopupMenu *pop = static_cast<PopupMenu *>(popupInfo);
    if (!pop)
        return;
    //printf("pop %d %d %d %d\n", pop->windowRect().x(), pop->windowRect().y(), pop->windowRect().width(), pop->windowRect().height());
    int itemCount = pop->client()->listSize();

    String tabIndex = "var myTabId1 = new Array(";
    String tabName = "var myTabName1 = new Array(";
    for (int i = 0; i < itemCount; ++i) {
        String text = pop->client()->itemText(i);
        if (text.isEmpty())
            continue;
        if (i == 0) {
            tabName += "\"";
            tabIndex += "\"";
        } else {
            tabName += ", \"";
            tabIndex += ", \"";
        }

        tabName += text;
        tabName += "\"";
        tabIndex += String::number(i+1);
        tabIndex += "\"";
    }
    tabIndex += ");";
    tabName += ");";
    String path = BOOKMARKLET_INSTALL_PATH;
    path +=  "popup.js";

    File *f = new File(path);
    if (!f)
        return;
    if (f->open('r') == -1)
        return ;
    String buffer(f->read(f->getSize()));
    f->close();
    delete f;

    String callCreateTab = "createTabs(myTabId1 ,myTabName1 ,";
    callCreateTab += String::number(pop->windowRect().x());
    callCreateTab += ", ";
    callCreateTab += String::number(pop->windowRect().y());
    callCreateTab += ", ";
    callCreateTab += String::number(pop->windowRect().width());
    callCreateTab += ");";

    buffer = buffer.replace("@TabIndexDefinition", tabIndex);
    buffer = buffer.replace("@TabNameDefinition", tabName);
    buffer = buffer.replace("@callCreateTab", callCreateTab);
    
//    printf("popup = %s \n", buffer.utf8().data());

    m_webView->stringByEvaluatingJavaScriptFromString(buffer);
}

void WebViewPrivate::updateView(BalWidget *surf, IntRect rect)
{
    if (!surf || rect.isEmpty())
        return;
    rect.intersect(m_rect);
    /* use SDL_Flip only if double buffering is available */
    SDL_Rect sdlRect = {rect.x(), rect.y(), rect.width(), rect.height()};
    //printf("updateView x=%d y=%d w=%d h=%d\n", sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
    sdlRect.x = max(sdlRect.x, (Sint16)0);
    sdlRect.y = max(sdlRect.y, (Sint16)0);
    if (surf->flags & SDL_DOUBLEBUF)
        SDL_Flip(surf);
    else
        SDL_UpdateRect(surf, sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
}

void WebViewPrivate::sendExposeEvent(IntRect)
{
    SDL_Event ev;
    ev.type = SDL_VIDEOEXPOSE;
    SDL_PushEvent(&ev);
}

void WebViewPrivate::repaint(const WebCore::IntRect& windowRect, bool contentChanged, bool immediate, bool repaintContentOnly)
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

void WebViewPrivate::scrollBackingStore(WebCore::FrameView*, int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect)
{
    sendExposeEvent(scrollViewRect);
    
    IntRect s;
    if (dy != 0) {
        s = IntRect(0, scrollViewRect.height(), frameRect().width(), frameRect().height() - scrollViewRect.height());
    } else
        if (dx != 0) {
            s = IntRect(scrollViewRect.width(), 0, frameRect().width() - scrollViewRect.width(), frameRect().height());
        }
    m_webView->addToDirtyRegion(s);
    sendExposeEvent(s);
}

