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

#include "NotImplemented.h"
#include "WindowsKeyboardCodes.h"
#include "BALBase.h"
#include "Logging.h"
#include "TextEncoding.h"
#include "SDL.h"
#include "Unicode.h"
#include "CString.h"

namespace WebCore {

// FIXME: This is incomplete.  We should change this to mirror
// more like what Firefox does, and generate these switch statements
// at build time.
static WebCore::String keyIdentifierForSDLKeyCode(SDLKey keyCode)
{
    switch (keyCode) {
        case SDLK_MENU:
        case SDLK_LALT:
        case SDLK_RALT:
            return "Alt";
        case SDLK_CLEAR:
            return "Clear";
        case SDLK_DOWN:
            return "Down";
            // "End"
        case SDLK_END:
            return "End";
            // "Enter"
        //case SDLK_ISO_Enter:
        case SDLK_RETURN:
            return "Enter";
        case SDLK_KP_ENTER:
            return "Enter";
        //case SDLK_EXECUTE:
        //    return "Execute";
        case SDLK_F1:
            return "F1";
        case SDLK_F2:
            return "F2";
        case SDLK_F3:
            return "F3";
        case SDLK_F4:
            return "F4";
        case SDLK_F5:
            return "F5";
        case SDLK_F6:
            return "F6";
        case SDLK_F7:
            return "F7";
        case SDLK_F8:
            return "F8";
        case SDLK_F9:
            return "F9";
        case SDLK_F10:
            return "F10";
        case SDLK_F11:
            return "F11";
        case SDLK_F12:
            return "F12";
        case SDLK_F13:
            return "F13";
        case SDLK_F14:
            return "F14";
        case SDLK_F15:
            return "F15";
/*        case SDLK_F16:
            return "F16";
        case SDLK_F17:
            return "F17";
        case SDLK_F18:
            return "F18";
        case SDLK_F19:
            return "F19";
        case SDLK_F20:
            return "F20";
        case SDLK_F21:
            return "F21";
        case SDLK_F22:
            return "F22";
        case SDLK_F23:
            return "F23";
        case SDLK_F24:
            return "F24";*/
        case SDLK_HELP:
            return "Help";
        case SDLK_HOME:
            return "Home";
        case SDLK_INSERT:
            return "Insert";
        case SDLK_LEFT:
            return "Left";
        case SDLK_PAGEDOWN:
            return "PageDown";
        case SDLK_PAGEUP:
            return "PageUp";
        case SDLK_PAUSE:
            return "Pause";
        //case SDLK_3270_PRINTSCREEN:
        //    return "PrintScreen";
        case SDLK_RIGHT:
            return "Right";
            //case SDLK_Scroll:
            //   return "Scroll";
        //case SDLK_SELECT:
        //    return "Select";
        case SDLK_UP:
            return "Up";
            // Standard says that DEL becomes U+00007F.
        case SDLK_DELETE:
            return "U+00007F";
        default:
            return WebCore::String::format("U+%04X", WTF::Unicode::toUpper(keyCode));
    }
}

static int ConvertSDLKeyToVirtualKey(SDLKey keycode, SDLMod mod)
{
    switch (keycode) {
        case SDLK_KP0:
            return VK_NUMPAD0;// (60) Numeric keypad 0 key
        case SDLK_KP1:
            return VK_NUMPAD1;// (61) Numeric keypad 1 key
        case SDLK_KP2:
            return  VK_NUMPAD2; // (62) Numeric keypad 2 key
        case SDLK_KP3:
            return VK_NUMPAD3; // (63) Numeric keypad 3 key
        case SDLK_KP4:
            return VK_NUMPAD4; // (64) Numeric keypad 4 key
        case SDLK_KP5:
            return VK_NUMPAD5; //(65) Numeric keypad 5 key
        case SDLK_KP6:
            return VK_NUMPAD6; // (66) Numeric keypad 6 key
        case SDLK_KP7:
            return VK_NUMPAD7; // (67) Numeric keypad 7 key
        case SDLK_KP8:
            return VK_NUMPAD8; // (68) Numeric keypad 8 key
        case SDLK_KP9:
            return VK_NUMPAD9; // (69) Numeric keypad 9 key
        case SDLK_KP_MULTIPLY:
            return VK_MULTIPLY; // (6A) Multiply key
        case SDLK_KP_PLUS:
            return VK_ADD; // (6B) Add key
        case SDLK_KP_MINUS:
            return VK_SUBTRACT; // (6D) Subtract key
        case SDLK_KP_PERIOD:// FIXME not sure of that
            return VK_DECIMAL; // (6E) Decimal key
        case SDLK_KP_DIVIDE:
            return VK_DIVIDE; // (6F) Divide key

        case SDLK_BACKSPACE:
            return VK_BACK; // (08) BACKSPACE key
        case SDLK_TAB:
            return VK_TAB; // (09) TAB key
        case SDLK_CLEAR:
            return VK_CLEAR; // (0C) CLEAR key
        case SDLK_RETURN:
#if ENABLE(CEHTML)
            return VK_ENTER; //(0D) Return key
#else
            return VK_RETURN; //(0D) Return key
#endif
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            return VK_SHIFT; // (10) SHIFT key
        case SDLK_LCTRL:
        case SDLK_RCTRL:
            return VK_CONTROL; // (11) CTRL key
        case SDLK_MENU:
        case SDLK_LALT:
        case SDLK_RALT:
#if ENABLE(CEHTML)
            return VK_ALT; // (12) ALT key
#else
            return VK_MENU; // (12) ALT key
#endif

        case SDLK_PAUSE:
            return VK_PAUSE; // (13) PAUSE key
        case SDLK_CAPSLOCK:
#if ENABLE(CEHTML)
            return VK_CAPS_LOCK;
#else
            return VK_CAPITAL; // (14) CAPS LOCK key
#endif
/*        case SDLK_KANA_LOCK:
        case SDLK_KANA_SHIFT:
            return VK_KANA; // (15) Input Method Editor (IME) Kana mode
        case SDLK_HANGUL:
            return VK_HANGUL; // VK_HANGUL (15) IME Hangul mode
            // VK_JUNJA (17) IME Junja mode
            // VK_FINAL (18) IME final mode
        case SDLK_HANGUL_HANJA:
            return VK_HANJA; // (19) IME Hanja mode
        case SDLK_KANJI:
            return VK_KANJI; // (19) IME Kanji mode*/
        case SDLK_ESCAPE:
            return VK_ESCAPE; // (1B) ESC key
            // VK_CONVERT (1C) IME convert
            // VK_NONCONVERT (1D) IME nonconvert
            // VK_ACCEPT (1E) IME accept
            // VK_MODECHANGE (1F) IME mode change request
        case SDLK_SPACE:
            return VK_SPACE; // (20) SPACEBAR
        case SDLK_PAGEUP:
#if ENABLE(CEHTML)
            return VK_PAGE_UP;
#else
            return VK_PRIOR; // (21) PAGE UP key
#endif
        case SDLK_PAGEDOWN:
#if ENABLE(CEHTML)
            return VK_PAGE_DOWN;
#else
            return VK_NEXT; // (22) PAGE DOWN key
#endif
        case SDLK_END:
            return VK_END; // (23) END key
        case SDLK_HOME:
            return VK_HOME; // (24) HOME key
        case SDLK_LEFT:
            return VK_LEFT; // (25) LEFT ARROW key
        case SDLK_UP:
            return VK_UP; // (26) UP ARROW key
        case SDLK_RIGHT:
            return VK_RIGHT; // (27) RIGHT ARROW key
        case SDLK_DOWN:
            return VK_DOWN; // (28) DOWN ARROW key
/*        case SDLK_SELECT:
            return VK_SELECT; // (29) SELECT key*/
        case SDLK_PRINT:
#if ENABLE(CEHTML)
            return VK_PRINTSCREEN;
#else
            return VK_PRINT; // (2A) PRINT key
#endif
/*        case SDLK_EXECUTE:
            return VK_EXECUTE;// (2B) EXECUTE key*/
            //dunno on this
            //case SDLK_PrintScreen:
            //      return VK_SNAPSHOT; // (2C) PRINT SCREEN key
        case SDLK_INSERT:
            return VK_INSERT; // (2D) INS key
        case SDLK_DELETE:
            return VK_DELETE; // (2E) DEL key
        case SDLK_HELP:
            return VK_HELP; // (2F) HELP key
        case SDLK_LEFTPAREN:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_5;
            return 0;
        case SDLK_0:
            return VK_0;    //  (30) 0 ) key
        case SDLK_1:
            return VK_1; //  (31) 1 ! key
        case SDLK_2:
        case SDLK_AT:
            return VK_2; //  (32) 2 & key
        case SDLK_3:
//         case SDLK_NUMBERSIGN:
            return VK_3; //case '3': case '#';
        case SDLK_4:
        case SDLK_DOLLAR: //  (34) 4 key '$';
            return VK_4;
        case SDLK_5:
//        case SDLK_PERCENT:
            return VK_5; //  (35) 5 key  '%'
        case SDLK_6:
/*        case SDLK_ASCIICIRCUM:
            return VK_6; //  (36) 6 key  '^'*/
        case SDLK_7:
            return VK_7; //  (37) 7 key  case '&'
        case SDLK_AMPERSAND:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_1;
            return 0;
//            return VK_AMPERSAND;
        case SDLK_8:
        case SDLK_ASTERISK:
            return VK_8; //  (38) 8 key  '*'
        case SDLK_9:
        case SDLK_RIGHTPAREN:
            return VK_9; //  (39) 9 key '('
        case SDLK_a:
//         case SDLK_A:
            return VK_A; //  (41) A key case 'a': case 'A': return 0x41;
        case SDLK_b:
#if ENABLE(CEHTML)
            if ((mod == KMOD_LCTRL) || (mod == KMOD_RCTRL))
                return VK_BLUE;
#endif
//         case SDLK_B:
            return VK_B; //  (42) B key case 'b': case 'B': return 0x42;
        case SDLK_c:
//         case SDLK_C:
            return VK_C; //  (43) C key case 'c': case 'C': return 0x43;
        case SDLK_d:
//         case SDLK_D:
#if ENABLE(CEHTML)
            if ((mod == KMOD_LCTRL) || (mod == KMOD_RCTRL))
                return VK_GUIDE;
#endif
            return VK_D; //  (44) D key case 'd': case 'D': return 0x44;
        case SDLK_e:
//         case SDLK_E:
            return VK_E; //  (45) E key case 'e': case 'E': return 0x45;
        case SDLK_f:
//         case SDLK_F:
            return VK_F; //  (46) F key case 'f': case 'F': return 0x46;
        case SDLK_g:
//         case SDLK_G:
#if ENABLE(CEHTML)
            if ((mod == KMOD_LCTRL) || (mod == KMOD_RCTRL))
                return VK_GREEN;
#endif
            return VK_G; //  (47) G key case 'g': case 'G': return 0x47;
        case SDLK_h:
//         case SDLK_H:
            return VK_H; //  (48) H key case 'h': case 'H': return 0x48;
        case SDLK_i:
//         case SDLK_I:
            return VK_I; //  (49) I key case 'i': case 'I': return 0x49;
        case SDLK_j:
//         case SDLK_J:
            return VK_J; //  (4A) J key case 'j': case 'J': return 0x4A;
        case SDLK_k:
//         case SDLK_K:
            return VK_K; //  (4B) K key case 'k': case 'K': return 0x4B;
        case SDLK_l:
//         case SDLK_L:
            return VK_L; //  (4C) L key case 'l': case 'L': return 0x4C;
        case SDLK_m:
//         case SDLK_M:
            return VK_M; //  (4D) M key case 'm': case 'M': return 0x4D;
        case SDLK_n:
#if ENABLE(CEHTML)
            if ((mod == KMOD_LCTRL) || (mod == KMOD_RCTRL))
                return VK_CHANNEL_UP;
#endif
//         case SDLK_N:
            return VK_N; //  (4E) N key case 'n': case 'N': return 0x4E;
        case SDLK_o:
//         case SDLK_O:
            return VK_O; //  (4F) O key case 'o': case 'O': return 0x4F;
        case SDLK_p:
#if ENABLE(CEHTML)
            if ((mod == KMOD_LCTRL) || (mod == KMOD_RCTRL))
                return VK_CHANNEL_DOWN;
#endif
//         case SDLK_P:
            return VK_P; //  (50) P key case 'p': case 'P': return 0x50;
        case SDLK_q:
//         case SDLK_Q:
            return VK_Q; //  (51) Q key case 'q': case 'Q': return 0x51;
        case SDLK_r:
#if ENABLE(CEHTML)
            if ((mod == KMOD_LCTRL) || (mod == KMOD_RCTRL))
                return VK_RED;
#endif
//         case SDLK_R:
            return VK_R; //  (52) R key case 'r': case 'R': return 0x52;
        case SDLK_s:
//         case SDLK_S:
            return VK_S; //  (53) S key case 's': case 'S': return 0x53;
        case SDLK_t:
//         case SDLK_T:
            return VK_T; //  (54) T key case 't': case 'T': return 0x54;
        case SDLK_u:
//         case SDLK_U:
            return VK_U; //  (55) U key case 'u': case 'U': return 0x55;
        case SDLK_v:
//         case SDLK_V:
            return VK_V; //  (56) V key case 'v': case 'V': return 0x56;
        case SDLK_w:
//         case SDLK_W:
            return VK_W; //  (57) W key case 'w': case 'W': return 0x57;
        case SDLK_x:
//         case SDLK_X:
            return VK_X; //  (58) X key case 'x': case 'X': return 0x58;
        case SDLK_y:
//         case SDLK_Y:
#if ENABLE(CEHTML)
            if ((mod == KMOD_LCTRL) || (mod == KMOD_RCTRL))
                return VK_YELLOW;
#endif
            return VK_Y; //  (59) Y key case 'y': case 'Y': return 0x59;
        case SDLK_z:
//         case SDLK_Z:
            return VK_Z; //  (5A) Z key case 'z': case 'Z': return 0x5A;
        case SDLK_LMETA:
#if ENABLE(CEHTML)
            return VK_META;
#else
            return VK_LWIN; // (5B) Left Windows key (Microsoft Natural keyboard)
#endif
        case SDLK_RMETA:
#if ENABLE(CEHTML)
            return VK_META;
#else
            return VK_RWIN; // (5C) Right Windows key (Natural keyboard)
#endif
            // VK_APPS (5D) Applications key (Natural keyboard)
            // VK_SLEEP (5F) Computer Sleep key
            // VK_SEPARATOR (6C) Separator key
            // VK_SUBTRACT (6D) Subtract key
            // VK_DECIMAL (6E) Decimal key
            // VK_DIVIDE (6F) Divide key
            // handled by key code above

        case SDLK_NUMLOCK:
#if ENABLE(CEHTML)
            return VK_NUM_LOCK;
#else
            return VK_NUMLOCK; // (90) NUM LOCK key
#endif

        case SDLK_SCROLLOCK:
#if ENABLE(CEHTML)
            return VK_SCROLL_LOCK;
#else
            return VK_SCROLL; // (91) SCROLL LOCK key
#endif

            // VK_LSHIFT (A0) Left SHIFT key
            // VK_RSHIFT (A1) Right SHIFT key
            // VK_LCONTROL (A2) Left CONTROL key
            // VK_RCONTROL (A3) Right CONTROL key
            // VK_LMENU (A4) Left MENU key
            // VK_RMENU (A5) Right MENU key
            // VK_BROWSER_BACK (A6) Windows 2000/XP: Browser Back key
            // VK_BROWSER_FORWARD (A7) Windows 2000/XP: Browser Forward key
            // VK_BROWSER_REFRESH (A8) Windows 2000/XP: Browser Refresh key
            // VK_BROWSER_STOP (A9) Windows 2000/XP: Browser Stop key
            // VK_BROWSER_SEARCH (AA) Windows 2000/XP: Browser Search key
            // VK_BROWSER_FAVORITES (AB) Windows 2000/XP: Browser Favorites key
            // VK_BROWSER_HOME (AC) Windows 2000/XP: Browser Start and Home key
            // VK_VOLUME_MUTE (AD) Windows 2000/XP: Volume Mute key
            // VK_VOLUME_DOWN (AE) Windows 2000/XP: Volume Down key
            // VK_VOLUME_UP (AF) Windows 2000/XP: Volume Up key
            // VK_MEDIA_NEXT_TRACK (B0) Windows 2000/XP: Next Track key
            // VK_MEDIA_PREV_TRACK (B1) Windows 2000/XP: Previous Track key
            // VK_MEDIA_STOP (B2) Windows 2000/XP: Stop Media key
            // VK_MEDIA_PLAY_PAUSE (B3) Windows 2000/XP: Play/Pause Media key
            // VK_LAUNCH_MAIL (B4) Windows 2000/XP: Start Mail key
            // VK_LAUNCH_MEDIA_SELECT (B5) Windows 2000/XP: Select Media key
            // VK_LAUNCH_APP1 (B6) Windows 2000/XP: Start Application 1 key
            // VK_LAUNCH_APP2 (B7) Windows 2000/XP: Start Application 2 key

            // VK_OEM_1 (BA) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ';:' key
        case SDLK_SEMICOLON:
        case SDLK_COLON:
#if ENABLE(CEHTML)
            return VK_SEMICOLON;
#else
            return VK_OEM_1; //case ';': case ':': return 0xBA;
#endif
            // VK_OEM_PLUS (BB) Windows 2000/XP: For any country/region, the '+' key
        case SDLK_PLUS:
        case SDLK_EQUALS:
#if ENABLE(CEHTML)
            return VK_ADD;
#else
            return VK_OEM_PLUS; //case '=': case '+': return 0xBB;
#endif
            // VK_OEM_COMMA (BC) Windows 2000/XP: For any country/region, the ',' key
        case SDLK_COMMA:
        case SDLK_LESS:
#if ENABLE(CEHTML)
            return VK_COMMA;
#else
            return VK_OEM_COMMA; //case ',': case '<': return 0xBC;
#endif
            // VK_OEM_MINUS (BD) Windows 2000/XP: For any country/region, the '-' key
        case SDLK_MINUS:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_6;
#if ENABLE(CEHTML)
            return VK_SUBTRACT;
#else
            return VK_OEM_MINUS; //case '-': case '_': return 0xBD;
#endif
        case SDLK_UNDERSCORE:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_8;
#if ENABLE(CEHTML)
            return VK_SUBTRACT;
#else
            return VK_OEM_MINUS; //case '-': case '_': return 0xBD;
#endif
            // VK_OEM_PERIOD (BE) Windows 2000/XP: For any country/region, the '.' key
        case SDLK_PERIOD:
        case SDLK_GREATER:
#if ENABLE(CEHTML)
            return VK_PERIOD;
#else
            return VK_OEM_PERIOD; //case '.': case '>': return 0xBE;
#endif
            // VK_OEM_2 (BF) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '/?' key
        case SDLK_SLASH:
        case SDLK_QUESTION:
#if ENABLE(CEHTML)
            return VK_SLASH;
#else
            return VK_OEM_2; //case '/': case '?': return 0xBF;
#endif
            // VK_OEM_3 (C0) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '`~' key
//        case SDLK_ASCIITILDE:
//         case SDLK_QUOTELEFT:
//             return VK_OEM_3; //case '`': case '~': return 0xC0;
            // VK_OEM_4 (DB) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '[{' key
/*        case SDLK_BRACKETLEFT:
        case SDLK_BRACELEFT:
            return VK_OEM_4; //case '[': case '{': return 0xDB;*/
            // VK_OEM_5 (DC) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '\|' key
        case SDLK_BACKSLASH:
//         case SDLK_BAR:
#if ENABLE(CEHTML)
            return VK_BACK_SLASH;
#else
            return VK_OEM_5; //case '\\': case '|': return 0xDC;
#endif
            // VK_OEM_6 (DD) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ']}' key
/*        case SDLK_BRACKETRIGHT:
        case SDLK_BRACERIGHT:
            return VK_OEM_6; // case ']': case '}': return 0xDD;*/
            // VK_OEM_7 (DE) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key
//         case SDLK_QUOTERIGHT:
        case SDLK_QUOTE:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_4;
#if ENABLE(CEHTML)
            return VK_QUOTE; // case '\'': case '"': return 0xDE;
#else
            return VK_OEM_7; // case '\'': case '"': return 0xDE;
#endif
        case SDLK_QUOTEDBL:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_3;
#if ENABLE(CEHTML)
            return VK_QUOTE; // case '\'': case '"': return 0xDE;
#else
            return VK_OEM_7; // case '\'': case '"': return 0xDE;
#endif
            // VK_OEM_8 (DF) Used for miscellaneous characters; it can vary by keyboard.
            // VK_OEM_102 (E2) Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
            // VK_PROCESSKEY (E5) Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key
            // VK_PACKET (E7) Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT,SendInput, WM_KEYDOWN, and WM_KEYUP
            // VK_ATTN (F6) Attn key
            // VK_CRSEL (F7) CrSel key
            // VK_EXSEL (F8) ExSel key
            // VK_EREOF (F9) Erase EOF key
            // VK_PLAY (FA) Play key
            // VK_ZOOM (FB) Zoom key
            // VK_NONAME (FC) Reserved for future use
            // VK_PA1 (FD) PA1 key
            // VK_OEM_CLEAR (FE) Clear key
        case SDLK_F1:
            return VK_F1;
        case SDLK_F2:
            return VK_F2;
        case SDLK_F3:
            return VK_F3;
        case SDLK_F4:
            return VK_F4;
        case SDLK_F5:
            return VK_F5;
        case SDLK_F6:
            return VK_F6;
        case SDLK_F7:
            return VK_F7;
        case SDLK_F8:
            return VK_F8;
        case SDLK_F9:
            return VK_F9;
        case SDLK_F10:
            return VK_F10;
        case SDLK_F11:
            return VK_F11;
        case SDLK_F12:
            return VK_F12;
        /*case SDLK_MODE:
            return VK_ALTGR;*/
#if !ENABLE(CEHTML)
        case SDLK_LSUPER:
            return VK_LWIN;
        case SDLK_RSUPER:
            return VK_RWIN;
#endif
        case SDLK_WORLD_64:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_0;
            return 0; 
        case SDLK_WORLD_71:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_9;
            return 0; 
        case SDLK_WORLD_72:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_7;
            return 0; 
        case SDLK_WORLD_73:
            if ((mod == KMOD_LSHIFT) || (mod == KMOD_RSHIFT))
                return VK_2;
            return 0; 
        default:
            LOG(Events, "ARGH!!! No virtual key!!!\n");
            return 0;
    }
}

#ifdef OWB_CONSOLE_INPUT
static WebCore::String keyIdentifierForStdin(const int keyCode)
{
    switch (keyCode) {
        //case SDLK_ISO_Enter:
        case 10:
            return "Enter";
        case SDLK_HELP:
            return "Help";
        case SDLK_HOME:
            return "Home";
        case SDLK_INSERT:
            return "Insert";
        case 52: // 4
            return "Left";
        case 54: // 6
            return "Right";
        case 56: // 8
            return "Up";
        case 50: // 2
            return "Down";
        case 51: // 3
            return "PageDown";
        case 57: // 9
            return "PageUp";
            // Standard says that DEL becomes U+00007F.
        case 127:
            return "U+00007F";
        default:
            return WebCore::String::format("U+%06X", toupper(keyCode));
    }
}

static int ConvertStdinToVirtualKey(const int keycode)
{
    switch (keycode) {
        case 127:
            return VK_BACK; // (08) BACKSPACE key
        case 9:
            return VK_TAB; // (09) TAB key
        case 10:
            return VK_RETURN; //(0D) Return key
        case 27:
            return VK_ESCAPE; // (1B) ESC key
        case 32:
            return VK_SPACE; // (20) SPACEBAR
        case 48:
            return VK_0;    //  (30) 0 ) key
        case 49:
            return VK_1; //  (31) 1 ! key
        case 50:
            return VK_2; //  (32) 2 & key
        case 51:
            return VK_3; //case '3': case '#';
        case 52:
            return VK_4;
        case 53:
            return VK_5; //  (35) 5 key  '%'
        case 54:
            return VK_6; //  (36) 6 key  '^'*/
        case 55:
            return VK_7; //  (37) 7 key  case '&'
        case 56:
            return VK_8; //  (38) 8 key  '*'
        case 57:
            return VK_9; //  (39) 9 key '('
        case 97:
        case 65:
            return VK_A; //  (41) A key case 'a': case 'A': return 0x41;
        case 98:
        case 66:
            return VK_B; //  (42) B key case 'b': case 'B': return 0x42;
        case 99:
        case 67:
            return VK_C; //  (43) C key case 'c': case 'C': return 0x43;
        case 100:
        case 68:
            return VK_D; //  (44) D key case 'd': case 'D': return 0x44;
        case 101:
        case 69:
            return VK_E; //  (45) E key case 'e': case 'E': return 0x45;
        case 102:
        case 70:
            return VK_F; //  (46) F key case 'f': case 'F': return 0x46;
        case 103:
        case 71:
            return VK_G; //  (47) G key case 'g': case 'G': return 0x47;
        case 104:
        case 72:
            return VK_H; //  (48) H key case 'h': case 'H': return 0x48;
        case 105:
        case 73:
            return VK_I; //  (49) I key case 'i': case 'I': return 0x49;
        case 106:
        case 74:
            return VK_J; //  (4A) J key case 'j': case 'J': return 0x4A;
        case 107:
        case 75:
            return VK_K; //  (4B) K key case 'k': case 'K': return 0x4B;
        case 108:
        case 76:
            return VK_L; //  (4C) L key case 'l': case 'L': return 0x4C;
        case 109:
        case 77:
            return VK_M; //  (4D) M key case 'm': case 'M': return 0x4D;
        case 110:
        case 78:
            return VK_N; //  (4E) N key case 'n': case 'N': return 0x4E;
        case 111:
        case 79:
            return VK_O; //  (4F) O key case 'o': case 'O': return 0x4F;
        case 112:
        case 80:
            return VK_P; //  (50) P key case 'p': case 'P': return 0x50;
        case 113:
        case 81:
            return VK_Q; //  (51) Q key case 'q': case 'Q': return 0x51;
        case 114:
        case 82:
            return VK_R; //  (52) R key case 'r': case 'R': return 0x52;
        case 115:
        case 83:
            return VK_S; //  (53) S key case 's': case 'S': return 0x53;
        case 116:
        case 84:
            return VK_T; //  (54) T key case 't': case 'T': return 0x54;
        case 117:
        case 85:
            return VK_U; //  (55) U key case 'u': case 'U': return 0x55;
        case 118:
        case 86:
            return VK_V; //  (56) V key case 'v': case 'V': return 0x56;
        case 119:
        case 87:
            return VK_W; //  (57) W key case 'w': case 'W': return 0x57;
        case 120:
        case 88:
            return VK_X; //  (58) X key case 'x': case 'X': return 0x58;
        case 121:
        case 89:
            return VK_Y; //  (59) Y key case 'y': case 'Y': return 0x59;
        case 122:
        case 90:
            return VK_Z; //  (5A) Z key case 'z': case 'Z': return 0x5A;
        case 59:
            return VK_OEM_1; //case ';': case ':': return 0xBA;
        case 43:
            return VK_OEM_PLUS; //case '=': case '+': return 0xBB;
        case 44:
            return VK_OEM_COMMA; //case ',': case '<': return 0xBC;
        case 45:
            return VK_OEM_MINUS; //case '-': case '_': return 0xBD;
        case 46:
            return VK_OEM_PERIOD; //case '.': case '>': return 0xBE;
        case 47:
            return VK_OEM_2; //case '/': case '?': return 0xBF;
        case 92:
            return VK_OEM_5; //case '\\': case '|': return 0xDC;
        case 34:
            return VK_OEM_7; // case '\'': case '"': return 0xDE;
        default:
            LOG(Events, "ARGH!!! No virtual key!!!\n");
            return 0;
    }
}
#endif

// Keep this in sync with the other platform event constructors
// TODO: m_balEventKey should be refcounted
PlatformKeyboardEvent::PlatformKeyboardEvent(BalEventKey* event)
    : m_type((event->state == SDL_RELEASED) ? KeyUp : KeyDown)
    , m_autoRepeat(false)
    , m_windowsVirtualKeyCode(ConvertSDLKeyToVirtualKey(event->keysym.sym, event->keysym.mod))
    , m_isKeypad(event->keysym.sym >= SDLK_KP0 && event->keysym.sym <= SDLK_KP_EQUALS)
    , m_shiftKey(event->keysym.mod & KMOD_SHIFT)
    , m_ctrlKey(event->keysym.mod & KMOD_CTRL)
    , m_altKey(event->keysym.mod & KMOD_ALT)
    , m_metaKey(event->keysym.mod & KMOD_META)
    , m_balEventKey(event)
{
    UChar aSrc[2];
    aSrc[0] = event->keysym.unicode;
    aSrc[1] = 0;

    WebCore::String aText(aSrc);
    WebCore::String aUnmodifiedText(aSrc);
    WebCore::String aKeyIdentifier = keyIdentifierForSDLKeyCode(event->keysym.sym);

    m_text = aText;
    m_unmodifiedText = aUnmodifiedText;
    m_keyIdentifier = aKeyIdentifier;

#if ENABLE(CEHTML)
    bool isVKKey = event->keysym.scancode == 0xFF && event->keysym.sym == event->keysym.unicode;
    if (UNLIKELY(isVKKey)) {
        WebCore::String vkKey = convertVKKeyToString(event->keysym.sym);
        ASSERT(!vkKey.isNull());
        m_keyIdentifier = vkKey;
        m_unmodifiedText = vkKey;
        m_windowsVirtualKeyCode = event->keysym.sym;
    }
#endif
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
    notImplemented();
    return false;
}

BalEventKey* PlatformKeyboardEvent::balEventKey() const
{
    return m_balEventKey;
}

void PlatformKeyboardEvent::getCurrentModifierState(bool& shiftKey, bool& ctrlKey, bool& altKey, bool& metaKey)
{
    SDLMod modState = SDL_GetModState();
    
    shiftKey = modState & (KMOD_LSHIFT | KMOD_RSHIFT);
    ctrlKey = modState & (KMOD_LCTRL | KMOD_RCTRL);
    altKey = modState & (KMOD_LALT | KMOD_RALT);
    metaKey = modState & (KMOD_LMETA | KMOD_RMETA);
}

}
