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
#ifndef BIWINDOWEVENT_H_
#define BIWINDOWEVENT_H_

#include "BIEvent.h"

namespace WebCore {
class IntRect;
}

namespace BAL {
    class BTWidget;

    /**
    * @brief the BIWindowEvent
    *
    * The window event. Manages events like expose, resize (not yet), etc.
    *
    * @see BIEvent, BIEventLoop, BIKeyboardEvent, BIMouseEvent, BIWheelEvent
    */
    class BIWindowEvent : public BIEvent {
        public:
            // this is mandatory
            /**
            * BIWindowEvent destructor
            */
            virtual ~BIWindowEvent() {};

            /**
            * BIWindowEvent clone
            */
            virtual BIEvent* clone() const = 0;

            enum ENUM_WINDOW { EXPOSE, ACTIVE, QUIT, REDRAW, RESIZE };

            /**
            * return window type
            */
            virtual BIWindowEvent::ENUM_WINDOW type() const = 0;
            /**
            * test if the window is exposed
            */
            virtual bool gain() const = 0;
            /**
            * get the window rectangle
            */
            virtual const WebCore::IntRect& getRectangle() const = 0;
            /**
            * get widget associated
            */
            virtual const BTWidget* widget() const = 0;

            virtual BIWindowEvent* queryIsWindowEvent() { return this; };
    };

}

#endif // BIWINDOWEVENT_H
