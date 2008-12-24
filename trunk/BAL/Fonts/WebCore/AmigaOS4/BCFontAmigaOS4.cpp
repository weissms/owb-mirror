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
#include "Font.h"

#include "GraphicsContext.h"
#include "SimpleFontData.h"
#include "cairo.h"
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>

namespace WKAL {

#define IS_HIGH_SURROGATE(u)  ((UChar)(u) >= (UChar)0xd800 && (UChar)(u) <= (UChar)0xdbff)
#define IS_LOW_SURROGATE(u)  ((UChar)(u) >= (UChar)0xdc00 && (UChar)(u) <= (UChar)0xdfff)




void Font::drawComplexText(GraphicsContext* context, const TextRun& run, const FloatPoint& point, int from, int to) const
{
    BalNotImplemented();
}


float Font::floatWidthForComplexText(const TextRun& run) const
{
    BalNotImplemented();
    return 0.0;
}

int Font::offsetForPositionForComplexText(const TextRun& run, int x, bool includePartialGlyphs) const
{
    BalNotImplemented();
    return 0;
}

FloatRect Font::selectionRectForComplexText(const TextRun& run, const IntPoint& point, int h, int from, int to) const
{
    BalNotImplemented();
    return FloatRect();
}

void Font::drawGlyphs(GraphicsContext* context, const SimpleFontData* font, const GlyphBuffer& glyphBuffer,
                      int from, int numGlyphs, const FloatPoint& point) const
{
    // Set the text color to use for drawing.
    Color penColor = context->fillColor();
    uint32 penalpha = penColor.alpha();
    uint32 penr = penColor.red();
    uint32 peng = penColor.green();
    uint32 penb = penColor.blue();

    GlyphBufferGlyph* glyphs = const_cast<GlyphBufferGlyph*>(glyphBuffer.glyphs(from));
    struct OutlineFont *face = font->m_font.m_face;
    uint32 ysize = font->m_font.m_size;

    int32 imagewidth = 0;
    uint32 onlyspaces = TRUE;
    for (int i = 0; i < numGlyphs; i++) {
        if (32 != glyphs[i])
            onlyspaces = FALSE;
        imagewidth += glyphBuffer.advanceAt(from + i);
    }

    if (onlyspaces)
       return;

    if (imagewidth <= 0)
        return;

    imagewidth += 2 * (ysize * amigaConfig.fontXDPI / 72);

    Vector<unsigned> *glyphRGBABuffer = new Vector<unsigned>(imagewidth * height());
    if (!glyphRGBABuffer)
        return;

    glyphRGBABuffer->fill(0);

    int32 offsetinimg = 0;
    uint32 shiftleft = 0;

    for (int i = 0; i < numGlyphs; i++) {
        if (32 == glyphs[i]) {
            offsetinimg += glyphBuffer.advanceAt(from + i);
            continue;
        }

        struct GlyphMap *glyph;

        if (IDiskfont->ESetInfo(&face->olf_EEngine,
                                OT_GlyphCode, glyphs[i],
                                TAG_END))
            continue; //do not handle error

        struct OutlineFont *curface = face;
        if (IDiskfont->EObtainInfo(&face->olf_EEngine,
                                   OT_GlyphMap8Bit, &glyph,
                                   TAG_END)) {
            uint32 ok = FALSE;

            if (amigaConfig.unicodeFace
             && !IDiskfont->ESetInfo(&amigaConfig.unicodeFace->olf_EEngine,
                                     OT_PointHeight, ysize << 16,
                                     OT_GlyphCode, glyphs[i],
                                     TAG_END)
             && !IDiskfont->EObtainInfo(&amigaConfig.unicodeFace->olf_EEngine,
                                        OT_GlyphMap8Bit, &glyph,
                                        TAG_END)) {
                curface = amigaConfig.unicodeFace;
                ok = TRUE;
            }

            if (!ok
             && !IDiskfont->ESetInfo(&face->olf_EEngine,
                                     OT_GlyphCode, 0xFFFD,
                                     TAG_END)
             && !IDiskfont->EObtainInfo(&face->olf_EEngine,
                                        OT_GlyphMap8Bit, &glyph,
                                        TAG_END))
                ok = TRUE;

            if (!ok
             && !IDiskfont->ESetInfo(&face->olf_EEngine,
                                     OT_GlyphCode, '?',
                                     TAG_END)
             && !IDiskfont->EObtainInfo(&face->olf_EEngine,
                                        OT_GlyphMap8Bit, &glyph,
                                        TAG_END))
                ok = TRUE;

            if (!ok)
                continue; //do not handle error
        }

        if (amigaConfig.fontKerning && i > 0)
            if (!IDiskfont->ESetInfo(&curface->olf_EEngine,
                                     OT_GlyphCode, glyphs[i -1],
                                     OT_GlyphCode2, glyphs[i],
                                     TAG_END)) {
                FIXED kern = 0;
                if (!IDiskfont->EObtainInfo(&curface->olf_EEngine,
                                            OT_TextKernPair, &kern,
                                            TAG_END))
                    if (kern)
                        offsetinimg -= (int)(kern / 65536.0 * ysize * amigaConfig.fontYDPI / 72);
            }

        uint32 left = glyph->glm_BlackLeft;
        uint32 top = glyph->glm_BlackTop;
        int32 width = glyph->glm_BlackWidth;
        int32 gheight = glyph->glm_BlackHeight;
        int32 x0 = glyph->glm_X0;
        int32 y0 = glyph->glm_Y0;
        uint32 modulo = glyph->glm_BMModulo;
        int32 imgoffsetx = -x0 + left;

        if (offsetinimg + imgoffsetx < 0) {
            if (0 == i) {
                shiftleft = -imgoffsetx;
                if (shiftleft > ysize) {
                    shiftleft = ysize;
                    imgoffsetx = -ysize;
                }
                offsetinimg = shiftleft;
            }
            else
                imgoffsetx = 0;
        }

        int32 imgoffsety = ascent() - y0 + top;

        if (imgoffsety < 0)
            imgoffsety = 0;

        if (offsetinimg + imgoffsetx + width > imagewidth) {
            int32 signedwidth = imagewidth - offsetinimg - imgoffsetx;
            if (signedwidth < 0)
                signedwidth = 0;
            width = signedwidth;
        }

        if (imgoffsety + gheight > (int)height())
            gheight = height() - imgoffsety;

        for (int32 y = 0; y < gheight ; y++) {
            uint32 top_mod_left = (y + top) * modulo + left;
            for (int32 x = 0; x < width; x++)
                if ((offsetinimg + imgoffsetx + x < imagewidth)
                 && (offsetinimg + imgoffsetx + x >= 0)) {
                    uint32 a = penalpha * glyph->glm_BitMap[top_mod_left + x] / 255;
                    if (a) {
                        uint32 oldARGB = (*glyphRGBABuffer)[(imgoffsety + y) * imagewidth + offsetinimg + imgoffsetx + x];
                        if (oldARGB)
                            a = std::max((oldARGB >> 24), a);
                        (*glyphRGBABuffer)[(imgoffsety + y) * imagewidth + offsetinimg + imgoffsetx + x] = (a << 24) | ((penr * a / 255) << 16) | ((peng * a / 255) << 8) | (penb * a / 255);
                    }
                }
        }

        offsetinimg += glyphBuffer.advanceAt(from + i);

        IDiskfont->EReleaseInfo(&curface->olf_EEngine, OT_GlyphMap8Bit, glyph, TAG_END);
    }

    cairo_surface_t *surface = cairo_image_surface_create_for_data((unsigned char *)glyphRGBABuffer->data(), CAIRO_FORMAT_ARGB32, imagewidth, height(), imagewidth * 4);
    if (surface) {
        cairo_t* cr = context->platformContext();

        cairo_set_source_surface(cr, surface, point.x() - shiftleft, point.y() - ascent());
        cairo_paint(cr);

        cairo_surface_destroy(surface);
    }

    delete glyphRGBABuffer;
}

}
