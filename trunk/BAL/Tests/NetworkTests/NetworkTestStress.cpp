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
#include "BTResourceHandle.h"
#include "BIResourceHandleManager.h"
#include "ResourceHandleClient.h"
#include "ResourceRequest.h"
#include "../TestManager/TestManager.h"

using namespace BAL;

class NetworkTestStress {
    public:

static void TestNetworkResourceHandleManagerSimple()
{
    BIResourceHandleManager *aResourceHandleManager = getBIResourceHandleManager();
    TestManager::AssertTrue("Resource handle manager exists", aResourceHandleManager != NULL);
    deleteBIResourceHandleManager();
    aResourceHandleManager = NULL;
}

static void TestNetworkResourceHandleManagerMultiple()
{
    BIResourceHandleManager *aResourceHandleManager;
    for (uint8_t i = 0; i < 255; i++) {
        aResourceHandleManager = getBIResourceHandleManager();
        TestManager::AssertTrue("Resource handle manager exists", aResourceHandleManager != NULL);
        deleteBIResourceHandleManager();
        aResourceHandleManager = NULL;
    }
}

static void TestNetworkResourceHandleCreate()
{
    ResourceHandleClient *aResourceHandleClient = new ResourceHandleClient();
    ResourceRequest aResourceRequest = ResourceRequest();
    RefPtr<BTResourceHandle> aResourceHandle = BTResourceHandle::create(aResourceRequest, aResourceHandleClient, 0, false, false, false);
    TestManager::AssertTrue("Resource handle exists", aResourceHandle != NULL);
    aResourceHandle = NULL;
    delete aResourceHandleClient;
    aResourceHandleClient = NULL;
}

static void TestNetworkResourceHandleCreateMultiple1()
{
    ResourceHandleClient *aResourceHandleClient = new ResourceHandleClient();
    ResourceRequest aResourceRequest = ResourceRequest();
    for (uint8_t i = 0; i < 255; i++) {
        RefPtr<BTResourceHandle> aResourceHandle = BTResourceHandle::create(aResourceRequest, aResourceHandleClient, 0, false, false, false);
        TestManager::AssertTrue("Resource handle exists", aResourceHandle != NULL);
        aResourceHandle = NULL;
    }
    delete aResourceHandleClient;
    aResourceHandleClient = NULL;
}

static void TestNetworkResourceHandleCreateMultiple2()
{
    ResourceHandleClient *aResourceHandleClient = new ResourceHandleClient();
    ResourceRequest aResourceRequest = ResourceRequest();
    RefPtr<BTResourceHandle> aResourceHandle[255];
    for (uint8_t i = 0; i < 255; i++) {
        aResourceHandle[i] = BTResourceHandle::create(aResourceRequest, aResourceHandleClient, 0, false, false, false);
        TestManager::AssertTrue("Resource handle exists", aResourceHandle[i] != NULL);
    }
    for (uint8_t i = 0; i < 255; i++)
        aResourceHandle[i] = NULL;
    delete aResourceHandleClient;
    aResourceHandleClient = NULL;
}

static void TestNetworkResourceHandleContains()
{
    BIResourceHandleManager *aResourceHandleManager = getBIResourceHandleManager();
    TestManager::AssertTrue("Resource handle manager exists", aResourceHandleManager != NULL);
    ResourceHandleClient *aResourceHandleClient = new ResourceHandleClient();
    ResourceRequest aResourceRequest = ResourceRequest();
    RefPtr<BTResourceHandle> aResourceHandle = BTResourceHandle::create(aResourceRequest, aResourceHandleClient, 0, false, false, false);
    TestManager::AssertTrue("Resource handle exists", aResourceHandle != NULL);

    aResourceHandle = NULL;
    delete aResourceHandleClient;
    aResourceHandleClient = NULL;
    deleteBIResourceHandleManager();
    aResourceHandleManager = NULL;
}

static void TestNetworkResourceHandleContainsMultiple()
{
    BIResourceHandleManager *aResourceHandleManager = getBIResourceHandleManager();
    TestManager::AssertTrue("Resource handle manager exists", aResourceHandleManager != NULL);
    ResourceHandleClient *aResourceHandleClient = new ResourceHandleClient();
    ResourceRequest aResourceRequest = ResourceRequest();
    RefPtr<BTResourceHandle> aResourceHandle[255];
    for (uint8_t i = 0; i < 255; i++) {
        aResourceHandle[i] = BTResourceHandle::create(aResourceRequest, aResourceHandleClient, 0, false, false, false);
        TestManager::AssertTrue("Resource handle exists", aResourceHandle[i] != NULL);
    }
    for (uint8_t i = 0; i < 255; i++) 
        aResourceHandle[i] = NULL;
    
    delete aResourceHandleClient;
    aResourceHandleClient = NULL;
    deleteBIResourceHandleManager();
    aResourceHandleManager = NULL;
}

};

static TestNode gTestNetworkResourceHandleManagerSimple = { "TestNetworkResourceHandleManagerSimple", "TestNetworkResourceHandleManagerSimple", TestNode::AUTO, NetworkTestStress::TestNetworkResourceHandleManagerSimple, NULL };
static TestNode gTestNetworkResourceHandleManagerMultiple = { "TestNetworkResourceHandleManagerMultiple", "TestNetworkResourceHandleManagerMultiple", TestNode::AUTO, NetworkTestStress::TestNetworkResourceHandleManagerMultiple, NULL };
static TestNode gTestNetworkResourceHandleCreate = { "TestNetworkResourceHandleCreate", "TestNetworkResourceHandleCreate", TestNode::AUTO, NetworkTestStress::TestNetworkResourceHandleCreate, NULL };
static TestNode gTestNetworkResourceHandleCreateMultiple1 = { "TestNetworkResourceHandleCreateMultiple1", "TestNetworkResourceHandleCreateMultiple1", TestNode::AUTO, NetworkTestStress::TestNetworkResourceHandleCreateMultiple1, NULL };
static TestNode gTestNetworkResourceHandleCreateMultiple2 = { "TestNetworkResourceHandleCreateMultiple2", "TestNetworkResourceHandleCreateMultiple2", TestNode::AUTO, NetworkTestStress::TestNetworkResourceHandleCreateMultiple2, NULL };
static TestNode gTestNetworkResourceHandleContains = { "TestNetworkResourceHandleContains", "TestNetworkResourceHandleContains", TestNode::AUTO, NetworkTestStress::TestNetworkResourceHandleContains, NULL };
static TestNode gTestNetworkResourceHandleContainsMultiple = { "TestNetworkResourceHandleContainsMultiple", "TestNetworkResourceHandleContainsMultiple", TestNode::AUTO, NetworkTestStress::TestNetworkResourceHandleContainsMultiple, NULL };

TestNode* gNetworkTestStressNodeList[] = {
    &gTestNetworkResourceHandleManagerSimple,
    &gTestNetworkResourceHandleManagerMultiple,
    &gTestNetworkResourceHandleCreate,
    &gTestNetworkResourceHandleCreateMultiple1,
    &gTestNetworkResourceHandleCreateMultiple2,
    &gTestNetworkResourceHandleContains,
    &gTestNetworkResourceHandleContainsMultiple,
    NULL
};

TestNode gTestSuiteNetworkStress = {
    "TestNetworkStress",
    "test NetworkStress",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gNetworkTestStressNodeList 
};
