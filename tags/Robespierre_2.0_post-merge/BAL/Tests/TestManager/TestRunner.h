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
 * @file TestRunner.h
 *
 * Unit Test tree walker
 */
#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

/********************************************************/
/*              Includes                                */
/********************************************************/
#include <stdio.h>
#include <signal.h>
#include <sys/resource.h>
#include <time.h>

#include "TestManager.h"

class TestRunner;

/**
 * Class TestVisitor
 */
class TestVisitor
{
public:
  virtual ~TestVisitor() {};
  virtual void Visit(TestRunner&, TestNode* aNode, int *pbKeepWalking) = 0;
};

/**
 * Class TestRunner
 *
 * Purpose :
 *   handle user commands and redirect them to the test manager.
 *   handle the test tree.
 */
class TestRunner
{
public:
  TestRunner( TestNode* aRoot ) : mTestRoot(aRoot) {}
  
 /**
	* @brief This methods displays results
	*/
  void DisplayResults();
  
  /**
   * @brief This methods displays a test tree,
   * from the given node, or from root if null
   */
  void DisplayTestTree( TestNode* aTestRoot = NULL );

  /**
   * @brief This methods execute a test tree
   * from the given node, or from root if null
   */
  void ExecuteTestTree( TestNode* aTestRoot = NULL );

  /**
    * @brief This methods execute a test node represented with
    * a series of number. for instance, 0 0 0.
    */
  void ExecuteTestNumber( TestNode* aTestRoot, const char* aInput );
  
  /**
    * @brief This methods look for a node, from its name.
    */
  int FindNode( const char* aCommand, TestNode** aNode );

  /**
    * @brief Set interactive mode 
    */
  void SetInteractiveMode( bool b ) { GetTestManager().mInteractiveMode = b; }
  
  /**
    * @brief Set StopAtFirstError 
    */
  void SetStopAtFirstErrorMode( bool b ) { GetTestManager().mStopAtFirstErrorMode = b; }

  /**
    * @brief This method interprets the command typed by the user
    */
  //int InterpretCommand( const char* aInput );
  
  TestManager& GetTestManager() { return TestManager::GetInstance(); }

  int GetWalkTopStack() const { return mWalkTopStack; }
  int GetWalkIndexStack(int i) const { return mWalkIndexStack[i]; }
protected:
  int mWalkIndexStack[256];
  int mWalkTopStack;
  TestNode* mTestRoot;

  void RecursWalkInTree( TestNode* aTestRoot, TestVisitor&, int* abKeepWalking );
  void WalkInTree( TestNode* aTestRoot, TestVisitor&, int* pbKeepWalking );
  
  TestNode* GetSon( TestNode* aCurrentNode, int aNodeIndex );
};

#endif
