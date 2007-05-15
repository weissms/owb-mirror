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
 * @file  BCKeyboardEvent.cpp
 *
 * @brief Bal Concretisation of keybord event
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 */

#include "BCKeyboardEvent.h"

using namespace BC;
using namespace BAL;

namespace BAL {

BIKeyboardEvent* createBIKeyboardEvent(const WebCore::String& aText,
    const WebCore::String& aUnmodifiedText, const WebCore::String& aKeyIdentifier,
    bool bIsKeyUp, bool bShiftKey, bool bCtrlKey, bool bAltKey, bool bMetaKey, bool bIsAutoRepeat,
    int aVKey)
{
	return new BCKeyboardEvent(aText, aUnmodifiedText, aKeyIdentifier, bIsKeyUp, bShiftKey, bCtrlKey, bAltKey, bMetaKey, bIsAutoRepeat, aVKey);
}

}

WebCore::String BCKeyboardEvent::text() const { return m_text; }
WebCore::String BCKeyboardEvent::unmodifiedText() const { return m_unmodifiedText; }
WebCore::String BCKeyboardEvent::keyIdentifier() const { return m_keyIdentifier; }
bool BCKeyboardEvent::isKeyUp() const { return m_isKeyUp; }
bool BCKeyboardEvent::isAutoRepeat() const { return m_autoRepeat; }
void BCKeyboardEvent::setIsAutoRepeat(bool b) { m_autoRepeat = b; }
int BCKeyboardEvent::WindowsKeyCode() const { return m_BALVirtualKeyCode; }
int BCKeyboardEvent::virtualKeyCode() const { return m_BALVirtualKeyCode; }
bool BCKeyboardEvent::isKeypad() const { return m_isKeypad; }

BCKeyboardEvent::BCKeyboardEvent(const WebCore::String& aText,
	const WebCore::String& aUnmodifiedText,
	const WebCore::String& aKeyIdentifier,
	bool bIsKeyUp,
	bool bShiftKey,
	bool bCtrlKey,
	bool bAltKey,
	bool bMetaKey,
	bool bIsAutorepeat,
	int aVKey)
	: m_text(aText)
	, m_unmodifiedText(aUnmodifiedText)
	, m_keyIdentifier(aKeyIdentifier)
	, m_isKeyUp(bIsKeyUp)
	, BCCommonInputEventData(bShiftKey, bCtrlKey, bAltKey, bMetaKey)
	, m_autoRepeat(bIsAutorepeat)
	, m_BALVirtualKeyCode(aVKey)
{
  m_isKeypad = false; // FIXME
}

BIEvent* BCKeyboardEvent::clone() const
{
	BCKeyboardEvent* aCloned = new BCKeyboardEvent(
    m_text, m_unmodifiedText, m_keyIdentifier, m_isKeyUp,
    shiftKey(),
    ctrlKey(),
    altKey(),
    metaKey(),
    isAutoRepeat(),
    m_BALVirtualKeyCode);

	return aCloned;
}

