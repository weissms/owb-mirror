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
#include "PlatformMouseEvent.h"

#include "SystemTime.h"
#include "Assertions.h"
#include "SDL.h"

namespace WKAL {

// FIXME: Would be even better to figure out which modifier is Alt instead of always using GDK_MOD1_MASK.

// Keep this in sync with the other platform event constructors
PlatformMouseEvent::PlatformMouseEvent(BalEventButton* event)
{
    //printf("PlatformMouseEvent eventbutton x=%d y=%d\n", (int)event->x, (int)event->y);
    m_timestamp = WebCore::currentTime();
    m_position = IntPoint((int)event->x, (int)event->y);
    m_globalPosition = IntPoint((int)event->x, (int)event->y);
    SDLMod mod = SDL_GetModState();
    m_shiftKey = mod & KMOD_SHIFT;
    m_ctrlKey = mod & KMOD_CTRL;
    m_altKey = mod & KMOD_ALT;
    m_metaKey = mod & KMOD_META;

    m_eventType = MouseEventMoved;
    m_button = NoButton;

    if( event->button == SDL_BUTTON_LEFT )
        m_button = LeftButton;
    else if( event->button == SDL_BUTTON_MIDDLE )
        m_button = MiddleButton;
    else if( event->button == SDL_BUTTON_RIGHT )
        m_button = RightButton;

    if(event->state == SDL_RELEASED) {
        m_eventType = MouseEventReleased;
        m_clickCount = 0;
    } else {
        m_eventType = MouseEventPressed;
        m_clickCount = 1;
    }
}

PlatformMouseEvent::PlatformMouseEvent(BalEventMotion* motion)
{
    m_timestamp = WebCore::currentTime();;
    m_position = IntPoint((int)motion->x, (int)motion->y);
    m_globalPosition = IntPoint((int)motion->xrel, (int)motion->yrel);
    SDLMod mod = SDL_GetModState();
    m_shiftKey = mod & KMOD_SHIFT;
    m_ctrlKey = mod & KMOD_CTRL;
    m_altKey = mod & KMOD_ALT;
    m_metaKey = mod & KMOD_META;

    m_eventType = MouseEventMoved;
    m_button = NoButton;
    m_clickCount = 0;
}

}
