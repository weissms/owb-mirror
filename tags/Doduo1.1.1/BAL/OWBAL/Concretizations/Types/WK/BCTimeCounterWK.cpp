/*
 *  Copyright (C) 2008 Pleyo. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */
 
#include "config.h"
#include "TimeCounter.h"


#include "Assertions.h"

#include "stdio.h"

namespace WTF {

#ifndef BENCH_LOAD_TIME

TimeCounter::TimeCounter(const char* description, bool shouldPrintCount) { }
TimeCounter::~TimeCounter() { }

void TimeCounter::startCounting() { }
void TimeCounter::stopCounting() { }

#else // BENCH_LOAD_TIME

// Number of usecs in a second.
#define SEC_IN_USEC 1000000

TimeCounter::TimeCounter(const char* description, bool shouldPrintCount)
    : m_description(description)
    , m_shouldPrintCount(shouldPrintCount)
    , m_callCount(0)
    , m_recursionCount(0)
    , m_totalTime(0)
    , m_isCounting(false)
{
}

TimeCounter::~TimeCounter()
{
    bool inaccurateResult = m_recursionCount || m_isCounting;
        
    long long seconds = m_totalTime / SEC_IN_USEC;
    long long useconds = m_totalTime % SEC_IN_USEC;

    if (inaccurateResult)
        printf("WARNING: time for %s is inaccurate\n", m_description);

    if (m_shouldPrintCount)
        printf("Time spend in %s: %lld s %lld usec for %d calls (total time %lld usec)\n", m_description, seconds, useconds, m_callCount, m_totalTime);
    else
        printf("Time spend in %s: %lld s %lld usec (total time %lld usec)\n", m_description, seconds, useconds, m_totalTime);
}

void TimeCounter::startCounting()
{
    ++m_callCount;
    ++m_recursionCount;

    if (m_isCounting)
        return;

    m_isCounting = true;
    if (gettimeofday(&m_startTime, 0) < 0) {
        if (!--m_recursionCount)
            m_isCounting = false;
    }
}

void TimeCounter::stopCounting()
{
    ASSERT(m_isCounting);
    if (!m_isCounting)
        return;

    // If m_recursion is not null, we are not in
    // the original caller.
    if (--m_recursionCount)
        return;

    m_isCounting = false;

    struct timeval endTime;
    if (gettimeofday(&endTime, 0) < 0)
        return;

    int seconds = endTime.tv_sec - m_startTime.tv_sec;
    int useconds = endTime.tv_usec - m_startTime.tv_usec;

    if (useconds < 0) {
        useconds += SEC_IN_USEC;
        seconds -= 1;
    }

    m_totalTime += (seconds * SEC_IN_USEC + useconds);
}

#endif // BENCH_LOAD_TIME

} // namespace WTF
