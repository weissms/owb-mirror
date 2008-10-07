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

#include "config.h"
#include "PlatformMouseEvent.h"

#include "SystemTime.h"
#include "Assertions.h"
#include "SDL.h"
#include <intuition/intuition.h>

namespace WKAL {

// FIXME: Would be even better to figure out which modifier is Alt instead of always using GDK_MOD1_MASK.

// Keep this in sync with the other platform event constructors
PlatformMouseEvent::PlatformMouseEvent(BalEventButton *event)
{
    //printf("PlatformMouseEvent eventbutton x=%d y=%d\n", (int)event->x, (int)event->y);
    m_timestamp = WebCore::currentTime();
    m_position = IntPoint((int)event->MouseX, (int)event->MouseY);
    m_globalPosition = IntPoint((int)event->MouseX, (int)event->MouseY);
    m_shiftKey = event->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
    m_ctrlKey = event->Qualifier & IEQUALIFIER_CONTROL;
    m_altKey = event->Qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT);
    m_metaKey = event->Qualifier & (IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND);

    m_eventType = MouseEventMoved;
    m_button = NoButton;
    m_clickCount = 0;

    if (IDCMP_MOUSEBUTTONS == event->Class) {
        if ((event->Code & ~IECODE_UP_PREFIX) == IECODE_LBUTTON)
           m_button = LeftButton;
        else if ((event->Code & ~IECODE_UP_PREFIX) == IECODE_MBUTTON)
            m_button = MiddleButton;
        else if ((event->Code & ~IECODE_UP_PREFIX) == IECODE_RBUTTON)
            m_button = RightButton;

        if (NoButton != m_button) {
            if (event->Code & IECODE_UP_PREFIX) {
                m_eventType = MouseEventReleased;
                m_clickCount = 0;
            } else {
                m_eventType = MouseEventPressed;
                m_clickCount = 1;
            }
        }
    }
    else { // IDCMP_MOUSEMOVE == event->Class
        if (event->Qualifier & IEQUALIFIER_LEFTBUTTON)
            m_button = LeftButton;
        else if (event->Qualifier & IEQUALIFIER_MIDBUTTON)
            m_button = MiddleButton;
        else if (event->Qualifier & IEQUALIFIER_RBUTTON)
            m_button = RightButton;
    }
}

}
