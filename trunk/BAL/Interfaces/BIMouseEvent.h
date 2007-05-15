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
 * @file BIMouseEvent.h
 *
 * Interface of Mouse Event
 */
#ifndef BIMouseEvent_h
#define BIMouseEvent_h

#include "IntPoint.h"
#include "BIInputEvent.h"
using WebCore::IntPoint;

namespace BAL {

    /**
    * @brief the BIMouseEvent
    *
    * The mouse event (buttons, mouse motion). wheel is managed with
    * wheel events.
    *
    * @see BIEvent, BIKeyboardEvent, BIWheelEvent
    */
    class BIMouseEvent : public BIInputEvent {
    public:
        static const struct CurrentEventTag {} currentEvent;

        virtual ~BIMouseEvent() {};

        // These button numbers match the ones used in the DOM API, 0 through 2, except for NoButton which isn't specified.
        enum MouseButton { NoButton = -1, LeftButton, MiddleButton, RightButton };
        enum MouseEventType { MouseEventMoved, MouseEventPressed, MouseEventReleased, MouseEventScroll };

        // clone the event
        virtual BIEvent* clone() const = 0;

        // position in window
        virtual const IntPoint& pos() const = 0;
        // global position
        virtual const IntPoint& globalPos() const = 0;
        // the pressed or release button
        virtual MouseButton button() const = 0;
        virtual MouseEventType eventType() const = 0;
        // click count 1=simple click, 2=double click 3=triple click
        virtual int clickCount() const = 0;
	// time in seconds
        virtual double timestamp() const = 0;

        virtual BIKeyboardEvent* queryIsKeyboardEvent() { return NULL; }
        virtual BIMouseEvent* queryIsMouseEvent() { return this; }
        virtual BIWheelEvent* queryIsWheelEvent() { return NULL; }
    };

}

using BAL::BIMouseEvent;

#endif
