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
 * @file  BIFontCache.h
 *
 * Interface file for BIFontCache.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BIFONTCACHE_H
#define BIFONTCACHE_H

#include "BIInternationalization.h"

namespace BAL {

    class BIFontData;
    class Font;
/**
 * The FontCache caches FontData.
 *
 * Depending on the implementation, FontData may be cached on the basis of its
 * font's family, font's size, bold and italic state.
 *
 * TODO Move Font and FontData from WebCore to BAL
 * @see Font, FontData
 */
class BIFontCache {
public:
    /**
     * Find a FontData in cache or create one the most similar possible.
     *
     * getFontData loops on all font families starting from familyIndex.
     * If not in cache, it creates it, and depending on the implementation,
     * it may return an approaching FontData or a LastResortFallbackFont.
     *
     * @param[in] Font the font associated with the FontData
     * @param[in] familyIndex the first index in the family list to start with
     *
     * @return FontData*
     * @return 0
     */
    virtual const BIFontData* getFontData(const Font&, int& familyIndex) = 0;

    /**
     * Find a FontData in cache supporting the given characters or create it.
     *
     * @warning Not implemented currently : returns simply a new FontData.
     *
     * @param[in] Font the font involved
     * @param[in] characters to be supported
     * @param[in] length of characters
     *
     * @return FontData*
     * @return 0
     */
    virtual const BIFontData* getFontDataForCharacters(
            const Font&, const UChar* characters, int length) = 0;

    /**
     * TODO platformInit() may be private...
     */
    virtual void Init() = 0;

    virtual ~BIFontCache() {};
};

#define IMPLEMENT_BIFONTCACHE \
    public: \
    virtual const BIFontData* getFontData(const Font&, int& familyIndex);\
    virtual const BIFontData* getFontDataForCharacters(\
            const Font&, const UChar* characters, int length);\
    virtual void Init();

}
#endif // BIFONTCACHE_H


