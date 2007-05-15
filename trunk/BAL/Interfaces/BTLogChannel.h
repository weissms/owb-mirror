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
 * @file  BTLogChannel.h
 *
 * @brief Header file for BTLogChannel.h
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#ifndef BTLOGCHANNEL_H
#define BTLOGCHANNEL_H

#include "wtf/RefPtr.h"
#include "Shared.h"
#include <string>
using std::string;

namespace BALFacilities {

class BTLogFormatter;

/**
 * @brief A channel implements the way to send log messages
 *
 */
class BTLogChannel : public WebCore::Shared<BTLogChannel> {
public:
    BTLogChannel();
    virtual ~BTLogChannel();
    /**
     * @brief Check if the channel is active
     */
    virtual bool getIsActive();


    /**
     * @brief Enable or disable the channel
     */
    virtual void setIsActive(bool value);

    /**
     * @brief Get the minimum level from which the log sending is accepted
     */
    virtual int getLevelThreshold();


    /**
     * @brief Set the minimum level from which the log sending is accepted
     * @param value a LogLevel
     */
    virtual void setLevelThreshold(int value);


    /**
     * Get the formatter associated
     * @return the formatter or null
     */
    virtual BTLogFormatter* getFormatter();


    /**
     * Set the formatter to use for this channel
     * @param value is a pointer on the LogFormatter
     */
    virtual void setFormatter(BTLogFormatter* value );


    /**
     * @brief Tell if the channel is synchronized
     */
    virtual bool getIsSynchronized();


    /**
     * @brief When synchronized, message is sent immediatly,
     * otherwise it is buffered.
     * TODO Should be named isBuffered ?
     * @warning Not implemented
     */
    virtual void setIsSynchronized(bool value);


    /**
     * Send the message
     * @param aMessage to be displayed by the channel
     */
    virtual void send (const char* message) = 0;


private:
    RefPtr<BTLogFormatter> m_logFormatter;
    bool            m_isActive;
    int             m_levelThreshold;
    bool            m_isSynchronized;

};

} // namespace BALFacilities

#endif //BTLOGCHANNEL_H

