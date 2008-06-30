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
#ifndef TextEncoding_h
#define TextEncoding_h
/**
 *  @file  TextEncoding.t
 *  TextEncoding description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "TextCodec.h"
#include <wtf/unicode/Unicode.h>

namespace OWBAL {

    class CString;
    class String;

    class TextEncoding : public OWBALBase {
    public:
    /**
     *  TextEncoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        TextEncoding();
    /**
     *  TextEncoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        TextEncoding(const char* name);
    /**
     *  TextEncoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        TextEncoding(const String& name);

    /**
     *  isValid description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        bool isValid() const ;
    /**
     *  name description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        const char* name() const ;
    /**
     *  usesVisualOrdering description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        bool usesVisualOrdering() const;
    /**
     *  isJapanese description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        bool isJapanese() const;
    /**
     *  backslashAsCurrencySymbol description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        UChar backslashAsCurrencySymbol() const;
    /**
     *  closest8BitEquivalent description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        const TextEncoding& closest8BitEquivalent() const;

    /**
     *  decode description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        String decode(const char* str, size_t length) const;
    /**
     *  decode description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        String decode(const char*, size_t length, bool stopOnError, bool& sawError) const;
    /**
     *  encode description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
        CString encode(const UChar*, size_t length, UnencodableHandling) const;

    private:
        const char* m_name;
    };

    /**
     *  operator== description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    inline bool operator==(const TextEncoding& a, const TextEncoding& b) ;
    /**
     *  operator!= description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    inline bool operator!=(const TextEncoding& a, const TextEncoding& b) ;

    /**
     *  ASCIIEncoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const TextEncoding& ASCIIEncoding();
    /**
     *  Latin1Encoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const TextEncoding& Latin1Encoding();
    /**
     *  UTF16BigEndianEncoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const TextEncoding& UTF16BigEndianEncoding();
    /**
     *  UTF16LittleEndianEncoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const TextEncoding& UTF16LittleEndianEncoding();
    /**
     *  UTF32BigEndianEncoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const TextEncoding& UTF32BigEndianEncoding();
    /**
     *  UTF32LittleEndianEncoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const TextEncoding& UTF32LittleEndianEncoding();
    /**
     *  UTF8Encoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const TextEncoding& UTF8Encoding();
    /**
     *  WindowsLatin1Encoding description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const TextEncoding& WindowsLatin1Encoding();

} // namespace OWBAL

#endif // TextEncoding_h




