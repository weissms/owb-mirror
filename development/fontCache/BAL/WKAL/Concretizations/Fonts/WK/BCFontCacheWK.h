/*
 * Copyright (C) 2006, 2008 Apple Computer, Inc.  All rights reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FontCache_h
#define FontCache_h

#include "config.h"
#include <limits.h>
#include <wtf/PassRefPtr.h>
#include <wtf/unicode/Unicode.h>
#include <wtf/Vector.h>

#if PLATFORM(WIN)
#include <objidl.h>
#include <mlang.h>
#endif

namespace WKAL{

class AtomicString;
class Font;
class FontPlatformData;
class FontData;
class FontDescription;
class FontSelector;
class SimpleFontData;

class FontCache {
public:
    friend FontCache* fontCache();

    ~FontCache();

    PassRefPtr<FontData> getFontData(const Font&, int& familyIndex, FontSelector*);
    void releaseFontData(const SimpleFontData*);
    
    // Called by platform to remove the FontPlatformData -> SimpleFontData binding.
    void releasePlatformData(FontPlatformData*);

    // This method is implemented by the platform.
    // FIXME: Font data returned by this method never go inactive because callers don't track and release them.
    PassRefPtr<SimpleFontData> getFontDataForCharacters(const Font&, const UChar* characters, int length);
    
    // Also implemented by the platform.
    void platformInit();
    void platformDestroy();

#if PLATFORM(WIN)
    IMLangFontLink2* getFontLinkInterface();
#endif

    void getTraitsInFamily(const AtomicString&, Vector<unsigned>&);

    PassRefPtr<SimpleFontData> getCachedFontData(const FontDescription&, const AtomicString& family, bool checkingAlternateName = false);
    PassRefPtr<SimpleFontData> getLastResortFallbackFont(const FontDescription&);

    void addClient(FontSelector*);
    void removeClient(FontSelector*);

    unsigned generation();
    void invalidate();

    size_t fontDataCount();
    size_t inactiveFontDataCount();
    void purgeInactiveFontData(int count = INT_MAX);

private:
    FontCache();

    // This method suppose that the caller store the result.
    // If not, no one will hold the SimpleFontData and it will be destroyed.
    PassRefPtr<SimpleFontData> getCachedFontData(FontPlatformData*);

    // These methods are implemented by each platform.
    PassRefPtr<FontPlatformData> getSimilarFontPlatformData(const Font&);
    PassRefPtr<FontPlatformData> createFontPlatformData(const FontDescription&, const AtomicString& family);

    friend class SimpleFontData;
    friend class FontFallbackList;
};

// Method to get the global font cache.
FontCache* fontCache();

} // namespace WKAL

#endif
