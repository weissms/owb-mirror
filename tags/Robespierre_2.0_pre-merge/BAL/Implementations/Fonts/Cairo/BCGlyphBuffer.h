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
 * @file  BCGlyphBuffer.h
 *
 * Header file for BCGlyphBuffer.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BCGLYPHBUFFER_H
#define BCGLYPHBUFFER_H

// MAX_GLYPH_EXPANSION is the maximum numbers of glyphs that may be
// use to represent a single Unicode code point.
#define MAX_GLYPH_EXPANSION 4
#define GLYPH_BUFFER_SIZE 2048

#include "BIGlyphBuffer.h"
#include <cairo.h>
#include "FloatSize.h"
#include "BTFont.h" // allow friend
#include "BCFontData.h"
#include <wtf/Vector.h>

using WebCore::FloatSize;

namespace BAL {

typedef cairo_glyph_t GlyphBufferGlyph;
typedef FloatSize GlyphBufferAdvance;

/**
 * BIGlyphBuffer implementation.
 *
 * @see Font, FontData
 */
class BCGlyphBuffer : public BIGlyphBuffer {
public:
    IMPLEMENT_BIGLYPHBUFFER;

public:
    BCGlyphBuffer() {};

private:
    // XXX SRO for these two, use a friend
    // only used in FontGdk, but can't be in BIGlyphBuffer (platform dependant) !
    GlyphBufferGlyph* glyphs(int from) const {
        return ((GlyphBufferGlyph*)m_glyphs.data()) + from;
    }

    GlyphBufferAdvance* advances(int from) const {
        return ((GlyphBufferAdvance*)m_advances.data()) + from;
    }
    // XXX SRO
    friend class Font;

    Vector<const BIFontData*, GLYPH_BUFFER_SIZE> m_fontData;
    Vector<GlyphBufferGlyph, GLYPH_BUFFER_SIZE> m_glyphs;
    Vector<GlyphBufferAdvance, GLYPH_BUFFER_SIZE> m_advances;
};

}
#endif // BCGLYPHBUFFER_H


