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
#ifndef BALFILE_WRITER_H_
#define BALFILE_WRITER_H_

/**
  * @file BALFileWriter
  *
	* @brief write into a file
	*/
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>

#include "DeprecatedArray.h" // FIXME should diseappear

namespace BALTest
{

class BALFileWriter
{
public:
	// Constructors opens the file
	BALFileWriter( const std::string& );

	// Destructor closes the file if not done already
	~BALFileWriter();

	// Returns the number of bytes read
	int Write( const char* aData, int aSize );

  // Closes the file
  void Close();
  
	bool IsOpened() const { return (mStream != NULL); }
        
        int size() { return m_size; }
        
private:
        int m_size;
  
protected:
	FILE *mStream;
};

inline BALFileWriter::BALFileWriter( const std::string& aName ) : mStream(NULL) {
	mStream = fopen(aName.c_str(), "w");
        m_size = 0;
}

inline BALFileWriter::~BALFileWriter()
{
  Close();
}

inline void BALFileWriter::Close()
{
	if( mStream ) {
		fclose(mStream);
    mStream = NULL;
	}
}

inline int BALFileWriter::Write( const char* aData, int aSize )
{
	if(mStream) {
            m_size += fwrite( aData, sizeof(char), aSize, mStream );
            return m_size;
	}
	return 0;
}

}
#endif

