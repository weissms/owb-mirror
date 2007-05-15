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
#ifndef BALFILE_READER_H_
#define BALFILE_READER_H_

/**
  * @file BALFileReader
	* @brief read a file and get data in a DeprecatedArray
	*/
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include "wtf/Vector.h"
#include "TestManager/TestManager.h"

namespace BALTest
{

class BALFileReader
{
public:
	// Constructors opens the file
	BALFileReader( const std::string& );

	// Destructor closes the file
	~BALFileReader();

	// Returns the number of bytes read
	int Read(Vector<char>&);

	bool IsOpened() const { return (mStream != NULL); }
protected:
	FILE *mStream;
	struct stat mBuf;
};

inline BALFileReader::BALFileReader( const std::string& aName ) : mStream(NULL) {
	// get data associated with lpszPathName
	int result = stat(aName.c_str(), &mBuf);
	if(result == 0) {
		mStream = fopen(aName.c_str(), "rb");
	}
}

inline BALFileReader::~BALFileReader()
{
	if( mStream ) {
		fclose(mStream);
	}
}

inline int BALFileReader::Read( Vector<char>& a )
{
	if(mStream) {
		a.resize( mBuf.st_size );

		/*int aRead = */fread(a.data(), sizeof(char), mBuf.st_size, mStream); // TO DO should loop here
		//printf("Read %d, size %ld\n", aRead, mBuf.st_size);
		return mBuf.st_size;
	}
	return 0;
}

}
#endif

