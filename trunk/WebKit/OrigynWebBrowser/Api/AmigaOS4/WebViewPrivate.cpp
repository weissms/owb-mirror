/*
 * Copyright (C) 2008 Joerg Strohmayer
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
#include "cairo.h"
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

#include <proto/intuition.h>
#include <proto/popupmenu.h>
#include <libraries/keymap.h>

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

    if (!m_webView->viewWindow()->window)
        return;

    GraphicsContext ctx(m_webView->viewWindow()->cr);
//    if (frame->contentRenderer() && frame->view() && !m_webView->dirtyRegion().isEmpty()) {
    if (frame->contentRenderer() && frame->view()) {
        frame->view()->layoutIfNeededRecursive();
        IntRect dirty = m_webView->dirtyRegion();
        if (dirty.isEmpty())
            dirty = frameRect();
        frame->view()->paint(&ctx, dirty);
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

    if (IDCMP_RAWKEY == event.Class) {
        FrameView* view = frame->view();
        SelectionController::EAlteration alteration;
        if (event.Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
            alteration = SelectionController::EXTEND;
        else
            alteration = SelectionController::MOVE;

        switch (event.Code) {
        case RAWKEY_CRSRDOWN:
            view->scrollBy(0, LINE_STEP);
            return;
        case RAWKEY_CRSRUP:
            view->scrollBy(0, -LINE_STEP);
            return;
        case RAWKEY_CRSRRIGHT:
            view->scrollBy(LINE_STEP, 0);
            return;
        case RAWKEY_CRSRLEFT:
            view->scrollBy(-LINE_STEP, 0);
            return;
        case RAWKEY_PAGEDOWN:
            view->scrollBy(0, view->visibleHeight() * 0.8);
            return;
        case RAWKEY_PAGEUP:
            view->scrollBy(0, view->visibleHeight() * -0.8);
            return;
        case RAWKEY_HOME:
            frame->selection()->modify(alteration, SelectionController::BACKWARD, DocumentBoundary, true);
            return;
        case RAWKEY_END:
            frame->selection()->modify(alteration, SelectionController::FORWARD, DocumentBoundary, true);
            return;
#if 0
        case SDLK_ESCAPE:
            SDL_Event ev;
            ev.type = SDL_QUIT;
            SDL_PushEvent(&ev);
            return;
#endif
        case RAWKEY_F1:
        {
            m_webView->goBack();
            GraphicsContext ctx(m_webView->viewWindow()->cr);
            if (frame->contentRenderer() && frame->view()) {
                frame->view()->layoutIfNeededRecursive();
                IntRect dirty(0, 0, m_rect.width(), m_rect.height());
                frame->view()->paint(&ctx, dirty);
                m_webView->clearDirtyRegion();
            }
            return;
        }
        case RAWKEY_F2:
        {
            m_webView->goForward();
            GraphicsContext ctx(m_webView->viewWindow()->cr);
            if (frame->contentRenderer() && frame->view()) {
                frame->view()->layoutIfNeededRecursive();
                IntRect dirty(0, 0, m_rect.width(), m_rect.height());
                frame->view()->paint(&ctx, dirty);
                m_webView->clearDirtyRegion();
            }
            return;
        }
        case RAWKEY_F3:
            if (m_webView->canZoomPageIn())
                m_webView->zoomPageIn();
            return;
        case RAWKEY_F4:
            if (m_webView->canZoomPageOut())
                m_webView->zoomPageOut();
            return;
        }
    }
    else if (event.Qualifier & (IEQUALIFIER_CONTROL | AMIGAKEYS))
        switch (event.Code) {
        case 'c':
        case 'x':
            frame->editor()->command("Copy").execute();
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

    if (IECODE_MBUTTON == event.Code)
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

    BalWidget *widget = m_webView->viewWindow();
    if (!widget)
        return;

    int itemCount = pop->client()->listSize();
    if (itemCount > 25)
        return;

    Object *menu, *items[itemCount];

    menu = (Object *)IIntuition->NewObject(IPopupMenu->POPUPMENU_GetClass(), NULL,
                                           PMA_Left, pop->windowRect().x() + widget->offsetx,
                                           PMA_Top, pop->windowRect().y() + widget->offsety,
                                           TAG_DONE);
    if (!menu)
        return;

    bool gotAllItems = true;

    for (int i = 0; i < itemCount; ++i) {
        CString text = pop->client()->itemText(i).latin1();
        items[i] = (Object *)IIntuition->NewObject(IPopupMenu->POPUPMENU_GetItemClass(), NULL,
                                                   PMIA_Title, pop->client()->itemIsSeparator(i) ? (const char *)~0 : text.data() ?: "",
                                                   PMIA_Hidden, !pop->client()->itemIsEnabled(i),
                                                   PMIA_FillPen, pop->client()->itemIsSelected(i),
                                                   TAG_DONE);
        if (!items[i])
            gotAllItems = false;
        else
            IIntuition->IDoMethod(menu, OM_ADDMEMBER, items[i]);
    }

    if (gotAllItems) {
        Object *selected = (Object *)IIntuition->IDoMethod(menu, PM_OPEN, widget->window);
        for (int i = 0; i < itemCount; ++i)
            if (selected == items[i]) {
                pop->client()->setTextFromItem(i);
                pop->client()->valueChanged(i);
                break;
            }
    }

    IIntuition->DisposeObject(menu);
}
