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
 * @file  BCWindowManager.h
 *
 * Header file for BCWindowManager.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BCWINDOWMANAGER_H
#define BCWINDOWMANAGER_H

#include <stdint.h>
#include "BIWindowManager.h"
#include "Timer.h"

namespace WebCore {
    class FrameView;
    class Frame;
}

namespace BAL {

    class BIEventLoop;
    class WindowBal;
/**
 * The WindowManager implementation
 */
class BCWindowManager : public BIWindowManager {
public:
    BCWindowManager();
    ~BCWindowManager();
    virtual BAL::BIWindow* openWindow(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h);
    virtual void closeWindow(BIWindow*);
    virtual BIWindow* activeWindow();
    virtual void handleEvent(BIEvent*);

    void timerCallback(WebCore::Timer<BCWindowManager>* timer);

private:
    void setActiveWindow(BIWindow*);
    void expose(BIWindow*);
    void redrawWindow(BIWindow*);
    void redrawView(WebCore::FrameView*, bool repaint);
    void shiftEventPos(BIEvent* event, WebCore::Frame* f);

    BIEventLoop*                    m_eventLoop;
    BIWindow*                       m_window;
    BIGraphicsContext*              m_gc;
    WebCore::Timer<BCWindowManager> m_timer;
    Vector<BIWindow*>               m_windowList;
};

}

#endif // BCWINDOWMANAGER_H


