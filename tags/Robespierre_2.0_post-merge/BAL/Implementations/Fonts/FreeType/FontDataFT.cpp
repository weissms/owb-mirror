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
#include "BTFontData.h"

#include "BCFontPlatformDataPrivateFT.h"
#include "FloatRect.h"
#include "Font.h"
#include "FontCache.h"
#include "FontDescription.h"
#include "GlyphBuffer.h"
#ifdef __OWB__
#include "BIInternationalization.h"
#else
#include <unicode/uchar.h>
#include <unicode/unorm.h>
#endif //__OWB__
#include <wtf/MathExtras.h>

namespace BAL {

void BTFontData::platformInit()
{
    FT_Face face = static_cast<BCFontPlatformDataPrivateFT *> (m_font.impl())->ftFace();
    if (face) {
        m_ascent = static_cast<int> (DOUBLE_FROM_26_6(face->size->metrics.ascender));
        m_descent = -static_cast<int> (DOUBLE_FROM_26_6(face->size->metrics.descender));
        m_lineSpacing = static_cast<int> (DOUBLE_FROM_26_6(face->size->metrics.height));

        FT_Load_Char(face, 'x', FT_LOAD_DEFAULT);
        m_xHeight = DOUBLE_FROM_26_6(face->glyph->metrics.height);
        m_spaceWidth = DOUBLE_FROM_26_6(face->glyph->metrics.horiAdvance);
        m_lineGap = m_lineSpacing - m_ascent + m_descent;
    }

//FIXME: cannot enable it => problem with #include "BTLogHelper.h"
//     logml(MODULE_FONTS, LEVEL_INFO, "BTFontData::platformInit:");
//     logml(MODULE_FONTS, LEVEL_INFO, make_message("\tm_ascent = %d - m_descent = %d", m_ascent, m_descent));
//     logml(MODULE_FONTS, LEVEL_INFO, make_message("\tm_lineSpacing = %d - m_lineGap = %d", m_lineSpacing, m_lineGap));
//     logml(MODULE_FONTS, LEVEL_INFO, make_message("\tm_xHeight = %f - m_spaceWidth = %f", m_xHeight, m_spaceWidth));
}

void BTFontData::platformDestroy()
{
//     FcPattern *pattern = static_cast<BCFontPlatformDataPrivateFT *> (m_font.impl())->fcPattern();
//     if (pattern && ((FcPattern*)-1 != pattern))
//         FcPatternDestroy(pattern);
// 
//     FT_Face face = static_cast<BCFontPlatformDataPrivateFT *> (m_font.impl())->ftFace();
//     if (face) {
//         FT_Done_Face(face);
//     }
    if (m_smallCapsFontData)
        delete m_smallCapsFontData;
    m_smallCapsFontData = NULL;
}

BTFontData* BTFontData::smallCapsFontData(const FontDescription& fontDescription) const
{
    if (!m_smallCapsFontData) {
        FontDescription desc = FontDescription(fontDescription);
        desc.setSpecifiedSize(0.70f*fontDescription.computedSize());
        const FontPlatformData* pdata = new FontPlatformData(desc, desc.family().family());
        m_smallCapsFontData = new FontData(*pdata);
    }
    return m_smallCapsFontData;
}

bool BTFontData::containsCharacters(const UChar* characters, int length) const
{
    FT_Face face = static_cast<BCFontPlatformDataPrivateFT *> (m_font.impl())->ftFace();

    if (!face)
        return false;

    for (int i = 0; i < length; i++) {
        if (FT_Get_Char_Index(face, characters[i]) == 0) {
            return false;
        }
    }
    return true;
}

void BTFontData::determinePitch()
{
    m_treatAsFixedPitch = m_font.isFixedPitch();
}

float BTFontData::platformWidthForGlyph(Glyph glyph) const
{
    FT_Face face = static_cast<BCFontPlatformDataPrivateFT *> (m_font.impl())->ftFace();
    if (FT_Load_Glyph(face, glyph, FT_LOAD_DEFAULT)) {
//     logml(MODULE_FONTS, LEVEL_INFO, "BTFontData::platformWidthForGlyph:\n");
//     logml(MODULE_FONTS, LEVEL_INFO, make_message("\tunable to load glyph index %d\n", glyph));
        return m_spaceWidth;
    }
    if (face->glyph->advance.x)
        return DOUBLE_FROM_26_6(face->glyph->advance.x);
    return m_spaceWidth;
}

}
