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
#include <unistd.h>

#include "wtf/Vector.h"
#include "vector"
#include "TestManager/TestManager.h"

class MyClass 
{
public:
  void foo();
  
private:
  int mA;
};

class TypeTest {
public:
  /**
   */
  static void TestVector()
  {
 }
  
  static void TestCharVector()
  {
 }
 
 static void Testvector()
 {
  {
    std::vector<char> a, b;
    a.push_back( 'z' );
    b.push_back( 'z' );
    TestManager::AssertTrue( "a and b equal", a == b );
  }
  {
    MyClass z;
    std::vector<MyClass> a, b;
    a.push_back( z );
    b.push_back( z );
  }
 }
 
};

static TestNode gTestVector = { "TestVector", "TestVector",
  TestNode::AUTO, TypeTest::TestVector, NULL };
static TestNode gTestvector = { "Testvector", "Testvector",
  TestNode::AUTO, TypeTest::Testvector, NULL };
static TestNode gTestCharVector = { "TestCharVector", "TestCharVector",
  TestNode::AUTO, TypeTest::TestCharVector, NULL };

TestNode* gTypesTestNodeList[] = {
  &gTestVector,
  &gTestvector,
  &gTestCharVector,
	NULL
};

TestNode gTestSuiteTypes = {
    "TestTypes",
    "test types",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
		gTypesTestNodeList 
};
