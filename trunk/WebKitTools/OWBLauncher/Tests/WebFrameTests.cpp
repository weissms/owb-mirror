#include <iostream>

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <WebKit.h>

class WebFrameTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(WebFrameTest);
    CPPUNIT_TEST(testInitialValue);
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
