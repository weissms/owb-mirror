/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Copyright (C) 2007 Pleyo
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file SharedTimerBal.cpp
 *
 * SharedTimers implemented on SDL (add a thread)
 */
#include "config.h"
#include "SharedTimer.h"
#include "SystemTime.h"
#include <wtf/Assertions.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include "BALConfiguration.h"
#include "BTLogHelper.h"
#include <assert.h>

namespace WebCore {

static SDL_TimerID timerId = 0;
void (*sharedTimerFiredFunction)() = NULL;

// Single timer, shared to implement all the timers managed by the Timer class.
// Not intended to be used directly; use the Timer class instead.

void setSharedTimerFiredFunction(void (*f)()) {
    if( sharedTimerFiredFunction == NULL )
    {
      sharedTimerFiredFunction = f;
        if (SDL_InitSubSystem(SDL_INIT_TIMER) < 0) {
            DBGML(MODULE_EVENTS, LEVEL_CRITICAL, "Unable to init SDL: %s\n", SDL_GetError());
            exit(1);
        }
    }
}


static unsigned int m_count = 0;
/**
 * We absolutely need to protect these 2 functions in a critical section.
 * Signals/timers will increment count, event loop will decrement it: 2 concurrent accesses.
 * Increment is usually done in 3 machine instructions, so there's room for a concurrent access.
 * See ticket #123. Maybe we should put timers in a separate thread to be able to use mutexes.
 */
void incrementTimerCount()
{
    m_count++;
}
void fireTimerIfNeeded()
{
    if (m_count>0) {
            m_count--;
            sharedTimerFiredFunction();
    }
}

Uint32 sdlTimeoutCallback(Uint32 interval, void *param) {
    if( sharedTimerFiredFunction ) {
        // the main thread must call sharedTimerFiredFunction
        // not the sdl thread for timers
        incrementTimerCount();
    }
    // 0 cancels the repetition of the callback
    return 0;
}

// The fire time is relative to the classic POSIX epoch of January 1, 1970,
// as the result of currentTime() is.
void setSharedTimerFireTime(double fireTime) {
    ASSERT(sharedTimerFiredFunction);

    double interval = fireTime - currentTime();
    Uint32 intervalInMS = 1; // shouldn't be 0
    if (interval > 0) {
        interval = interval * 1000;
        intervalInMS = static_cast<Uint32>(interval);
        if (intervalInMS == 0)
            intervalInMS = 1;
    }

    stopSharedTimer();

    timerId = SDL_AddTimer(intervalInMS, sdlTimeoutCallback, 0);
    // Do not put method call in assert ! Because it won't work in release
    assert(timerId);
}

void stopSharedTimer() {
    if (timerId) {
        //ASSERT(SDL_RemoveTimer(timerId)); // XXX FIXME SRO fails everytime !
        if (!SDL_RemoveTimer(timerId))
            ;
        timerId = 0;
    }
}

}
