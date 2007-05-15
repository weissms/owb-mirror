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
 * @file  BCFontData.h
 *
 * Implementation file for FontData.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#ifndef BCFONTDATA_H
#define BCFONTDATA_H

#include "BIFontData.h"
#include "BTFontPlatformData.h"
#include "BTGlyphMap.h"
#include "GlyphWidthMap.h"

namespace WebCore {
  class FontDescription;
}

namespace BAL {
    typedef unsigned short Glyph;


class BCFontData : public BIFontData {
public:
    BCFontData(const FontPlatformData& f);
    ~BCFontData();

    BIFontData* smallCapsFontData(const WebCore::FontDescription& fontDescription) const;

    const FontPlatformData& platformData() const {
    return m_font;
    }


    int ascent() const;
    int descent() const;
    int lineSpacing() const;
    int lineGap() const;
    float xHeight() const;
    float widthForGlyph(Glyph) const;
    float platformWidthForGlyph(Glyph) const;
    bool containsCharacters(const UChar* characters, int length) const;

    void determinePitch();
    Pitch pitch() const;

    GlyphMap& getGlyphMap() const {
        return m_characterToGlyphMap;
    }

    void setFont(cairo_t*) const;

    Glyph getGlyphIndex(UChar c) const
    {
    return m_font.index(c);
    }

    float adjustedWidthForGlyph(Glyph glyph) const;

private:
    void platformInit();
    void platformDestroy();

public:
    int m_ascent;
    int m_descent;
    int m_lineSpacing;
    int m_lineGap;
    float m_xHeight;

    FontPlatformData m_font;
    mutable BAL::GlyphMap m_characterToGlyphMap;
    mutable WebCore::GlyphWidthMap m_glyphToWidthMap;

    bool m_treatAsFixedPitch;
    Glyph m_spaceGlyph;
    float m_spaceWidth;
    float m_adjustedSpaceWidth;

    mutable BIFontData* m_smallCapsFontData;

};// class BCFontData public BIFontData



}// namespace BAL


#endif
