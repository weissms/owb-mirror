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
#include "config.h"
#include "SharedTimer.h"

#include "CurrentTime.h"
#include <wtf/Assertions.h>
#include <assert.h>
#include "Platform.h"
#include <stdio.h>

namespace WebCore {

void (*sharedTimerFiredFunction)() = NULL;
void (*incrementTimerCountCallback)() = NULL;


#if !PLATFORM(GTK)
static unsigned int m_count = 0;
void incrementTimerCount()
{
    m_count++;
    if (incrementTimerCountCallback)
        incrementTimerCountCallback();
}

int fireTimerCount()
{
    return m_count;
}

void fireTimerIfNeeded()
{
    if (m_count>0) {
        m_count--;
        sharedTimerFiredFunction();
    }
}
#endif

void catcher( int sig ) 
{
    if( sharedTimerFiredFunction ) {
#if !PLATFORM(GTK)
        incrementTimerCount();
#else
        sharedTimerFiredFunction();
#endif
    }
    else {
        fprintf(stderr, "no sharedTimerFiredFunction\n");
    }

}

void setSharedTimerFiredFunction(void (*f)()) 
{
    if(!sharedTimerFiredFunction)
        sharedTimerFiredFunction = f;
}

void setIncrementTimerCountCallback(void(*f)())
{
    if (incrementTimerCountCallback == NULL)
        incrementTimerCountCallback = f;
}

// The fire time is relative to the classic POSIX epoch of January 1, 1970,
// as the result of currentTime() is.
void setSharedTimerFireTime(double fireTime) 
{
    assert(sharedTimerFiredFunction);

    incrementTimerCount();
}

void stopSharedTimer() 
{
    if (m_count)
        m_count--;
}

}
