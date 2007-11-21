/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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

/**
 * @file  BCWindowManager.cpp
 *
 * Implementation file for BCWindowManager.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#include "config.h"
#include "BALConfiguration.h"
#include "BCWindowManager.h"
#include "BIEvent.h"
#include "BIEventLoop.h"
#include "BIGraphicsDevice.h"
#include "BINativeImage.h"
#include "BIWindowEvent.h"
#include "BIWindow.h"
#include "EventHandler.h"
#include "HitTestResult.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameView.h"
#include "FrameTree.h"
#include "IntRect.h"
#include "Page.h"
#include "Timer.h"
#include "WindowBal.h"


IMPLEMENT_GET_DELETE(BIWindowManager, BCWindowManager);

using namespace BAL;

namespace BAL {

BCWindowManager::BCWindowManager() : m_timer(this, &BCWindowManager::timerCallback)
{
}

BCWindowManager::~BCWindowManager()
{
    delete m_gc;
}
BIWindow* BCWindowManager::openWindow(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h)
{
    // NOTE windows' size is not working yet
    m_window = new WindowBal(x, y, w, h);
    // NOTE m_gc should be in WindowBal not shared
    m_gc = createBIGraphicsContext();
    m_gc->setWidget(m_window->widget());
    m_windowList.append(m_window);
    return m_window;
}

void BCWindowManager::closeWindow(BIWindow* window)
{
    if (!m_windowList.isEmpty() && window) {
        m_windowList.removeLast();// FIXME (window);
        getBIGraphicsDevice()->fillRect(*window->widget(), IntRect(0, 0, window->widget()->width(), window->widget()->height()), Color(0x0, 0x0, 0x0, 0xff));
        getBIGraphicsDevice()->update(*window->widget(), IntRect(0, 0, window->widget()->width(), window->widget()->height()));
        delete window;
        if (!m_windowList.isEmpty()) {
            if (m_window == window) // do not forget to reset active window
                setActiveWindow(m_windowList.last());

            for(int i = m_windowList.size() - 1; i >= 0; i--) {
                // post redraw event
                BIWindowEvent* event = createBIWindowEvent(BIWindowEvent::EXPOSE, true, IntRect(0,0,0,0), m_windowList[i]->widget());
                getBIEventLoop()->PushEvent(event);
            }
        }
        else
            setActiveWindow(NULL);
    }
    if (m_windowList.isEmpty()) {
        // the last window was closed: quit the application
        BAL::getBIEventLoop()->quit();
    }
}

BIWindow* BCWindowManager::activeWindow()
{
    return m_window;
}

void BCWindowManager::setActiveWindow(BIWindow* window)
{
    m_window = window;
}


void BCWindowManager::expose(BIWindow* window)
{
    WebCore::FrameView* view = const_cast<WebCore::FrameView*>(static_cast<const WebCore::FrameView*>(window->widget()));
    WebCore::FrameTree* childTree = view->frame()->tree();
    WebCore::Frame* child = 0;

    redrawView(view, false);
}
void BCWindowManager::redrawWindow(BIWindow* window)
{
    WebCore::FrameView* view = const_cast<WebCore::FrameView*>(static_cast<const WebCore::FrameView*>(window->widget()));
    if (!view->didFirstLayout()) {
            // no first layout yet, shall we really continue ?
            return;
    }
    WebCore::FrameTree* childTree = view->frame()->tree();
    // correct event pos to match window pos
    WebCore::Frame* child = 0;
    // paint main frame
    redrawView(view, true);
}

void BCWindowManager::redrawView(WebCore::FrameView* view, bool repaint)
{
    if (!view->frame()->renderer() || view->dirtyRect()->isEmpty())
        return;

    view->layoutIfNeededRecursive();
    IntRect rect(*view->dirtyRect());

    m_gc->setWidget(view);
    m_gc->save();

    // we have to clip to rect, because background paint may erase the graphical buffer around rect zone
    m_gc->clip(rect);
    getBIGraphicsDevice()->clear(*view, rect);
    // paint the needed rect in our m_gc
    // do not try to paint() the view instead
    if (repaint)
        view->frame()->paint(m_gc, rect);

    // intersect with frame's viewport or else subframe may overlap its viewport
    rect.intersect(IntRect(view->contentsX(), view->contentsY(), view->width(), view->height()));
    getBIGraphicsDevice()->update(*view, rect);
    m_gc->restore();

    view->setDirtyRect(IntRect(0, 0, 0, 0));

    WebCore::FrameTree* childTree = view->frame()->tree();
    // correct event pos to match window pos
    WebCore::Frame* child = 0;
}

static bool isAccepted = false;

// FIXME split this code into smaller methods
void BCWindowManager::handleEvent(BIEvent* event)
{
    if (m_windowList.isEmpty())
        return;

    IntSize offset;
    BTScrollView *mainView = (BTScrollView*)activeWindow()->widget();
    offset = mainView->scrollOffset();
    IntRect viewPort(0, 0, mainView->visibleWidth(), mainView->visibleHeight());

    BAL::BIWindowEvent* windowEvent = event->queryIsWindowEvent();
    // window event handling
    if (windowEvent) {
        const BTWidget* widget = windowEvent->widget();
        if (windowEvent->type() == BAL::BIWindowEvent::EXPOSE ||
           windowEvent->type() == BAL::BIWindowEvent::ACTIVE && windowEvent->gain() ) {
            BIWindow* window;
            // find the window to repaint, from back to front (new appended windows are in front)
            for(int i = m_windowList.size() - 1; i >= 0; i--) {
                window = m_windowList[i];
                expose(window);
            }
        } else if (windowEvent->type() == BAL::BIWindowEvent::RESIZE) {
            if (!widget)
                widget = mainView;
            if (widget->isFrameView()) {
                WebCore::FrameView* view = const_cast<WebCore::FrameView*>(static_cast<const WebCore::FrameView*>(widget));
                IntRect rect = windowEvent->getRectangle();
                getBIGraphicsDevice()->initialize(rect.width(), rect.height(), 32);
                view->setFrameGeometry(rect);
                view->frame()->forceLayout();
                view->adjustViewSize();
            }

        } else if (windowEvent->type() == BAL::BIWindowEvent::REDRAW) {
            BIWindow* window;
            // find the window to repaint, from back to front (new appended windows are in front)
            for(int i = m_windowList.size() - 1; i >= 0; i--) {
                window = m_windowList[i];
                redrawWindow(window);
            }
        }
    // mouse event handling
    } else {
        isAccepted = false;
        shiftEventPos(event, static_cast<WebCore::FrameView*> (mainView)->frame());
    static_cast<WebCore::FrameView*> (mainView)->frame()->handleEvent(event);
    }

}

void BCWindowManager::shiftEventPos(BIEvent* event, WebCore::Frame* f)
{
    WebCore::Frame *frame = f;

    WebCore::FrameTree* childTree = frame->tree();

    // correct event pos to match window pos
    WebCore::Frame* child = 0;
    bool isFound = false;
    // iterate all frame child to find one matching the event
    for (child = childTree->firstChild(); child; child = child->tree()->nextSibling()) {
        WebCore::FrameView* view = const_cast<WebCore::FrameView*>(child->view());
        if (view){
            BIMouseEvent *mouseEvent = event->queryIsMouseEvent();
            if (mouseEvent){
                IntPoint mousePos(mouseEvent->pos());
                if( ( view->x() <= mousePos.x() )
                &&  ( view->y() <= mousePos.y() )
                &&  ( view->x() + view->width() >= mousePos.x() )
                &&  ( view->y() + view->height() >= mousePos.y() ) ){
                    shiftEventPos( event, view->frame());
                    if (!isAccepted){
                        isAccepted = true;
                        mouseEvent->shiftPos(-view->x(), -view->y());
                        view->frame()->handleEvent(event);
                    }
                    else{
                        break;
                    }
                }
            }
            else
            {
                BIWheelEvent *wheelEvent = event->queryIsWheelEvent();
                if (wheelEvent){
                    IntPoint mousePos(wheelEvent->pos());
                    if( ( view->x() <= mousePos.x() )
                    &&  ( view->y() <= mousePos.y() )
                    &&  ( view->x() + view->width() >= mousePos.x() )
                    &&  ( view->y() + view->height() >= mousePos.y() ) ){
                        shiftEventPos( event, view->frame());
                        if (!wheelEvent->isAccepted()){
                            view->frame()->setIsActive(true);
                            view->frame()->handleEvent(event);
                            redrawView(view, true);
                        }
                        else
                            break;
                    }
                }
            }
        }
    }
}

void BCWindowManager::timerCallback(WebCore::Timer<BCWindowManager>* timer)
{
// disabled because ScrollView::updateContents posts redraw event directly
}

}
