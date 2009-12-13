/*
 * Copyright (C) 2009 Joerg Strohmayer
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
#include <sys/time.h>
#include <stdio.h>
#include "Platform.h"
#include "BALBase.h"

#include <proto/exec.h>
#include <devices/timer.h>

extern Task *mainTask;
extern uint32 sharedTimerSignal;

namespace WebCore {

void (*sharedTimerFiredFunction)() = 0;
static unsigned int m_count = 0;

// Single timer, shared to implement all the timers managed by the Timer class.
// Not intended to be used directly; use the Timer class instead.

void fireTimerIfNeeded()
{
    sharedTimerFiredFunction();
}

static uint32 timerint_func(ExceptionContext* Context, ExecBase* SysBase, void* UserData)
{
    if (sharedTimerFiredFunction)
        IExec->Signal(mainTask, sharedTimerSignal);
    return 0;
}

static MsgPort* mp = 0;
static TimeRequest* timereq = 0;
static Interrupt timerint = { { 0, 0, NT_UNKNOWN, 0, (char*)"OWB shared timer int" }, 0, (void (*)(void)) timerint_func };

static void cleanup(void)
{
    IExec->AbortIO(&timereq->Request);
    IExec->CloseDevice(&timereq->Request);
    IExec->FreeSysObject(ASOT_IOREQUEST, timereq);
    IExec->FreeSysObject(ASOT_PORT, mp);
}

void setSharedTimerFiredFunction(void (*f)()) 
{
    if ( 0 == sharedTimerFiredFunction ) {
        sharedTimerFiredFunction = f;

        mp = (MsgPort*)IExec->AllocSysObjectTags(ASOT_PORT,
                                                 ASOPORT_Action, PA_SOFTINT,
                                                 ASOPORT_AllocSig, FALSE,
                                                 ASOPORT_Target, &timerint,
                                                 TAG_DONE);
        if (mp) {
            timereq = (TimeRequest*)IExec->AllocSysObjectTags(ASOT_IOREQUEST,
                                                              ASOIOR_Size, sizeof(struct TimeRequest),
                                                              ASOIOR_ReplyPort, mp,
                                                              TAG_DONE);
            if (timereq) {
                if (0 == IExec->OpenDevice("timer.device", UNIT_MICROHZ, &timereq->Request, 0)) {
                    timereq->Request.io_Command = TR_ADDREQUEST;
                    timereq->Time.Seconds = 1;
                    timereq->Time.Microseconds = 0;
                    IExec->SendIO(&timereq->Request);

                    atexit(cleanup);
                    return;
                }
                IExec->FreeSysObject(ASOT_IOREQUEST, timereq);
                timereq = 0;
            }
            IExec->FreeSysObject(ASOT_PORT, mp);
        }
        fprintf(stderr, "couldn't create shared timer\n");
        exit(1);
    }
}

// The fire time is relative to the classic POSIX epoch of January 1, 1970,
// as the result of currentTime() is.
void setSharedTimerFireTime(double fireTime) 
{
    assert(sharedTimerFiredFunction);

    stopSharedTimer();

    double interval = fireTime - currentTime();

    if ( interval <= 0 )
        interval = 0.000001;

    timereq->Time.Seconds = (uint32)interval;
    timereq->Time.Microseconds = (uint32)((interval-(uint32)interval)*1000000.0);
#if 0
    if (0 == timereq->Time.Seconds && 0 == timereq->Time.Microseconds)
        timereq->Time.Microseconds = 1;
    IExec->SendIO(&timereq->Request);
#else
    if (0 == timereq->Time.Seconds && timereq->Time.Microseconds < 10)
        IExec->Signal(mainTask, sharedTimerSignal);
    else
        IExec->SendIO(&timereq->Request);
#endif
}

void stopSharedTimer() 
{
    IExec->AbortIO(&timereq->Request);
}

}
