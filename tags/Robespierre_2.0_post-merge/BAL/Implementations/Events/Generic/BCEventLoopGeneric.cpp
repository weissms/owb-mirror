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
 * @file  BCEventLoopGeneric.cpp
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

#include "BCEventLoopGeneric.h"
#include "BCKeyboardEvent.h"
#include "BCWindowEvent.h"
#include "BCMouseEvent.h"
#include "BCWheelEvent.h"
//#include "BIInternationalization.h"
#include "SharedTimer.h"
#include "BTLogHelper.h"

using namespace BAL;

namespace BAL {
static BC::BCEventLoopGeneric* gMainLoop = NULL;

    BIEventLoop* getBIEventLoop() {
        if( gMainLoop == NULL )
            gMainLoop = new BC::BCEventLoopGeneric();

        return gMainLoop;
    }

    void deleteBIEventLoop() {
        delete gMainLoop;
        gMainLoop = 0;
  }
}

/**
 * Constructor of Event Loop
 *
 * Construcs and set the m_initialized flag
 *
 */
BC::BCEventLoopGeneric::BCEventLoopGeneric()
    : m_initialized(false)
    , m_event(0)
{
    WebCore::stopSharedTimer();
    m_initialized = true;
}

/**
 * Destructor
 *
 */
BC::BCEventLoopGeneric::~BCEventLoopGeneric()
{
    WebCore::stopSharedTimer();
    for(unsigned int i=0; i < m_event.size(); i++)
        delete m_event[i];
    m_event.clear();
}

/**
 * isInitialized
 *
 * returns true if event loop correctly initialized
 *
 */
bool BC::BCEventLoopGeneric::isInitialized() const
{
  return m_initialized;
}

/**
 * quit() posts an event in event loop to quit
 *
 */
void BC::BCEventLoopGeneric::quit()
{
    BCWindowEvent* aWindowEvent = new BCWindowEvent(BIWindowEvent::QUIT);
    PushEvent(aWindowEvent);
}

/**
 * WaitEvent waits for next event
 *
 * @param aBALEvent(out) the BAL event returned
 *
 * @return true if event ok and loop can go on
 * @return false else
 */
bool BC::BCEventLoopGeneric::WaitEvent(BIEvent*& aBALEvent)
{
    if (!m_initialized)
        return false;

    while (m_event.isEmpty()) {// waitEvent is blocking
        usleep(10000);
/*
#ifndef NDEBUG
        // we must wait for the last timer to fire !
        if (getenv("LAYOUT_TEST") && !WebCore::sharedTimerFiredFunction) 
            // timers are stopped, no chance to get any events
            return false; 
#endif
  */         
    }

    aBALEvent = m_event.first();
    m_event.remove(0);

    if (aBALEvent->queryIsTimerEvent()) {
        WebCore::sharedTimerFiredFunction();
        return true;
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
bool BC::BCEventLoopGeneric::PushEvent(BIEvent* aBALEvent)
{
    if (!m_initialized)
        return false;

    if (!aBALEvent)
        return false;

    m_event.append(aBALEvent);
    return true;
}

