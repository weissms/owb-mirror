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
 * @file  BCGlyphBufferFT.h
 *
 * Header file for BCGlyphBufferFT.h
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BCGlyphBufferFreeType_h
#define BCGlyphBufferFreeType_h

#include "BIGlyphBuffer.h"
#include "BTFontData.h"

namespace BAL {

class BTFontData;

class BCGlyphBufferFreeType : public BIGlyphBuffer {
    public:
        virtual bool isEmpty() const;
        virtual int size() const;
        virtual void clear();
        virtual GlyphBufferGlyph* glyphs(int from);
        virtual const GlyphBufferGlyph* glyphs(int from) const;
        virtual GlyphBufferAdvance* advances(int from);
        virtual const GlyphBufferAdvance* advances(int from) const;
        virtual const BTFontData* fontDataAt(int index) const;
        virtual FloatSize offsetAt(int index) const;
        virtual void swap(int index1, int index2);
        virtual Glyph glyphAt(int index) const;
        virtual float advanceAt(int index) const;
        virtual void add(Glyph glyph, const BTFontData* font, float width, const FloatSize* offset = 0);
    private:
        Vector<const BTFontData*, 2048> m_fontData;
        Vector<GlyphBufferGlyph, 2048> m_glyphs;
        Vector<GlyphBufferAdvance, 2048> m_advances;
};

} //namespace BAL

#endif //BCGlyphBufferFT_h
