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

#include "config.h"
#include "WebViewPrivate.h"
#include "IntRect.h"
#include "FrameView.h"
#include "Frame.h"
#include "GraphicsContext.h"
#include "Page.h"
#include "EventHandler.h"
#include "FocusController.h"
#include "HitTestResult.h"
#include "HitTestRequest.h"
#include <MainThread.h>
#include "NotImplemented.h"
#include <PlatformKeyboardEvent.h>
#include <PlatformMouseEvent.h>
#include <PlatformWheelEvent.h>
#include "SelectionController.h"
#include <SharedTimer.h>
#include "Editor.h"
#include "owb-config.h"
#include "PopupMenu.h"
#include "CString.h"
#include "FileIO.h"
#include "WebFrame.h"
#include "WebView.h"
#include <QPainter>

using namespace WebCore;

WebViewPrivate::WebViewPrivate(WebView *webView)
    : m_webView(webView)
    , isInitialized(false)
//    , m_scrollSurface(0)
{
}

void WebViewPrivate::fireWebKitEvents()
{
}

void WebViewPrivate::onExpose(BalEventExpose ev){
    QPainter painter(m_webView->viewWindow());
    const QRegion clip = ev->region();
    paint(&painter, clip);
    m_webView->viewWindow()->update();
}

void WebViewPrivate::paint(QPainter *painter, const QRegion &clip)
{
    Frame* frame = core(m_webView->mainFrame());
    if (!frame)
        return;

    if (!frame->view() || !frame->contentRenderer())
        return;

    if(!isInitialized) {
        isInitialized = true;
        //frame->view()->resize(m_rect.width(), m_rect.height());
        frame->view()->setFrameRect(m_rect);
        frame->view()->forceLayout();
        frame->view()->adjustViewSize();
    }

    frame->view()->layoutIfNeededRecursive();

    GraphicsContext ctx(painter);
    QVector<QRect> vector = clip.rects();
    WebCore::FrameView* view = frame->view();
    for (int i = 0; i < vector.size(); ++i) {
        if (i > 0) {
            painter->save();
            painter->setClipRect(vector.at(i), Qt::IntersectClip);
        }
        view->paint(&ctx, vector.at(i));

        if (i > 0)
            painter->restore();
    }
}

void WebViewPrivate::onKeyDown(BalEventKey ev)
{
    bool handled = false;
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (!frame)
        return;

    // we forward the key event to WebCore first to handle potential DOM
    // defined event handlers and later on end up in EditorClientQt::handleKeyboardEvent
    // to trigger editor commands via triggerAction().
    if (!handled)
        handled = frame->eventHandler()->keyEvent(ev);
    if (!handled) {
        handled = true;
        FrameView* view = frame->view();
        SelectionController::EAlteration alteration;
        alteration = SelectionController::MOVE;

        switch (ev->key()) {
        case Qt::Key_Down:
            view->scrollBy(IntSize(0, (int)cMouseWheelPixelsPerLineStep));
            break;
        case Qt::Key_Up:
            view->scrollBy(IntSize(0, (int)-cMouseWheelPixelsPerLineStep));
            break;
        case Qt::Key_Right:
            view->scrollBy(IntSize((int)cMouseWheelPixelsPerLineStep, 0));
            break;
        case Qt::Key_Left:
            view->scrollBy(IntSize((int)-cMouseWheelPixelsPerLineStep, 0));
            break;
        case Qt::Key_Home:
            frame->selection()->modify(alteration, SelectionController::BACKWARD, DocumentBoundary, true);
            break;
        case Qt::Key_End:
            frame->selection()->modify(alteration, SelectionController::FORWARD, DocumentBoundary, true);
            break;
        case Qt::Key_Escape:
            m_webView->viewWindow()->close();
            break;
        case Qt::Key_F1:
            m_webView->goBack();
            break;
        case Qt::Key_F2:
            m_webView->goForward();
            break;
        default:
            break;
        }
    }
    ev->setAccepted(handled);
}

void WebViewPrivate::onKeyUp(BalEventKey ev)
{
    if (ev->isAutoRepeat()) {
        ev->setAccepted(true);
        return;
    }

    WebCore::Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    bool handled = frame->eventHandler()->keyEvent(ev);
    ev->setAccepted(handled);
}

void WebViewPrivate::onMouseMotion(BalEventMotion ev)
{
    WebCore::Frame* frame = core(m_webView->mainFrame());
    if (!frame->view())
        return;

    bool accepted = frame->eventHandler()->mouseMoved(PlatformMouseEvent(ev, 0));
    ev->setAccepted(accepted);
}

void WebViewPrivate::onMouseButtonDown(BalEventButton ev)
{
    WebCore::Frame* frame = core(m_webView->mainFrame());
    if (!frame->view())
        return;

    bool accepted = false;
    PlatformMouseEvent mev(ev, 1);
    // ignore the event if we can't map Qt's mouse buttons to WebCore::MouseButton
    if (mev.button() != NoButton)
        accepted = frame->eventHandler()->handleMousePressEvent(mev);
    ev->setAccepted(accepted);
}

void WebViewPrivate::onMouseButtonUp(BalEventButton ev)
{
    WebCore::Frame* frame = core(m_webView->mainFrame());
    if (!frame->view())
        return;

    bool accepted = false;
    PlatformMouseEvent mev(ev, 0);
    // ignore the event if we can't map Qt's mouse buttons to WebCore::MouseButton
    if (mev.button() != NoButton)
        accepted = frame->eventHandler()->handleMouseReleaseEvent(mev);
    ev->setAccepted(accepted);
}

void WebViewPrivate::onScroll(BalEventScroll ev)
{
    WebCore::Frame* frame = core(m_webView->mainFrame());
    if (!frame->view())
        return;

    WebCore::PlatformWheelEvent pev(ev);
    bool accepted = frame->eventHandler()->handleWheelEvent(pev);
    ev->setAccepted(accepted);
}

void WebViewPrivate::onResize(BalResizeEvent ev)
{
    m_rect = QRect(QPoint(0,0), ev->size());

    Frame* frame = core(m_webView->mainFrame());

    if (frame && frame->view()) {
        frame->view()->setFrameRect(m_rect);
        frame->view()->forceLayout();
        frame->view()->adjustViewSize();
    }
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
}

void WebViewPrivate::updateView(BalWidget *surf, IntRect rect)
{
}

void WebViewPrivate::sendExposeEvent(IntRect)
{
}

void WebViewPrivate::repaint(const WebCore::IntRect& windowRect, bool contentChanged, bool immediate, bool repaintContentOnly)
{
    // No double buffer, so only update the QWidget if content changed.
    if (contentChanged) {
        QWidget* view = m_webView->viewWindow();
        if (view) {
            QRect rect(windowRect);
            rect = rect.intersected(m_rect);
            if (!rect.isEmpty()) {
                view->update(rect);
            }
        }
    } 
}

void WebViewPrivate::scrollBackingStore(WebCore::FrameView* frameView, int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect)
{
    QWidget* view = m_webView->viewWindow();
    if (view) {
        view->scroll(dx, dy, scrollViewRect);
        view->update();
    }
    //emit view->scroll(dx, dy, scrollViewRect);
}

void WebViewPrivate::runJavaScriptAlert(WebFrame* frame, const char* message)
{
    printf("Javascript Alert: %s (from frame %p)\n", message, frame);
}

void WebViewPrivate::clearTransparentView()
{
    // FIXME: Implement.
    balNotImplemented();
}
