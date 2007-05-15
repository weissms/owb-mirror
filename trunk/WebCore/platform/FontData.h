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
 * @file  BIFontData.h
 *
 * Interface file for BIFontData.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BIFONTDATA_H
#define BIFONTDATA_H

#include "BIInternationalization.h" //UChar

namespace WebCore {
    class FontDescription;
}

namespace BAL {
    class GlyphMap;

/**
 * a Glyph is an ID representing a character in a font map.
 */
    typedef unsigned short Glyph;

/**
 * FontData represents the metrics of a Font.
 * Vertical metrics are ascent, descent, lineSpacing, lineGap, xHeight
 * and horizontal metrics such as *WidthGlyph and Pitch.
 * It is linked with a GlyphMap of all needed characters for the font.
 * FontData manages the platform implementation of all Glyphs.
 *
 * @see Font
 */
class BIFontData {
public:
    enum Pitch { UnknownPitch, FixedPitch, VariablePitch };

    /**
     * Get the FontData for the small caps font.
     * May be computed from the current font.
     *
     * @param[in] fontDescription
     *
     * @return BIFontData*
     * @return 0
     */
    virtual BIFontData* smallCapsFontData(const WebCore::FontDescription& fontDescription) const = 0;

    /**
     * Vertical distance between baseline and top of the character.
     *
     * @return int
     * @return 0
     */
    virtual int ascent() const = 0;
    /**
     * Vertical distance between baseline and bottom of the character.
     *
     * @return int
     * @return 0
     */
    virtual int descent() const = 0;
    /**
     * TODO
     *
     * @return int
     * @return 0
     */
    virtual int lineSpacing() const = 0;
    /**
     * TODO
     *
     * @return int
     * @return 0
     */
    virtual int lineGap() const = 0;
    /**
     * TODO
     *
     * @return float
     * @return 0
     */
    virtual float xHeight() const = 0;

    /**
     * Gets the pitch of the font.
     *
     * @return Pitch
     * @return 0
     */
    virtual Pitch pitch() const = 0;

    /**
     * Ask the GlyphWidthMap for a glyph width,
     * or else returns platformWidthForGlyph.
     *
     * @return float the glyph's width
     */
    virtual float widthForGlyph(Glyph) const = 0;
    /**
     * Ask the font engine for a glyph width.
     *
     * @param[in] Glyph
     *
     * @return float the glyph's width
     */
    virtual float platformWidthForGlyph(Glyph) const = 0;

    /**
     * Returns true if all characters are in the GlyphMap
     *
     * @param[in] characters to be found
     * @param[in] length of the characters' string
     *
     * @return bool
     */
    virtual bool containsCharacters(const UChar* characters, int length) const = 0;

    /**
     * Find if font is mono spaced or variable spaced.
     *
     */
    virtual void determinePitch() = 0;

    // we should own a GlyphMap
    virtual GlyphMap& getGlyphMap() const = 0;

    virtual float adjustedWidthForGlyph(Glyph glyph) const = 0;
    /**
     * Maps a Unicode char to a glyph index.
     * This function uses information from several possible underlying encoding
     * tables to work around broken fonts.  As a result, this function isn't
     * designed to be used in performance sensitive areas; results from this
     * function are intended to be cached by higher level functions.
     */
    // needed for GlyphMapGdk
    virtual Glyph getGlyphIndex(UChar c) const = 0;

    // mandatory
    virtual ~BIFontData() {};
};

}
#endif // BIFONTDATA_H


