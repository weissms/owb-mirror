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
#ifndef BALFILE_COMPARISON_H_
#define BALFILE_COMPARISON_H_

/**
  * @file BALFileComparison.h
  *
	* @brief compare two files
	*/
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>

#include "BALFileReader.h"
using namespace WTF;

namespace BALTest
{

/**
 * BAL File Comparison
 *
 * Compare two files
 */
class BALFileComparison
{
public:
	/**
	 * Returns true if equal, false in any other case.
     * Based on Vectors. operator== was added by Origyn in Vector.h
	 */
	bool AreEqual( const std::string& aFileName1, const std::string& aFileName2 );
};

inline bool BALFileComparison::AreEqual( const std::string& aFileName1, const std::string& aFileName2 )
{
	BALFileReader aR1( aFileName1 );
	BALFileReader aR2( aFileName2 );
	if( aR1.IsOpened() && aR2.IsOpened() )
	{
		Vector<char> a1, a2;
		aR1.Read(a1);
		aR2.Read(a2);
        return (a1 == a2);
	}
	TestManager::AssertTrue("First file is opened", aR1.IsOpened());
	TestManager::AssertTrue("Second file is opened", aR2.IsOpened());
	return false;
}

}

#endif
