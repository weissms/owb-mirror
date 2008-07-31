/*
 * Copyright (C) 2008 Joerg Strohmayer.
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
#include "GlyphPageTreeNode.h"
#include "BALBase.h"
#include "SimpleFontData.h"
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>

namespace WKAL {

bool GlyphPage::fill(unsigned offset, unsigned length, UChar* buffer, unsigned bufferLength, const SimpleFontData* fontData)
{
    if (bufferLength > GlyphPage::size)
        return false;

    struct OutlineFont *face = fontData->m_font.m_face;

    if (!face)
        return false;

#if 1 // wrong, but much faster
    bool haveGlyphs = true;
    for (unsigned i = 0; i < bufferLength; i++) {
       setGlyphDataForIndex(i, buffer[i], fontData);
    }
#else
    bool haveGlyphs = false;
    for (unsigned i = 0; i < bufferLength; i++) {
        struct GlyphMap *glyph;

        if (!IDiskfont->ESetInfo(&face->olf_EEngine,
                                 OT_GlyphCode, buffer[i],
                                 TAG_END)
         && !IDiskfont->EObtainInfo(&face->olf_EEngine,
                                    OT_GlyphMap8Bit, &glyph,
                                    TAG_END)) {
            IDiskfont->EReleaseInfo(&face->olf_EEngine, OT_GlyphMap8Bit, glyph, TAG_END);
            setGlyphDataForIndex(i, buffer[i], fontData);
            haveGlyphs = true;
        } else
            setGlyphDataForIndex(i, 0, 0);
    }
#endif

    return haveGlyphs;
}

}
