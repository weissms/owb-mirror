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

#ifndef BTTextCodec_h
#define BTTextCodec_h

#include "TextCodec.h"
#include "TextEncoding.h"


namespace WebCore {

    class BTTextCodec : public TextCodec {
    public:
        /**
        * register Encoding names
        */
        static void registerEncodingNames(EncodingNameRegistrar);
        /**
        * register codecs
        */
        static void registerCodecs(TextCodecRegistrar);

        /**
        * constructor
        */
        BTTextCodec(const TextEncoding&);
        /**
        * destructor
        */
        virtual ~BTTextCodec();

        /**
        * decode text
        */
        virtual String decode(const char*, size_t length, bool flush = false);
        /**
        * encode text
        */
        virtual CString encode(const UChar*, size_t length, bool allowEntities = false);

    private:
        TextEncoding m_encoding;
        bool m_haveBufferedByte;
        unsigned char m_bufferedByte;
    };

} // namespace WebCore

#endif // TextCodecICU_h
