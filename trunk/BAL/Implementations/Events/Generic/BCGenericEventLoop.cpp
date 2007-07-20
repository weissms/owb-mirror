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
 * @file  BCGenericEventLoop.cpp
 *
 * @brief Bal Concretisation of Event Loop
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 */

#include "config.h"
#include "PlatformString.h"

#include "BCGenericEventLoop.h"
#include "BCKeyboardEvent.h"
#include "BCWindowEvent.h"
#include "BCMouseEvent.h"
#include "BCWheelEvent.h"
#include "BIInternationalization.h"
#include "SharedTimer.h"
#include "BTLogHelper.h"

// utilities functions for conversion
//static WebCore::String keyIdentifierForSDLKeyCode(SDLKey keyCode);
//static int ConvertSDLKeyToVirtualKey(SDLKey keycode);
//static BAL::BIInputEvent* CreateMouseorWheelEventFromSDLEvent( const SDL_Event& aSDLEvent );

using namespace BAL;

namespace BAL {
static BC::BCGenericEventLoop* gMainLoop = NULL;

    BIEventLoop* getBIEventLoop() {
        if( gMainLoop == NULL ) {
            printf("create a new event loop\n");
	   gMainLoop = new BC::BCGenericEventLoop();
        }
        return gMainLoop;
    }

    void deleteBIEventLoop( BIEventLoop* /*aEventLoop*/ ) {
    //delete aEventLoop; FIXME need refcount
  }
}

/**
 * Constructor of Event Loop
 *
 * Construcs and set the mIsInitialized flag
 *
 */
BC::BCGenericEventLoop::BCGenericEventLoop()
    : mIsInitialized(false)
    , m_event(0)
{
    /*if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0) {
        logml(MODULE_EVENTS, LEVEL_CRITICAL,
              make_message("Unable to init SDL: %s\n", SDL_GetError()));
    } else {
        mIsInitialized = true;
        SDL_EnableUNICODE(1);
        if (1 != SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL))
            logm(MODULE_EVENTS, "SDL can't enable key repeat");
        atexit(SDL_Quit);
    }*/
    mIsInitialized = true;
}

/**
 * Destructor
 *
 */
BC::BCGenericEventLoop::~BCGenericEventLoop()
{
    //SDL_Quit();
}

/**
 * isInitialized
 *
 * returns true if event loop correctly initialized
 *
 */
bool BC::BCGenericEventLoop::isInitialized() const
{
  return mIsInitialized;
}

/**
 * quit() posts an event in event loop to quit
 *
 */
void BC::BCGenericEventLoop::quit()
{
    /*SDL_Event aSDLEvent;
    aSDLEvent.quit.type = SDL_QUIT;

    SDL_PushEvent(&aSDLEvent);*/
}

/**
 * WaitEvent waits for next event
 *
 * @param aBALEvent(out) the BAL event returned
 *
 * @return true if event ok and loop can go on
 * @return false else
 */
bool BC::BCGenericEventLoop::WaitEvent(BIEvent*& aBALEvent)
{
    if (!mIsInitialized || m_event.isEmpty())
        return false;

//     printf("------ODO------ consume an event (#%d @%p)\n", m_event.size(), m_event.first());
    aBALEvent = m_event.first();
    m_event.remove(0);
    if (aBALEvent->queryIsTimerEvent()) {
        WebCore::sharedTimerFiredFunction();
        return false;
    }
    return true;
}

/**
 *
 *
 * @param aBALEvent(in)
 *
 * @return true if event ok and loop can go on
 * @return false else
 */
bool BC::BCGenericEventLoop::PushEvent(BIEvent* aBALEvent)
{
    if (!mIsInitialized)
        return false;

    if (!aBALEvent)
        return false;

//     printf("------ODO------ receive an event %p for size %d\n", aBALEvent, m_event.size());
    m_event.append(aBALEvent);
    return true;
    /*SDL_Event *event = new SDL_Event;
    if (!event)
        return false;
    event->type = SDL_USEREVENT;
    event->user.code = 2;
    event->user.data1 = aBALEvent;
    int error = SDL_PushEvent(event);
    if (error == -1) {
        logml(MODULE_EVENTS, LEVEL_WARNING, "event is lost: please increase SDL queue size (greater than the default 128)");
    	return false;
    } else
        return true;*/
}

/**
 * CreateEventFromSDLEvent converts SDL event into BAL event
 *
 * @param aSDLEvent(in) the SDL event
 *
 * @return the BAL event
 */
BIEvent* BC::BCGenericEventLoop::CreateEventFromGenericEvent( /*const SDL_Event& aSDLEvent*/ )
{
#if 0
  switch( aSDLEvent.type )
  {
    // Keyboard case
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
      bool bIsKeyUp =  aSDLEvent.type == SDL_KEYUP;
      bool bShiftKey = aSDLEvent.key.keysym.mod & KMOD_SHIFT;
      bool bCtrlKey = aSDLEvent.key.keysym.mod & KMOD_CTRL;
      bool bAltKey = aSDLEvent.key.keysym.mod & KMOD_ALT;
      bool bMetaKey = aSDLEvent.key.keysym.mod & KMOD_META; /*meta ?*/
      int aVKey = ConvertSDLKeyToVirtualKey( aSDLEvent.key.keysym.sym );

      UChar aSrc[2];
      aSrc[0] = aSDLEvent.key.keysym.unicode;
      aSrc[1] = 0;

      logm(MODULE_EVENTS, make_message("SDL KEY CODE %d\nWHICH %d MOD %d\n"
              "SCANCODE %d UNICODE %d END\nSHIFT %d CTRL %d ALT %d META %d",
              aSDLEvent.key.keysym.sym, aSDLEvent.key.which, aSDLEvent.key.keysym.mod,
              aSDLEvent.key.keysym.scancode, aSDLEvent.key.keysym.unicode,
              bShiftKey, bCtrlKey, bAltKey, bMetaKey));

      WebCore::String aText(aSrc);
      WebCore::String aUnmodifiedText(aSrc);
      WebCore::String aKeyIdentifier = keyIdentifierForSDLKeyCode( aSDLEvent.key.keysym.sym );
      BCKeyboardEvent* aKeyboardEvent = new BCKeyboardEvent( aText, aUnmodifiedText, aKeyIdentifier,
        bIsKeyUp, bShiftKey, bCtrlKey, bAltKey, bMetaKey, false/*bAutoRepeat*/, aVKey );
      return aKeyboardEvent;
    }
    case SDL_ACTIVEEVENT: /* Application loses/gains visibility */
    {
      BCWindowEvent* aWindowEvent = new BCWindowEvent( BIWindowEvent::ACTIVE, aSDLEvent.active.gain );
      return aWindowEvent;
    }
    case SDL_VIDEORESIZE: /* User resized video mode */
    {
      logm(MODULE_EVENTS, "Resize event NOT HANDLED");
      //BCWindowEvent* aWindowEvent = new BCWindowEvent( BIWindowEvent::RESIZE,
      //  aSDLEvent.resize.w, aSDLEvent.resize.h );
      return NULL;
    }
    case SDL_VIDEOEXPOSE: /* Screen needs to be redrawn */
    {
      BCWindowEvent* aWindowEvent = new BCWindowEvent(BIWindowEvent::EXPOSE);
      return aWindowEvent;
    }
    case SDL_MOUSEMOTION: 	/* Mouse moved */
    case SDL_MOUSEBUTTONDOWN: /* Mouse button pressed */
    case SDL_MOUSEBUTTONUP: /* Mouse button released */
    {
      // Mouse event
      BIInputEvent* aMouseEvent = CreateMouseorWheelEventFromSDLEvent(aSDLEvent);
      return aMouseEvent;
    }
    case SDL_QUIT: 	/* User-requested quit */
    {
      BCWindowEvent* aWindowEvent = new BCWindowEvent(BIWindowEvent::QUIT);
      return aWindowEvent;
    }
    case SDL_JOYAXISMOTION: /* Joystick axis motion */
    case SDL_JOYBALLMOTION: /* Joystick trackball motion */
    case SDL_JOYHATMOTION: /* Joystick hat position change */
    case SDL_JOYBUTTONDOWN: /* Joystick button pressed */
    case SDL_JOYBUTTONUP: 	/* Joystick button released */
    {
      logm(MODULE_EVENTS, "Joystick NOT HANDLED");
      return NULL;
    }
    case SDL_SYSWMEVENT: 	/* System specific event */
    {
      logm(MODULE_EVENTS, "System specific event NOT HANDLED");
      return NULL;
    }
    default:
    {
      //BCWindowEvent* aWindowEvent = new BCWindowEvent(aKeyIdentifier,
        logm(MODULE_EVENTS, make_message("Unknown event '%d'", aSDLEvent.type));
        return NULL;
    }
  }
#endif
  return NULL;
}


// FIXME: This is incomplete.  We should change this to mirror
// more like what Firefox does, and generate these switch statements
// at build time.
#if 0
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
            return WebCore::String::format("U+%06X", toupper(keyCode));
    }
}

//void BrowserExtensionGdk::setIconURL(const KURL& url){}
static int ConvertSDLKeyToVirtualKey(SDLKey keycode)
{
    switch (keycode) {
        case SDLK_KP0:
            return BAL::BIKeyboardEvent::VK_NUMPAD0;// (60) Numeric keypad 0 key
        case SDLK_KP1:
            return BAL::BIKeyboardEvent::VK_NUMPAD1;// (61) Numeric keypad 1 key
        case SDLK_KP2:
            return  BAL::BIKeyboardEvent::VK_NUMPAD2; // (62) Numeric keypad 2 key
        case SDLK_KP3:
            return BAL::BIKeyboardEvent::VK_NUMPAD3; // (63) Numeric keypad 3 key
        case SDLK_KP4:
            return BAL::BIKeyboardEvent::VK_NUMPAD4; // (64) Numeric keypad 4 key
        case SDLK_KP5:
            return BAL::BIKeyboardEvent::VK_NUMPAD5; //(65) Numeric keypad 5 key
        case SDLK_KP6:
            return BAL::BIKeyboardEvent::VK_NUMPAD6; // (66) Numeric keypad 6 key
        case SDLK_KP7:
            return BAL::BIKeyboardEvent::VK_NUMPAD7; // (67) Numeric keypad 7 key
        case SDLK_KP8:
            return BAL::BIKeyboardEvent::VK_NUMPAD8; // (68) Numeric keypad 8 key
        case SDLK_KP9:
            return BAL::BIKeyboardEvent::VK_NUMPAD9; // (69) Numeric keypad 9 key
        case SDLK_KP_MULTIPLY:
            return BAL::BIKeyboardEvent::VK_MULTIPLY; // (6A) Multiply key
        case SDLK_KP_PLUS:
            return BAL::BIKeyboardEvent::VK_ADD; // (6B) Add key
        case SDLK_KP_MINUS:
            return BAL::BIKeyboardEvent::VK_SUBTRACT; // (6D) Subtract key
        case SDLK_KP_PERIOD:// FIXME not sure of that
            return BAL::BIKeyboardEvent::VK_DECIMAL; // (6E) Decimal key
        case SDLK_KP_DIVIDE:
            return BAL::BIKeyboardEvent::VK_DIVIDE; // (6F) Divide key

        case SDLK_BACKSPACE:
            return BAL::BIKeyboardEvent::VK_BACK; // (08) BACKSPACE key
        case SDLK_TAB:
            return BAL::BIKeyboardEvent::VK_TAB; // (09) TAB key
        case SDLK_CLEAR:
            return BAL::BIKeyboardEvent::VK_CLEAR; // (0C) CLEAR key
        case SDLK_RETURN:
            return BAL::BIKeyboardEvent::VK_RETURN; //(0D) Return key
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            return BAL::BIKeyboardEvent::VK_SHIFT; // (10) SHIFT key
        case SDLK_LCTRL:
        case SDLK_RCTRL:
            return BAL::BIKeyboardEvent::VK_CONTROL; // (11) CTRL key
        case SDLK_MENU:
        case SDLK_LALT:
        case SDLK_RALT:
            return BAL::BIKeyboardEvent::VK_MENU; // (12) ALT key

        case SDLK_PAUSE:
            return BAL::BIKeyboardEvent::VK_PAUSE; // (13) PAUSE key
        case SDLK_CAPSLOCK:
            return BAL::BIKeyboardEvent::VK_CAPITAL; // (14) CAPS LOCK key
/*        case SDLK_KANA_LOCK:
        case SDLK_KANA_SHIFT:
            return BAL::BIKeyboardEvent::VK_KANA; // (15) Input Method Editor (IME) Kana mode
        case SDLK_HANGUL:
            return BAL::BIKeyboardEvent::VK_HANGUL; // BAL::BIKeyboardEvent::VK_HANGUL (15) IME Hangul mode
            // BAL::BIKeyboardEvent::VK_JUNJA (17) IME Junja mode
            // BAL::BIKeyboardEvent::VK_FINAL (18) IME final mode
        case SDLK_HANGUL_HANJA:
            return BAL::BIKeyboardEvent::VK_HANJA; // (19) IME Hanja mode
        case SDLK_KANJI:
            return BAL::BIKeyboardEvent::VK_KANJI; // (19) IME Kanji mode*/
        case SDLK_ESCAPE:
            return BAL::BIKeyboardEvent::VK_ESCAPE; // (1B) ESC key
            // BAL::BIKeyboardEvent::VK_CONVERT (1C) IME convert
            // BAL::BIKeyboardEvent::VK_NONCONVERT (1D) IME nonconvert
            // BAL::BIKeyboardEvent::VK_ACCEPT (1E) IME accept
            // BAL::BIKeyboardEvent::VK_MODECHANGE (1F) IME mode change request
        case SDLK_SPACE:
            return BAL::BIKeyboardEvent::VK_SPACE; // (20) SPACEBAR
        case SDLK_PAGEUP:
            return BAL::BIKeyboardEvent::VK_PRIOR; // (21) PAGE UP key
        case SDLK_PAGEDOWN:
            return BAL::BIKeyboardEvent::VK_NEXT; // (22) PAGE DOWN key
        case SDLK_END:
            return BAL::BIKeyboardEvent::VK_END; // (23) END key
        case SDLK_HOME:
            return BAL::BIKeyboardEvent::VK_HOME; // (24) HOME key
        case SDLK_LEFT:
            return BAL::BIKeyboardEvent::VK_LEFT; // (25) LEFT ARROW key
        case SDLK_UP:
            return BAL::BIKeyboardEvent::VK_UP; // (26) UP ARROW key
        case SDLK_RIGHT:
            return BAL::BIKeyboardEvent::VK_RIGHT; // (27) RIGHT ARROW key
        case SDLK_DOWN:
            return BAL::BIKeyboardEvent::VK_DOWN; // (28) DOWN ARROW key
/*        case SDLK_SELECT:
            return BAL::BIKeyboardEvent::VK_SELECT; // (29) SELECT key*/
        case SDLK_PRINT:
            return BAL::BIKeyboardEvent::VK_PRINT; // (2A) PRINT key
/*        case SDLK_EXECUTE:
            return BAL::BIKeyboardEvent::VK_EXECUTE;// (2B) EXECUTE key*/
            //dunno on this
            //case SDLK_PrintScreen:
            //      return BAL::BIKeyboardEvent::VK_SNAPSHOT; // (2C) PRINT SCREEN key
        case SDLK_INSERT:
            return BAL::BIKeyboardEvent::VK_INSERT; // (2D) INS key
        case SDLK_DELETE:
            return BAL::BIKeyboardEvent::VK_DELETE; // (2E) DEL key
        case SDLK_HELP:
            return BAL::BIKeyboardEvent::VK_HELP; // (2F) HELP key
        case SDLK_0:
        case SDLK_LEFTPAREN:
            return BAL::BIKeyboardEvent::VK_0;    //  (30) 0 ) key
        case SDLK_1:
            return BAL::BIKeyboardEvent::VK_1; //  (31) 1 ! key
        case SDLK_2:
        case SDLK_AT:
            return BAL::BIKeyboardEvent::VK_2; //  (32) 2 & key
        case SDLK_3:
//         case SDLK_NUMBERSIGN:
            return BAL::BIKeyboardEvent::VK_3; //case '3': case '#';
        case SDLK_4:
        case SDLK_DOLLAR: //  (34) 4 key '$';
            return BAL::BIKeyboardEvent::VK_4;
        case SDLK_5:
//        case SDLK_PERCENT:
            return BAL::BIKeyboardEvent::VK_5; //  (35) 5 key  '%'
        case SDLK_6:
/*        case SDLK_ASCIICIRCUM:
            return BAL::BIKeyboardEvent::VK_6; //  (36) 6 key  '^'*/
        case SDLK_7:
            return BAL::BIKeyboardEvent::VK_7; //  (37) 7 key  case '&'
//        case SDLK_AMPERSAND:
//            return BAL::BIKeyboardEvent::VK_AMPERSAND;
        case SDLK_8:
        case SDLK_ASTERISK:
            return BAL::BIKeyboardEvent::VK_8; //  (38) 8 key  '*'
        case SDLK_9:
        case SDLK_RIGHTPAREN:
            return BAL::BIKeyboardEvent::VK_9; //  (39) 9 key '('
        case SDLK_a:
//         case SDLK_A:
            return BAL::BIKeyboardEvent::VK_A; //  (41) A key case 'a': case 'A': return 0x41;
        case SDLK_b:
//         case SDLK_B:
            return BAL::BIKeyboardEvent::VK_B; //  (42) B key case 'b': case 'B': return 0x42;
        case SDLK_c:
//         case SDLK_C:
            return BAL::BIKeyboardEvent::VK_C; //  (43) C key case 'c': case 'C': return 0x43;
        case SDLK_d:
//         case SDLK_D:
            return BAL::BIKeyboardEvent::VK_D; //  (44) D key case 'd': case 'D': return 0x44;
        case SDLK_e:
//         case SDLK_E:
            return BAL::BIKeyboardEvent::VK_E; //  (45) E key case 'e': case 'E': return 0x45;
        case SDLK_f:
//         case SDLK_F:
            return BAL::BIKeyboardEvent::VK_F; //  (46) F key case 'f': case 'F': return 0x46;
        case SDLK_g:
//         case SDLK_G:
            return BAL::BIKeyboardEvent::VK_G; //  (47) G key case 'g': case 'G': return 0x47;
        case SDLK_h:
//         case SDLK_H:
            return BAL::BIKeyboardEvent::VK_H; //  (48) H key case 'h': case 'H': return 0x48;
        case SDLK_i:
//         case SDLK_I:
            return BAL::BIKeyboardEvent::VK_I; //  (49) I key case 'i': case 'I': return 0x49;
        case SDLK_j:
//         case SDLK_J:
            return BAL::BIKeyboardEvent::VK_J; //  (4A) J key case 'j': case 'J': return 0x4A;
        case SDLK_k:
//         case SDLK_K:
            return BAL::BIKeyboardEvent::VK_K; //  (4B) K key case 'k': case 'K': return 0x4B;
        case SDLK_l:
//         case SDLK_L:
            return BAL::BIKeyboardEvent::VK_L; //  (4C) L key case 'l': case 'L': return 0x4C;
        case SDLK_m:
//         case SDLK_M:
            return BAL::BIKeyboardEvent::VK_M; //  (4D) M key case 'm': case 'M': return 0x4D;
        case SDLK_n:
//         case SDLK_N:
            return BAL::BIKeyboardEvent::VK_N; //  (4E) N key case 'n': case 'N': return 0x4E;
        case SDLK_o:
//         case SDLK_O:
            return BAL::BIKeyboardEvent::VK_O; //  (4F) O key case 'o': case 'O': return 0x4F;
        case SDLK_p:
//         case SDLK_P:
            return BAL::BIKeyboardEvent::VK_P; //  (50) P key case 'p': case 'P': return 0x50;
        case SDLK_q:
//         case SDLK_Q:
            return BAL::BIKeyboardEvent::VK_Q; //  (51) Q key case 'q': case 'Q': return 0x51;
        case SDLK_r:
//         case SDLK_R:
            return BAL::BIKeyboardEvent::VK_R; //  (52) R key case 'r': case 'R': return 0x52;
        case SDLK_s:
//         case SDLK_S:
            return BAL::BIKeyboardEvent::VK_S; //  (53) S key case 's': case 'S': return 0x53;
        case SDLK_t:
//         case SDLK_T:
            return BAL::BIKeyboardEvent::VK_T; //  (54) T key case 't': case 'T': return 0x54;
        case SDLK_u:
//         case SDLK_U:
            return BAL::BIKeyboardEvent::VK_U; //  (55) U key case 'u': case 'U': return 0x55;
        case SDLK_v:
//         case SDLK_V:
            return BAL::BIKeyboardEvent::VK_V; //  (56) V key case 'v': case 'V': return 0x56;
        case SDLK_w:
//         case SDLK_W:
            return BAL::BIKeyboardEvent::VK_W; //  (57) W key case 'w': case 'W': return 0x57;
        case SDLK_x:
//         case SDLK_X:
            return BAL::BIKeyboardEvent::VK_X; //  (58) X key case 'x': case 'X': return 0x58;
        case SDLK_y:
//         case SDLK_Y:
            return BAL::BIKeyboardEvent::VK_Y; //  (59) Y key case 'y': case 'Y': return 0x59;
        case SDLK_z:
//         case SDLK_Z:
            return BAL::BIKeyboardEvent::VK_Z; //  (5A) Z key case 'z': case 'Z': return 0x5A;
        case SDLK_LMETA:
            return BAL::BIKeyboardEvent::VK_LWIN; // (5B) Left Windows key (Microsoft Natural keyboard)
        case SDLK_RMETA:
            return BAL::BIKeyboardEvent::VK_RWIN; // (5C) Right Windows key (Natural keyboard)
            // BAL::BIKeyboardEvent::VK_APPS (5D) Applications key (Natural keyboard)
            // BAL::BIKeyboardEvent::VK_SLEEP (5F) Computer Sleep key
            // BAL::BIKeyboardEvent::VK_SEPARATOR (6C) Separator key
            // BAL::BIKeyboardEvent::VK_SUBTRACT (6D) Subtract key
            // BAL::BIKeyboardEvent::VK_DECIMAL (6E) Decimal key
            // BAL::BIKeyboardEvent::VK_DIVIDE (6F) Divide key
            // handled by key code above

        case SDLK_NUMLOCK:
            return BAL::BIKeyboardEvent::VK_NUMLOCK; // (90) NUM LOCK key

        case SDLK_SCROLLOCK:
            return BAL::BIKeyboardEvent::VK_SCROLL; // (91) SCROLL LOCK key

            // BAL::BIKeyboardEvent::VK_LSHIFT (A0) Left SHIFT key
            // BAL::BIKeyboardEvent::VK_RSHIFT (A1) Right SHIFT key
            // BAL::BIKeyboardEvent::VK_LCONTROL (A2) Left CONTROL key
            // BAL::BIKeyboardEvent::VK_RCONTROL (A3) Right CONTROL key
            // BAL::BIKeyboardEvent::VK_LMENU (A4) Left MENU key
            // BAL::BIKeyboardEvent::VK_RMENU (A5) Right MENU key
            // BAL::BIKeyboardEvent::VK_BROWSER_BACK (A6) Windows 2000/XP: Browser Back key
            // BAL::BIKeyboardEvent::VK_BROWSER_FORWARD (A7) Windows 2000/XP: Browser Forward key
            // BAL::BIKeyboardEvent::VK_BROWSER_REFRESH (A8) Windows 2000/XP: Browser Refresh key
            // BAL::BIKeyboardEvent::VK_BROWSER_STOP (A9) Windows 2000/XP: Browser Stop key
            // BAL::BIKeyboardEvent::VK_BROWSER_SEARCH (AA) Windows 2000/XP: Browser Search key
            // BAL::BIKeyboardEvent::VK_BROWSER_FAVORITES (AB) Windows 2000/XP: Browser Favorites key
            // BAL::BIKeyboardEvent::VK_BROWSER_HOME (AC) Windows 2000/XP: Browser Start and Home key
            // BAL::BIKeyboardEvent::VK_VOLUME_MUTE (AD) Windows 2000/XP: Volume Mute key
            // BAL::BIKeyboardEvent::VK_VOLUME_DOWN (AE) Windows 2000/XP: Volume Down key
            // BAL::BIKeyboardEvent::VK_VOLUME_UP (AF) Windows 2000/XP: Volume Up key
            // BAL::BIKeyboardEvent::VK_MEDIA_NEXT_TRACK (B0) Windows 2000/XP: Next Track key
            // BAL::BIKeyboardEvent::VK_MEDIA_PREV_TRACK (B1) Windows 2000/XP: Previous Track key
            // BAL::BIKeyboardEvent::VK_MEDIA_STOP (B2) Windows 2000/XP: Stop Media key
            // BAL::BIKeyboardEvent::VK_MEDIA_PLAY_PAUSE (B3) Windows 2000/XP: Play/Pause Media key
            // BAL::BIKeyboardEvent::VK_LAUNCH_MAIL (B4) Windows 2000/XP: Start Mail key
            // BAL::BIKeyboardEvent::VK_LAUNCH_MEDIA_SELECT (B5) Windows 2000/XP: Select Media key
            // BAL::BIKeyboardEvent::VK_LAUNCH_APP1 (B6) Windows 2000/XP: Start Application 1 key
            // BAL::BIKeyboardEvent::VK_LAUNCH_APP2 (B7) Windows 2000/XP: Start Application 2 key

            // BAL::BIKeyboardEvent::VK_OEM_1 (BA) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ';:' key
        case SDLK_SEMICOLON:
        case SDLK_COLON:
            return BAL::BIKeyboardEvent::VK_OEM_1; //case ';': case ':': return 0xBA;
            // BAL::BIKeyboardEvent::VK_OEM_PLUS (BB) Windows 2000/XP: For any country/region, the '+' key
        case SDLK_PLUS:
        case SDLK_EQUALS:
            return BAL::BIKeyboardEvent::VK_OEM_PLUS; //case '=': case '+': return 0xBB;
            // BAL::BIKeyboardEvent::VK_OEM_COMMA (BC) Windows 2000/XP: For any country/region, the ',' key
        case SDLK_COMMA:
        case SDLK_LESS:
            return BAL::BIKeyboardEvent::VK_OEM_COMMA; //case ',': case '<': return 0xBC;
            // BAL::BIKeyboardEvent::VK_OEM_MINUS (BD) Windows 2000/XP: For any country/region, the '-' key
        case SDLK_MINUS:
        case SDLK_UNDERSCORE:
            return BAL::BIKeyboardEvent::VK_OEM_MINUS; //case '-': case '_': return 0xBD;
            // BAL::BIKeyboardEvent::VK_OEM_PERIOD (BE) Windows 2000/XP: For any country/region, the '.' key
        case SDLK_PERIOD:
        case SDLK_GREATER:
            return BAL::BIKeyboardEvent::VK_OEM_PERIOD; //case '.': case '>': return 0xBE;
            // BAL::BIKeyboardEvent::VK_OEM_2 (BF) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '/?' key
        case SDLK_SLASH:
        case SDLK_QUESTION:
            return BAL::BIKeyboardEvent::VK_OEM_2; //case '/': case '?': return 0xBF;
            // BAL::BIKeyboardEvent::VK_OEM_3 (C0) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '`~' key
//        case SDLK_ASCIITILDE:
//         case SDLK_QUOTELEFT:
//             return BAL::BIKeyboardEvent::VK_OEM_3; //case '`': case '~': return 0xC0;
            // BAL::BIKeyboardEvent::VK_OEM_4 (DB) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '[{' key
/*        case SDLK_BRACKETLEFT:
        case SDLK_BRACELEFT:
            return BAL::BIKeyboardEvent::VK_OEM_4; //case '[': case '{': return 0xDB;*/
            // BAL::BIKeyboardEvent::VK_OEM_5 (DC) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '\|' key
        case SDLK_BACKSLASH:
//         case SDLK_BAR:
            return BAL::BIKeyboardEvent::VK_OEM_5; //case '\\': case '|': return 0xDC;
            // BAL::BIKeyboardEvent::VK_OEM_6 (DD) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ']}' key
/*        case SDLK_BRACKETRIGHT:
        case SDLK_BRACERIGHT:
            return BAL::BIKeyboardEvent::VK_OEM_6; // case ']': case '}': return 0xDD;*/
            // BAL::BIKeyboardEvent::VK_OEM_7 (DE) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key
//         case SDLK_QUOTERIGHT:
        case SDLK_QUOTEDBL:
            return BAL::BIKeyboardEvent::VK_OEM_7; // case '\'': case '"': return 0xDE;
            // BAL::BIKeyboardEvent::VK_OEM_8 (DF) Used for miscellaneous characters; it can vary by keyboard.
            // BAL::BIKeyboardEvent::VK_OEM_102 (E2) Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
            // BAL::BIKeyboardEvent::VK_PROCESSKEY (E5) Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key
            // BAL::BIKeyboardEvent::VK_PACKET (E7) Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. The BAL::BIKeyboardEvent::VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT,SendInput, WM_KEYDOWN, and WM_KEYUP
            // BAL::BIKeyboardEvent::VK_ATTN (F6) Attn key
            // BAL::BIKeyboardEvent::VK_CRSEL (F7) CrSel key
            // BAL::BIKeyboardEvent::VK_EXSEL (F8) ExSel key
            // BAL::BIKeyboardEvent::VK_EREOF (F9) Erase EOF key
            // BAL::BIKeyboardEvent::VK_PLAY (FA) Play key
            // BAL::BIKeyboardEvent::VK_ZOOM (FB) Zoom key
            // BAL::BIKeyboardEvent::VK_NONAME (FC) Reserved for future use
            // BAL::BIKeyboardEvent::VK_PA1 (FD) PA1 key
            // BAL::BIKeyboardEvent::VK_OEM_CLEAR (FE) Clear key
        case SDLK_F1:
            return BAL::BIKeyboardEvent::VK_F1;
        case SDLK_F2:
            return BAL::BIKeyboardEvent::VK_F2;
        case SDLK_F3:
            return BAL::BIKeyboardEvent::VK_F3;
        case SDLK_F4:
            return BAL::BIKeyboardEvent::VK_F4;
        case SDLK_F5:
            return BAL::BIKeyboardEvent::VK_F5;
        case SDLK_F6:
            return BAL::BIKeyboardEvent::VK_F6;
        case SDLK_F7:
            return BAL::BIKeyboardEvent::VK_F7;
        case SDLK_F8:
            return BAL::BIKeyboardEvent::VK_F8;
        case SDLK_F9:
            return BAL::BIKeyboardEvent::VK_F9;
        case SDLK_F10:
            return BAL::BIKeyboardEvent::VK_F10;
        case SDLK_F11:
            return BAL::BIKeyboardEvent::VK_F11;
        case SDLK_F12:
            return BAL::BIKeyboardEvent::VK_F12;
        /*case SDLK_MODE:
            return BAL::BIKeyboardEvent::VK_ALTGR;*/
        case SDLK_LSUPER:
            return BAL::BIKeyboardEvent::VK_LWIN;
        case SDLK_RSUPER:
            return BAL::BIKeyboardEvent::VK_RWIN;
        default:
            logm(MODULE_EVENTS, "ARGH!!! No virtual key!!!");
            return 0;
    }
}
#endif

// in your main event loop (following the way you handle mouse buttons events)
/*BIInputEvent* CreateMouseorWheelEventFromSDLEvent( const SDL_Event& aSDLEvent )
{
  switch( aSDLEvent.type )
  {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    {
      const SDL_MouseButtonEvent& aMouseButtonEvent = aSDLEvent.button;
      // state
      BIMouseEvent::MouseEventType type = (aSDLEvent.type == SDL_MOUSEBUTTONDOWN) ?
        BIMouseEvent::MouseEventPressed : BIMouseEvent::MouseEventReleased;

      // Position
      IntPoint aPos( aMouseButtonEvent.x, aMouseButtonEvent.y );
      IntPoint aGlobalPos( aMouseButtonEvent.x, aMouseButtonEvent.y ); // FIXME RME not global yet
      // Button
      BIMouseEvent::MouseButton aMouseButton = BIMouseEvent::LeftButton;
      int delta = 0;
      // do not use SDL_BUTTON(X) : (1 << ((X)-1)
      if( aMouseButtonEvent.button == SDL_BUTTON_LEFT ) {
        aMouseButton = BIMouseEvent::LeftButton;
      } else if( aMouseButtonEvent.button == SDL_BUTTON_MIDDLE ) {
        aMouseButton = BIMouseEvent::MiddleButton;
      } else if( aMouseButtonEvent.button == SDL_BUTTON_RIGHT ) {
          aMouseButton = BIMouseEvent::RightButton;
      } else if( aMouseButtonEvent.button == SDL_BUTTON_WHEELUP ) {
          delta = 15;
      } else if( aMouseButtonEvent.button == SDL_BUTTON_WHEELDOWN ) {
          delta = -15;
      } else {
        logm(MODULE_EVENTS, "Error unknown mouse button");
      }
      logm(MODULE_EVENTS, make_message("MouseButton (%d) X=%d Y=%d\n", aMouseButtonEvent.button,
          aPos.x(), aPos.y()));
      // Click count FIXME RME
      // Hot keys FIXME RME
      if (delta != 0 && type == BIMouseEvent::MouseEventPressed) {
          // don't generate 2 events for only one wheel event, so select mouse press
          BC::BCWheelEvent* aWheelEvent = new BC::BCWheelEvent( aPos, aGlobalPos,
                                             0, delta, false,
                                             false, false, false, false );
          return aWheelEvent;
      }
      else {
            BC::BCMouseEvent* aMouseEvent = new BC::BCMouseEvent(
                    type, aPos, aGlobalPos, aMouseButton, 1,
                    false, false, false, false);
            return aMouseEvent;
      }
    }
    case SDL_MOUSEMOTION:
    {
      const SDL_MouseMotionEvent& aMouseButtonEvent = aSDLEvent.motion;
      // Position
      IntPoint aPos(aMouseButtonEvent.x, aMouseButtonEvent.y);
      IntPoint aGlobalPos(aMouseButtonEvent.x, aMouseButtonEvent.y); // FIXME RME not global yet
      // Click count FIXME RME
      // Hot keys FIXME RME
      BC::BCMouseEvent* aMouseEvent = new BC::BCMouseEvent(BIMouseEvent::MouseEventMoved, aPos, aGlobalPos,
        BIMouseEvent::MouseButton(-1), 0, false, false, false, false);

      return aMouseEvent;
    }
  }
  return NULL;
}*/
