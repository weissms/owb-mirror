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
#include "BIMath.h"
#include "BIThread.h"

#include "TestManager/TestManager.h"


using namespace BAL;

static pthread_key_t registeredThreadKey;
static pthread_mutex_t interpreterLock;
static int interpreterLockCount =0;

static void destroyRegisteredThread(void *)
{
    printf( "destroyRegisteredThread\n");
}

static void initializeRegisteredThreadKey()
{
    interpreterLockCount++;
    pthread_mutex_lock(&interpreterLock);
    pthread_key_create(&registeredThreadKey, destroyRegisteredThread);
    pthread_mutex_unlock(&interpreterLock);
    interpreterLockCount--;
}

class PosixTest {
public:

    static void setUp()
    {
        // do nothing
    }

    static void tearDown()
    {
        // do nothing
    }

    static void testCeilf()
    {
        setUp();

        TestManager::AssertTrue("ceilf( -1.542) == -1", ceilf( -1.542) == -1 );
        TestManager::AssertTrue("ceilf( 100.56) == 101", ceilf( 100.56) == 101  );
        TestManager::AssertTrue("ceilf( 360.99) == 361", ceilf( 360.99) == 361 );

        tearDown();
    }

    static void testCeil()
    {
        setUp();

        TestManager::AssertTrue("ceil( -1.542) == -1", ceil( -1.542) == -1 );
        TestManager::AssertTrue("ceil( 100.56) == 101", ceil( 100.56) == 101  );
        TestManager::AssertTrue("ceil( 360.99) == 361", ceil( 360.99) == 361 );

        tearDown();
    }

    static void testFmodf()
    {
        setUp();

        TestManager::AssertTrue("fmodf( -1, 2 ) == -1", fmodf( -1, 2 ) == -1 );
        TestManager::AssertTrue("fmodf( 0, 8 ) == 0", fmodf( 0, 8 ) == 0  );

        tearDown();
    }

    static void testLroundf()
    {
        setUp();

        TestManager::AssertTrue("lroundf( 150.5 ) == 151", lroundf( 150.5 ) == 151 );
        TestManager::AssertTrue("lroundf( 150.4 ) == 150", lroundf( 150.4 ) == 150 );

        tearDown();
    }

    static void testLround()
    {
        setUp();

        TestManager::AssertTrue("lround( 150.5 ) == 151", lround( 150.5 ) == 151 );
        TestManager::AssertTrue("lround( 150.4 ) == 150", lround( 150.4 ) == 150 );

        tearDown();
    }

    static void testFloorf()
    {
        setUp();

        TestManager::AssertTrue("floorf( 150.5 ) == 150", floorf( 150.5 ) == 150 );
        TestManager::AssertTrue("floorf( 150.4 ) == 150", floorf( 150.4 ) == 150 );

        tearDown();
    }

    static void testFloor()
    {
        setUp();

        TestManager::AssertTrue("floor( 150.5 ) == 150", floor( 150.5 ) == 150 );
        TestManager::AssertTrue("floor( 150.4 ) == 150", floor( 150.4 ) == 150 );

        tearDown();
    }

    static void testRound()
    {
        setUp();

        TestManager::AssertTrue("round( 150.5 ) == 151", round( 150.5 ) == 151 );
        TestManager::AssertTrue("round( 150.4 ) == 150", round( 150.4 ) == 150 );

        tearDown();
    }

    static void testRoundf()
    {
        setUp();

        TestManager::AssertTrue("roundf( 150.5 ) == 151", roundf( 150.5 ) == 151 );
        TestManager::AssertTrue("roundf( 150.4 ) == 150", roundf( 150.4 ) == 150 );

        tearDown();
    }

    static void testNextAfter()
    {
        setUp();

        TestManager::AssertTrue("nextafter( 1, 2 ) == 1.0000000000000002", nextafter( 1, 2 ) == 1.0000000000000002 );

        tearDown();
    }
    static void testNextAfterf()
    {
        setUp();

        TestManager::AssertTrue("nextafterf( 1, 2 ) == 1.00000011920928961", nextafterf( 1, 2 ) == 1.0000001192092896 );

        tearDown();
    }

    static void testIsFinite()
    {
        setUp();

        TestManager::AssertTrue("isfinite( -150 ) == 1", isfinite( -150 ) == 1 );
        TestManager::AssertTrue("isfinite( HUGE_VAL ) == 0", isfinite( HUGE_VAL ) == 0 );
        TestManager::AssertTrue("isfinite( HUGE_VAL + 1 ) == 0", isfinite( HUGE_VAL + 1 ) == 0 );

        tearDown();
    }

    static void testFabs()
    {
        setUp();

        TestManager::AssertTrue("fabs( -150 ) == 150", fabs( -150 ) == 150 );
        TestManager::AssertTrue("fabs( 150 ) == 150", fabs( 150 ) == 150 );
        TestManager::AssertTrue("fabs( 0 ) == 0", fabs( 0 ) == 0 );

        tearDown();
    }

    static void testCopysign()
    {
        setUp();

        TestManager::AssertTrue("copysign( 150, -1.9 ) == -150", copysign( 150, -1.9 ) == -150 );

        tearDown();
    }

    static void testExp()
    {
        setUp();

        TestManager::AssertTrue("exp( 0 ) == 1 ", exp( 0 ) == 1 );

        tearDown();
    }

    static void testLog()
    {
        setUp();

        TestManager::AssertTrue("log( 1 ) == 0 ", log( 1 ) == 0 );
        TestManager::AssertTrue("log( exp(1) ) == 1", log( exp(1) ) == 1 );

        tearDown();
    }

    static void testSqrt()
    {
        setUp();

        TestManager::AssertTrue("sqrt( 0 ) == 0 ", sqrt( 0 ) == 0 );
        TestManager::AssertTrue("sqrt( 25 ) == 0 ", sqrt( 25 ) == 5 );

        tearDown();
    }

    static void testAcos()
    {
        setUp();

        TestManager::AssertTrue("acos( 1 ) == 0  ", acos( 1 ) == 0 );

        tearDown();
    }

    static void testAsin()
    {
        setUp();

        TestManager::AssertTrue("asin( 0 ) == 0 ", asin( 0 ) == 0 );

        tearDown();
    }

    static void testAtan()
    {
        setUp();

        TestManager::AssertTrue("atan( 0 ) == 0 ", atan( 0 ) == 0 );

        tearDown();
    }

    static void testAtan2()
    {
        setUp();

        TestManager::AssertTrue("atan2( 0, 0 ) == 0 ", atan2( 0, 0 ) == 0 );

        tearDown();
    }

    static void testCos()
    {
        setUp();

        TestManager::AssertTrue("cos( 0 ) == 1 ", cos( 0 ) == 1 );

        tearDown();
    }

    static void testSin()
    {
        setUp();

        TestManager::AssertTrue("sin( 0 ) == 0 ", sin( 0 ) == 0 );

        tearDown();
    }

    static void testTan()
    {
        setUp();

        TestManager::AssertTrue("tan( 0 ) == 0 ", tan( 0 ) == 0 );

        tearDown();
    }

    static void testSignBit()
    {
        setUp();

        TestManager::AssertTrue("signbit( 1 ) == 0 ", signbit( 1 ) == 0 );

        tearDown();
    }

    static void testPow()
    {
        setUp();

        TestManager::AssertTrue("pow( 1, 5 ) == 0 ", pow( 1, 5 ) == 1 );

        tearDown();
    }

    static void testFmod()
    {
        setUp();

        TestManager::AssertTrue("fmod(2,1) == 0 ", fmod( 2,1 ) == 0 );

        tearDown();
    }

    static void testLog10()
    {
        setUp();

        TestManager::AssertTrue("log10( 1 ) == 0 ", log10( 1 ) == 0 );

        tearDown();
    }

    static void testIsNan()
    {
        setUp();

        TestManager::AssertTrue("isnan(1) == 0 ", isnan( 1 ) == 0 );

        tearDown();
    }

    static void testIsInf()
    {
        setUp();
        TestManager::AssertTrue("isinf( 1 ) == 0 ", isinf( 1 ) == 0 );
        tearDown();
    }

    static void testRemainder()
    {
        setUp();

        TestManager::AssertTrue("remainder(1, 2) == 0 ", remainder( 1, 2 ) == 1 );

        tearDown();
    }

    static void testThread()
    {
        pthread_mutexattr_t attr;

        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init(&interpreterLock, &attr);

        pthread_once_t registeredThreadKeyOnce = PTHREAD_ONCE_INIT;

        pthread_once(&registeredThreadKeyOnce, initializeRegisteredThreadKey);

        pthread_t pthread;
        if (!pthread_getspecific(registeredThreadKey)) {
            pthread = pthread_self();
        }

        TestManager::AssertTrue("test thread ", interpreterLockCount == 0 );

    }

private:

};

static TestNode gtestCeilf =
 { "testCeilf", "testCeilf", TestNode::AUTO, PosixTest::testCeilf, NULL };
static TestNode gtestCeil =
 { "testCeil", "testCeil", TestNode::AUTO, PosixTest::testCeil, NULL };
static TestNode gtestFmodf =
 { "testFmodf", "testFmodf", TestNode::AUTO, PosixTest::testFmodf, NULL };
static TestNode gtestLroundf =
 { "testLroundf", "testLroundf", TestNode::AUTO, PosixTest::testLroundf, NULL };
static TestNode gtestLround =
 { "testLround", "testLround", TestNode::AUTO, PosixTest::testLround, NULL };
static TestNode gtestFloorf =
 { "testFloorf", "testFloorf", TestNode::AUTO, PosixTest::testFloorf, NULL };
static TestNode gtestFloor =
 { "testFloor", "testFloor", TestNode::AUTO, PosixTest::testFloor, NULL };
static TestNode gtestRound =
 { "testRound", "testRound", TestNode::AUTO, PosixTest::testRound, NULL };
static TestNode gtestRoundf =
 { "testRoundf", "testRoundf", TestNode::AUTO, PosixTest::testRoundf, NULL };
static TestNode gtestNextAfter =
 { "testNextAfter", "testNextAfter", TestNode::AUTO, PosixTest::testNextAfter, NULL };
static TestNode gtestNextAfterf =
 { "testNextAfterf", "testNextAfterf", TestNode::AUTO, PosixTest::testNextAfterf, NULL };
static TestNode gtestIsFinite =
 { "testIsFinite", "testIsFinite", TestNode::AUTO, PosixTest::testIsFinite, NULL };
static TestNode gtestFabs =
 { "testFabs", "testFabs", TestNode::AUTO, PosixTest::testFabs, NULL };
static TestNode gtestCopysign =
 { "testCopysign", "testCopysign", TestNode::AUTO, PosixTest::testCopysign, NULL };
static TestNode gtestExp =
 { "testExp", "testExp", TestNode::AUTO, PosixTest::testExp, NULL };
static TestNode gtestLog =
 { "testLog", "testLog", TestNode::AUTO, PosixTest::testLog, NULL };
static TestNode gtestSqrt =
 { "testSqrt", "testSqrt", TestNode::AUTO, PosixTest::testSqrt, NULL };
static TestNode gtestAcos =
 { "testAcos", "testAcos", TestNode::AUTO, PosixTest::testAcos, NULL };
static TestNode gtestAsin =
 { "testAsin", "testAsin", TestNode::AUTO, PosixTest::testAsin, NULL };
static TestNode gtestAtan =
 { "testAtan", "testAtan", TestNode::AUTO, PosixTest::testAtan, NULL };
static TestNode gtestAtan2 =
 { "testAtan2", "testAtan2", TestNode::AUTO, PosixTest::testAtan2, NULL };
static TestNode gtestCos =
 { "testCos", "testCos", TestNode::AUTO, PosixTest::testCos, NULL };
static TestNode gtestSin =
 { "testSin", "testSin", TestNode::AUTO, PosixTest::testSin, NULL };
static TestNode gtestTan =
 { "testTan", "testTan", TestNode::AUTO, PosixTest::testTan, NULL };
static TestNode gtestSignBit =
 { "testSignBit", "testSignBit", TestNode::AUTO, PosixTest::testSignBit, NULL };
static TestNode gtestPow =
 { "testPow", "testPow", TestNode::AUTO, PosixTest::testPow, NULL };
static TestNode gtestFmod =
 { "testFmod", "testFmod", TestNode::AUTO, PosixTest::testFmod, NULL };
static TestNode gtestLog10 =
 { "testLog10", "testLog10", TestNode::AUTO, PosixTest::testLog10, NULL };
static TestNode gtestIsNan =
 { "testIsNan", "testIsNan", TestNode::AUTO, PosixTest::testIsNan, NULL };
static TestNode gtestIsInf =
 { "testIsInf", "testIsInf", TestNode::AUTO, PosixTest::testIsInf, NULL };
static TestNode gtestRemainder =
 { "testRemainder", "testRemainder", TestNode::AUTO, PosixTest::testRemainder, NULL };
static TestNode gtestThread =
 { "testThread", "testThread", TestNode::AUTO, PosixTest::testThread, NULL };


TestNode* gPosixTestNodeList[] = {
  &gtestCeilf,
  &gtestCeil,
  &gtestFmodf,
  &gtestLroundf,
  &gtestLround,
  &gtestFloorf,
  &gtestFloor,
  &gtestRound,
  &gtestRoundf,
  &gtestNextAfter,
  &gtestNextAfterf,
  &gtestIsFinite,
  &gtestFabs,
  &gtestCopysign,
  &gtestExp,
  &gtestLog,
  &gtestSqrt,
  &gtestAcos,
  &gtestAsin,
  &gtestAtan,
  &gtestAtan2,
  &gtestCos,
  &gtestSin,
  &gtestTan,
  &gtestSignBit,
  &gtestPow,
  &gtestFmod,
  &gtestLog10,
  &gtestIsNan,
  &gtestIsInf,
  &gtestRemainder,
  &gtestThread,
	NULL
};

TestNode gTestSuitePosix = {
    "TestPosix",
    "test posix",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gPosixTestNodeList
};
