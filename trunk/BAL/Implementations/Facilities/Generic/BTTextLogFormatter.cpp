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
 * @file  BTTextLogFormatter.cpp
 *
 * @brief Implementation file for BTTextLogFormatter.cpp
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#include "BTLogManager.h"
#include "BTTextLogFormatter.h"
#include <time.h>
#include <sys/time.h>

namespace BALFacilities {

/**
 * For linux console colors see :
 * http://www.developer.com/open/article.php/10930_631241_1
 */
char* BTTextLogFormatter::format(BTLogModule balModule, BTLogLevel level,
const char* filename, int lineNumber, const char* functionName)
{
    // compute nanosec
    struct timeval tv;
    long ms = -1;
    if (gettimeofday(&tv, 0) != -1)
        ms = tv.tv_usec/1000;

    // compute date
    time_t now = time(0);
    static char date[32];
    if (now != static_cast<time_t> (-1)) {
        strcpy(date, ctime(&now));
        // remove \n
        date[strlen(date) - 1] = '\0';
    }

    snprintf(m_buffer, m_maxLength,
                "\033[37;7m(%20s) %s .%03ld %20s in "
                "%s:%d at %s()\033[0m\n",
                getNameForModule(balModule), date, ms,
                getNameForLevel(level),
                filename, lineNumber, functionName);
    int length = strlen(m_buffer);
    if (length == m_maxLength - 1)
        snprintf(m_buffer + m_maxLength - 11, 11, "TRUNCATED\n");

    return m_buffer;
}

} // namespace BALFacilities
