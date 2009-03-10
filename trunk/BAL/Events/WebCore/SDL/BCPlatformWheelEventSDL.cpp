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
#include "PlatformWheelEvent.h"
#include "Scrollbar.h"
#include "SDL.h"

namespace WebCore {

// Keep this in sync with the other platform event constructors
PlatformWheelEvent::PlatformWheelEvent(BalEventScroll* event)
{
    static const float delta = 1;

    m_deltaX = 0;
    m_deltaY = 0;

    SDLMod mod = SDL_GetModState();
    m_shiftKey = mod & KMOD_SHIFT;
    m_ctrlKey = mod & KMOD_CTRL;
    m_altKey = mod & KMOD_ALT;
    m_metaKey = mod & KMOD_META;

    //FIXME
    // Docs say an upwards scroll (away from the user) has a positive delta
    switch (event->button) {
        case SDL_BUTTON_WHEELUP: {
            //if(!m_ctrlKey)
            m_deltaY = delta;
/*            int verticalMultiplier = verticalLineMultiplier();
            // A multiplier of -1 is used to mean that vertical wheel scrolling should be done by page.
            m_granularity = (verticalMultiplier == -1) ? ScrollByPageWheelEvent : ScrollByLineWheelEvent;
            if (m_granularity == ScrollByLineWheelEvent)
                m_deltaY *= verticalMultiplier;*/
            break;
        }
        case SDL_BUTTON_WHEELDOWN: {
            //if(m_ctrlKey)
            m_deltaY = -delta;
            /*int verticalMultiplier = verticalLineMultiplier();
            // A multiplier of -1 is used to mean that vertical wheel scrolling should be done by page.
            m_granularity = (verticalMultiplier == -1) ? ScrollByPageWheelEvent : ScrollByLineWheelEvent;
            if (m_granularity == ScrollByLineWheelEvent)
                m_deltaY *= verticalMultiplier;*/
            break;
    }
        default:
            break;
    }

    
    //printf("m_deltaY = %f m_deltaX=%f delta=%f\n", m_deltaY, m_deltaX, delta);
    // FIXME: retrieve the user setting for the number of lines to scroll on each wheel event
    m_deltaX *= static_cast<float>(cScrollbarPixelsPerLineStep);
    m_deltaY *= static_cast<float>(cScrollbarPixelsPerLineStep);

    m_position = IntPoint((int)event->x, (int)event->y);
    m_globalPosition = IntPoint((int)event->x, (int)event->y);
    m_isAccepted = false;

}

}
