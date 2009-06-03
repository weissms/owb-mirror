/*
 * Copyright (C) 2009 Pleyo.  All rights reserved.
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

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "WebViewPlatformTestsCommon.h"
#include <string.h>

static WebView* webView;

// Custom WebNotificationDelegate
class TestNotificationDelegate : public WebNotificationDelegate {
public:
    TestNotificationDelegate()
        : m_seenStartLoadNotification(false)
        , m_seenLastProgressNotification(false)
        , m_seenFinishedLoadNotification(false)
    {
    }

    ~TestNotificationDelegate()
    {
    }

    virtual void startLoadNotification(WebFrame*)
    {
        m_seenStartLoadNotification = true;
    }

    virtual void progressNotification(WebFrame* webFrame)
    {
        double progress = webFrame->webView()->estimatedProgress();
        CPPUNIT_ASSERT(progress >= 0 && progress <= 1);
        if (progress == 1)
            m_seenLastProgressNotification = true;
    }

    virtual void finishedLoadNotification(WebFrame*)
    {
        m_seenFinishedLoadNotification = true;
    }

    virtual void windowObjectClearNotification(WebFrame*, void*, void*)
    {
    }

    virtual void consoleMessage(WebFrame*, int line, const char *message)
    {
        printf("CONSOLE MESSAGE: line %d: %s\n", line, message);
    }

    virtual bool jsAlert(WebFrame *frame, const char *message)
    {
        printf("Javascript Alert: %s (from frame %p)\n", message, frame);
        return true;
    }

    virtual bool jsConfirm(WebFrame *frame, const char *message)
    {
        printf("Javascript Confirm: %s (from frame %p), answer is 'false' by default.\n", message, frame);
        return true;
    }

    virtual bool jsPrompt(WebFrame *frame, const char *message, const char *defaultValue, char **value)
    {
        printf("Javascript Prompt: %s (from frame %p), answer is 'false' by default.\n", message, frame);
        *value = strdup(defaultValue);
        return true;
    }

    virtual void titleChange(WebFrame*, const char*)
    {
    }

    virtual void didStartLoad(WebFrame*)
    {
    }

    virtual void didCommitLoad(WebFrame*)
    {
    }

    virtual void didFinishLoad(WebFrame*)
    {
    }

    virtual void didFailLoad(WebFrame*)
    {
    }


    bool m_seenStartLoadNotification;
    bool m_seenLastProgressNotification;
    bool m_seenFinishedLoadNotification;
};

static const char* googleFr = "http://www.google.fr";

class WebViewTestSDL : public CPPUNIT_NS::TestCase 
{
    CPPUNIT_TEST_SUITE(WebViewTestSDL);
    CPPUNIT_TEST(WebViewLoadURL);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void)
    {
        webView = createPlatformWebView();
    }

    void tearDown(void)
    {
        delete webView;
    }

protected:
   void WebViewLoadURL()
   {
       webView->mainFrame()->loadURL(googleFr);
       waitEventUntilLoaded();
       const char* url = webView->mainFrame()->url();
       CPPUNIT_ASSERT(!strncmp(googleFr, url, strlen(googleFr)));
   }

   void WebViewNotifiers()
   {
       TestNotificationDelegate* testNotificationDelegate = new TestNotificationDelegate();
       webView->setWebNotificationDelegate(testNotificationDelegate);
       webView->mainFrame()->loadURL(googleFr);
       waitEventUntilLoaded();
       CPPUNIT_ASSERT(testNotificationDelegate->m_seenStartLoadNotification);
       CPPUNIT_ASSERT(testNotificationDelegate->m_seenLastProgressNotification);
       CPPUNIT_ASSERT(testNotificationDelegate->m_seenFinishedLoadNotification);
       // Clear the WebNotificationDelegate.
       webView->setWebNotificationDelegate(0);
       delete testNotificationDelegate;
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WebViewTestSDL);

int main( int ac, char **av )
{
#if PLATFORM(SDL)
    //--- Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    //--- Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener( &result );        

    //--- Add a listener that print dots as test run.
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener( &progress );      

    //--- Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( controller );

    return result.wasSuccessful() ? 0 : 1;
#else
    return 0;
#endif
}
