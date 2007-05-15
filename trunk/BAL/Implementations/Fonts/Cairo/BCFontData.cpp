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
 * @file  BCFontData.cpp
 *
 * Implementation file for FontData.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */



#include "config.h"
#include "BTFont.h"

#include <wtf/MathExtras.h>
#include "BCFontData.h"
#include "BTLogHelper.h"

namespace BAL
{

BIFontData* createBIFontData(const FontPlatformData& f) {
    return new BCFontData(f);
}
void deleteBIFontData(BIFontData* data) {
    delete data;
}

BCFontData::BCFontData(const FontPlatformData& f) : m_font(f), m_treatAsFixedPitch(false), m_smallCapsFontData(0)
{
    m_font = f;

    platformInit();

    // Nasty hack to determine if we should round or ceil space widths.
    // If the font is monospace or fake monospace we ceil to ensure that
    // every character and the space are the same width.  Otherwise we round.

    m_spaceGlyph = m_characterToGlyphMap.glyphDataForCharacter(' ', this).glyph;

    float width = widthForGlyph(m_spaceGlyph);
    m_spaceWidth = width;
    determinePitch();
    m_adjustedSpaceWidth = m_treatAsFixedPitch ? ceilf(width) : roundf(width);

}




BCFontData::~BCFontData()
{
    platformDestroy();

    // We only get deleted when the cache gets cleared.  Since the smallCapsRenderer is also in that cache,
    // it will be deleted then, so we don't need to do anything here.
}


int BCFontData::ascent() const {
     return m_ascent;
}
int BCFontData::descent() const {
     return m_descent;
}
int BCFontData::lineSpacing() const {
     return m_lineSpacing;
}
int BCFontData::lineGap() const {
     return m_lineGap;
}
float BCFontData::xHeight() const {
     return m_xHeight;
}
BIFontData::Pitch BCFontData::pitch() const {
     return m_treatAsFixedPitch ? FixedPitch : VariablePitch;
}

float BCFontData::widthForGlyph(Glyph glyph) const
{
    float width = m_glyphToWidthMap.widthForGlyph(glyph);
    if (width != WebCore::cGlyphWidthUnknown)
        return width;

    width = platformWidthForGlyph(glyph);
    m_glyphToWidthMap.setWidthForGlyph(glyph, width);

    return width;
}



float BCFontData::adjustedWidthForGlyph(Glyph glyph) const
{
    float width;
    width = this->widthForGlyph(glyph);
    // We special case spaces in two ways when applying word rounding.
    // First, we round spaces to an adjusted width in all fonts.
    // Second, in fixed-pitch fonts we ensure that all characters that
    // match the width of the space character have the same width as the space character.
    if (width == this->m_spaceWidth &&
        (this->m_treatAsFixedPitch || glyph == this->m_spaceGlyph)) {
        width = this->m_adjustedSpaceWidth;
    }
    return width;
}

//
// What follows was in gdk/FontDataGdk.cpp
//

void BCFontData::platformInit()
{
    if (!m_font.m_scaledFont) {
        logml(MODULE_FONTS, LEVEL_EMERGENCY, "No font support");
        exit(1);
    }
    cairo_font_extents_t font_extents;
    cairo_text_extents_t text_extents;
    cairo_scaled_font_extents(m_font.m_scaledFont,&font_extents);
    m_ascent = static_cast<int>(font_extents.ascent);
    m_descent = static_cast<int>(font_extents.descent);
    m_lineSpacing = static_cast<int>(font_extents.height);
    cairo_scaled_font_text_extents(m_font.m_scaledFont,"x",&text_extents);
    m_xHeight = text_extents.height;
    cairo_scaled_font_text_extents(m_font.m_scaledFont," ",&text_extents);
    m_spaceWidth =  static_cast<int>(text_extents.x_advance);
    assert( m_spaceWidth != 0 );
    m_lineGap = m_lineSpacing-m_ascent-m_descent;
    //m_spaceGlyph =;
}

void BCFontData::platformDestroy()
{
    // We don't hash this on Linux, so it's effectively owned by us.
    delete m_smallCapsFontData;
}

BIFontData* BCFontData::smallCapsFontData(const WebCore::FontDescription& fontDescription) const
{
    if (!m_smallCapsFontData) {
        WebCore::FontDescription desc = WebCore::FontDescription(fontDescription);
        desc.setSpecifiedSize(0.70f *fontDescription.computedSize());
        const FontPlatformData * pdata = new FontPlatformData(desc,
                desc.family().family());
        m_smallCapsFontData = new BCFontData(*pdata);
    }
    return m_smallCapsFontData;
}

bool BCFontData::containsCharacters(const UChar* characters, int length) const
{
    for (unsigned i = 0; i < length; i++) {
        if( getGlyphIndex(characters[i]) == 0 )
            return false;
    }
    return true;
}

void BCFontData::determinePitch()
{
    m_treatAsFixedPitch = m_font.isFixedPitch();
}

float BCFontData::platformWidthForGlyph(Glyph glyph) const
{
    float w = 0;
    cairo_glyph_t  cglyph;
    cairo_text_extents_t extents;
    cglyph.index = (int)glyph;
    cairo_scaled_font_glyph_extents (m_font.m_scaledFont,&cglyph,1,&extents);
    //w = extents.width; //or is the and advance ?
    w = extents.x_advance; //or is the and advance ?
    if( w == 0 ) {
        w = m_spaceWidth;
    }
    return w;
}

void BCFontData::setFont(cairo_t *cr) const
{
    m_font.setFont(cr);
}


}

