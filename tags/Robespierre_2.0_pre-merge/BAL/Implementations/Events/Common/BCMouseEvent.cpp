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
 * @file  BCMouseEvent.cpp
 *
 * @brief Bal Concretisation of mouse event
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 */

#include "BALConfiguration.h"
#include "BCMouseEvent.h"

using namespace BC;

namespace BAL {

BIMouseEvent* createBIMouseEvent()
{
    return new BCMouseEvent(BIMouseEvent::MouseEventMoved, IntPoint(), IntPoint(), BIMouseEvent::NoButton, 0, false, false, false, false);
}

}

const IntPoint& BCMouseEvent::pos() const
{
    return m_position;
}
void BCMouseEvent::shiftPos(int dx, int dy)
{
    m_position = m_position + WebCore::IntSize(dx, dy);
}

const IntPoint& BCMouseEvent::globalPos() const
{
	return m_globalPosition;
}

BAL::BIMouseEvent::MouseButton BCMouseEvent::button() const
{
	return m_button;
}

int BCMouseEvent::clickCount() const
{
	return m_clickCount;
}

BCMouseEvent::BCMouseEvent(MouseEventType type, const IntPoint& pos, const IntPoint& globalPos, MouseButton button,
                int clickCount, bool shift, bool ctrl, bool alt, bool meta)
    : BCCommonInputEventData( shift, ctrl, alt, meta )
	, m_position(pos)
	, m_globalPosition(globalPos)
	, m_button(button)
	, m_eventType(type)
	, m_clickCount(clickCount)
{
}

BAL::BIEvent* BCMouseEvent::clone() const
{
	return new BCMouseEvent(m_eventType, m_position, m_globalPosition, m_button, m_clickCount,
    shiftKey(), ctrlKey(), altKey(), metaKey());
}
