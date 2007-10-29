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
 * @file  BCKeyboardEvent.h
 *
 * @brief Concretisation of keyboard event
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

#include "BIKeyboardEvent.h"
#include "../Common/BCCommonInputEventData.h"

namespace BC
{

/**
 * @brief the KeyboardEvent
 *
 * The keyboard event
 *
 * @see BIEvent, BIEventLoop
 */
class BCKeyboardEvent : public BAL::BIKeyboardEvent, public BCCommonInputEventData {
public:
  BCKeyboardEvent(const WebCore::String& aText,
                    const WebCore::String& aUnmodifiedText,
                    const WebCore::String& aKeyIdentifier,
                    bool bIsKeyUp,
                    bool bShiftKey,
                    bool bCtrlKey,
                    bool bAltKey,
                    bool bMetaKey,
                    bool bIsAutoRepeat,
                    int aVKey);

  virtual WebCore::String text() const;
  virtual WebCore::String unmodifiedText() const;
  virtual WebCore::String keyIdentifier() const;
  virtual bool isKeyUp() const;
  virtual bool isAutoRepeat() const;
  virtual void setIsAutoRepeat(bool);
  virtual int WindowsKeyCode() const;
  virtual int setWindowsKeyCode(int code) const;
  virtual int virtualKeyCode() const;
  virtual bool isKeypad() const;

  virtual BIEvent* clone() const;

  virtual bool shiftKey() const { return BCCommonInputEventData::shiftKey(); }
  virtual bool ctrlKey() const { return BCCommonInputEventData::ctrlKey(); }
  virtual bool altKey() const { return BCCommonInputEventData::altKey(); }
  virtual bool metaKey() const { return BCCommonInputEventData::metaKey(); }

protected:
  WebCore::String m_text;
  WebCore::String m_unmodifiedText;
  WebCore::String m_keyIdentifier;
  bool m_isKeyUp;
  bool m_autoRepeat;
  int m_BALVirtualKeyCode;
  bool m_isKeypad;

};

}
