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
 * @file  BCWebView.cpp
 *
 * Implementation file for BCWebView.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#include "config.h"
#include "BALConfiguration.h"
#include "BCWebView.h"
#include "BIKeyboardEvent.h"
#include "BIMouseEvent.h"
#include "BTLogHelper.h"
#include "ChromeClientBal.h"
#include "ContextMenuClientBal.h"
#include "CString.h"
#include "Document.h"
#include "DragClientBal.h"
#include "EditorClientBal.h"
#include "EventHandler.h"
#include "WindowBal.h"
#include "FocusController.h"
#include "FrameBal.h"
#include "FrameLoaderClientBal.h"
#include "FrameLoader.h"
#include "FrameLoadRequest.h"
#include "FrameView.h"
#include "KeyboardEvent.h"
#include "Page.h"
#include "RenderTreeAsText.h"
#include "TextStream.h"

using WebCore::EventHandler;
using WebCore::Frame;
using WebCore::FrameBal;
using WebCore::FrameLoadRequest;
using WebCore::FrameLoaderClientBal;
using WebCore::Page;

// should not be in namespace BAL, so don't use the macro
static BIWebView* __view = 0;
BIWebView* getBIWebView()
{
    if (__view)
        return __view;
    else
        return __view = new BCWebView();
}

void deleteBIWebView()
{
    delete __view;
    __view = 0;
}

BCWebView::BCWebView() : m_frameLoaderClient(0)
{
    WebCore::ChromeClientBal* chromeClient = new WebCore::ChromeClientBal();
    WebCore::ContextMenuClientBal* contextMenuClient = new WebCore::ContextMenuClientBal();
    WebCore::EditorClientBal* editorClient = new WebCore::EditorClientBal();
    Page* page = new Page(chromeClient, contextMenuClient, editorClient,
                    new WebCore::DragClientBal());
    editorClient->setPage(page);
    FrameLoaderClientBal* frameLoaderClient = new FrameLoaderClientBal();
    m_mainFrame = new FrameBal(new BAL::WindowBal(0,0,800,600,32), page, 0, frameLoaderClient);
    frameLoaderClient->setFrame(m_mainFrame);
    chromeClient->setFrame(m_mainFrame);
}

BCWebView::~BCWebView()
{
    // delete page as in WebKit WebFrame
    delete m_mainFrame->page();

    delete m_mainFrame;
}

#ifndef NDEBUG
void dumpRenderer(WebCore::RenderObject* aRenderObject)
{
    WebCore::DeprecatedString str;
    WebCore::DeprecatedString ind;
    WebCore::TextStream ts(&str);
    if (aRenderObject) {
        logml(MODULE_GLUE, LEVEL_INFO, "DUMP RENDER TREE");
        aRenderObject->dump( &ts, ind );
        log( str.ascii() );
    }
    else
        logml(MODULE_GLUE, LEVEL_INFO, "No renderer");
}

void dumpShowTree(WebCore::RenderObject* aRenderObject)
{
    if (aRenderObject) {
        logm(MODULE_GLUE, "SHOW TREE FOR THIS" );
        aRenderObject->showTreeForThis();
    }
    else
        logm(MODULE_GLUE, "No object");
}

void dumpDOM(WebCore::Node* document)
{
    WebCore::DeprecatedString str;
    WebCore::DeprecatedString ind;
    WebCore::TextStream ts(&str);
    if (document)
        document->dump(&ts, ind);
    else
        ts << "No document\n";

    log(str.ascii());
}

void dumpDebugData(BAL::BIEvent* event, FrameBal* f)
{
    BAL::BIKeyboardEvent* aKeyboardEvent = event->queryIsKeyboardEvent();
    if (aKeyboardEvent) {
        switch (aKeyboardEvent->virtualKeyCode()) {
        case BAL::BIKeyboardEvent::VK_T:
            if(aKeyboardEvent->ctrlKey()) {
                String txt = externalRepresentation(f->renderer());
                WebCore::CString utf8Str = txt.utf8();
                const char *utf8 = utf8Str.data();
                if (utf8)
                    printf("%s", utf8);
            }
            break;
        case BAL::BIKeyboardEvent::VK_D:
            if (aKeyboardEvent->ctrlKey())
                dumpDOM(f->document());
            break;
        case BAL::BIKeyboardEvent::VK_S:
            if (aKeyboardEvent->ctrlKey())
                dumpShowTree(f->renderer());
            break;
        case BAL::BIKeyboardEvent::VK_I:
            if (aKeyboardEvent->ctrlKey() && f->document())
                dumpDOM(f->document()->hoverNode());
            break;
        }
    }
}
#endif

void BCWebView::handleEvent(BAL::BIEvent *event)
{
    bool isHandled = false;
    EventHandler* handler = m_mainFrame->eventHandler();
    BAL::BIKeyboardEvent* key = 0;
    BAL::BIMouseEvent* mouseEvent = 0;
    BAL::BIWheelEvent* wheelEvent = 0;

    if ((key = event->queryIsKeyboardEvent()))
        isHandled = handler->keyEvent(*key);
    else if ((mouseEvent = (event->queryIsMouseEvent()))) {
        switch (mouseEvent->eventType()) {
        case BAL::BIMouseEvent::MouseEventPressed:
            isHandled = handler->handleMousePressEvent(*mouseEvent);
            break;
        case BAL::BIMouseEvent::MouseEventReleased:
            isHandled = handler->handleMouseReleaseEvent(*mouseEvent);
            break;
        case BAL::BIMouseEvent::MouseEventMoved:
            isHandled =  handler->handleMouseMoveEvent(*mouseEvent);
            break;
        case BAL::BIMouseEvent::MouseEventScroll:
            break;
        }
    }
    else if ((wheelEvent = event->queryIsWheelEvent()))
        isHandled = handler->handleWheelEvent(*wheelEvent);

    if (isHandled)
        return;

    WebCore::RenderLayer* renderLayer = 0;
    if (m_mainFrame->renderer())
        renderLayer = m_mainFrame->renderer()->layer();

    if (key && !key->isKeyUp() && renderLayer) {
#ifndef NDEBUG
        dumpDebugData(key, m_mainFrame);
#endif
    switch (key->virtualKeyCode()) {
        case BAL::BIKeyboardEvent::VK_LEFT:
            if (key->altKey())
                m_mainFrame->loader()->goBackOrForward(-1);
            else
                renderLayer->scroll(WebCore::ScrollLeft, WebCore::ScrollByLine);
            break;
        case BAL::BIKeyboardEvent::VK_RIGHT:
            if (key->altKey())
                m_mainFrame->loader()->goBackOrForward(1);
            else
                renderLayer->scroll(WebCore::ScrollRight, WebCore::ScrollByLine);
            break;
        case BAL::BIKeyboardEvent::VK_UP:
            renderLayer->scroll(WebCore::ScrollUp, WebCore::ScrollByLine);
            break;
        case BAL::BIKeyboardEvent::VK_PRIOR:
            renderLayer->scroll(WebCore::ScrollUp, WebCore::ScrollByPage);
            break;
        case BAL::BIKeyboardEvent::VK_NEXT:
            renderLayer->scroll(WebCore::ScrollDown, WebCore::ScrollByPage);
            break;
        case BAL::BIKeyboardEvent::VK_DOWN:
            renderLayer->scroll(WebCore::ScrollDown, WebCore::ScrollByLine);
            break;
        case BAL::BIKeyboardEvent::VK_HOME:
            renderLayer->scroll(WebCore::ScrollUp, WebCore::ScrollByDocument);
            break;
        case BAL::BIKeyboardEvent::VK_END:
            renderLayer->scroll(WebCore::ScrollDown, WebCore::ScrollByDocument);
            break;
        case BAL::BIKeyboardEvent::VK_SPACE:
            if (key->shiftKey())
                renderLayer->scroll(WebCore::ScrollUp, WebCore::ScrollByPage);
            else
                renderLayer->scroll(WebCore::ScrollDown, WebCore::ScrollByPage);
            break;
        case BAL::BIKeyboardEvent::VK_TAB: // On tab, change focus
        {
            WebCore::KeyboardEvent* keyEvent = new WebCore::KeyboardEvent(*key, m_mainFrame->document()->defaultView());
            m_mainFrame->page()->focusController()->advanceFocus(keyEvent);
            delete keyEvent;
        }
            break;
        case BAL::BIKeyboardEvent::VK_RETURN: // On enter, dispatch event to the focused node
            break;
        case BAL::BIKeyboardEvent::VK_BACK:
            logm(MODULE_GLUE, "BAL::BIKeyboardEvent::VK_BACK pushed, going backward");
            m_mainFrame->loader()->goBackOrForward(-1);
            break;
        default:
            break;
        } // end switch
    } // end if key

    if (wheelEvent && renderLayer) {
        ScrollDirection direction = WebCore::ScrollDown;
        if (wheelEvent->deltaX())
            direction = wheelEvent->deltaX() < 0 ? WebCore::ScrollRight : WebCore::ScrollLeft;
        if (wheelEvent->deltaY())
            direction = wheelEvent->deltaY() < 0 ? WebCore::ScrollDown : WebCore::ScrollUp;
        renderLayer->scroll(direction, WebCore::ScrollByWheel);
    }

    BAL::BIWindowEvent* windowEvent = event->queryIsWindowEvent();
    if (windowEvent) {
        if (windowEvent->type() == BAL::BIWindowEvent::EXPOSE ||
           windowEvent->type() == BAL::BIWindowEvent::ACTIVE && windowEvent->gain() ) {
            if (m_mainFrame->view()->drawable()) {
                m_mainFrame->view()->drawable()->expose(windowEvent->getRectangle());
            }
        } else if (windowEvent->type() == BAL::BIWindowEvent::REDRAW) {
            if (m_mainFrame->view()->drawable())
                m_mainFrame->view()->drawable()->redraw(windowEvent->getRectangle());
        }
    }
}

void BCWebView::setURL(const KURL& url)
{
    if (url.protocol().isEmpty() && !url.isLocalFile()) {
        KURL urlWithProtocol(url.url().prepend("http://"));
        m_url = urlWithProtocol;
        m_mainFrame->loader()->load(urlWithProtocol);
        return;
    }
    m_url = url;
    m_mainFrame->loader()->load(url);
}

const KURL& BCWebView::getURL()
{
    return m_url;
}

void BCWebView::setFrameLoaderClient(WebCore::FrameLoaderClientBal* client)
{
    m_frameLoaderClient = client;
    Page* currentPage = m_mainFrame->page();
    BIWindow* window = m_mainFrame->view()->drawable();
    delete m_mainFrame;

    // construct a new frame
    WebCore::ChromeClientBal* chromeClient = new WebCore::ChromeClientBal();
    WebCore::ContextMenuClientBal* contextMenuClient = new WebCore::ContextMenuClientBal();
    WebCore::EditorClientBal* editorClient = new WebCore::EditorClientBal();
    Page* page = new Page(chromeClient, contextMenuClient, editorClient,
                    new WebCore::DragClientBal());
    editorClient->setPage(page);
    FrameLoaderClientBal* frameLoaderClient = new FrameLoaderClientBal();
    m_mainFrame = new FrameBal(window, page, 0, m_frameLoaderClient);
    frameLoaderClient->setFrame(m_mainFrame);
    chromeClient->setFrame(m_mainFrame);

}


