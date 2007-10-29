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
 * @file BIKeyboardEvent.h
 *
 * Interface of Keyboard Event
 */

#ifndef BIKeyboardEvent_H
#define BIKeyboardEvent_H

#include "config.h"
#include "PlatformString.h"

#include "BIInputEvent.h"

namespace BAL {

    /**
    * @brief the KeyboardEvent
    *
    * The keyboard event.
    *
    * @see BIEvent, BIEventLoop
    */
    class BIKeyboardEvent : public BIInputEvent {
    public:
        virtual ~BIKeyboardEvent() {};

         /**
          * returns the key, in lower case for the letters.
          * some keys can have no corresponding text (Enter, Arrow, etc.)
          */
        virtual WebCore::String text() const = 0;

         /**
          * returns the key unmodified. It means in lower case or upper case,
          * according to the shift/capslock key
          */
        virtual WebCore::String unmodifiedText() const = 0;

         /**
          * returns a string for all the keys. It is a set over the text() method,
          * completed with string like "Enter", for the Enter key, and so on.
          */
        virtual WebCore::String keyIdentifier() const = 0;

         /**
          * whether key is up or down (release or pressed).
          */
        virtual bool isKeyUp() const = 0;

        /**
          * autorepeat
          */
        virtual bool isAutoRepeat() const = 0;

        /**
          * autorepeat
          */
        virtual void setIsAutoRepeat(bool) = 0;

        /**
          * Get the windows keycode
          */
        virtual int WindowsKeyCode() const = 0;

        /**
          * Get the windows keycode
          */
        virtual int setWindowsKeyCode(int) const = 0;

         /**
          * returns a VK for a key. It is important to note that
          * some keys can have no VK. VKs are mainly used for function (F1, F2, etc),
          * navigation (Arrow, etc.). For keys which purpose is to be written,
          * use the text method, to get the resulting text.
          */
        virtual int virtualKeyCode() const = 0;

         /**
          * isKeyPad
          */
        virtual bool isKeypad() const = 0;

         /**
          * clone the object
          */
        virtual BIEvent* clone() const = 0;

        virtual BIKeyboardEvent* queryIsKeyboardEvent() { return this; }

    public:

      enum EnumVirtualKey
      {
        // VK_LBUTTON (01) Left mouse button
        // VK_RBUTTON (02) Right mouse button
        // VK_CANCEL (03) Control-break processing
        // VK_MBUTTON (04) Middle mouse button (three-button mouse)
        // VK_XBUTTON1 (05)
        // VK_XBUTTON2 (06)
        // VK_BACK (08) BACKSPACE key
        VK_BACK = 0x08,

        // VK_TAB (09) TAB key
        VK_TAB = 0x09,

        // VK_CLEAR (0C) CLEAR key
        VK_CLEAR = 0x0C,

        // VK_RETURN (0D)
        VK_RETURN = 0x0D,

        // VK_SHIFT (10) SHIFT key
        VK_SHIFT = 0x10,

        // VK_CONTROL (11) CTRL key
        VK_CONTROL = 0x11,

        // VK_MENU (12) ALT key
        VK_MENU = 0x12,

        // VK_PAUSE (13) PAUSE key
        VK_PAUSE = 0x13,

        // VK_CAPITAL (14) CAPS LOCK key
        VK_CAPITAL = 0x14,

        // VK_KANA (15) Input Method Editor (IME) Kana mode
        VK_KANA = 0x15,

        // VK_HANGUEL (15) IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
        // VK_HANGUL (15) IME Hangul mode
        VK_HANGUL = 0x15,

        // VK_JUNJA (17) IME Junja mode
        VK_JUNJA = 0x17,

        // VK_FINAL (18) IME final mode
        VK_FINAL = 0x18,

        // VK_HANJA (19) IME Hanja mode
        VK_HANJA = 0x19,

        // VK_KANJI (19) IME Kanji mode
        VK_KANJI = 0x19,


        // VK_ESCAPE (1B) ESC key
        VK_ESCAPE = 0x1B,


        // VK_CONVERT (1C) IME convert
        VK_CONVERT = 0x1C,

        // VK_NONCONVERT (1D) IME nonconvert
        VK_NONCONVERT = 0x1D,

        // VK_ACCEPT (1E) IME accept
        VK_ACCEPT = 0x1E,

        // VK_MODECHANGE (1F) IME mode change request
        VK_MODECHANGE = 0x1F,

        // VK_SPACE (20) SPACEBAR
        VK_SPACE = 0x20,

        // VK_PRIOR (21) PAGE UP key
        VK_PRIOR = 0x21,

        // VK_NEXT (22) PAGE DOWN key
        VK_NEXT = 0x22,

        // VK_END (23) END key
        VK_END = 0x23,

        // VK_HOME (24) HOME key
        VK_HOME = 0x24,

        // VK_LEFT (25) LEFT ARROW key
        VK_LEFT = 0x25,

        // VK_UP (26) UP ARROW key
        VK_UP = 0x26,

        // VK_RIGHT (27) RIGHT ARROW key
        VK_RIGHT = 0x27,

        // VK_DOWN (28) DOWN ARROW key
        VK_DOWN = 0x28,

        // VK_SELECT (29) SELECT key
        VK_SELECT = 0x29,

        // VK_PRINT (2A) PRINT key
        VK_PRINT = 0x2A,

        // VK_EXECUTE (2B) EXECUTE key
        VK_EXECUTE = 0x2B,

        // VK_SNAPSHOT (2C) PRINT SCREEN key
        VK_SNAPSHOT = 0x2C,

        // VK_INSERT (2D) INS key
        VK_INSERT = 0x2D,

        // VK_DELETE (2E) DEL key
        VK_DELETE = 0x2E,

        // VK_HELP (2F) HELP key
        VK_HELP = 0x2F,

        //  (30) 0 key
        VK_0 = 0x30,

        //  (31) 1 key
        VK_1 = 0x31,

        //  (32) 2 key
        VK_2 = 0x32,

        //  (33) 3 key
        VK_3 = 0x33,

        //  (34) 4 key
        VK_4 = 0x34,

        //  (35) 5 key;

        VK_5 = 0x35,

        //  (36) 6 key
        VK_6 = 0x36,

        //  (37) 7 key
        VK_7 = 0x37,

        //  (38) 8 key
        VK_8 = 0x38,

        //  (39) 9 key
        VK_9 = 0x39,

        //  (41) A key
        VK_A = 0x41,

        //  (42) B key
        VK_B = 0x42,

        //  (43) C key
        VK_C = 0x43,

        //  (44) D key
        VK_D = 0x44,

        //  (45) E key
        VK_E = 0x45,

        //  (46) F key
        VK_F = 0x46,

        //  (47) G key
        VK_G = 0x47,

        //  (48) H key
        VK_H = 0x48,

        //  (49) I key
        VK_I = 0x49,

        //  (4A) J key
        VK_J = 0x4A,

        //  (4B) K key
        VK_K = 0x4B,

        //  (4C) L key
        VK_L = 0x4C,

        //  (4D) M key
        VK_M = 0x4D,

        //  (4E) N key
        VK_N = 0x4E,

        //  (4F) O key
        VK_O = 0x4F,

        //  (50) P key
        VK_P = 0x50,

        //  (51) Q key
        VK_Q = 0x51,

        //  (52) R key
        VK_R = 0x52,

        //  (53) S key
        VK_S = 0x53,

        //  (54) T key
        VK_T = 0x54,

        //  (55) U key
        VK_U = 0x55,

        //  (56) V key
        VK_V = 0x56,

        //  (57) W key
        VK_W = 0x57,

        //  (58) X key
        VK_X = 0x58,

        //  (59) Y key
        VK_Y = 0x59,

        //  (5A) Z key
        VK_Z = 0x5A,

        // VK_LWIN (5B) Left Windows key (Microsoft Natural keyboard)
        VK_LWIN = 0x5B,

        // VK_RWIN (5C) Right Windows key (Natural keyboard)
        VK_RWIN = 0x5C,

        // VK_APPS (5D) Applications key (Natural keyboard)
        VK_APPS = 0x5D,

        // VK_SLEEP (5F) Computer Sleep key
        VK_SLEEP = 0x5F,

        // VK_NUMPAD0 (60) Numeric keypad 0 key
        VK_NUMPAD0 = 0x60,

        // VK_NUMPAD1 (61) Numeric keypad 1 key
        VK_NUMPAD1 = 0x61,

        // VK_NUMPAD2 (62) Numeric keypad 2 key
        VK_NUMPAD2 = 0x62,

        // VK_NUMPAD3 (63) Numeric keypad 3 key
        VK_NUMPAD3 = 0x63,

        // VK_NUMPAD4 (64) Numeric keypad 4 key
        VK_NUMPAD4 = 0x64,

        // VK_NUMPAD5 (65) Numeric keypad 5 key
        VK_NUMPAD5 = 0x65,

        // VK_NUMPAD6 (66) Numeric keypad 6 key
        VK_NUMPAD6 = 0x66,

        // VK_NUMPAD7 (67) Numeric keypad 7 key
        VK_NUMPAD7 = 0x67,

        // VK_NUMPAD8 (68) Numeric keypad 8 key
        VK_NUMPAD8 = 0x68,

        // VK_NUMPAD9 (69) Numeric keypad 9 key
        VK_NUMPAD9 = 0x69,

        // VK_MULTIPLY (6A) Multiply key
        VK_MULTIPLY = 0x6A,

        // VK_ADD (6B) Add key
        VK_ADD = 0x6B,

        // VK_SEPARATOR (6C) Separator key
        VK_SEPARATOR = 0x6C,

        // VK_SUBTRACT (6D) Subtract key
        VK_SUBTRACT = 0x6D,

        // VK_DECIMAL (6E) Decimal key
        VK_DECIMAL = 0x6E,

        // VK_DIVIDE (6F) Divide key
        VK_DIVIDE = 0x6F,

        // VK_F1 (70) F1 key
        VK_F1 = 0x70,

        // VK_F2 (71) F2 key
        VK_F2 = 0x71,

        // VK_F3 (72) F3 key
        VK_F3 = 0x72,

        // VK_F4 (73) F4 key
        VK_F4 = 0x73,

        // VK_F5 (74) F5 key
        VK_F5 = 0x74,

        // VK_F6 (75) F6 key
        VK_F6 = 0x75,

        // VK_F7 (76) F7 key
        VK_F7 = 0x76,

        // VK_F8 (77) F8 key
        VK_F8 = 0x77,

        // VK_F9 (78) F9 key
        VK_F9 = 0x78,

        // VK_F10 (79) F10 key
        VK_F10 = 0x79,

        // VK_F11 (7A) F11 key
        VK_F11 = 0x7A,

        // VK_F12 (7B) F12 key
        VK_F12 = 0x7B,

        // VK_F13 (7C) F13 key
        VK_F13 = 0x7C,

        // VK_F14 (7D) F14 key
        VK_F14 = 0x7D,

        // VK_F15 (7E) F15 key
        VK_F15 = 0x7E,

        // VK_F16 (7F) F16 key
        VK_F16 = 0x7F,

        // VK_F17 (80H) F17 key
        VK_F17 = 0x80,

        // VK_F18 (81H) F18 key
        VK_F18 = 0x81,

        // VK_F19 (82H) F19 key
        VK_F19 = 0x82,

        // VK_F20 (83H) F20 key
        VK_F20 = 0x83,

        // VK_F21 (84H) F21 key
        VK_F21 = 0x84,

        // VK_F22 (85H) F22 key
        VK_F22 = 0x85,

        // VK_F23 (86H) F23 key
        VK_F23 = 0x86,

        // VK_F24 (87H) F24 key
        VK_F24 = 0x87,

        // VK_NUMLOCK (90) NUM LOCK key
        VK_NUMLOCK = 0x90,

        // VK_SCROLL (91) SCROLL LOCK key
        VK_SCROLL = 0x91,

        // VK_LSHIFT (A0) Left SHIFT key
        VK_LSHIFT = 0xA0,

        // VK_RSHIFT (A1) Right SHIFT key
        VK_RSHIFT = 0xA1,

        // VK_LCONTROL (A2) Left CONTROL key
        VK_LCONTROL = 0xA2,

        // VK_RCONTROL (A3) Right CONTROL key
        VK_RCONTROL = 0xA3,

        // VK_LMENU (A4) Left MENU key
        VK_LMENU = 0xA4,

        // VK_RMENU (A5) Right MENU key
        VK_RMENU = 0xA5,

        // VK_BROWSER_BACK (A6) Windows 2000/XP: Browser Back key
        VK_BROWSER_BACK = 0xA6,

        // VK_BROWSER_FORWARD (A7) Windows 2000/XP: Browser Forward key
        VK_BROWSER_FORWARD = 0xA7,

        // VK_BROWSER_REFRESH (A8) Windows 2000/XP: Browser Refresh key
        VK_BROWSER_REFRESH = 0xA8,

        // VK_BROWSER_STOP (A9) Windows 2000/XP: Browser Stop key
        VK_BROWSER_STOP = 0xA9,

        // VK_BROWSER_SEARCH (AA) Windows 2000/XP: Browser Search key
        VK_BROWSER_SEARCH = 0xAA,

        // VK_BROWSER_FAVORITES (AB) Windows 2000/XP: Browser Favorites key
        VK_BROWSER_FAVORITES = 0xAB,

        // VK_BROWSER_HOME (AC) Windows 2000/XP: Browser Start and Home key
        VK_BROWSER_HOME = 0xAC,

        // VK_VOLUME_MUTE (AD) Windows 2000/XP: Volume Mute key
        VK_VOLUME_MUTE = 0xAD,

        // VK_VOLUME_DOWN (AE) Windows 2000/XP: Volume Down key
        VK_VOLUME_DOWN = 0xAE,

        // VK_VOLUME_UP (AF) Windows 2000/XP: Volume Up key
        VK_VOLUME_UP = 0xAF,

        // VK_MEDIA_NEXT_TRACK (B0) Windows 2000/XP: Next Track key
        VK_MEDIA_NEXT_TRACK = 0xB0,

        // VK_MEDIA_PREV_TRACK (B1) Windows 2000/XP: Previous Track key
        VK_MEDIA_PREV_TRACK = 0xB1,

        // VK_MEDIA_STOP (B2) Windows 2000/XP: Stop Media key
        VK_MEDIA_STOP = 0xB2,

        // VK_MEDIA_PLAY_PAUSE (B3) Windows 2000/XP: Play/Pause Media key
        VK_MEDIA_PLAY_PAUSE = 0xB3,

        // VK_LAUNCH_MAIL (B4) Windows 2000/XP: Start Mail key
        VK_MEDIA_LAUNCH_MAIL = 0xB4,

        // VK_LAUNCH_MEDIA_SELECT (B5) Windows 2000/XP: Select Media key
        VK_MEDIA_LAUNCH_MEDIA_SELECT = 0xB5,

        // VK_LAUNCH_APP1 (B6) Windows 2000/XP: Start Application 1 key
        VK_MEDIA_LAUNCH_APP1 = 0xB6,

        // VK_LAUNCH_APP2 (B7) Windows 2000/XP: Start Application 2 key
        VK_MEDIA_LAUNCH_APP2 = 0xB7,

        // VK_OEM_1 (BA) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ';:' key
        VK_OEM_1 = 0xBA,

        // VK_OEM_PLUS (BB) Windows 2000/XP: For any country/region, the '+' key
        VK_OEM_PLUS = 0xBB,

        // VK_OEM_COMMA (BC) Windows 2000/XP: For any country/region, the ',' key
        VK_OEM_COMMA = 0xBC,

        // VK_OEM_MINUS (BD) Windows 2000/XP: For any country/region, the '-' key
        VK_OEM_MINUS = 0xBD,

        // VK_OEM_PERIOD (BE) Windows 2000/XP: For any country/region, the '.' key
        VK_OEM_PERIOD = 0xBE,

        // VK_OEM_2 (BF) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '/?' key
        VK_OEM_2 = 0xBF,

        // VK_OEM_3 (C0) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '`~' key
        VK_OEM_3 = 0xC0,

        // VK_OEM_4 (DB) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '[{' key
        VK_OEM_4 = 0xDB,

        // VK_OEM_5 (DC) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '\|' key
        VK_OEM_5 = 0xDC,

        // VK_OEM_6 (DD) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ']}' key
        VK_OEM_6 = 0xDD,

        // VK_OEM_7 (DE) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key
        VK_OEM_7 = 0xDE,

        // VK_OEM_8 (DF) Used for miscellaneous characters; it can vary by keyboard.
        VK_OEM_8 = 0xDF,

        // VK_OEM_102 (E2) Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
        VK_OEM_102 = 0xE2,

        // VK_PROCESSKEY (E5) Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key
        VK_PROCESSKEY = 0xE5,

        // VK_PACKET (E7) Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT,SendInput, WM_KEYDOWN, and WM_KEYUP
        VK_PACKET = 0xE7,

        // VK_ATTN (F6) Attn key
        VK_ATTN = 0xF6,

        // VK_CRSEL (F7) CrSel key
        VK_CRSEL = 0xF7,

        // VK_EXSEL (F8) ExSel key
        VK_EXSEL = 0xF8,

        // VK_EREOF (F9) Erase EOF key
        VK_EREOF = 0xF9,

        // VK_PLAY (FA) Play key
        VK_PLAY = 0xFA,

        // VK_ZOOM (FB) Zoom key
        VK_ZOOM = 0xFB,

        // VK_NONAME (FC) Reserved for future use
        VK_NONAME = 0xFC,

        // VK_PA1 (FD) PA1 key
        VK_PA1 = 0xFD,

        // VK_OEM_CLEAR (FE) Clear key
        VK_OEM_CLEAR = 0xFE,

        VK_UNKOWN = 0
      };
   };
}

#endif
