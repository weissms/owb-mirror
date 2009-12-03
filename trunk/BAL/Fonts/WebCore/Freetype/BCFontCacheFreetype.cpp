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

#include "CString.h"
#include "Font.h"
#include "Logging.h"
#include "SimpleFontData.h"
#include "UnusedParam.h"
#include <wtf/Assertions.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig/fontconfig.h>

namespace WebCore {

void FontCache::platformInit()
{
    if (!FontPlatformData::init())
        ASSERT_NOT_REACHED();
}

const SimpleFontData* FontCache::getFontDataForCharacters(const Font& font, const UChar* characters, int length)
{
    FcResult fresult;
    // Avoid copying FontPlatformData.
    FontPlatformData* prim = const_cast<FontPlatformData*>(&font.primaryFont()->m_platformData);

    if (!prim->m_fallbacks)
        prim->m_fallbacks = FcFontSort(NULL, prim->m_pattern, FcTrue, NULL, &fresult);

    FcFontSet* fs = prim->m_fallbacks;
    
    FT_Library library = FontPlatformData::m_library;

    FcChar8* fc_filename;
    char* filename;
    int id;
    FT_Face face;
    for (int i = 0; i < fs->nfont; i++) {
        FcPattern* fin = FcFontRenderPrepare(0, prim->m_pattern, fs->fonts[i]);

        if (FcPatternGetString(fin, FC_FILE, 0, &fc_filename) != FcResultMatch)
            continue;

        filename = (char *) fc_filename; // Use C cast as FcChar is a fontconfig type.

        if (FcPatternGetInteger(fin, FC_INDEX, 0, &id) != FcResultMatch)
            continue;

        if (FT_Error error = FT_New_Face(library, filename, id, &face)) {
#ifdef NDEBUG
            UNUSED_PARAM(error);
#endif
            LOG(FontEngine, "fail to open fonti %s with index %d (error = 0x%x)\n", filename, id, error);
            continue;
        }

        // FIXME: is it really necessary ?
        FT_Set_Pixel_Sizes(face, 0, static_cast<uint> (font.fontDescription().computedSize()));

        FontPlatformData alternateFont(face, font.fontDescription().computedPixelSize(), false, false);
        // FIXME: FT_Done_Face(face); we should clean the face correctly the FT_Face but we can't do that here...
        alternateFont.m_pattern = fin;
        SimpleFontData* sfd = getCachedFontData(&alternateFont);
        if (sfd->containsCharacters(characters, length))
            return sfd;
    }
    return 0;
}

FontPlatformData* FontCache::getSimilarFontPlatformData(const Font& font)
{
    return new FontPlatformData(font.fontDescription(), font.family().family());
}

FontPlatformData* FontCache::getLastResortFallbackFont(const FontDescription& fontDescription)
{
    // FIXME: Would be even better to somehow get the user's default font here.
    // For now we'll pick the default that the user would get without changing any prefs.
    static AtomicString timesStr("Times New Roman");
    return getCachedFontPlatformData(fontDescription, timesStr);
}

void FontCache::getTraitsInFamily(const AtomicString& familyName, Vector<unsigned>& traitsMasks)
{
}

FontPlatformData* FontCache::createFontPlatformData(const FontDescription& fontDescription, const AtomicString& family)
{
    return new FontPlatformData(fontDescription, family);
}

}
