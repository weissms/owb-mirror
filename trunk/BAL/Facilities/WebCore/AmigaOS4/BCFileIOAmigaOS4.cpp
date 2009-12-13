/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
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
#include "BCFileIOAmigaOS4.h"
#include "owb-config.h"
#include "CString.h"


extern char* utf8ToAmiga(const char*);

namespace WebCore {

File::File(const String path)
    : m_file(0)
    , m_filePath(path)
{
}

File::~File()
{
    close();
}

int File::open(char openType)
{
    char* amigaPath = utf8ToAmiga(m_filePath.utf8().data());

    if (openType == 'w')
        m_file = ::fopen(amigaPath, "w");
    else if (openType == 'r')
        m_file = ::fopen(amigaPath, "r");

    free(amigaPath);

    if (m_file) {
        setvbuf(m_file, 0, _IOFBF, 64 * 1024);
        return 0;
    }

    return -1;
}

void File::close()
{
    if (m_file)
        ::fclose(m_file);
    m_file = 0;
}

char* File::read(size_t size)
{
    char* readData = new char[size + 1];
    ::fread(readData, size, 1, m_file);
    readData[size] = '\0';
    return readData;
}

void File::write(String dataToWrite)
{
    ::fwrite(dataToWrite.utf8().data(), dataToWrite.length(), 1, m_file);
}

void File::write(const void* data, size_t length)
{
    ::fwrite(data, length, 1, m_file);
}

int File::getSize()
{
    int fileSize, current;

    //save the current offset
    current = ftell(m_file);

    //save the size
    fseek(m_file, 0, SEEK_END);
    fileSize = ftell(m_file);

    //go back to the previous offset
    fseek(m_file, current, SEEK_SET);

    return fileSize;
}

}
