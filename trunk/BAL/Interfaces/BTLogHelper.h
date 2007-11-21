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
 * @file  BTLogHelper.h
 *
 * @brief Header file for BTLogHelper.h
 *
 * This file contains macros to use in order to log.
 * All is needed to use the log is to include this file, there's no need
 * to include LogManager.h
 * Use make_message to construct complex strings to log :
 * @code
 *      log(make_message("%s won %d-%d\n", "France", 3, 0));
 * @endcode
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#ifndef BTLOGHELPER_H
#define BTLOGHELPER_H

#ifdef BAL_LOG

#include "BTLogManager.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

namespace BALFacilities {

extern BTLogManager logger;
// FIXME SRO temporary length of variadic args size to 0xff

#define DBG(...) BALFacilities::logger.send(MODULE_UNDEFINED, LEVEL_INFO, __FILE__,     __LINE__, __func__, __VA_ARGS__)

#define DBGM(module, ...) BALFacilities::logger.send(module, LEVEL_INFO, \
                   __FILE__, __LINE__, __func__, __VA_ARGS__)

#define DBGML(module, level, ...) BALFacilities::logger.send(module, level,\
                   __FILE__, __LINE__, __func__, __VA_ARGS__)

} // namespace BALFacilities

#else  // BAL_LOG
// we're in release mode

namespace BALFacilities {

#define DBG(...)                    ((void)0)
#define DBGM(module, ...)           ((void)0)
#define DBGML(module, level, ...)   ((void)0)

inline char* make_message(const char *format, ...) { return 0; }

} // namespace BALFacilities

#endif // DEBUG
#endif // BTLOGHELPER_H
