/*
 * Copyright (C) 2009 Pleyo.  All rights reserved.
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

#include "EventSender.h"

#include "DumpRenderTree.h"
#include "SDL.h"
#include <sys/time.h> 
#include <unistd.h>

#if ENABLE(CEHTML)
#include "VKKeyConversion.h"
#endif

BalEventMotion EventSenderController::makeMouseMotionEvent(bool state, int x, int y)
{
    SDL_MouseMotionEvent me;
    if (state)
        me.state = SDL_BUTTON_LEFT;
    me.x = x;
    me.y = y;

    return me;
}


void EventSenderController::dispatchMouseMotion(BalEventMotion em)
{
#if ENABLE(DAE_APPLICATION)
    WebEventSender* eventSender = WebEventSender::getEventSender();
    eventSender->onMouseMotion(em);
#else
    getWebView()->onMouseMotion(em);
#endif
}

BalEventButton EventSenderController::makeMouseButtonEvent(bool up, int x, int y)
{
    SDL_MouseButtonEvent be;
    if (up)
        be.type = SDL_MOUSEBUTTONUP;
    else
        be.type = SDL_MOUSEBUTTONDOWN;

    be.button = SDL_BUTTON_LEFT;

    if (up)
        be.state = SDL_RELEASED;
    else
        be.state = SDL_PRESSED;
    be.x = x;
    be.y = y;

    return be;
}

void EventSenderController::dispatchMouseButton(BalEventButton eb)
{
#if ENABLE(DAE_APPLICATION)
    WebEventSender* eventSender = WebEventSender::getEventSender();
    if (eb.type == SDL_MOUSEBUTTONUP)
        eventSender->onMouseButtonUp(eb);
    else
        eventSender->onMouseButtonDown(eb);
#else
    if (eb.type == SDL_MOUSEBUTTONUP)
        getWebView()->onMouseButtonUp(eb);
    else
        getWebView()->onMouseButtonDown(eb);
#endif
}

static SDLKey ConvertKeycodeToSDLKey(const int keycode)
{
    //printf("ConvertKeycodeToSDLKey %d %d\n", keycode, SDLK_DELETE);
    switch (keycode) {
        case 8:
        case 127:
            return SDLK_BACKSPACE; // (08) BACKSPACE key
        case 9:
            return SDLK_TAB; // (09) TAB key
        case 10:
            return SDLK_RETURN; //(0D) Return key
        case 27:
            return SDLK_ESCAPE; // (1B) ESC key
        case 32:
            return SDLK_SPACE; // (20) SPACEBAR
        case 48:
            return SDLK_0;    //  (30) 0 ) key
        case 49:
            return SDLK_1; //  (31) 1 ! key
        case 50:
            return SDLK_2; //  (32) 2 & key
        case 51:
            return SDLK_3; //case '3': case '#';
        case 52:
            return SDLK_4;
        case 53:
            return SDLK_5; //  (35) 5 key  '%'
        case 54:
            return SDLK_6; //  (36) 6 key  '^'*/
        case 55:
            return SDLK_7; //  (37) 7 key  case '&'
        case 56:
            return SDLK_8; //  (38) 8 key  '*'
        case 57:
            return SDLK_9; //  (39) 9 key '('
        case 97:
        case 65:
            return SDLK_a; //  (41) A key case 'a': case 'A': return 0x41;
        case 98:
        case 66:
            return SDLK_b; //  (42) B key case 'b': case 'B': return 0x42;
        case 99:
        case 67:
            return SDLK_c; //  (43) C key case 'c': case 'C': return 0x43;
        case 100:
        case 68:
            return SDLK_d; //  (44) D key case 'd': case 'D': return 0x44;
        case 101:
        case 69:
            return SDLK_e; //  (45) E key case 'e': case 'E': return 0x45;
        case 102:
        case 70:
            return SDLK_f; //  (46) F key case 'f': case 'F': return 0x46;
        case 103:
        case 71:
            return SDLK_g; //  (47) G key case 'g': case 'G': return 0x47;
        case 104:
        case 72:
            return SDLK_h; //  (48) H key case 'h': case 'H': return 0x48;
        case 105:
        case 73:
            return SDLK_i; //  (49) I key case 'i': case 'I': return 0x49;
        case 106:
        case 74:
            return SDLK_j; //  (4A) J key case 'j': case 'J': return 0x4A;
        case 107:
        case 75:
            return SDLK_k; //  (4B) K key case 'k': case 'K': return 0x4B;
        case 108:
        case 76:
            return SDLK_l; //  (4C) L key case 'l': case 'L': return 0x4C;
        case 109:
        case 77:
            return SDLK_m; //  (4D) M key case 'm': case 'M': return 0x4D;
        case 110:
        case 78:
            return SDLK_n; //  (4E) N key case 'n': case 'N': return 0x4E;
        case 111:
        case 79:
            return SDLK_o; //  (4F) O key case 'o': case 'O': return 0x4F;
        case 112:
        case 80:
            return SDLK_p; //  (50) P key case 'p': case 'P': return 0x50;
        case 113:
        case 81:
            return SDLK_q; //  (51) Q key case 'q': case 'Q': return 0x51;
        case 114:
        case 82:
            return SDLK_r; //  (52) R key case 'r': case 'R': return 0x52;
        case 115:
        case 83:
            return SDLK_s; //  (53) S key case 's': case 'S': return 0x53;
        case 116:
        case 84:
            return SDLK_t; //  (54) T key case 't': case 'T': return 0x54;
        case 117:
        case 85:
            return SDLK_u; //  (55) U key case 'u': case 'U': return 0x55;
        case 118:
        case 86:
            return SDLK_v; //  (56) V key case 'v': case 'V': return 0x56;
        case 119:
        case 87:
            return SDLK_w; //  (57) W key case 'w': case 'W': return 0x57;
        case 120:
        case 88:
            return SDLK_x; //  (58) X key case 'x': case 'X': return 0x58;
        case 121:
        case 89:
            return SDLK_y; //  (59) Y key case 'y': case 'Y': return 0x59;
        case 122:
        case 90:
            return SDLK_z; //  (5A) Z key case 'z': case 'Z': return 0x5A;
        case 59:
            return SDLK_SEMICOLON; //case ';': case ':': return 0xBA;
        case 43:
            return SDLK_PLUS; //case '=': case '+': return 0xBB;
        case 44:
            return SDLK_COMMA; //case ',': case '<': return 0xBC;
        case 45:
            return SDLK_MINUS; //case '-': case '_': return 0xBD;
        case 46:
            return SDLK_PERIOD; //case '.': case '>': return 0xBE;
        case 47:
            return SDLK_SLASH; //case '/': case '?': return 0xBF;
        case 92:
            return SDLK_BACKSLASH; //case '\\': case '|': return 0xDC;
        case 34:
            return SDLK_QUOTE; // case '\'': case '"': return 0xDE;
        default:
            //LOG(Events, "ARGH!!! No virtual key!!!\n");
            return (SDLKey)0;
    }
}

BalEventKey EventSenderController::makeKeyboardEvent(bool up, bool shift, bool control, bool alt, bool meta, int charCode, bool isVKKey)
{
    SDL_KeyboardEvent ke;

    if (up) {
        ke.type = SDL_KEYUP;
        ke.state = SDL_RELEASED;
    } else {
        ke.type = SDL_KEYDOWN;
        ke.state = SDL_PRESSED;
    }

    int kmod = KMOD_NONE;
    if (shift)
        kmod |= KMOD_LSHIFT;
    if (control)
        kmod |= KMOD_LCTRL;
    if (alt)
        kmod |= KMOD_LALT;
    if (meta)
        kmod |= KMOD_LMETA;

    // FIXME: We should be able to OR the bytes together but gcc complains about it.
    // It should be fine to cast it to a SDLMod as it is an enum internally (ie an int so
    // no overflow possible).
    ke.keysym.mod = (SDLMod)kmod;

#if ENABLE(CEHTML)
    if (isVKKey) {
        ke.keysym.sym = (SDLKey)charCode;
        // We embed the VKKey information into the scancode.
        ke.keysym.scancode = 0xFF;
    } else
#endif
    {
        ke.keysym.sym = ConvertKeycodeToSDLKey(charCode);
        ke.keysym.scancode = 0;
    }

    //hack for enter key
    if (charCode == 0xa || charCode == 0x20)
        ke.keysym.unicode = 0xd;
    else
        ke.keysym.unicode = charCode;

    return ke;
}

void EventSenderController::dispatchKeyboardEvent(BalEventKey ek)
{
#if ENABLE(DAE_APPLICATION)
    WebEventSender* eventSender = WebEventSender::getEventSender();
    eventSender->onKeyDown(ek);
#else
    getWebView()->onKeyDown(ek);
#endif
}

int EventSenderController::getCharCode(char* code)
{
    if (!strcmp(code, "leftArrow"))
        return 52;
    else if (!strcmp(code, "rightArrow"))
        return 54;
    else if (!strcmp(code, "upArrow"))
        return 56;
    else if (!strcmp(code, "downArrow"))
        return 50;
    else if (!strcmp(code, "pageUp"))
        return 57;
    else if (!strcmp(code, "pageDown"))
        return 51;
    else if (!strcmp(code, "home"))
        return SDLK_HOME;
    else if (!strcmp(code, "end"))
        return SDLK_END;
    else if (!strcmp(code, "delete"))
        return SDLK_BACKSPACE;
    return 0;    
}
