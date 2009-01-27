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
#include "config.h"
#include "FontCache.h"

#include "Font.h"
#include "SimpleFontData.h"
#include <wtf/Assertions.h>
#include <wtf/HashMap.h>
#include <wtf/RefPtr.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig/fontconfig.h>

namespace WKAL {

// FIXME: we should use the same mechanism (custom key...) as the cache
// but I think it should be alright just to use the Font as a key.
typedef HashMap<const Font*, RefPtr<SimpleFontData> > FontFallbackCache;
FontFallbackCache fallbacks;

void FontCache::platformInit()
{
    if (!FontPlatformData::init())
        ASSERT_NOT_REACHED();
}

void FontCache::platformDestroy()
{
    if (!FontPlatformData::destroy())
        ASSERT_NOT_REACHED();

    fallbacks.clear();
}

// This method just need to bypass the cache as we could register our alternative font.
// This is a pity but is required to have asian fonts working.
PassRefPtr<SimpleFontData> FontCache::getFontDataForCharacters(const Font& font, const UChar* characters, int length)
{
    // Avoid copying FontPlatformData.
    RefPtr<FontPlatformData> primaryFontPlatformData = font.primaryFont()->m_font;

    // Check if the cache was already filled.
    if (fallbacks.contains(&font)) {
        ASSERT(primaryFontPlatformData->m_fallbacks);
        FontFallbackCache::iterator it = fallbacks.find(&font);
        return it->second;
    }

    if (!primaryFontPlatformData->m_fallbacks) {
        FcResult fresult;
        primaryFontPlatformData->m_fallbacks = FcFontSort(NULL, primaryFontPlatformData->m_pattern, FcTrue, NULL, &fresult);
    }

    FcFontSet* fs = primaryFontPlatformData->m_fallbacks;
    
    for (int i = 0; i < fs->nfont; i++) {
        FcPattern* fin = FcFontRenderPrepare(0, primaryFontPlatformData->m_pattern, fs->fonts[i]);

        FcChar8* fc_filename;
        if (FcPatternGetString(fin, FC_FILE, 0, &fc_filename) != FcResultMatch)
            continue;

        char* filename = (char *) fc_filename; // Use C cast as FcChar is a fontconfig type.

        int id;
        if (FcPatternGetInteger(fin, FC_INDEX, 0, &id) != FcResultMatch)
            continue;

        FT_Face face;
        if (FT_Error error = FT_New_Face(FontPlatformData::m_library, filename, id, &face)) {
            printf("FT_New_Face failed for filename = %s with FT_Error = %d\n", filename, error);
            continue;
        }

        // FIXME: is it really necessary ?
        FT_Set_Pixel_Sizes(face, 0, static_cast<uint> (font.fontDescription().computedSize()));

        RefPtr<FontPlatformData> platformData = adoptRef(new FontPlatformData(face, font.fontDescription().computedPixelSize(), false, false));
        platformData->m_pattern = fin;

        if (platformData->containsCharacters(characters, length)) {
            RefPtr<SimpleFontData> fontData = adoptRef(new SimpleFontData(platformData.get()));
            fallbacks.add(&font, fontData);
            return fontData;
        }
    }
    // Fallback: use the font in the main cache.
    RefPtr<SimpleFontData> fontData = getCachedFontData(font.fontDescription(), font.family().family());
    return fontData;
}

PassRefPtr<FontPlatformData> FontCache::getSimilarFontPlatformData(const Font& font)
{
    return adoptRef(new FontPlatformData(font.fontDescription(), font.family().family()));
}

void FontCache::getTraitsInFamily(const AtomicString& familyName, Vector<unsigned>& traitsMasks)
{
}

PassRefPtr<FontPlatformData> FontCache::createFontPlatformData(const FontDescription& description, const AtomicString& family)
{
    return adoptRef(new FontPlatformData(description, family));
}

}
