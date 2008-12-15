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

#include "SystemTime.h"
#include <wtf/Assertions.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include "Platform.h"

namespace OWBAL {

static struct itimerval itimer;
/* Our SIGALRM signal handler */
static struct sigaction sa;

void (*sharedTimerFiredFunction)() = NULL;

// Single timer, shared to implement all the timers managed by the Timer class.
// Not intended to be used directly; use the Timer class instead.

#if !PLATFORM(GTK)
/**
 * Macro to use to protect code from firing timers.
 * This ensures that there is no concurrent access on ressource and prevent deadlocks.
 */
#define ENTER_CRITICAL_SECTION sigset_t newmask, oldmask, zeromask; \
                                sigemptyset(&newmask); sigemptyset(&zeromask); \
                                sigaddset(&newmask, SIGALRM); \
                                sigprocmask(SIG_BLOCK, &newmask, &oldmask);

#define LEAVE_CRITICAL_SECTION sigprocmask(SIG_SETMASK, &oldmask, NULL);


static unsigned int m_count = 0;
/**
 * We absolutely need to protect these 2 functions in a critical section.
 * Signals/timers will increment count, event loop will decrement it: 2 concurrent accesses.
 * Increment is usually done in 3 machine instructions, so there's room for a concurrent access.
 * See ticket #123. Maybe we should put timers in a separate thread to be able to use mutexes.
 */
void incrementTimerCount()
{
    ENTER_CRITICAL_SECTION;
    m_count++;
    LEAVE_CRITICAL_SECTION;
}

void fireTimerIfNeeded()
{
    ENTER_CRITICAL_SECTION;
    if (m_count>0) {
        m_count--;
        sharedTimerFiredFunction();
    }
    LEAVE_CRITICAL_SECTION;
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
    if( sharedTimerFiredFunction == NULL )
    {
        sharedTimerFiredFunction = f;
        /* Install our signal handler */
        sa.sa_handler = catcher;
        sigemptyset( &sa.sa_mask );
        sa.sa_flags = SA_RESTART|SA_SIGINFO;
        /*use SIGVTALRM if  ITIMER_VIRTUAL */
        if (sigaction (SIGALRM, &sa, 0) == -1) {
            perror("sigaction");
            exit(1);
        }
    }
}

// The fire time is relative to the classic POSIX epoch of January 1, 1970,
// as the result of currentTime() is.
void setSharedTimerFireTime(double fireTime) 
{
    assert(sharedTimerFiredFunction);

    double interval = fireTime - currentTime();

    if( interval <= 0 )
        interval = 0.000001;

    /* Request SIGALRM */
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = (long)interval;
    itimer.it_value.tv_usec = (long)((interval-(long)interval)*1000000.0);
    //FIXME: must not be zero or does not fire on all kernels
    if( itimer.it_value.tv_sec == 0 && itimer.it_value.tv_usec == 0 )
        itimer.it_value.tv_usec = 1;

    /*modes ITIMER_PROF ITIMER_REAL ITIMER_VIRTUAL*/
    if (-1 == setitimer(ITIMER_REAL, &itimer, NULL))
        perror("setSharedTimerFireTime");
}

void stopSharedTimer() 
{
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 0;

    if (-1 == setitimer(ITIMER_REAL, &itimer, NULL))
        perror("can't stopSharedTimer");

    /* Install our signal handler */
    sa.sa_handler = NULL;
    sigemptyset(&sa.sa_mask);

    if (sigaction (SIGALRM, 0, 0) == -1) {
        perror("sigaction");
        exit(1);
    }
}

}
