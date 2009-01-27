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

#ifndef TextCodecICU_h
#define TextCodecICU_h

#include "TextCodec.h"
#include "TextEncoding.h"

typedef struct UConverter UConverter;

namespace OWBAL {

    class TextCodecICU : public TextCodec {
    public:
        static void registerBaseEncodingNames(EncodingNameRegistrar);
        static void registerBaseCodecs(TextCodecRegistrar);

        static void registerExtendedEncodingNames(EncodingNameRegistrar);
        static void registerExtendedCodecs(TextCodecRegistrar);

        TextCodecICU(const TextEncoding&);
        virtual ~TextCodecICU();

        virtual String decode(const char*, size_t length, bool flush, bool stopOnError, bool& sawError);
        virtual CString encode(const UChar*, size_t length, UnencodableHandling);

    private:
        void createICUConverter() const;
        void releaseICUConverter() const;
        bool needsGBKFallbacks() const { return m_needsGBKFallbacks; }
        void setNeedsGBKFallbacks(bool needsFallbacks) { m_needsGBKFallbacks = needsFallbacks; }
        
        int decodeToBuffer(UChar* buffer, UChar* bufferLimit, const char*& source,
            const char* sourceLimit, int32_t* offsets, bool flush, UErrorCode& err);

        TextEncoding m_encoding;
        unsigned m_numBufferedBytes;
        unsigned char m_bufferedBytes[16]; // bigger than any single multi-byte character
        //mutable iconv_t m_converterICU;
        mutable bool m_needsGBKFallbacks;
    };

} // namespace WebCore

#endif // TextCodecICU_h
