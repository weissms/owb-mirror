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
 * This file declares and implements interactive logger tests.
 * Only in debug mode.
 */
#ifndef NDEBUG
#include <errno.h>
#include <iostream>
#include <iomanip>
#include <termios.h>
#include <errno.h>
using std::cin;
using std::cout;
using std::endl;

#include "BTLogHelper.h"
#include "BTDeviceChannel.h"
#include "BTSyslogChannel.h"
#include "BTTextLogFormatter.h"
#include "TestManager/TestManager.h"

using BALFacilities::BTDeviceChannel;
using BALFacilities::BTSyslogChannel;
using BALFacilities::BTTextLogFormatter;
using BALFacilities::logger;

class LogManagerTestInteractif {
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
        // RefPtr will auto destroy
    }
    void ask(const char* message)
    {
        // some code to disable echo on console
        // and to enable one only char capture
        struct termios *termios_p;
        termios_p = new struct termios;
        tcflag_t lflag;

        // Get terminal attributes
        tcgetattr( 0, termios_p );
        // Save attributes
        lflag=termios_p->c_lflag;
        // Modify
        termios_p->c_lflag &= ~( ECHO | ICANON );
        tcsetattr( 0, TCSANOW,termios_p );

        char aChar;
        bool isAnswered = false;

        cout << "**" << message << " ? [y/n]" << endl;


        while( ( false == isAnswered ) )
        {
            cin >> aChar;
            if( 'y' == aChar || 'Y' ==  aChar || 'n' == aChar || 'N' == aChar )
            {
                isAnswered = true;
            }
            else
            {
                cout << "Invalid answer '" << aChar << "'." << endl;
                cout << message << " ? [y/n]" << endl;
            }
        }

        // Restore original attributes
        termios_p->c_lflag = lflag;
        termios_p->c_lflag &= ECHO;
        tcsetattr( 0, TCSANOW, termios_p );

        delete termios_p;

        // do this at the because of the exit()
        TestManager::AssertTrue(message, 'y' == aChar || 'Y' ==  aChar);
    }
    void testLogMacro()
    {
        initialize();
        DBG(
        "000000000011111111112222222222333333333344444444445555555555\n"
        "6666666666777777777788888888889999999999\n"
        "000000000011111111112222222222333333333344444444445555555555\n"
        "6666666666777777777788888888889999999999\n"
        "000000000011111111112222222222333333333344444444445555555555\n"
        "6666666666777777777788888888889999999999\n");
        ask("You shouldn't see 300 chars, but a line with TRUNCATED at the end");
        // testing modules
        DBGM(MODULE_FONTS, "FONT\n");
        ask("is the log from MODULE_FONTS");
        DBGML(MODULE_FONTS,LEVEL_CRITICAL, "FONT/CRITICAL\n");
        ask("is the log from MODULE_FONTS and at LEVEL_CRITICAL");

        // now trying to log to pts console 2
        m_channel->setPathName("/dev/pts/2");
        if (errno)
            DBG(strerror(errno));
        TestManager::AssertTrue("open /dev/pts/2", errno == 0 );
        ask("You should see this on pts console 2");
        finalize();
    }

    void testManager()
    {
        // testing LogManager
        BALFacilities::BTLogManager trace;
        // shouldn't do anything
        trace.send( MODULE_UNDEFINED, LEVEL_UNDEFINED, 0, 1, 0,
                    "default log\n" );
        ask("do you see 'M0,L0 (null):1 at (null)'");

        // remove inexistant channel
        trace.removeChannel(reinterpret_cast<BALFacilities::BTLogChannel*>(0xdeadbeef));
        // remove twice the same channel
        RefPtr<BTDeviceChannel> channel = new BTDeviceChannel;

        trace.addChannel(channel.get());
        trace.removeChannel(channel.get());
        trace.removeChannel(channel.get());

        trace.send( MODULE_UNDEFINED, LEVEL_UNDEFINED, 0, 2, 0,
                    "Removing twice the same channel\n" );
        ask("do you see a log");

        // add twice the same channel
        RefPtr<BTDeviceChannel> channel2 = new BTDeviceChannel;
        trace.addChannel(channel2.get());
        trace.addChannel(channel2.get());
        trace.removeChannel(channel2.get());
        trace.send( MODULE_UNDEFINED, LEVEL_UNDEFINED, 0, 3, 0,
                    "log with channel\n" );
        ask("do you see '(no formatter) 0|0|(null)|3|(null)|log with channel'");

        trace.removeChannel(channel2.get());
        trace.send( MODULE_UNDEFINED, LEVEL_UNDEFINED, 0, 4, 0,
                    "log without channel\n" );
        ask("do you see only 'M0,L0 (null):4 at (null): log without channel'");
    }

private:
    RefPtr<BTDeviceChannel>     m_channel;
    RefPtr<BTTextLogFormatter>  m_textLogFormatter;
    RefPtr<BTSyslogChannel>     m_syslogChannel;

};

class StaticLogManagerTestInteractif
{
public:
  static void testLogMacroInteratif() {
    LogManagerTestInteractif a;
    a.testLogMacro();
  }

  static void testManagerInteractif() {
    LogManagerTestInteractif a;
    a.testManager();
  }
};

static TestNode gtestLogMacroInteratif = { "testLogMacroInteratif", "testLogMacroInteratif",
  TestNode::FUNCTIONALY_INTERACTIVE, StaticLogManagerTestInteractif::testLogMacroInteratif, NULL };
static TestNode gtestManagerInteractif = { "testManagerInteractif", "testManagerInteractif",
  TestNode::FUNCTIONALY_INTERACTIVE, StaticLogManagerTestInteractif::testManagerInteractif, NULL };

TestNode* gLogTestNodeListInteractif[] = {
  &gtestLogMacroInteratif,
  &gtestManagerInteractif,
	NULL
};

TestNode gTestSuiteLogInteractif = {
    "TestLogInteractif",
    "test log interactif",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
		gLogTestNodeListInteractif 
};

#endif
