/*
 * Copyright (C) 2008 Joerg Strohmayer
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
#include <sys/time.h>
#include <stdio.h>
#include "Platform.h"

#include DEEPSEE_INCLUDE

#include <proto/exec.h>
#include <devices/timer.h>

namespace OWBAL {

void (*sharedTimerFiredFunction)() = NULL;

// Single timer, shared to implement all the timers managed by the Timer class.
// Not intended to be used directly; use the Timer class instead.

static uint32 m_count = 0;

asm("\n\
AtomicIncrease:\n\
   lwarx   4, 0, 3\n\
   addi    5, 4, 1\n\
   stwcx.  5, 0, 3\n\
   bne-    AtomicIncrease\n\
   isync\n\
   blr\n\
\n\
AtomicTestDecrease:\n\
   lwarx   4, 0, 3\n\
   bne     1f\n\
   subi    5, 4, 1\n\
   stwcx.  5, 0, 3\n\
   bne-    AtomicTestDecrease\n\
1: isync\n\
   mr      3, 4\n\
   blr");

extern "C"
{
   void AtomicIncrease(uint32 *);
   uint32 AtomicTestDecrease(uint32 *);
};

void incrementTimerCount()
{
   AtomicIncrease(&m_count);
}

void fireTimerIfNeeded()
{
   if (AtomicTestDecrease(&m_count) > 0)
      sharedTimerFiredFunction();
}

static uint32 timerint_func(struct ExceptionContext *Context, struct ExecBase *SysBase, void *UserData)
{
   if (sharedTimerFiredFunction)
      incrementTimerCount();
   return 0;
}

static struct MsgPort *mp = NULL;
static struct TimeRequest *timereq = NULL;
static struct Interrupt timerint = { { NULL, NULL, NT_UNKNOWN, 0, (char *)"OWB shared timer int" }, NULL, (void (*)(void)) timerint_func };

static void cleanup(void)
{
   IExec->AbortIO((struct IORequest *)timereq);
   IExec->CloseDevice((struct IORequest *)timereq);
   IExec->FreeSysObject(ASOT_IOREQUEST, timereq);
   IExec->FreeSysObject(ASOT_PORT, mp);
}

void setSharedTimerFiredFunction(void (*f)()) 
{
    if ( NULL == sharedTimerFiredFunction ) {
        sharedTimerFiredFunction = f;

        mp = (struct MsgPort *)IExec->AllocSysObjectTags(ASOT_PORT,
                                                         ASOPORT_Action, PA_SOFTINT,
                                                         ASOPORT_AllocSig, FALSE,
                                                         ASOPORT_Target, &timerint,
                                                         TAG_DONE);
        if (mp) {
           timereq = (struct TimeRequest *)IExec->AllocSysObjectTags(ASOT_IOREQUEST,
                                                                     ASOIOR_Size, sizeof(struct TimeRequest),
                                                                     ASOIOR_ReplyPort, mp,
                                                                     TAG_DONE);
           if (timereq) {
               if (0 == IExec->OpenDevice("timer.device", UNIT_MICROHZ, (struct IORequest *)timereq, 0)) {
                   timereq->Request.io_Command = TR_ADDREQUEST;
                   timereq->Time.Seconds = 1;
                   timereq->Time.Microseconds = 0;

                   atexit(cleanup);
                   return;
               }
               IExec->FreeSysObject(ASOT_IOREQUEST, timereq);
               timereq = NULL;
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

    double interval = fireTime - currentTime();
    uint32 intervalInUS = 0;
    if (interval > 0) {
        interval = interval * 1000 * 1000;
        intervalInUS = static_cast<uint32>(interval);
    }

    if (intervalInUS < 10 * 1000) // min. time 1/100 sec.
        intervalInUS = 10 * 1000;

    stopSharedTimer();

    timereq->Time.Seconds = intervalInUS / (1000 * 1000);
    timereq->Time.Microseconds = intervalInUS % (1000 * 1000);
    IExec->SendIO((struct IORequest *)timereq);
}

void stopSharedTimer() 
{
    IExec->AbortIO((struct IORequest *)timereq);
}

}
