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
#include "PlatformWheelEvent.h"
#include "SDL.h"
#include <intuition/intuition.h>

namespace WKAL {

// Keep this in sync with the other platform event constructors
PlatformWheelEvent::PlatformWheelEvent(BalEventScroll* event)
{
    m_deltaX = 0;
    m_deltaY = 0;

    struct IntuiWheelData *wd = (struct IntuiWheelData *)event->IAddress;

    bool bShift = event->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
    bool bCtrl  = event->Qualifier & IEQUALIFIER_CONTROL;
    bool bAlt   = event->Qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT);

    if (IMSGCODE_INTUIWHEELDATA != event->Code)
        fprintf(stderr, "%s class IDCMP_EXTENDEDMOUSE code 0x%04x not handled\n", __PRETTY_FUNCTION__, event->Code);
    else if (!wd)
        fprintf(stderr, "%s class IDCMP_EXTENDEDMOUSE NULL IntuiWheelData\n", __PRETTY_FUNCTION__);
    else if (INTUIWHEELDATA_VERSION != wd->Version)
        fprintf(stderr, "%s class IDCMP_EXTENDEDMOUSE unknow IntuiWheelData version %u\n", __PRETTY_FUNCTION__, wd->Version);
    else if (0 == wd->WheelX && 0 == wd->WheelY)
        fprintf(stderr, "%s class IDCMP_EXTENDEDMOUSE code WheelY = 0, WheelX = 0\n", __PRETTY_FUNCTION__);
    else {
        double deltax, deltay;

        if (bCtrl) {
            deltax = wd->WheelX * -10000;
            deltay = wd->WheelY * -10000;
        } else if (bShift) {
            deltax = (int)wd->WheelX * event->IDCMPWindow->Width / -50;
            deltay = (int)wd->WheelY * event->IDCMPWindow->Height / -50;
        } else {
            deltax = wd->WheelX * -1;
            deltay = wd->WheelY * -1;
        }

        if (bAlt) {
            double temp;
            temp   = deltax;
            deltax = deltay;
            deltay = temp;
        }

        m_deltaX = deltax;
        m_deltaY = deltay;
    }

    m_position = IntPoint((int)event->MouseX /*- g_amiga_window->BorderLeft*/, (int)event->MouseY /*- g_amiga_window->BorderTop - g_amiga_toolbar_limits.MinHeight*/);
    m_globalPosition = IntPoint((int)event->MouseX /*- g_amiga_window->BorderLeft*/, (int)event->MouseY /*- g_amiga_window->BorderTop - g_amiga_toolbar_limits.MinHeight*/);
    m_isAccepted = false;
    m_isContinuous = false;
}

}
