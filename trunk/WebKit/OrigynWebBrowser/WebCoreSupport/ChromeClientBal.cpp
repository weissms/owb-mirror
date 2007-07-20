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
#include "BIWindow.h"
#include "BIWindowManager.h"
#include "ChromeClientBal.h"

#include "Frame.h"
#include "FrameView.h"
#include "FrameLoadRequest.h"

namespace WebCore
{

ChromeClientBal::ChromeClientBal() 
    : m_frame(0)
    , m_window(0)
{

}

ChromeClientBal::~ChromeClientBal()
{

}

void ChromeClientBal::setWindowRect(const FloatRect& rect)
{
    if (!m_frame)
        return;
    m_frame->view()->resize(static_cast<int>(rect.width()), static_cast<int>(rect.height()));
}

FloatRect ChromeClientBal::windowRect()
{
    if (!m_frame)
        return FloatRect();
    BALNotImplemented();
    return FloatRect();
}

FloatRect ChromeClientBal::pageRect()
{
    if (!m_frame)
        return FloatRect();
    BALNotImplemented();
    return FloatRect();
}

float ChromeClientBal::scaleFactor()
{
    BALNotImplemented();
    return 1;
}

void ChromeClientBal::focus()
{
    if (!m_frame)
        return;
    BALNotImplemented();
}

void ChromeClientBal::unfocus()
{
    if (!m_frame)
        return;
    BALNotImplemented();
}

bool ChromeClientBal::canTakeFocus(FocusDirection)
{
    BALNotImplemented();
    if (!m_frame)
        return false;
    return false;
}

void ChromeClientBal::takeFocus(FocusDirection)
{
    BALNotImplemented();
    if (!m_frame)
        return;
}

Page* ChromeClientBal::createWindow(const FrameLoadRequest& request)
{
    // we must display the ui here
    m_window = BAL::getBIWindowManager()->openWindow(100, 100, 200, 200);
    m_window->setURL(request.resourceRequest().url());
    return (static_cast<const FrameView*>(m_window->widget()))->frame()->page();
}

Page* ChromeClientBal::createModalDialog(const FrameLoadRequest& request)
{
    // we shouldn't display the ui here
    m_window = BAL::getBIWindowManager()->openWindow(100, 100, 200, 200);
    m_window->setURL(request.resourceRequest().url());
    return (static_cast<const FrameView*>(m_window->widget()))->frame()->page();
}

void ChromeClientBal::show()
{
    if (!m_frame)
        return;
    BALNotImplemented();
//    m_frame->topLevelWidget()->show();
}

bool ChromeClientBal::canRunModal()
{
    BALNotImplemented();
    return false;
}

void ChromeClientBal::runModal()
{
    BALNotImplemented();
}

void ChromeClientBal::setToolbarsVisible(bool)
{
    BALNotImplemented();
}

bool ChromeClientBal::toolbarsVisible()
{
    BALNotImplemented();
    return false;
}

void ChromeClientBal::setStatusbarVisible(bool)
{
    BALNotImplemented();
}

bool ChromeClientBal::statusbarVisible()
{
    BALNotImplemented();
    return false;
}

void ChromeClientBal::setScrollbarsVisible(bool)
{
    BALNotImplemented();
}

bool ChromeClientBal::scrollbarsVisible()
{
    BALNotImplemented();
    return true;
}

void ChromeClientBal::setMenubarVisible(bool)
{
    BALNotImplemented();
}

bool ChromeClientBal::menubarVisible()
{
    BALNotImplemented();
    return false;
}

void ChromeClientBal::setResizable(bool)
{
    BALNotImplemented();
}

void ChromeClientBal::addMessageToConsole(const String& message, unsigned int lineNumber,
                                         const String& sourceID)
{
    BALNotImplemented();
    printf("Javascript: line:%d source:%s msg:%s \n",lineNumber,sourceID.deprecatedString().ascii(),message.deprecatedString().ascii());

}

void ChromeClientBal::chromeDestroyed()
{
    BALNotImplemented();
}

bool ChromeClientBal::canRunBeforeUnloadConfirmPanel()
{
    BALNotImplemented();
    return false;
}

bool ChromeClientBal::runBeforeUnloadConfirmPanel(const String& message, Frame* frame)
{
    BALNotImplemented();
    return false;
}

void ChromeClientBal::closeWindowSoon()
{
    // we do not know at this point which window will close: assuming it is activeWindow
    BAL::getBIWindowManager()->closeWindow(BAL::getBIWindowManager()->activeWindow());
}

void ChromeClientBal::runJavaScriptAlert(Frame* frame, const String& msg)
{
    printf("Javascript Alert: %s (from frame %p)\n", msg.deprecatedString().ascii(), frame);   
}

bool ChromeClientBal::runJavaScriptConfirm(Frame* frame, const String& msg)
{
    printf("Javascript Confirm: %s (from frame %p), answer is 'false' by default.\n", msg.deprecatedString().ascii(), frame);   
    return false;
}

bool ChromeClientBal::runJavaScriptPrompt(Frame* frame, const String& msg, const String& defaultValue, String& result)
{
    printf("Javascript Prompt: %s (from frame %p), answer is 'false' by default.\n", msg.deprecatedString().ascii(), frame);   
    return false;
}

void ChromeClientBal::setStatusbarText(const String&)
{
    BALNotImplemented();
}

bool ChromeClientBal::shouldInterruptJavaScript()
{
    BALNotImplemented();
    return false;
}

bool ChromeClientBal::tabsToLinks() const
{
    return true;
}

IntRect ChromeClientBal::windowResizerRect() const
{
    return IntRect();
}

void ChromeClientBal::addToDirtyRegion(const IntRect&)
{
    BALNotImplemented();
}

void ChromeClientBal::scrollBackingStore(int, int, const IntRect&, const IntRect&)
{
}

void ChromeClientBal::updateBackingStore()
{
}

void ChromeClientBal::setFrame(Frame* frame)
{
    m_frame = frame;
}

}


