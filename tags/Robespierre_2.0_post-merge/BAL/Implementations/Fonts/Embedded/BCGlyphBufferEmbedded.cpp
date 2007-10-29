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
#include "BALConfiguration.h"
#include "BCGlyphBufferEmbedded.h"

IMPLEMENT_CREATE_DELETE(BIGlyphBuffer, BCGlyphBufferEmbedded);

namespace BAL {

bool BCGlyphBufferEmbedded::isEmpty() const
{
    return m_fontData.isEmpty();
}

int BCGlyphBufferEmbedded::size() const
{
    return m_fontData.size();
}

void BCGlyphBufferEmbedded::clear()
{
    m_fontData.clear();
    m_glyphs.clear();
    m_advances.clear();
}

GlyphBufferGlyph* BCGlyphBufferEmbedded::glyphs(int from)
{
    return m_glyphs.data() + from;
}

const GlyphBufferGlyph* BCGlyphBufferEmbedded::glyphs(int from) const
{
    return m_glyphs.data() + from;
}

GlyphBufferAdvance* BCGlyphBufferEmbedded::advances(int from)
{
    return m_advances.data() + from;
}

const GlyphBufferAdvance* BCGlyphBufferEmbedded::advances(int from) const
{
    return m_advances.data() + from;
}

const BTFontData* BCGlyphBufferEmbedded::fontDataAt(int index) const
{
    return m_fontData[index];
}

void BCGlyphBufferEmbedded::swap(int index1, int index2)
{
    const BTFontData* f = m_fontData[index1];
    m_fontData[index1] = m_fontData[index2];
    m_fontData[index2] = f;

    GlyphBufferGlyph g = m_glyphs[index1];
    m_glyphs[index1] = m_glyphs[index2];
    m_glyphs[index2] = g;

    GlyphBufferAdvance s = m_advances[index1];
    m_advances[index1] = m_advances[index2];
    m_advances[index2] = s;
}

Glyph BCGlyphBufferEmbedded::glyphAt(int index) const
{
    return m_glyphs[index];
}

float BCGlyphBufferEmbedded::advanceAt(int index) const
{
    return m_advances[index].width();
}

FloatSize BCGlyphBufferEmbedded::offsetAt(int index) const
{
    return FloatSize();
}

void BCGlyphBufferEmbedded::add(Glyph glyph, const BTFontData* font, float width, const FloatSize* offset)
{
    m_fontData.append(font);
    m_glyphs.append(glyph);
    m_advances.append(FloatSize(width, 0));
}

} //namespace BAL
