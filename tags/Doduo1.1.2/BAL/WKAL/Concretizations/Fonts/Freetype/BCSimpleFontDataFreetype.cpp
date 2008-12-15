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
#include "SimpleFontData.h"

#include "FloatRect.h"
#include "Font.h"
#include "FontCache.h"
#include "FontDescription.h"
#include "GlyphBuffer.h"
#include "BALBase.h"
#include <unicode/uchar.h>
#include <unicode/unorm.h>
#include <wtf/MathExtras.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig/fontconfig.h>

namespace WKAL {

void SimpleFontData::platformInit()
{
    FT_Face face = m_font.m_face;
    if (face) {
        m_ascent = static_cast<int> (DOUBLE_FROM_26_6(face->size->metrics.ascender));
        m_descent = -static_cast<int> (DOUBLE_FROM_26_6(face->size->metrics.descender));
        m_lineSpacing = static_cast<int> (DOUBLE_FROM_26_6(face->size->metrics.height));

        FT_Load_Char(face, 'x', FT_LOAD_DEFAULT);
        m_xHeight = DOUBLE_FROM_26_6(face->glyph->metrics.height);
        FT_Load_Char(face, ' ', FT_LOAD_DEFAULT);
        m_spaceWidth = DOUBLE_FROM_26_6(face->glyph->metrics.horiAdvance);
        m_lineGap = m_lineSpacing - m_ascent + m_descent;
    }
}

void SimpleFontData::platformDestroy()
{
    if (m_font.m_pattern && ((FcPattern*)-1 != m_font.m_pattern)) {
        FcPatternDestroy(m_font.m_pattern);
        m_font.m_pattern = 0;
    }
    
    if (m_font.m_fallbacks) {
        FcFontSetDestroy(m_font.m_fallbacks);
        m_font.m_fallbacks = 0;
    }
    
    if (m_smallCapsFontData)
        delete m_smallCapsFontData;
    m_smallCapsFontData = NULL;
}

SimpleFontData* SimpleFontData::smallCapsFontData(const FontDescription& fontDescription) const
{
    if (!m_smallCapsFontData) {
        FontDescription desc = FontDescription(fontDescription);
        desc.setSpecifiedSize(0.70f*fontDescription.computedSize());
        const FontPlatformData* pdata = new FontPlatformData(desc, desc.family().family());
        m_smallCapsFontData = new SimpleFontData(*pdata);
    }
    return m_smallCapsFontData;
}

bool SimpleFontData::containsCharacters(const UChar* characters, int length) const
{
    FT_Face face = m_font.m_face;
    if (!face)
        return false;

    for (int i = 0; i < length; i++) {
        if (FT_Get_Char_Index(face, characters[i]) == 0) {
            return false;
        }
    }
    return true;
}

void SimpleFontData::determinePitch()
{
    m_treatAsFixedPitch = m_font.isFixedPitch();
}

float SimpleFontData::platformWidthForGlyph(Glyph glyph) const
{
    FT_Face face = m_font.m_face;
    if (FT_Load_Glyph(face, glyph, FT_LOAD_DEFAULT)) {
        return m_spaceWidth;
    }
    if (face->glyph->advance.x)
        return DOUBLE_FROM_26_6(face->glyph->advance.x);
    return m_spaceWidth;
}

void SimpleFontData::setFont(BalFont* cr) const
{
    ASSERT(cr);
    m_font.setFont(cr);
}

}
