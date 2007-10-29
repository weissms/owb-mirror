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
 * @file main.cpp Main TestRunner code, the test unit engine.
 *
 * @namespace BALTest Unit tests for OWBAL.
 */
#include <iostream>

#include "TestRunner.h"
#include "BTDeviceChannel.h"
#include "BTTextLogFormatter.h"
#include "BTLogHelper.h"
#ifdef __OWBAL_PLATFORM_MACPORT__
// FIXME SDL on Mac has some tricks with main() but this dependency is not wanted here
#include <SDL/SDL.h>
#endif

extern TestNode gTestRoot;

/**
	* @brief print help banner
	*/
void printBanner( char* argv[] )
{
	std::cout << "Usage: " << argv[0] << "[TESTPATH|COMMAND]" << std::endl;
	std::cout << " -l [NODE]: list all tests, or test under given node" << std::endl;
	std::cout << " -h: this help" << std::endl;
	std::cout << " -a: automatic mode, no interactive test" << std::endl;
	std::cout << " -p: test path" << std::endl;
}

/**
	* @brief main, to list test or execute them
	*/
int main( int argc, char* argv[] )
{
    TestRunner aTestRunner(&gTestRoot);
    std::string testPath; // default is root
    aTestRunner.SetInteractiveMode( true ); // default behaviour is all test including interactive ones

    // Setup a nice formatter for log
    RefPtr<BALFacilities::BTDeviceChannel> channel = new BALFacilities::BTDeviceChannel;
    RefPtr<BALFacilities::BTTextLogFormatter> textLogFormatter = new BALFacilities::BTTextLogFormatter;
    channel->setFormatter(textLogFormatter.get());
#ifndef NDEBUG
    BALFacilities::logger.addChannel(channel.get());
#endif

    printf("Origyn test runner:\n");

    TestNode* aTestNode=NULL;
    bool bExecute = true; // if false, just display.
    for(int i=1; i<argc; i++)
    {
      if( std::string("-h") == argv[i] )
      {
        printBanner( argv );
        TestManager::deleteInstance();
        return 1;
      }
      else if ( std::string("-l") == argv[i] )
      {
        bExecute = false;
      }
      else if ( std::string("-a") == argv[i] )
      {
        aTestRunner.SetInteractiveMode( false );
      }
      else if ( std::string("-p") == argv[i] )
      {
        TestManager::GetInstance().setPath(argv[i+1]);
        i++;
      }
      else if( aTestRunner.FindNode( argv[i], &aTestNode ) )
      {
        ; // do nothing
      }
      else
      {
        printf("Unknown argument %s\n", argv[i]);
        printBanner(argv);
        TestManager::deleteInstance();
        return 2;
      }
    }

    if( bExecute ) {
        aTestRunner.ExecuteTestTree(aTestNode);

        printf("RESULT OK %d FAILED %d\n",
          aTestRunner.GetTestManager().GetTestResult().mTheOkTotal,
          aTestRunner.GetTestManager().GetTestResult().mTheFailureTotal);

    } else {
        aTestRunner.DisplayTestTree(aTestNode);
        TestManager::deleteInstance();
        return 1;
    }

    bool bWasSuccessfull = aTestRunner.GetTestManager().GetTestResult().mTheFailureTotal == 0;
    TestManager::deleteInstance();
    return bWasSuccessfull ? 0 : 1;
}

extern TestNode gTestSuiteEvent;
extern TestNode gTestSuiteEventLoop;
extern TestNode gTestSuiteFacilities;
extern TestNode gTestSuiteFont;
extern TestNode gTestSuiteFontStress;
extern TestNode gTestSuiteGraphics;
extern TestNode gTestSuiteGraphicsStress;
extern TestNode gTestSuiteImageDecoder;
extern TestNode gTestSuiteLog;
extern TestNode gTestSuiteLogInteractif;
extern TestNode gTestSuiteScrollView;
extern TestNode gTestSuiteWidget;
extern TestNode gTestSuiteNetwork;
extern TestNode gTestSuiteNetworkStress;
extern TestNode gTestSuiteTypes;
extern TestNode gTestSuiteTimer;
extern TestNode gTestSuitePosix;
extern TestNode gTestSuiteXml;
extern TestNode gTestSuiteInternationalization;

TestNode* gRootTestNodeList[] = {
  &gTestSuiteEvent,
  &gTestSuiteEventLoop,
  &gTestSuiteFacilities,
  &gTestSuiteGraphics,
  &gTestSuiteGraphicsStress,
// FIXME disabled for now
//  &gTestSuiteFont,
  &gTestSuiteFontStress,
  &gTestSuiteImageDecoder,
#ifndef NDEBUG
  &gTestSuiteLog,
  &gTestSuiteLogInteractif,
#endif
  &gTestSuiteScrollView,
  &gTestSuiteWidget,
  &gTestSuiteNetwork,
  &gTestSuiteNetworkStress,
  &gTestSuiteTypes,
  &gTestSuiteTimer,
  &gTestSuitePosix,
#ifdef __OWB_XML_SUPPORT__
  &gTestSuiteXml,
#endif
  &gTestSuiteInternationalization,
	NULL
};

TestNode gTestRoot = {
    "root",
    "the test root",
    TestNode::TEST_SUITE,
    NULL,
    gRootTestNodeList
};
