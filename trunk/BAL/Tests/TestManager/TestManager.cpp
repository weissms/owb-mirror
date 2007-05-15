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

/********************************************************/
/*              Includes                                */
/********************************************************/
#include <stdio.h>
#include <signal.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

#include "TestManager.h"

TestManager* TestManager::gTestManager = NULL;

/**
	* @brief Get singleton instance
	*/
TestManager& TestManager::GetInstance()
{
    if(!gTestManager)
    {
        gTestManager = new TestManager();
    }
    return *gTestManager;
}

/**
	* @brief Constructor
	*/
TestManager::TestManager()
{
  mInteractiveMode = 1; /* default value: interactive mode */
  mStopAtFirstErrorMode = 0 /* default value: don't stop at first error */;
  mTestResult.mTheOkTotal = 0;
  mTestResult.mTheFailureTotal = 0;
}

/**
	* @brief This methods logs a string message.
	*/
int TestManager::LogMessage( const char* aMessage )
{
  printf(aMessage);
	return 0;
}

/**
	* @brief This method logs an error message, formatting it
	* to emphasize the error aspect.
	*/
int TestManager::LogErrorMessage( const char* aMessage )
{
  printf("ERROR: %s\n", aMessage);
	return 0;
}

/**
	* @brief Ask a closed question to user, for checking purpose
	*
	*  In interactive mode, user should enter answer on standard input,
	* after aMessage is displayed on standard output.
	* Increments Oks if Y, increments Failures if N.
	*  In non interactive mode, question will be displayed shorly, and
	* test goes on.
	*/
int TestManager::AskForChecking( const char* aQuestionMessage )
{
	if( GetInstance().IsInteractiveMode() )
	{
		return AskQuestion(aQuestionMessage);
	}
	else
	{
		LogMessage("NOT INTERACTIVE MODE: ");
    LogMessage(aQuestionMessage);
		LogMessage("\nWaiting for 1 second...\n");
		sleep(1);
		LogMessage("Test goes on\n");
		return 0;
	}
}
    
/**
	* @brief This methods asserts a fact, and increments the corresponding
	* counter wheter the assert is true or false.
	*/
int TestManager::AssertTrue( const char* aMessage, int aAssert )
{
	if( aAssert )
	{
			AddOk();
			LogMessage("OK: ");
      LogMessage(aMessage);
			LogMessage("\n");
	}
	else
	{
			AddFailure();
			LogMessage("ERROR. AssertTrue failed: ");
      LogMessage(aMessage);
			LogMessage("\n");
	}
	return 0;
}

/* ************* implementation ********** */
/**
 * @brief Increment ok count of manager and of current context
 */
int TestManager::AddOk()
{
  GetInstance().GetTestResult().mTheOkTotal++;
	return 0;
}

/**
 * @brief Increment failure count of manager and of current context
 */
int TestManager::AddFailure()
{
  GetInstance().GetTestResult().mTheFailureTotal++;
	return 0;
}

/**
	* @brief This methods asks user a question
	*/
int TestManager::AskQuestion( const char* aQuestionMessage )
{
	LogMessage(aQuestionMessage);
	LogMessage(" ? (0=NO, 1=YES) :");
	char cAskResult = getc(stdin);
	if ('1' != cAskResult)
	{
		AddFailure();
	}
	else
	{
		AddOk();
	}
  return 0;
}
