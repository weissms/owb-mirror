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
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestPath.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <iostream>
#include <stdexcept>

#include "BALComplex.h"

class ComplexNumberTests : public CPPUNIT_NS::TestFixture  {

  CPPUNIT_TEST_SUITE( ComplexNumberTests );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testEquality );
  CPPUNIT_TEST( testAddition );
  CPPUNIT_TEST_SUITE_END();

public: 

  void testConstructor()
  {
    BALComplex m_10_1( 10, 1 );
    CPPUNIT_ASSERT_EQUAL( 10, (int) m_10_1.realPart() );
    CPPUNIT_ASSERT_EQUAL( 1, (int) m_10_1.imaginaryPart() );
  }

  void testEquality()
  {
    BALComplex m_10_1( 10, 1 );
    BALComplex m_1_10( 1, 10 );
    CPPUNIT_ASSERT( m_10_1 == m_10_1 );
    CPPUNIT_ASSERT( !(m_10_1 == m_1_10) );
    CPPUNIT_ASSERT( m_10_1 != m_1_10 );
  }

  void testAddition()
  {
    CPPUNIT_ASSERT( BALComplex(10, 1) + BALComplex(1, 2) == BALComplex(11, 3) );
  }

};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ComplexNumberTests, "All Tests" );
