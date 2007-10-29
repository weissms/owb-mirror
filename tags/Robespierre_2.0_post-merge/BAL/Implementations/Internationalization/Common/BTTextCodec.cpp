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

#include "config.h"
#include "BTTextCodec.h"
#include "PlatformString.h"
#include "DeprecatedString.h"
#include "CString.h"

namespace WebCore {

const UChar BOM = 0xFEFF;

void BTTextCodec::registerEncodingNames(EncodingNameRegistrar registrar)
{
    registrar("UTF-8", "UTF-8");
}

static std::auto_ptr<TextCodec> newBTTextCodec(const TextEncoding& encoding, const void*)
{
    return std::auto_ptr<TextCodec>(new BTTextCodec(encoding));
}

void BTTextCodec::registerCodecs(TextCodecRegistrar registrar)
{
    registrar("UTF-8", newBTTextCodec, 0);
    //registrar("ISO-8859-8-I", newBTTextCodec, 0);
}

BTTextCodec::BTTextCodec(const TextEncoding& encoding)
    : m_encoding(encoding), m_haveBufferedByte(false)
{
}


BTTextCodec::~BTTextCodec()
{
    m_encoding = 0;
}


String BTTextCodec::decode(const char* bytes, size_t length, bool flush)
{
    if (!length)
        return String();
    
    return String( bytes, length );
}

CString BTTextCodec::encode(const UChar* characters, size_t length, bool allowEntities)
{
    char* bytes;
    CString string = CString::newUninitialized(length * 2, bytes);
    return string;
}


} // namespace WebCore
