#include <iostream>

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <WebKit.h>

class WebViewTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(WebViewTest);
    CPPUNIT_TEST(testInitialValue);
    CPPUNIT_TEST(testInitWithFrame);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void) {}
    void tearDown(void) {} 

protected:
    void testInitialValue()
    {
        WebView* view = WebView::createInstance();
        CPPUNIT_ASSERT(view);
        CPPUNIT_ASSERT(!view->mainFrame());
        delete view;
    }

    void testInitWithFrame()
    {
        WebView* view = WebView::createInstance();
        BalRectangle dummyRectangle;
        const char* foobarGroup = "foobarGroup";
        const char* foobarFrame = "foobarFrame";
        view->initWithFrame(dummyRectangle, foobarFrame, foobarGroup);
        CPPUNIT_ASSERT(!strncmp(foobarGroup, view->groupName(), strlen(foobarGroup)));
        CPPUNIT_ASSERT(!strncmp(foobarFrame, view->mainFrame()->name(), strlen(foobarFrame)));
        delete foobarGroup;
        delete foobarFrame;
        delete view;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(WebViewTest);

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
