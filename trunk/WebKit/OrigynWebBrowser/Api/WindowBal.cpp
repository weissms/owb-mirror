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


#include "config.h"
#include "BALConfiguration.h"
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
#include "FrameBal.h"
#include "FrameLoaderClientBal.h"
#include "FrameLoader.h"
#include "FrameLoadRequest.h"
#include "FrameView.h"
#include "Page.h"
#include "RenderTreeAsText.h"
#include "TextStream.h"
#include "WindowBal.h"

using WebCore::EventHandler;
using WebCore::Frame;
using WebCore::FrameBal;
using WebCore::FrameLoadRequest;
using WebCore::FrameLoaderClientBal;
using WebCore::Page;

namespace BAL {

WindowBal::WindowBal(
const uint16_t x, 
const uint16_t y, 
const uint16_t width, 
const uint16_t height) 
    : m_frameLoaderClient(0)
    , m_x(x)
    , m_y(y)
    , m_width(width)
    , m_height(height)
{
    WebCore::ChromeClientBal* chromeClient = new WebCore::ChromeClientBal();
    WebCore::ContextMenuClientBal* contextMenuClient = new WebCore::ContextMenuClientBal();
    WebCore::EditorClientBal* editorClient = new WebCore::EditorClientBal();
    Page* page = new Page(chromeClient, contextMenuClient, editorClient,
                    new WebCore::DragClientBal());
    editorClient->setPage(page);
    FrameLoaderClientBal* frameLoaderClient = new FrameLoaderClientBal();
    m_mainFrame = new FrameBal(page, 0, frameLoaderClient);
    frameLoaderClient->setFrame(m_mainFrame.get());
    chromeClient->setFrame(m_mainFrame.get());
    page->chrome()->setWindowRect(FloatRect(x, y, width, height));
    m_mainFrame->view()->resize(width, height);
    m_mainFrame->view()->move(x, y);
}

WindowBal::~WindowBal()
{
    delete m_mainFrame->page();
    m_mainFrame = 0;
    // how do we release WebCore::allPages ?
}

void WindowBal::handleEvent(BAL::BIEvent *event)
{
    return;
    // Window events are handled in WindowManager
}

bool WindowBal::canGoBackOrForward(int distance)
{
	return m_mainFrame->loader()->canGoBackOrForward(distance);
}

void WindowBal::goBackOrForward(int distance)
{
	m_mainFrame->loader()->goBackOrForward(distance);
}

void WindowBal::setURL(const KURL& url)
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

const KURL& WindowBal::URL()
{
    return m_url;
}

const BTWidget* WindowBal::widget() const
{
    return m_mainFrame->view();
}

void WindowBal::setFrameLoaderClient(WebCore::FrameLoaderClientBal* client)
{
    m_frameLoaderClient = client;
    Page* page = m_mainFrame->page();
    page->setMainFrame(0);
    static_cast<WebCore::ChromeClientBal*>(page->chrome()->client())->setFrame(0);
    m_mainFrame = new FrameBal(page, 0, m_frameLoaderClient);
    static_cast<WebCore::ChromeClientBal*>(page->chrome()->client())->setFrame(m_mainFrame.get());
    m_frameLoaderClient->setFrame(m_mainFrame.get());
}

}
