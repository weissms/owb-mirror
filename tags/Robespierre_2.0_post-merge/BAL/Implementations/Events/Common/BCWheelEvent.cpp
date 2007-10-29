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
 * @file  BCWheelEvent.cpp
 *
 * @brief Bal Concretisation of wheel event
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 */

#include "BCWheelEvent.h"

using namespace BC;

const IntPoint& BCWheelEvent::pos() const
{
	return m_position;
}

const IntPoint& BCWheelEvent::globalPos() const
{
	return m_globalPosition;
}

int BCWheelEvent::deltaX() const
{
	return m_deltaX;
}

int BCWheelEvent::deltaY() const
{
	return m_deltaY;
}

bool BCWheelEvent::isAccepted() const
{
	return m_isAccepted;
}

void BCWheelEvent::accept()
{
	m_isAccepted = true;
}

void BCWheelEvent::ignore()
{
	m_isAccepted = false;
}

bool BCWheelEvent::isContinuous() const
{
	return true;
}

float BCWheelEvent::continuousDeltaX() const
{
	return 1.0;
}

float BCWheelEvent::continuousDeltaY() const
{
	return 1.0;
}

BCWheelEvent::BCWheelEvent(const IntPoint& pos, const IntPoint& globalPos,
                           int deltaX, int deltaY, bool isAccepted,
                           bool shift, bool ctrl, bool alt, bool meta)
	: BCCommonInputEventData( shift, ctrl, alt, meta )
	, m_position(pos)
	, m_globalPosition(globalPos)
	, m_deltaX(deltaX)
	, m_deltaY(deltaY)
	, m_isAccepted(isAccepted)
{
}

BAL::BIEvent* BCWheelEvent::clone() const
{
    return new BCWheelEvent(m_position, m_globalPosition, m_deltaX, m_deltaY, m_isAccepted,
                             shiftKey(), ctrlKey(), altKey(), metaKey());
}

