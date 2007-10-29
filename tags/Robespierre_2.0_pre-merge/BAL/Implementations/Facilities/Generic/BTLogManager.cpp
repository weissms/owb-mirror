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
 * @file  BTLogManager.cpp
 *
 * @brief Implementation file for BTLogManager.cpp
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#include "BTLogFormatter.h"
#include "BTLogManager.h"
#include "BTLogChannel.h"

namespace BALFacilities {

// define what is declared in BTLogHelper.h
BTLogManager logger;

BTLogManager::BTLogManager()
	: m_isActive( true )
	, m_levelThreshold( 0 )
{
    char* envVar;
    // for each module defined, find the level threshold in environment variable
    int module = 0;
    while (module != MODULE_UNDEFINED) {
        // reset the level
        m_levelForModule[module] = LEVEL_UNDEFINED;
        // envVar contains the string defined for the module var
        envVar = getenv(getNameForModule((BTLogModule)module));
        if (envVar) {
            // module has been setup inside shell
            // find level threshold
            int level = 0;
            while (level != LEVEL_UNDEFINED) {
                if (0 == strcmp(envVar, getNameForLevel((BTLogLevel)level))) {
                    // we found the level number corresponding to the string
                    m_levelForModule[module] = (BTLogLevel)level;
                }
                level++;
            }
        }
        module++;
    }
}

BTLogManager::~BTLogManager()
{
}

/**
 *
 */
bool BTLogManager::getIsActive()
{
    return m_isActive;
}

/**
 *
 */
void BTLogManager::setIsActive(bool value)
{
    m_isActive = value;
}

/**
 *
 */
int BTLogManager::getLevelThreshold()
{
    return m_levelThreshold;
}

/**
 *
 */
void BTLogManager::setLevelThreshold(int value)
{
    m_levelThreshold = value;
}

/**
 *
 */
void BTLogManager::addChannel(BTLogChannel* aChannel)
{
    m_channels.push_back( aChannel );
}

/**
 * @bug Deleting channel before calling removeChannel() may cause a crash !
 */
void BTLogManager::removeChannel (BTLogChannel* aChannel)
{
    vector< RefPtr<BTLogChannel> >::iterator it;
    it = std::find( m_channels.begin(), m_channels.end(), aChannel);
    if (it == m_channels.end()) {
        // end of vector
    } else
        m_channels.erase( it );
}

/**
 * Attaching a log formatter to the log channels is not mandatory :
 * the BTLogManager uses a default one
 * TODO make the default formatter case a class : DefaultFormatter
 */
void BTLogManager::send(BTLogModule module, BTLogLevel level, const char* file, int
lineNumber, const char* functionName, const char* message)
{
    vector< RefPtr<BTLogChannel> >::iterator it;
    BTLogFormatter* formatter;

    // exit if log manager is not active
    if (!m_isActive)
        return;

    // first check if the level passes the threshold
    if (level < m_levelThreshold)
        return;

    // get threshold from environment variables
    if (level < m_levelForModule[module])
        return;

    if (m_channels.empty()) {
        fprintf( stderr, "M%d,L%d %s:%d at %s: %s\n", module,
                  level, file, lineNumber, functionName, message);
    } else {
        for (it = m_channels.begin(); it != m_channels.end(); ++it) {
            formatter = (*it)->getFormatter();
            if (formatter) {
                (*it)->send(formatter->format(module, level, file, lineNumber, functionName));
                (*it)->send(message);
                (*it)->send("\n");
            } else {
                // there is no formatter for this channel so using default one
                static char str[256];
                snprintf(str, 256, "(no formatter) %d|%d|%s|%d|%s|%s\n", module, level, file, lineNumber, functionName, message);
                (*it)->send(str);
            }
        }
    }
}

const char* getNameForLevel(BALFacilities::BTLogLevel level)
{
	static const char* LogLevelNames[] = {
    	"LEVEL_INFO",
    	"LEVEL_WARNING",
    	"LEVEL_ERROR",
    	"LEVEL_CRITICAL",
    	"LEVEL_EMERGENCY",
    	"LEVEL_UNDEFINED"
	};

	return LogLevelNames[level];
}

const char* getNameForModule(BALFacilities::BTLogModule module)
{
	static const char* LogModuleNames[] = {
		"MODULE_BRIDGE",
		"MODULE_EVENTS",
		"MODULE_FACILITIES",
		"MODULE_FONTS",
		"MODULE_GLUE",
		"MODULE_GRAPHICS",
		"MODULE_IMAGEDECODERS",
		"MODULE_NETWORK",
		"MODULE_TYPES",
		"MODULE_WIDGETS",
		"MODULE_UNDEFINED"
	};
	return LogModuleNames[module];
}

} // namespace BALFacilities
