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
 * @file
 * This file declares and implements logger tests. Only
 * in debug mode.
 */
#ifndef NDEBUG

#include "BTLogHelper.h"
#include "BTDeviceChannel.h"
#include "BTSyslogChannel.h"
#include "BTTextLogFormatter.h"
#include "TestManager/TestManager.h"

using BALFacilities::BTDeviceChannel;
using BALFacilities::BTSyslogChannel;
using BALFacilities::BTTextLogFormatter;
using BALFacilities::logger;

class LogManagerTest {
public:

    void initialize() {
        m_channel = new BTDeviceChannel();
        m_syslogChannel = new BTSyslogChannel();
        m_textLogFormatter = new BTTextLogFormatter();

        m_channel->setFormatter( m_textLogFormatter.get() );

        m_syslogChannel->setHostname( "sroret" );
        m_syslogChannel->setFormatter( m_textLogFormatter.get() );
        logger.addChannel( m_channel.get() );
        logger.addChannel( m_syslogChannel.get() );
    }
    void finalize() {
        logger.removeChannel( m_channel.get() );
        logger.removeChannel( m_syslogChannel.get() );
    }

    void testLogMacro()
    {
        initialize();
        // one char test
        log("a");
        // null argument
        log(NULL);
        // testing string > 256 chars
        log(
        "000000000011111111112222222222333333333344444444445555555555"
        "6666666666777777777788888888889999999999"
        "000000000011111111112222222222333333333344444444445555555555"
        "6666666666777777777788888888889999999999"
        "000000000011111111112222222222333333333344444444445555555555"
        "6666666666777777777788888888889999999999");

        // testing modules
        logm(MODULE_FONTS, "FONT");

        logml(MODULE_FONTS,LEVEL_CRITICAL, "FONT/CRITICAL" );

        // now trying to log to pts console 2
        m_channel->setPathName( "/dev/pts/2" );
        log("You should see this on pts console 2");
        finalize();
    }

    void testManager()
    {
        // testing LogManager
        BALFacilities::BTLogManager trace;
        trace.send( MODULE_UNDEFINED, LEVEL_UNDEFINED, 0, 1, 0,
                    "you should see this" );


        // remove inexistant channel
        trace.removeChannel(reinterpret_cast<BALFacilities::BTLogChannel*>(0xdeadbeef));
        // remove twice the same channel
        RefPtr<BTDeviceChannel> channel = new BTDeviceChannel;

        trace.addChannel(channel.get());
        trace.removeChannel(channel.get());
        trace.removeChannel(channel.get());
        trace.send( MODULE_UNDEFINED, LEVEL_UNDEFINED, 0, 2, 0,
                    "you should see this" );
    // remove twice the same channel
        RefPtr<BTDeviceChannel> channel2 = new BTDeviceChannel;
        trace.addChannel(channel2.get());
        trace.addChannel(channel2.get());
        trace.removeChannel(channel2.get());
        trace.send( MODULE_UNDEFINED, LEVEL_UNDEFINED, 0, 3, 0,
                    "you should see this" );

        trace.removeChannel(channel2.get());
        trace.send( MODULE_UNDEFINED, LEVEL_UNDEFINED, 0, 4, 0,
                    "youo should see" );
    }

private:
    RefPtr<BTDeviceChannel>     m_channel;
    RefPtr<BTTextLogFormatter>  m_textLogFormatter;
    RefPtr<BTSyslogChannel>     m_syslogChannel;

};

class StaticLogTests
{
public:
  static void testLogMacro() {
    LogManagerTest a;
    a.testLogMacro();
  }
  static void testManager() {
    LogManagerTest a;
    a.testManager();
  }
};

static TestNode gtestLogMacro = { "testLogMacro", "testLogMacro",
  TestNode::AUTO, StaticLogTests::testLogMacro, NULL };
static TestNode gtestManager = { "testManager", "testManager",
  TestNode::AUTO, StaticLogTests::testManager, NULL };

TestNode* gLogTestNodeList[] = {
  &gtestLogMacro,
  &gtestManager,
	NULL
};

TestNode gTestSuiteLog = {
    "TestLog",
    "test log",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
		gLogTestNodeList 
};
#endif
