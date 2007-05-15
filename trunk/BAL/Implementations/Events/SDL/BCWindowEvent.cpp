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
 * @file  BCWindowEvent.cpp
 *
 * @brief Bal Concretisation of window event
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 */

#include "BCWindowEvent.h"

using namespace BC;

namespace BAL {
    BIWindowEvent* createBIWindowEvent(BIWindowEvent::ENUM_WINDOW aW, bool bGain = false, const WebCore::IntRect& rect = WebCore::IntRect())
    {
        return new BC::BCWindowEvent(aW, bGain, rect);
    }
}

BAL::BIWindowEvent::ENUM_WINDOW BCWindowEvent::type() const
{
	return m_aWindowType;
}

bool BCWindowEvent::gain() const
{
	return m_bGain;
}

BCWindowEvent::BCWindowEvent(BIWindowEvent::ENUM_WINDOW aW, bool bGain, const WebCore::IntRect& rect)
    : m_aWindowType( aW )
	, m_bGain( bGain )
	, m_rect(rect)
{
}

BAL::BIEvent* BCWindowEvent::clone() const
{
	return new BCWindowEvent(m_aWindowType, m_bGain);
}

const WebCore::IntRect& BCWindowEvent::getRectangle() const
{
    return m_rect;
}
