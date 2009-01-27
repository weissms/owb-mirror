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
 
#ifndef TimeCounter_h
#define TimeCounter_h

#include "config.h"

#ifdef BENCH_LOAD_TIME
#include <sys/time.h>
#endif

namespace WTF {

    struct TimeCounter {

        explicit TimeCounter(const char* description, bool shouldPrintCount);
        ~TimeCounter();

        void startCounting();
        void stopCounting();

    private:
#ifdef BENCH_LOAD_TIME
        const char* m_description;
        bool m_shouldPrintCount;

        volatile int m_callCount;
        volatile int m_recursionCount;
        volatile long long m_totalTime;

        // This could be more accurate on Windows (see DateMath.cpp)
        struct timeval m_startTime;
#endif // BENCH_LOAD_TIME
    };

} // namespace WTF

#endif // TimeCounter_h
