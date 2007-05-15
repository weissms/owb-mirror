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

/**
 * @file TestManager.h
 *
 * Manager of all Unit Tests
 */
#ifndef TEST_MANAGER_H
#define TEST_MANAGER_H

#include <stdio.h>
#include <signal.h>
#include <sys/resource.h>
#include <time.h>
#include <string>
using namespace std;

typedef void (*tTestCase_Execute)(void);

/**
 * The TestNode structure
 */
class TestNode
{
public:
    char* msName;
    char* msPurpose;
    enum EnumInteractive { AUTO, FUNCTIONALY_INTERACTIVE, TEST_SUITE };
    EnumInteractive meFunctionalyInteractive;
    tTestCase_Execute mExecuteFunction;
		TestNode** mTestNodeVector;  /* null terminated list of test node */
};

/**
 * Class TestResult
 */
class TestResult
{
public:
  TestResult() : mTheOkTotal(0), mTheFailureTotal(0) {};
  
  int mTheOkTotal;
  int mTheFailureTotal;
};

/**
 * Class TestManager
 * Pattern Singleton
 */
class TestManager
{
public:
  static TestManager& GetInstance();
  
  static int AssertTrue( const char* aMessage, int aAssert );
  static int AddOk();
  static int AddFailure();
  static int AskForChecking( const char* aQuestionMessage );
  static int AskQuestion( const char* aQuestionMessage );
  
  static int LogMessage( const char* aMessage );
  static int LogErrorMessage( const char* aMessage );

  TestResult& GetTestResult() { return mTestResult; }
  bool IsInteractiveMode() { return mInteractiveMode; }
  bool IsStopAtFirstErrorMode() { return mStopAtFirstErrorMode; }
  
  string getPath() { return mPath; }
  void setPath( string aPath ) { mPath = aPath; }
  
private:
  TestManager();

friend class TestRunner;
  bool mInteractiveMode; /* default value: interactive mode */
  bool mStopAtFirstErrorMode; /* default value: don't stop at first error */;
  TestResult mTestResult; /* global test result */
  string mPath;
  
  static TestManager* gTestManager; // singleton
};

#endif
