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
 * @file  BTLogFormatter.h
 *
 * @brief Header file for BTLogFormatter.h
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#ifndef BTLOGFORMATTER_H
#define BTLOGFORMATTER_H

#include "BTLogEnum.h"
#include "Shared.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

namespace BALFacilities {

/**
 * @brief Used by BTLogChannel to build and format messages
 *
 */
class BTLogFormatter : public WebCore::Shared<BTLogFormatter> {
public:
    BTLogFormatter();
    virtual ~BTLogFormatter();
    /**
     * @brief Tell if the formatter sends timed messages
     */
    virtual bool getIsTimed();


    /**
     * @brief If the message is timed, milliseconds are included in message
     */
    virtual void setIsTimed(bool value );


    /**
     * @brief Tell if the formatter is dating its messages
     */
    virtual bool getIsDated( );


    /**
     * @brief Dated message have current date included
     * In the form "Thu Jun 22 15:05:54 2006"
     */
    virtual void setIsDated(bool value );


    /**
     * Construct the header message.
     * The body message is not included to avoid buffer copy, and to
     * let the owner allocate it.
     *
     * @param aBalModule name of the module sending the message
     * @param aLevel level of severity
     * @param aFile the file calling send()
     * @param aLineNumber the line number in the file
     * @param aFunctionName the function containing the call to send()
     * @param aMessage the message to log
     */
    virtual char* format(BTLogModule aBalModule, BTLogLevel aLevel, const char*
filename, int lineNumber, const char* functionName) = 0;

protected:
    int m_maxLength;
    char* m_buffer;
private:
    bool m_isTimed;
    bool m_isDated;
};

} // namespace BALFacilities

#endif //BTLOGFORMATTER_H

