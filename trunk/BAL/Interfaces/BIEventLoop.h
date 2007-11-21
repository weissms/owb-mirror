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
 * @file BIEventLoop.h
 *
 * BIEvent handler and main loop
 */
#ifndef BIEVENTLOOP_H_
#define BIEVENTLOOP_H_

namespace BAL {

    class BIEvent;

    /**
    * @brief the BIEventLoop
    *
    * The event, base class of all events
    *
    * @see BIEvent
    */
    class BIEventLoop {
        public:
        // this is mandatory
        /**
        * BIEventLoop destructor
        */
        virtual ~BIEventLoop() {};

        /**
         * isInitialized returns true if event loop is correctly initialized
         * (initialization is performed in the constructor)
         */
        virtual bool isInitialized() const = 0;

        /**
         * Post a quit event
         */
        virtual void quit() = 0;
            
        /**
         * WaitEvent wait for an event, and returns when a new event is available.
         * If loop can goes on, it return true. Else false.
         */
            virtual bool WaitEvent(BIEvent*&) = 0;

        /**
         * Put an event in the event loop.
         */
            virtual bool PushEvent(BIEvent*) = 0;
    };
}

#endif // BIEVENTLOOP_H

