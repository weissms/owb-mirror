#include <iostream>

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <WebKit.h>
#include <string.h>

class WebFrameTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(WebFrameTest);
    CPPUNIT_TEST(testInitialValue);
    CPPUNIT_TEST(testWebViewWebFrame);
    CPPUNIT_TEST(testSettingWebFrameName);
    CPPUNIT_TEST(testSettingWebViewWebFrameName);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void) {}
    void tearDown(void) {} 

protected:
    void testInitialValue()
    {
        WebFrame* frame = WebFrame::createInstance();
        CPPUNIT_ASSERT(frame);
        CPPUNIT_ASSERT(!frame->webView());
        CPPUNIT_ASSERT(!frame->parentFrame());
        delete frame;
    }

    void testWebViewWebFrame()
    {
        WebView* webView = WebView::createInstance();
        BalRectangle dummyRect;
        const char* foobarFrameName = "foobarFrame";
        webView->initWithFrame(dummyRect, foobarFrameName, "");
        WebFrame* webFrame = webView->mainFrame();
        CPPUNIT_ASSERT(webFrame->webView() == webView);
        const char* webFrameName = webFrame->name();
        CPPUNIT_ASSERT(!strncmp(foobarFrameName, webFrameName, strlen(foobarFrameName)));
        WebFrame* foundFrame = webFrame->findFrameNamed(foobarFrameName);
        CPPUNIT_ASSERT(foundFrame == webFrame);
        delete webFrameName;
        delete webView;
    }

    void testSettingWebFrameName()
    {
        WebFrame* webFrame = WebFrame::createInstance();
        const char* foobarFrameName = "foorbarFrame";
        webFrame->setName(foobarFrameName);
        const char* webFrameName = webFrame->name();
        // FIXME: This one fails because the WebFrame has no associated Frame.
        CPPUNIT_ASSERT(!strncmp("", webFrameName, 1));
        delete webFrameName;
        delete webFrame;
    }

    void testSettingWebViewWebFrameName()
    {
        WebView* webView = WebView::createInstance();
        BalRectangle dummyRect;
        webView->initWithFrame(dummyRect, "", "");
        WebFrame* webFrame = webView->mainFrame();
        const char* webFrameName = webFrame->name();
        CPPUNIT_ASSERT(!strncmp("", webFrameName, 1));
        const char* foobarFrameName = "foobarFrame";
        webFrame->setName(foobarFrameName);
        delete webFrameName;
        webFrameName = webFrame->name();
        CPPUNIT_ASSERT(!strncmp(foobarFrameName, webFrameName, strlen(foobarFrameName)));
        delete webFrameName;
        delete webView;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WebFrameTest);

int main( int ac, char **av )
{
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
}
