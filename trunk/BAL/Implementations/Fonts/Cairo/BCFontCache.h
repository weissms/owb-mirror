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
 * @file  BCFontCache.h
 *
 * Header file for BCFontCache.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BCFONTCACHE_H
#define BCFONTCACHE_H

#include "BIFontCache.h"
#include "BIFontData.h"
#include "BIInternationalization.h"

namespace WebCore {
    class AtomicString;
}

namespace BAL {
    class FontPlatformData;

/**
 * The BIFontCache implementation
 *
 * @see Font, FontData
 */
class BCFontCache : public BIFontCache {
public:
    IMPLEMENT_BIFONTCACHE;
private:
    FontPlatformData* getCachedFontPlatformData(const WebCore::FontDescription&, const WebCore::AtomicString& family, bool checkingAlternateName = false);
    BIFontData* getCachedFontData(const FontPlatformData*);

// These three methods are implemented by each platform.
    FontPlatformData* getSimilarFontPlatformData(const Font&);
    FontPlatformData* getLastResortFallbackFont(const Font&);
    FontPlatformData* createFontPlatformData(const WebCore::FontDescription&, const WebCore::AtomicString& family);

    friend class BIFontData;
    friend class FontFallbackList;
};

}
#endif // BCFONTCACHE_H


