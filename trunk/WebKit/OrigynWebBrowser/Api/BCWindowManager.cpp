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
#include "FocusController.h"
#include "Frame.h"
#include "FrameView.h"
#include "FrameTree.h"
#include "IntRect.h"
#include "Page.h"
#include "Timer.h"
#include "WindowBal.h"

using namespace BAL;

IMPLEMENT_GET_DELETE(BIWindowManager, BCWindowManager);

namespace BAL {
    
BCWindowManager::BCWindowManager() : m_timer(this, &BCWindowManager::timerCallback) 
{
    m_timer.startRepeating(0.05); // 1/20th of a second
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

            if (widget) {
                IntRect rect(0, 0, widget->width(), widget->height());
                getBIGraphicsDevice()->update(*widget, rect);
            }
        } else if (windowEvent->type() == BAL::BIWindowEvent::REDRAW) {

            const BIWindow* window;
            // find the window to repaint, from back to front (new appended windows are in front)
            for(int i = m_windowList.size() - 1; i >= 0; i--) {
                window = m_windowList[i];
                
                WebCore::FrameView* view = const_cast<WebCore::FrameView*>(static_cast<const WebCore::FrameView*>(window->widget()));
    
                if (!view->didFirstLayout()) {
                     // no first layout yet, shall we really continue ?
                     return;
                }
                // we must find  what window has to be redraw
                // for now, assume activeWindow() is the only one
                    widget = window->widget();
                    if (view->didFirstLayout() && widget && widget->isFrameView() && !widget->dirtyRect()->isEmpty()) {
        
                        view = const_cast<WebCore::FrameView*>(static_cast<const WebCore::FrameView*>(widget));
        
                        if (view->frame()->renderer()) {
                        if (view->layoutPending())
                            view->layout(true);
                        // paint the needed rect in our m_gc
                        
                        IntRect rect(*widget->dirtyRect());
                        getBIGraphicsDevice()->clear(*widget, rect);
                        m_gc->setWidget(widget);
                        view->frame()->paint(m_gc, rect);
                        // NOTE do something to scroll view before copy !
                        // we must clip our widget or else the update won't work
                        rect.intersect(viewPort);
                        getBIGraphicsDevice()->update(*widget, rect);
                        // FIXME we shouldn't const cast here
                        const_cast<BTWidget*>(widget)->setDirtyRect(IntRect(0, 0, 0, 0));
                        }
                    }
            }
        }
    // mouse event handling
    } else {
        IntRect mouseRect;
        // all other events (ie not windowing ones) are treated here
        BIMouseEvent* mouseEvent = event->queryIsMouseEvent();
        if (mouseEvent)
            mouseRect = IntRect(mouseEvent->pos(), IntSize(1,1));
        BIWheelEvent* wheelEvent = event->queryIsWheelEvent();
        if (wheelEvent)
            mouseRect = IntRect(wheelEvent->pos(), IntSize(1,1));

        BIWindow* window = NULL; // the window that will handle the event 
        if (mouseEvent || wheelEvent) {
                               
            // find active window for mouse event
            for(int i = m_windowList.size() - 1; i >= 0; i--) {
                window = m_windowList[i];
                IntRect winRect(window->widget()->pos(), window->widget()->size());
                if (mouseRect.intersects(winRect))
                    break;    
            }
            
               WebCore::FrameTree* childTree = ((WebCore::FrameView*)window->widget())->frame()->tree();
                // correct event pos to match window pos
               WebCore::Frame* child = 0;
               const BTWidget* widget;
               bool accepted = false;
               for (child = childTree->firstChild(); child; child = child->tree()->nextSibling()) {
                   widget = child->view();
                   IntRect widgetRect(widget->pos() + window->widget()->pos(), widget->size());

                   if (mouseRect.intersects(widgetRect)) {
                        // we found our frame
                        if (mouseEvent) {
                            mouseEvent->shiftPos(-widget->x() - window->widget()->x(), -widget->y() - window->widget()->y());
                            if (mouseEvent->button() == BIMouseEvent::LeftButton &&
                                mouseEvent->eventType() == BIMouseEvent::MouseEventPressed) {
                                setActiveWindow(window);
                                child->view()->frame()->page()->focusController()->setFocusedFrame(child->view()->frame());
                            }
                        }
                        else if (wheelEvent) {
                            // NOTE wheelEvent should have right pos also
//                            wheelEvent->shiftPos(-widget->x() - window->widget()->x(), -widget->y() - window->widget()->y());
                        }
                        ((WebCore::FrameView*)widget)->frame()->handleEvent(event);
                        // do not propagate this event twice
                        accepted = true;
                        break;
                   }
               }
        
            IntRect widgetRect(window->widget()->pos(), window->widget()->size());
            if (!accepted && mouseRect.intersects(widgetRect)) {
                // shift event coordsw
                int dx = window->widget()->x();
                int dy = window->widget()->y();
                if (mouseEvent) {
                    mouseEvent->shiftPos(-dx, -dy);
                    if (mouseEvent->button() == BIMouseEvent::LeftButton &&
                        mouseEvent->eventType() == BIMouseEvent::MouseEventPressed)
                        setActiveWindow(window);
                }
                else if (wheelEvent) {
                    // NOTE wheelEvent should have right pos also
//                    wheelEvent->shiftPos(-dx, -dy);
                }
                
                ((WebCore::FrameView*)window->widget())->frame()->handleEvent(event);
            }
        }
        // keyboard and wheel events handling
        else {
            ((WebCore::FrameView*)(activeWindow())->widget())->frame()->handleEvent(event);
        }
    }
    
}   

void BCWindowManager::timerCallback(WebCore::Timer<BCWindowManager>* timer)
{
    // post redraw event
    BIWindowEvent* event = createBIWindowEvent(BIWindowEvent::REDRAW, true, IntRect(0,0,0,0), NULL);
    getBIEventLoop()->PushEvent(event);
}

}
