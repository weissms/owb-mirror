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
 * @file  BCWindowEvent.h
 *
 * @brief Concretisation of window event
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 * This header file is private. Only IDL interface is public.
 *
 */

#include "config.h"
#include "PlatformString.h"

#include "BIWindowEvent.h"
#include "IntRect.h"

namespace BC
{

/**
 * @brief the WindowEvent
 *
 * The window event. Events like expose, resize (not handled yet), etc.
 * are managed here.
 *
 * @see BIEvent, BIEventLoop
 */
class BCWindowEvent : public BAL::BIWindowEvent {
public:
    BCWindowEvent(BIWindowEvent::ENUM_WINDOW, bool bGain = false,
                   const WebCore::IntRect& rect = WebCore::IntRect());

    virtual BIEvent* clone() const;
    virtual const WebCore::IntRect& getRectangle() const;
    virtual BIWindowEvent::ENUM_WINDOW type() const;
    virtual bool gain() const;

protected:
    BIWindowEvent::ENUM_WINDOW    m_aWindowType;
    bool                          m_bGain; // 0 or 1 if active
    WebCore::IntRect              m_rect;
};

}
