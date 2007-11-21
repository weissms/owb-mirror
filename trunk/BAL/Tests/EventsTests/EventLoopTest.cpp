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

#include "config.h"
#include "BALConfiguration.h"
#include "BIEventLoop.h"
#include "IntRect.h"
#include "../TestManager/TestManager.h"

using namespace BAL;

class EventLoopTest {
    public:

static void gTestEventLoopCreateDelete()
{
    BIEventLoop *aEventLoop = getBIEventLoop();
    TestManager::AssertTrue("Event loop exists", aEventLoop != NULL);
    deleteBIEventLoop();
    aEventLoop = NULL;
}

static void gTestEventLoopCreateDeleteMultiple()
{
    BIEventLoop *aEventLoop;
    for (uint8_t i = 0; i < 255; i++) {
        aEventLoop = getBIEventLoop();
        TestManager::AssertTrue("Event loop exists", aEventLoop != NULL);
        deleteBIEventLoop();
        aEventLoop = NULL;
    }
}

static void gTestEventLoopInitialized()
{
    BIEventLoop *aEventLoop = getBIEventLoop();
    TestManager::AssertTrue("Event loop exists", aEventLoop != NULL);
    TestManager::AssertTrue("Event loop is initialized", aEventLoop->isInitialized());
    deleteBIEventLoop();
    aEventLoop = NULL;
}

static void gTestEventLoopPushWait()
{
    BIEventLoop *aEventLoop = getBIEventLoop();
    BIEvent *aWindowEvent = createBIWindowEvent(BIWindowEvent::ACTIVE, false, WebCore::IntRect(), NULL);
    BIEvent *aEvent = NULL;

    TestManager::AssertTrue("Push an event", aEventLoop->PushEvent(aWindowEvent));
    TestManager::AssertTrue("Wait an event", aEventLoop->WaitEvent(aEvent));

    delete aEvent;
    aEvent = NULL;
    deleteBIEventLoop();
    aEventLoop = NULL;
}

static void gTestEventLoopPushWaitMultiple1()
{
    BIEventLoop *aEventLoop = getBIEventLoop();
    BIEvent *aEvent = NULL;

    for (uint8_t i = 0; i < 255; i++) {
        BIEvent *aWindowEvent = createBIWindowEvent(BIWindowEvent::ACTIVE, false, WebCore::IntRect(), NULL);
        TestManager::AssertTrue("Push an event", aEventLoop->PushEvent(aWindowEvent));
        TestManager::AssertTrue("Wait an event", aEventLoop->WaitEvent(aEvent));
        delete aEvent;
        aEvent = NULL;
    }

    deleteBIEventLoop();
    aEventLoop = NULL;
}

static void gTestEventLoopPushWaitMultiple2()
{
    BIEventLoop *aEventLoop = getBIEventLoop();
    BIEvent *aWindowEvent[255];
    BIEvent *aEvent = NULL;

    for (uint8_t i = 0; i < 255; i++) {
        aWindowEvent[i] = createBIWindowEvent(BIWindowEvent::ACTIVE, false, WebCore::IntRect(), NULL);
        TestManager::AssertTrue("Push an event", aEventLoop->PushEvent(aWindowEvent[i]));
    }
    for (uint8_t i = 0; i < 255; i++) {
        TestManager::AssertTrue("Wait an event", aEventLoop->WaitEvent(aEvent));
        delete aEvent;
        aEvent = NULL;
    }

    deleteBIEventLoop();
    aEventLoop = NULL;
}

};

static TestNode gTestEventLoopCreateDelete = { "TestEventLoopCreateDelete", "TestEventLoopCreateDelete", TestNode::AUTO, EventLoopTest::gTestEventLoopCreateDelete, NULL };
static TestNode gTestEventLoopCreateDeleteMultiple = { "TestEventLoopCreateDeleteMultiple", "TestEventLoopCreateDeleteMultiple", TestNode::AUTO, EventLoopTest::gTestEventLoopCreateDeleteMultiple, NULL };
static TestNode gTestEventLoopInitialized = { "TestEventLoopInitialized", "TestEventLoopInitialized", TestNode::AUTO, EventLoopTest::gTestEventLoopInitialized, NULL };
static TestNode gTestEventLoopPushWait = { "TestEventLoopPushWait", "TestEventLoopPushWait", TestNode::AUTO, EventLoopTest::gTestEventLoopPushWait, NULL };
static TestNode gTestEventLoopPushWaitMultiple1 = { "TestEventLoopPushWaitMultiple1", "TestEventLoopPushWaitMultiple1", TestNode::AUTO, EventLoopTest::gTestEventLoopPushWaitMultiple1, NULL };
static TestNode gTestEventLoopPushWaitMultiple2 = { "TestEventLoopPushWaitMultiple2", "TestEventLoopPushWaitMultiple2", TestNode::AUTO, EventLoopTest::gTestEventLoopPushWaitMultiple2, NULL };

TestNode* gEventLoopTestNodeList[] = {
    &gTestEventLoopCreateDelete,
    &gTestEventLoopCreateDeleteMultiple,
    &gTestEventLoopInitialized,
    &gTestEventLoopPushWait,
    &gTestEventLoopPushWaitMultiple1,
    &gTestEventLoopPushWaitMultiple2,
    NULL
};

TestNode gTestSuiteEventLoop = {
    "TestEventLoop",
    "test event loop",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gEventLoopTestNodeList
};
