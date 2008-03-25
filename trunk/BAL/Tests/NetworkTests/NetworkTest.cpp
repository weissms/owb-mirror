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
#include <stdexcept>
#include <unistd.h>
#include <string>

#include "config.h"
#include "BTLogHelper.h"
#include "PlatformString.h"

#include "BALConfiguration.h"
#include "BIResourceHandleManager.h"
#include "BTResourceHandle.h"
#include "BIEventLoop.h"
#include "BTFormData.h"
#include "TestHelpers/BALFileWriter.h"
#include "TestHelpers/BALFileComparison.h"
#include "TestHelpers/BALFileHelper.h"
#include "ResourceRequest.h"
#include "Timer.h"

// FIXME need to have a apache server in the environment
#define  TEST_WEB_SITE "http://192.168.2.27/transfertest/"

using namespace BAL;
using namespace BALTest;

using WebCore::DeprecatedString;

namespace BAL {

int gTransferFileCount(0);
bool gTimeout(false);

class TestResourceHandleClient : public ResourceHandleClient
{
public:
  TestResourceHandleClient( const std::string& aFileName )
   : mFileWriter( aFileName )
  {
  }

  virtual ~TestResourceHandleClient() { }
  virtual void receivedRedirect(BTResourceHandle*, const KURL&) {}; // TODO have a test for redirection
  virtual void receivedData(BTResourceHandle*, const char*, int)
  {
    TestManager::AssertTrue("OBSOLETE CALL", 0);
  }

  virtual void receivedAllData(BTResourceHandle*)
  {
    TestManager::AssertTrue("OBSOLETE CALL", 0);
  }

  virtual void didReceiveData(BAL::BTResourceHandle*, const char* aData, int aSize, int)
  {
    mFileWriter.Write(aData, aSize);
  }

  virtual void didFinishLoading(BAL::BTResourceHandle* resource)
  {
      if (mFileWriter.size()) {
        mFileWriter.Close();
        gTransferFileCount--;
      }
  }

protected:
  BALFileWriter mFileWriter;
};

}

class NetworkTest {
public:
  /**
   * Test a simple transfer
   */
  static void TestNetworkSimpleTransfer()
  {
    std::string simple = TestManager::GetInstance().getPath() + "NetworkTests/Refs/simple.html";
    TestNetworkSimpleTransfer( TEST_WEB_SITE"simple.html", simple.c_str() );
  }

  /**
   * Test a simple transfer with a mire 1,5M
   */
  static void TestNetworkSimpleTransferMire()
  {
    std::string mire = TestManager::GetInstance().getPath() + "NetworkTests/Refs/mirejpeg1.bmp";
    TestNetworkSimpleTransfer( TEST_WEB_SITE"mirejpeg1.bmp", mire.c_str() );
  }

  /**
   * Test a simple transfer with some parameters
   */
  static void TestNetworkGetTransferWithParameters()
  {
    std::string get = TestManager::GetInstance().getPath() + "NetworkTests/Refs/testgetwithparam.html";
    TestNetworkSimpleTransfer( TEST_WEB_SITE"testgetwithparam.php?param1=toto&param2=kiki",
       get.c_str() );
  }

  /**
   * Test a transfer with a bad url
   */
  static void TestNetworkBadURL()
  {
    // FIXME Setup a http server to serve the files
    TestManager::AssertTrue("No web server setup for the tests", false);
    return;
    
    BIResourceHandleManager* aResourceHandleManager = getBIResourceHandleManager();
    TestManager::AssertTrue("Transfer Job Manager obtained", aResourceHandleManager != NULL);

    std::string aFileName;
    GetTemporaryUnexistingFileName("/tmp/transfer", ".data", aFileName);

    TestResourceHandleClient aTestResourceHandleClient(aFileName);

    KURL url("htpp://foo/bar");
    ResourceRequest aResourceRequest1 = ResourceRequest(url);
    RefPtr<BTResourceHandle> aResourceHandle1 = BTResourceHandle::create(aResourceRequest1, &aTestResourceHandleClient, 0, false, false, false);

    url.setProtocol("http");
    ResourceRequest aResourceRequest2 = ResourceRequest(url);
    RefPtr<BTResourceHandle> aResourceHandle2 = BTResourceHandle::create(aResourceRequest2, &aTestResourceHandleClient, 0, false, false, false);

    gTransferFileCount = 2;

    // Wait for the end of the transfer.
    gTimeout = false;
    WebCore::Timer<NetworkTest> aTimeoutTimer(NULL,&NetworkTest::TimeoutCallback);
    aTimeoutTimer.startOneShot(5); // timeout in 5s : should be enough to retrieve errors

    BIEventLoop* aEventLoop = BAL::getBIEventLoop();
    BIEvent* event;
    while( gTransferFileCount && !gTimeout )
    {
      aEventLoop->WaitEvent(event);
    }

    //TODO: Find a way to retrieve errors (Bad protocol and hostname resolution failure)
    TestManager::AssertTrue("Transfer failed", gTransferFileCount == 2 );
//     TestManager::AssertTrue("Error reported", aResourceHandle->error() );

    unlink( aFileName.c_str() );
  }

  /**
   * Test concurrent transfer
   */
  static void TestNetworkConcurrentTransfer()
  {
    // FIXME Setup a http server to serve the files
    TestManager::AssertTrue("No web server setup for the tests", false);
    return;
    
    BIResourceHandleManager* aResourceHandleManager = getBIResourceHandleManager();
    TestManager::AssertTrue("Transfer Job Manager obtained", aResourceHandleManager != NULL);

    std::string aFileName1;
    GetTemporaryUnexistingFileName("/tmp/transfer", ".data", aFileName1);
    TestResourceHandleClient aTestResourceHandleClient1(aFileName1);
    ResourceRequest aResourceRequest1 = ResourceRequest(static_cast<const KURL&> (TEST_WEB_SITE"simple.html"));
    RefPtr<BTResourceHandle> aJob1 = BTResourceHandle::create(aResourceRequest1, &aTestResourceHandleClient1, 0, true, false, false);


    std::string aFileName2;
    GetTemporaryUnexistingFileName("/tmp/transfer", ".data", aFileName2);
    TestResourceHandleClient aTestResourceHandleClient2(aFileName2);
    ResourceRequest aResourceRequest2 = ResourceRequest(static_cast<const KURL&> (TEST_WEB_SITE"mirejpeg2.bmp"));
    RefPtr<BTResourceHandle> aJob2 = BTResourceHandle::create(aResourceRequest2, &aTestResourceHandleClient2, 0, false, false, false);


    std::string aFileName3;
    GetTemporaryUnexistingFileName("/tmp/transfer", ".data", aFileName3);
    TestResourceHandleClient aTestResourceHandleClient3(aFileName3);
    ResourceRequest aResourceRequest3 = ResourceRequest(static_cast<const KURL&> (TEST_WEB_SITE"mirejpeg3.bmp"));
    RefPtr<BTResourceHandle> aJob3 = BTResourceHandle::create(aResourceRequest3, &aTestResourceHandleClient3, 0, false, false, false);


    std::string aFileName4;
    GetTemporaryUnexistingFileName("/tmp/transfer", ".data", aFileName4);
    TestResourceHandleClient aTestResourceHandleClient4(aFileName4);
    ResourceRequest aResourceRequest4 = ResourceRequest(static_cast<const KURL&> (TEST_WEB_SITE"mirejpeg4.bmp"));
    RefPtr<BTResourceHandle> aJob4 = BTResourceHandle::create(aResourceRequest4, &aTestResourceHandleClient4, 0, false, false, false);


    gTransferFileCount = 4;

    // Wait for the end of the transfer.
    gTimeout = false;
    WebCore::Timer<NetworkTest> aTimeoutTimer(NULL,&NetworkTest::TimeoutCallback);
    aTimeoutTimer.startOneShot(150); // timeout in 150s

    BIEventLoop* aEventLoop = BAL::getBIEventLoop();
    BIEvent* event;
    while( gTransferFileCount && !gTimeout )
    {
      aEventLoop->WaitEvent(event);
    }

    BALFileComparison aFileComparison;
    TestManager::AssertTrue("Transfer done", !gTransferFileCount );

    std::string aPath = TestManager::GetInstance().getPath() + "NetworkTests/Refs/simple.html";
    bool bEqual = aFileComparison.AreEqual( aFileName1, aPath.c_str());
    TestManager::AssertTrue("File1 as expected", bEqual );

    aPath = TestManager::GetInstance().getPath() + "NetworkTests/Refs/mirejpeg2.bmp";
    bEqual = aFileComparison.AreEqual( aFileName2, aPath.c_str());
    TestManager::AssertTrue("File2 as expected", bEqual );

    aPath = TestManager::GetInstance().getPath() + "NetworkTests/Refs/mirejpeg3.bmp";
    bEqual = aFileComparison.AreEqual( aFileName3, aPath.c_str());
    TestManager::AssertTrue("File3 as expected", bEqual );

    aPath = TestManager::GetInstance().getPath() + "NetworkTests/Refs/mirejpeg4.bmp";
    bEqual = aFileComparison.AreEqual( aFileName4, aPath.c_str());
    TestManager::AssertTrue("File4 as expected", bEqual );


    unlink( aFileName1.c_str() );
    unlink( aFileName2.c_str() );
    unlink( aFileName3.c_str() );
    unlink( aFileName4.c_str() );
  }

  /**
   * Test a POST form transfer
   */
  static void TestNetworkPostTransfer()
  {
    // FIXME Setup a http server to serve the files
    TestManager::AssertTrue("No web server setup for the tests", false);
    return;
    
    const std::string aURL(TEST_WEB_SITE"testpostwithparam.php");
    std::string aPath = TestManager::GetInstance().getPath() + "NetworkTests/Refs/testpostwithparam.html";
    const std::string aRef(aPath);

    BIResourceHandleManager* aResourceHandleManager = getBIResourceHandleManager();
    TestManager::AssertTrue("Transfer Job Manager obtained", aResourceHandleManager != NULL);

    std::string aFileName;
    GetTemporaryUnexistingFileName("/tmp/transfer", ".txt", aFileName);

    TestResourceHandleClient aTestResourceHandleClient(aFileName);

    FormData formData;
    DeprecatedString aParams;
    aParams = "param1=toto&param2=kiki";
    formData.appendData( static_cast<const void *> (aParams.ascii()), aParams.length() );

    ResourceRequest aResourceRequest = ResourceRequest(static_cast<const KURL&> (aURL.c_str()));
    aResourceRequest.setHTTPMethod("POST");
    aResourceRequest.setHTTPReferrer(TEST_WEB_SITE);
    aResourceRequest.setHTTPBody(formData.copy());;

    RefPtr<BTResourceHandle> aResourceHandle = BTResourceHandle::create(aResourceRequest, &aTestResourceHandleClient, 0, false, false, false);

    TestManager::AssertTrue("FormData flatten", aResourceRequest.httpBody()->flattenToString() == "param1=toto&param2=kiki" );
    TestManager::AssertTrue("Form data with correct element", aResourceRequest.httpBody()->elements().size() == 1 );
    TestManager::AssertTrue("Correct Metadata", aResourceRequest.httpReferrer() == TEST_WEB_SITE );
    //TestManager::AssertTrue("Correct Method", aResourceHandle->method() == "POST");

    gTransferFileCount = 1;

    // Wait for the end of the transfer.
    gTimeout = false;
    WebCore::Timer<NetworkTest> aTimeoutTimer(NULL,&NetworkTest::TimeoutCallback);
    aTimeoutTimer.startOneShot(20); // timeout in 20s

    BIEventLoop* aEventLoop = BAL::getBIEventLoop();
    BIEvent* event;
    while( gTransferFileCount && !gTimeout ) {
      aEventLoop->WaitEvent(event);
    }

    TestManager::AssertTrue("Transfer done", !gTransferFileCount );
    BALFileComparison aFileComparison;
    bool bEqual = aFileComparison.AreEqual( aFileName, aRef );
    TestManager::AssertTrue("File as expected", bEqual );

    unlink( aFileName.c_str() );
  }

protected:
  void TimeoutCallback(WebCore::Timer<NetworkTest>* /*timer*/) {
    gTimeout = true;
  }

  /**
   * Function to permform simple transfer test
   */
  static void TestNetworkSimpleTransfer( const std::string& aURL, const std::string& aRef )
  {
    // FIXME Setup a http server to serve the files
    TestManager::AssertTrue("No web server setup for the tests", false);
    return;
    
    BIResourceHandleManager* aResourceHandleManager = getBIResourceHandleManager();
    TestManager::AssertTrue("Transfer Job Manager obtained", aResourceHandleManager != NULL);

    std::string aFileName;
    GetTemporaryUnexistingFileName("/tmp/transfer", ".txt", aFileName);

    TestResourceHandleClient aTestResourceHandleClient(aFileName);
    ResourceRequest aResourceRequest = ResourceRequest(static_cast<const KURL&> (aURL.c_str()));

    RefPtr<BTResourceHandle> aResourceHandle = BTResourceHandle::create(aResourceRequest, &aTestResourceHandleClient, 0, false, false, false);

    gTransferFileCount = 1;

    // Wait for the end of the transfer.
    gTimeout = false;
    WebCore::Timer<NetworkTest> aTimeoutTimer(NULL,&NetworkTest::TimeoutCallback);
    aTimeoutTimer.startOneShot(20); // timeout in 20s

    BIEventLoop* aEventLoop = BAL::getBIEventLoop();
    BIEvent* event;
    while( gTransferFileCount && !gTimeout ) {
      aEventLoop->WaitEvent(event);
    }

    TestManager::AssertTrue("Transfer done", !gTransferFileCount );
    BALFileComparison aFileComparison;
    bool bEqual = aFileComparison.AreEqual( aFileName, aRef );
    TestManager::AssertTrue("File as expected", bEqual );

    unlink( aFileName.c_str() );
  }

};

static TestNode gTestNetworkSimpleTransfer = { "TestNetworkSimpleTransfer", "TestNetworkSimpleTransfer", TestNode::AUTO, NetworkTest::TestNetworkSimpleTransfer, NULL };
static TestNode gTestNetworkSimpleTransferMire = { "TestNetworkSimpleTransferMire", "TestNetworkSimpleTransferMire", TestNode::AUTO, NetworkTest::TestNetworkSimpleTransferMire, NULL };
static TestNode gTestNetworkConcurrentTransfer = { "TestNetworkConcurrentTransfer", "TestNetworkConcurrentTransfer", TestNode::AUTO, NetworkTest::TestNetworkConcurrentTransfer, NULL };
static TestNode gTestNetworkBadURL = { "TestNetworkBadURL", "TestNetworkBadURL", TestNode::AUTO, NetworkTest::TestNetworkBadURL, NULL };
static TestNode gTestNetworkGetTransferWithParameters = { "TestNetworkGetTransferWithParameters", "TestNetworkGetTransferWithParameters", TestNode::AUTO, NetworkTest::TestNetworkGetTransferWithParameters, NULL };
static TestNode gTestNetworkPostTransfer = { "TestNetworkPostTransfer", "TestNetworkPostTransfer", TestNode::AUTO, NetworkTest::TestNetworkPostTransfer, NULL };

TestNode* gNetworkTestNodeList[] = {
    &gTestNetworkSimpleTransfer,
    &gTestNetworkSimpleTransferMire,
    &gTestNetworkConcurrentTransfer,
    &gTestNetworkBadURL,
    &gTestNetworkGetTransferWithParameters,
    &gTestNetworkPostTransfer,
    NULL
};

TestNode gTestSuiteNetwork = {
    "TestNetwork",
    "test Network",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gNetworkTestNodeList 
};
