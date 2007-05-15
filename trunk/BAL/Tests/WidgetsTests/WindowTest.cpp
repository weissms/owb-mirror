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
#include <iostream>

#include "config.h"
#include "BTLogHelper.h"
#include "TestManager/TestManager.h"

#include "BALConfiguration.h"
#include "BIEvent.h"
#include "BIEventLoop.h"
#include "BIKeyboardEvent.h"
#include "BIWindow.h"

using namespace BAL;

class WindowTest {
public:

    static void WaitForKey( BIEventLoop& aEventLoop)
    {
        BIEvent* event;
        while(1)
        {
            bool bValid = aEventLoop.WaitEvent(event);
            if( !bValid )
              continue;

            BIKeyboardEvent* aKeyboardEvent = event->queryIsKeyboardEvent();
            if( aKeyboardEvent )
            {
                printf("La touche %x a �t� press�e!\n", aKeyboardEvent->virtualKeyCode() );
                if(aKeyboardEvent->virtualKeyCode() == BAL::BIKeyboardEvent::VK_Q ) {
                  break; // stop loop
                }
            }
        }
    }

    static void creation()
    {
        log("start");

        BIEventLoop* aEventLoop = getBIEventLoop();
        TestManager::AssertTrue("Event loop ok", aEventLoop != NULL);
        if( aEventLoop == NULL ) {
          return;
        }

        TestManager::AssertTrue("Event loop correctly initialized", aEventLoop->isInitialized());
        if( !aEventLoop->isInitialized() ) {
          return;
        }
// FIXME WindowBal must be fully independant in order to be tested in BAL
/*
        BIWindow* aWindow = createBCWindowBal(0,0,200,201,32);
        TestManager::AssertTrue("window ok", aWindow != NULL);
        if( aWindow == NULL ) {
          log("no window");
          return;
        }

        TestManager::AssertTrue("Width is 200", 200 == aWindow->width());
        TestManager::AssertTrue("Height is 201", 201 == aWindow->height());
        TestManager::AssertTrue("Depth is 32", 32 == aWindow->depth());
        //WaitForKey( *aEventLoop );

        delete aWindow;
*/
        log("end");
    }


private:

};

static TestNode gtestWindowCreation = { "testWindowCreation", "testWindowCreation",
  TestNode::AUTO, WindowTest::creation, NULL };

TestNode* gWindowTestNodeList[] = {
  &gtestWindowCreation,
	NULL
};

TestNode gTestSuiteWindow = {
    "TestWindow",
    "test window",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
		gWindowTestNodeList
};

