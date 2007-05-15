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
 * @file  BIGlyphBuffer.h
 *
 * Interface file for BIGlyphBuffer.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BIGLYPHBUFFER_H
#define BIGLYPHBUFFER_H

namespace BAL {

/**
 * a Glyph is an ID representing a character in a font map.
 */
typedef unsigned short Glyph;
class BIFontData;

/**
 * The GlyphBuffer associates Glyphs with their widths and FontData.
 * Glyphs, glyphs' widths, and FontData are each one stored in a vector.
 * Thus glyphs, widths, and FontData can be accessed by index with glyphAt()
 * advanceAt() and fonDataAt().
 * Width are used to know the advance for each glyphs when walking through
 * a string of characters.
 *
 * @see Font, FontData
 */
class BIGlyphBuffer {
public:
    /**
     * Tell if the glyph buffer is empty.
     *
     * @return bool true if empty, false otherwise
     */
    virtual bool isEmpty() const = 0;
    /**
     * Return the glyph buffer size
     *
     * @return size of glyph buffer
     */
    virtual int size() const = 0;

    /**
     * Clear all the glyph buffer.
     */
    virtual void clear() = 0;

    /**
     * Return the FontData at the given index
     *
     * @param[in] index of the glyph
     */
    virtual const BIFontData* fontDataAt(int index) const = 0;

    /**
     * Swaps two indexes of the GlyphBuffer
     * This move the glyph, width and FontData altogether.
     *
     * @param[in] index1 of the first glyph to be swapped
     * @param[in] index2 of the second glyph
     */
    virtual void swap(int index1, int index2) = 0;

    /**
     * Returns the glyph at the given index.
     *
     * @param[in] index of the glyph
     * @return glyph the glyph found
     */
    virtual Glyph glyphAt(int index) const = 0;

    /**
     * Returns the width of the glyph stored at the given index.
     *
     * @param[in] index of the glyph
     * @return float the width
     */
    virtual float advanceAt(int index) const = 0;

    /**
     * Stores the glyph with the given width and the given font
     *
     * @param[in] glyph
     * @param[in] font
     * @param[in] width
     */
    virtual void add(Glyph glyph, const BIFontData* font, float width) = 0;

    virtual ~BIGlyphBuffer() {};
};

#define IMPLEMENT_BIGLYPHBUFFER \
    public: \
    virtual bool                isEmpty() const;\
    virtual int                 size() const;\
    virtual void                clear();\
    virtual const BIFontData*   fontDataAt(int index) const;\
    virtual void                swap(int index1, int index2);\
    virtual Glyph               glyphAt(int index) const;\
    virtual float               advanceAt(int index) const;\
    virtual void                add(Glyph, const BIFontData*, float width);

}
#endif // BIGLYPHBUFFER_H


