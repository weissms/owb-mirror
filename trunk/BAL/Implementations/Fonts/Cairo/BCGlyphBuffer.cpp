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
 * @file  BCGlyphBuffer.cpp
 *
 * Implementation file for BCGlyphBuffer.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BCGLYPHBUFFER_CPP
#define BCGLYPHBUFFER_CPP

#include "config.h"
#include "BALConfiguration.h"
#include "BCGlyphBuffer.h"

IMPLEMENT_CREATE_DELETE(BIGlyphBuffer, BCGlyphBuffer);

namespace BAL {

bool BCGlyphBuffer::isEmpty() const {
     return m_fontData.isEmpty();
}

int BCGlyphBuffer::size() const {
    return m_fontData.size();
}

void BCGlyphBuffer::clear() {
    m_fontData.clear();
    m_glyphs.clear();
    m_advances.clear();
}

const BIFontData* BCGlyphBuffer::fontDataAt(int index) const {
    return m_fontData[index];
}

void BCGlyphBuffer::swap(int index1, int index2) {
    const BIFontData* f = m_fontData[index1];
    m_fontData[index1] = m_fontData[index2];
    m_fontData[index2] = f;

    GlyphBufferGlyph g = m_glyphs[index1];
    m_glyphs[index1] = m_glyphs[index2];
    m_glyphs[index2] = g;

    GlyphBufferAdvance s = m_advances[index1];
    m_advances[index1] = m_advances[index2];
    m_advances[index2] = s;
}

Glyph BCGlyphBuffer::glyphAt(int index) const {
    return m_glyphs[index].index;
}

float BCGlyphBuffer::advanceAt(int index) const {
    return m_advances[index].width();
}

void BCGlyphBuffer::add(Glyph glyph, const BIFontData* font, float width) {
    m_fontData.append(font);
    cairo_glyph_t cairoGlyph;
    cairoGlyph.index = glyph;
    cairoGlyph.y = 0;
    m_glyphs.append(cairoGlyph);
    m_advances.append(FloatSize(width, 0));
}


}
#endif // BCGLYPHBUFFER_CPP


