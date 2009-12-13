/*
 * Copyright (C) 2009 Joerg Strohmayer
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
 *     documentation and/or other materials http://gaara-fr.com/index2.phpprovided with the distribution.
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
#include "PlatformKeyboardEvent.h"

#include "KeyboardCodes.h"
#include "BALBase.h"
#include "TextEncoding.h"
#include "Unicode.h"
#include "CString.h"
#include <libraries/keymap.h>
#include <proto/input.h>

extern uint32 amigaToUnicodeChar(uint32 c);

namespace WebCore {

static int ConvertAmigaKeyToVirtualKey(struct IntuiMessage *im)
{
    if (IDCMP_RAWKEY == im->Class) {
        switch (im->Code & ~IECODE_UP_PREFIX) {
        case RAWKEY_TAB:       return VK_TAB;     break;
        case RAWKEY_INSERT:    return VK_INSERT;  break;
        case RAWKEY_PAGEUP:    return VK_PRIOR;   break;
        case RAWKEY_PAGEDOWN:  return VK_NEXT;    break;
        case RAWKEY_F11:       return VK_F11;     break;
        case RAWKEY_CRSRUP:    return VK_UP;      break;
        case RAWKEY_CRSRDOWN:  return VK_DOWN;    break;
        case RAWKEY_CRSRRIGHT: return VK_RIGHT;   break;
        case RAWKEY_CRSRLEFT:  return VK_LEFT;    break;
        case RAWKEY_F1:        return VK_F1;      break;
        case RAWKEY_F2:        return VK_F2;      break;
        case RAWKEY_F3:        return VK_F3;      break;
        case RAWKEY_F4:        return VK_F4;      break;
        case RAWKEY_F5:        return VK_F5;      break;
        case RAWKEY_F6:        return VK_F6;      break;
        case RAWKEY_F7:        return VK_F7;      break;
        case RAWKEY_F8:        return VK_F8;      break;
        case RAWKEY_F9:        return VK_F9;      break;
        case RAWKEY_F10:       return VK_F10;     break;
        case RAWKEY_HELP:      return VK_HELP;    break;
        case RAWKEY_LSHIFT:    return VK_SHIFT;   break;
        case RAWKEY_RSHIFT:    return VK_SHIFT;   break;
        case RAWKEY_CAPSLOCK:  return VK_CAPITAL; break;
        case RAWKEY_LCTRL:     return VK_CONTROL; break;
        case RAWKEY_LALT:      return VK_MENU;    break;
        case RAWKEY_RALT:      return VK_MENU;    break;
        case RAWKEY_MENU:      return VK_MENU;    break;
        case RAWKEY_LCOMMAND:  return VK_LWIN;    break;
        case RAWKEY_RCOMMAND:  return VK_RWIN;    break;
        case RAWKEY_PRINTSCR:  return VK_PRINT;   break;
        case RAWKEY_BREAK:     return VK_PAUSE;   break;
        case RAWKEY_F12:       return VK_F12;     break;
        case RAWKEY_HOME:      return VK_HOME;    break;
        case RAWKEY_END:       return VK_END;     break;
        case 0x79:             return VK_NUMLOCK; break;

        default:
            if ((im->Code & ~IECODE_UP_PREFIX) >= RAWKEY_SPACE)
                fprintf(stderr, "%s class IDCMP_RAWKEY code 0x%04x not handled\n", __PRETTY_FUNCTION__, im->Code);
            return 0;
        break;
        }
    } else {
        switch (im->Code) {
        case   8: return VK_BACK;   break;
        case   9: return VK_TAB;    break;
        case  13: return VK_RETURN; break;
        case  27: return VK_ESCAPE; break;
        case  32: return VK_SPACE;  break;
        case 127: return VK_DELETE; break;
        case   1: return VK_A;      break;
        case   3: return VK_C;      break;
        case  22: return VK_V;      break;
        case  24: return VK_X;      break;

        case '0' ... '9':
            return VK_0 + im->Code - '0';
        break;

        case 'a' ... 'z':
            return VK_A + im->Code - 'a';
        break;

        case 'A' ... 'Z':
            return VK_A + im->Code - 'A';
        break;

        default:
        {
            bool bMeta = im->Qualifier & (IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND);
            if (bMeta && 'a' == im->Code)
                return VK_A;
            else if (bMeta && 'c' == im->Code)
                return VK_C;
            else if (bMeta && 'v' == im->Code)
                return VK_V;
            else if (bMeta && 'x' == im->Code)
                return VK_X;
            else {
                if (im->Code < 32
                || (im->Code > 127 && im->Code < 160))
                    fprintf(stderr, "%s VANILLAKEY %d, qualifier 0x%04x not handled\n", __PRETTY_FUNCTION__, im->Code, im->Qualifier);
                return 0;
            }
        }
        break;
        }
    }
}

static WebCore::String keyIdentifierForAmigaKeyCode(struct IntuiMessage *im)
{
    if (IDCMP_RAWKEY == im->Class) {
        switch (im->Code & ~IECODE_UP_PREFIX) {
        case RAWKEY_TAB:       return "U+0009";   break;
        case RAWKEY_INSERT:    return "Insert";   break;
        case RAWKEY_PAGEUP:    return "PageUp";   break;
        case RAWKEY_PAGEDOWN:  return "PageDown"; break;
        case RAWKEY_F11:       return "F11";      break;
        case RAWKEY_CRSRUP:    return "Up";       break;
        case RAWKEY_CRSRDOWN:  return "Down";     break;
        case RAWKEY_CRSRRIGHT: return "Right";    break;
        case RAWKEY_CRSRLEFT:  return "Left";     break;
        case RAWKEY_F1:        return "F1";       break;
        case RAWKEY_F2:        return "F2";       break;
        case RAWKEY_F3:        return "F3";       break;
        case RAWKEY_F4:        return "F4";       break;
        case RAWKEY_F5:        return "F5";       break;
        case RAWKEY_F6:        return "F6";       break;
        case RAWKEY_F7:        return "F7";       break;
        case RAWKEY_F8:        return "F8";       break;
        case RAWKEY_F9:        return "F9";       break;
        case RAWKEY_F10:       return "F10";      break;
        case RAWKEY_HELP:      return "Help";     break;
        case RAWKEY_LALT:      return "Alt";      break;
        case RAWKEY_RALT:      return "Alt";      break;
        case RAWKEY_MENU:      return "Alt";      break;
        case RAWKEY_BREAK:     return "Pause";    break;
        case RAWKEY_F12:       return "F12";      break;
        case RAWKEY_HOME:      return "Home";     break;
        case RAWKEY_END:       return "End";      break;
        default:               return "U+0000";   break;
        }
    } else {
        switch (im->Code) {
            case 13: return "Enter"; break;

            default:
            {
                bool bMeta = im->Qualifier & (IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND);
                if (bMeta && 'a' == im->Code)
                   return "U+0001";
                else if (bMeta && 'c' == im->Code)
                   return "U+0003";
                else if (bMeta && 'v' == im->Code)
                   return "U+0016";
                else if (bMeta && 'x' == im->Code)
                   return "U+0018";
                else
                   return WebCore::String::format("U+%04X", WTF::Unicode::toUpper(amigaToUnicodeChar(im->Code)));
            }
            break;
        }
    }
}

PlatformKeyboardEvent::PlatformKeyboardEvent(BalEventKey* event)
    : m_type(KeyDown)
    , m_autoRepeat(event->Qualifier & IEQUALIFIER_REPEAT)
    , m_isKeypad(event->Qualifier & IEQUALIFIER_NUMERICPAD)
    , m_shiftKey(event->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
    , m_ctrlKey(event->Qualifier & IEQUALIFIER_CONTROL)
    , m_altKey(event->Qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT))
    , m_metaKey(event->Qualifier & (IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND))
    , m_balEventKey(*event)
{
    UChar aSrc[2] = { 0, 0 };
    if (IDCMP_RAWKEY == event->Class) {
        if (event->Code & IECODE_UP_PREFIX)
            m_type = KeyUp;
    }
    else
        if (event->Code & 0x100) {
            // fake KeyUp event with impossible code
            event->Code &= ~0x100;
            m_type = KeyUp;
        }
        else
            aSrc[0] = amigaToUnicodeChar(event->Code);

    m_windowsVirtualKeyCode = ConvertAmigaKeyToVirtualKey(event);

    WebCore::String aText(aSrc);
    WebCore::String aUnmodifiedText(aSrc);
    WebCore::String aKeyIdentifier = keyIdentifierForAmigaKeyCode(event);

    m_text = aText;
    m_unmodifiedText = aUnmodifiedText;
    m_keyIdentifier = aKeyIdentifier;
}

void PlatformKeyboardEvent::disambiguateKeyDownEvent(Type type, bool backwardCompatibilityMode)
{
    // Can only change type from KeyDown to RawKeyDown or Char, as we lack information for other conversions.
    ASSERT(m_type == KeyDown);
    m_type = type;

    if (backwardCompatibilityMode)
        return;

    if (type == RawKeyDown) {
        m_text = String();
        m_unmodifiedText = String();
    } else {
        m_keyIdentifier = String();
        m_windowsVirtualKeyCode = 0;
    }
}

bool PlatformKeyboardEvent::currentCapsLockState()
{
    return IInput->PeekQualifier() & IEQUALIFIER_CAPSLOCK;
}

BalEventKey* PlatformKeyboardEvent::balEventKey() const
{
    return (BalEventKey*)&m_balEventKey;
}

}
