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
 * @file  BTLogManager.h
 *
 * @brief Header file for BTLogManager.h
 *
 * @namespace BALFacilities Helper classes for OWBAL.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#ifndef BTLOGMANAGER_H
#define BTLOGMANAGER_H

#include "BTLogEnum.h"
#include "wtf/RefPtr.h"
#include <vector>
using std::vector;

namespace BALFacilities {

class BTLogChannel;

/**
 * @brief Main class of the trace system
 * First you have to configure your channels, by which the messages are printed
 * Then you must attach them to the log manager.
 * At last, you can send your log with send(), which will dispatch them
 * to all the channels attached.
 *
 */
class BTLogManager {
public:
    BTLogManager();
    virtual ~BTLogManager();

    /**
     * @brief Tell if the manager is active
     */
    bool getIsActive();


    /**
     * @brief Activate or Deactivate the log system
     */
    void setIsActive(bool value );


    /**
     * @brief Get the minimum level from which the log sending is accepted
     */
    int getLevelThreshold();


    /**
     * @brief Set the minimum level from which the log sending is accepted
     * @param value a LogLevel
     */
    void setLevelThreshold(int value );


    /**
     * @brief Add a channel to which logs are sent
     * @param aChannel
     */
    void addChannel(BTLogChannel *channel);


    /**
     * @brief Remove a channel added before by addChannel
     * @param aChannel
     */
    void removeChannel(BTLogChannel *channel);


    /**
     * @brief Send a message to all channels
     * @param aModule name of the module sending the message
     * @param aLevel level of severity
     * @param aFile the file calling send()
     * @param aLineNumber the line number in the file
     * @param aFunctionName the function containing the call to send()
     * @param aMessage the message to log
     */
    void send(BTLogModule module, BTLogLevel level, const char* file, int
lineNumber, const char* functionName, const char* message);

private:
    /**
     * @brief All channels are stored by their pointers :
     * beware no to delete them before calling removeChannel() !
     */
    vector< RefPtr<BTLogChannel> > m_channels;
    bool                m_isActive;
    int                 m_levelThreshold;
    BTLogLevel          m_levelForModule[MODULE_UNDEFINED];
};

} // namespace BALFacilities

#endif //BTLOGMANAGER_H

