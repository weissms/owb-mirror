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

#include "BCFontPlatformDataPrivateEmbedded.h"
#include "BTLogHelper.h"
#include "Font.h"
#include "FontCache.h"
#include "FontDescription.h"
#include "GlyphBuffer.h"

namespace BAL {

void BTFontData::platformInit()
{
    PixelFontType *pixelFont = static_cast<BCFontPlatformDataPrivateEmbedded *> (m_font.impl())->fontType();
    m_ascent = pixelFont->height + 5; //must be 5 else text is misplaced
    m_descent = 2;
    m_lineSpacing = pixelFont->height;
    m_xHeight = pixelFont->height;
    m_spaceWidth = pixelFont->width;
    m_lineGap = 4;

    DBGML(MODULE_FONTS, LEVEL_INFO, "BTFontData::platformInit:\n");
    DBGML(MODULE_FONTS, LEVEL_INFO, "\tm_ascent = %d - m_descent = %d\n", m_ascent, m_descent);
    DBGML(MODULE_FONTS, LEVEL_INFO, "\tm_lineSpacing = %d - m_lineGap = %d\n", m_lineSpacing, m_lineGap);
    DBGML(MODULE_FONTS, LEVEL_INFO, "\tm_xHeight = %f - m_spaceWidth = %f\n", m_xHeight, m_spaceWidth);
}

void BTFontData::platformDestroy()
{
    delete m_smallCapsFontData;
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
    for (int i = 0; i < length; i++) {
        if (static_cast<uint8_t> (characters[i]) == 0) {
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
    //fixed font => constant width
    return m_spaceWidth;
}

}
