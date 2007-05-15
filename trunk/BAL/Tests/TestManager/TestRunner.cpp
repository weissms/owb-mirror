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
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/resource.h>
#include <string>

#include "TestManager.h"
#include "TestRunner.h"

class DisplayVisitor : public TestVisitor
{
public:
  virtual void Visit(TestRunner&, TestNode* aNode, int *pbKeepWalking);
};
  
class ExecuteVisitor : public TestVisitor
{
public:
  virtual void Visit(TestRunner&, TestNode* aNode, int *pbKeepWalking);
};
  
class NodeFinderVisitor : public TestVisitor
{
public:
  virtual void Visit(TestRunner&, TestNode* aNode, int *pbKeepWalking);
protected:
friend class TestRunner;
  const char* sNodeNameToFind;
  TestNode* gNodeFound;
};
  
/**
	* @brief This methods walks in the node tree
	*/
void TestRunner::WalkInTree( TestNode* aTestRoot, TestVisitor& aVisitor, int* pbKeepWalking )
{
	mWalkTopStack = 1;
	mWalkIndexStack[0] = 0;
	TestRunner::RecursWalkInTree( aTestRoot, aVisitor, pbKeepWalking );
}

/**
	* @brief This methods walks recursively in the node tree, calling the callback
	* in a prefix way.
	*/
void TestRunner::RecursWalkInTree( TestNode* aTestRoot, TestVisitor& aVisitor, int* abKeepWalking )
{
	int bKeepWalking = 1;
	aVisitor.Visit( *this, aTestRoot, &bKeepWalking ); /* prefixed call (depth first search) */
	
	if( bKeepWalking )
	{
		if( aTestRoot->mTestNodeVector )
		{
			int nIndex=0;
			TestNode* aTestNode = (TestNode*)aTestRoot->mTestNodeVector[nIndex];
			mWalkTopStack++;
			mWalkIndexStack[mWalkTopStack-1] = 0;
			while( aTestNode )
			{
				RecursWalkInTree( aTestNode, aVisitor, &bKeepWalking );
				if( !bKeepWalking )
					break;
				nIndex++;
				aTestNode = (TestNode*)aTestRoot->mTestNodeVector[nIndex];
				mWalkIndexStack[mWalkTopStack-1]++;
			}
			mWalkTopStack--;
		}
	}
	
	if( !bKeepWalking )
	{
		/* tell caller to stop walking */
		*abKeepWalking = 0;
	}
}

/**
	* @brief This methods displays results
	*/
void TestRunner::DisplayResults()
{
	printf("Number of OK: \t%d\n", GetTestManager().mTestResult.mTheOkTotal);
	printf("Number of Failure: \t%d\n", GetTestManager().mTestResult.mTheFailureTotal);
}

/**
	* @brief This methods is visits to display node tree.
	*/
void DisplayVisitor::Visit(TestRunner& aTestRunner, TestNode* aTestNode, int* bKeepWalking)
{
	std::string sTab;
	
	*bKeepWalking = 1;
	for(int i=0; i < aTestRunner.GetWalkTopStack(); ++i )
	{
		sTab += ' ';
	}
	
  printf("%s%s\n", sTab.c_str(), aTestNode->msName);
}

/**
	* @brief This methods visits to execute a node.
	*   If function requires interactivity, and mode is not interactive,
	*   then function is not executed.
	*   If test fails, and mode "stopatfirsterror" is activated, then,
	*   it stops execution of next tests.
	*/
void ExecuteVisitor::Visit(TestRunner& aTestRunner, TestNode* aTestNode, int* bKeepWalking)
{
	*bKeepWalking = 1;
	if( aTestNode->mExecuteFunction )
	{
		if( !aTestRunner.GetTestManager().IsInteractiveMode() &&
        (aTestNode->meFunctionalyInteractive == TestNode::FUNCTIONALY_INTERACTIVE) )
		{
			printf("TESTCASE '%s' NOT EXECUTED (requires interactivity)\n", aTestNode->msName);
		}
		else
		{
			int aFailureTotal = aTestRunner.GetTestManager().GetTestResult().mTheFailureTotal;

			printf("=> Executing '%s'\n", aTestNode->msName);
			
			aTestNode->mExecuteFunction();
		
			printf("=> Done\n", aTestNode->msName);
			
			/* if test case has failed, and "stop at first error" mode is set, stops */
			if( aTestRunner.GetTestManager().IsStopAtFirstErrorMode() )
			{
				if( aFailureTotal != aTestRunner.GetTestManager().GetTestResult().mTheFailureTotal )
				{
					*bKeepWalking = 0; /* stop executing more tests */
					printf("TESTCASE '%s' has failed\n", aTestNode->msName);
				}
			}
		}
	}
	else
	{
		; /* do nothing: it's a test suite */
	}
}

/**
	* @brief This methods visits to find a given test node.
	*/
void NodeFinderVisitor::Visit(TestRunner&, TestNode* aTestNode, int* bKeepWalking)
{
	if( strcmp( aTestNode->msName, sNodeNameToFind ) == 0 )
	{
		*bKeepWalking = 0;
		gNodeFound = aTestNode;
	}
}

/**
	* @brief This methods displays a test tree
	*/
void TestRunner::DisplayTestTree( TestNode* aTestRoot )
{
	int bKeepWalking = 1;
  DisplayVisitor aDisplayVisitor;
	WalkInTree( aTestRoot ? aTestRoot : mTestRoot, aDisplayVisitor, &bKeepWalking );
}

/**
	* @brief This methods execute a test tree
	*/
void TestRunner::ExecuteTestTree( TestNode* aTestRoot )
{
	int bKeepWalking = 1;
  printf("EXECUTE TEST\n");
	
  ExecuteVisitor aVisitor;
	WalkInTree( aTestRoot ? aTestRoot : mTestRoot, aVisitor, &bKeepWalking );
}

/**
	* @brief This methods access a given son of a test node.
	*  It is implemented with a loop to ensure that index is
	*  ok.
	*/
TestNode* TestRunner::GetSon( TestNode* aCurrentNode, int aNodeIndex )
{
	if( aCurrentNode->mTestNodeVector )
	{
		int nIndex = 0;
		while( aCurrentNode->mTestNodeVector[nIndex] && nIndex < aNodeIndex )
		{
			nIndex++;
		}
		return (TestNode*)aCurrentNode->mTestNodeVector[nIndex];
	}
	else
	{
		return NULL;
	}
}

/**
	* @brief This methods look for a node, from its name.
	*/
int TestRunner::FindNode( const char* aCommand, TestNode** aNode )
{
	int bKeepWalking = 1;
	*aNode = NULL;
	
  NodeFinderVisitor aVisitor;
  
	/* set the name to find */
	aVisitor.sNodeNameToFind = aCommand;
	aVisitor.gNodeFound = NULL;

	/* walk on tree to find it */
	WalkInTree( mTestRoot, aVisitor, &bKeepWalking );
	
	if( !bKeepWalking )
	{
		/* a test node has been found */
		*aNode = aVisitor.gNodeFound;
		return 1;
	}
	
	return 0;
}
