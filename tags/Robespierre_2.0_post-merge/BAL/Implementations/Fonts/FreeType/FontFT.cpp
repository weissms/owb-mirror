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
#include "BTFont.h"

#include "BCFontPlatformDataPrivateFT.h"
#include "BIGraphicsDevice.h"
#include "FontData.h"
#include "GraphicsContext.h"
#include "RGBA32Array.h"

namespace BAL {

void BTFont::drawGlyphs(GraphicsContext* graphicsContext, const FontData* font, const GlyphBuffer& glyphBuffer,
                      int from, int numGlyphs, const FloatPoint& point) const
{
    // Set the text color to use for drawing.
    float red, green, blue, alpha;
    Color penColor = graphicsContext->fillColor();
    penColor.getRGBA(red, green, blue, alpha);

    GlyphBufferGlyph* glyphs = const_cast<GlyphBufferGlyph*>(glyphBuffer.glyphs(from));
    float offset = point.x();
    FT_Face face = static_cast<BCFontPlatformDataPrivateFT *> (font->platformData().impl())->ftFace();
    FT_Bitmap bitmap;
    IntRect dstRect;
    IntPoint aPoint;
    for (int i = 0; i < numGlyphs; i++) {
        if (FT_Load_Glyph(face, glyphs[i], FT_LOAD_DEFAULT)) {
            printf("FT_Load_Glyph fail\n");
            continue; //do not handle error
        }

        if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
            printf("FT_Render_Glyph fail\n");
            continue; //do not handle error
        }
        bitmap = face->glyph->bitmap;

        if (bitmap.buffer) {
            if (i > 0) {
                FT_Vector kerning;
                FT_Get_Kerning(face, glyphs[i - 1], glyphs[i], FT_KERNING_UNFITTED, &kerning);
                offset += DOUBLE_FROM_26_6(kerning.x);
            }
            aPoint.setX(static_cast<uint16_t> (offset + DOUBLE_FROM_26_6(face->glyph->metrics.horiBearingX)));
            aPoint.setY(static_cast<uint16_t> (point.y()) - face->glyph->bitmap_top);
            dstRect.setX(0);
            dstRect.setY(0);
            dstRect.setWidth(bitmap.width);
            dstRect.setHeight(bitmap.rows);

            RGBA32Array *glyphRGBABuffer = new RGBA32Array(bitmap.rows * bitmap.width * bitmap.pitch);
            for (int i = 0; i < bitmap.rows; i++) {
                for (int j = 0; j < bitmap.width * bitmap.pitch; j++) {
                    glyphRGBABuffer->insert(i* bitmap.width * bitmap.pitch + j, ((penColor.alpha()/255 * *bitmap.buffer++) << 24) | (penColor.red() << 16) | (penColor.green() << 8) | penColor.blue());
                }
            }
            BINativeImage *img = getBIGraphicsDevice()->createNativeImage(*glyphRGBABuffer, WebCore::IntSize(bitmap.width, bitmap.rows));
            getBIGraphicsDevice()->copy(*(graphicsContext->widget()), *img, dstRect, aPoint, graphicsContext->alphaLayer());

            delete img;
        }
        offset += glyphBuffer.advanceAt(from + i);
    }
}

void BTFont::drawComplexText(GraphicsContext*, const TextRun&, const TextStyle&, const FloatPoint&, int from, int to) const
{
    BALNotImplemented();
}

float BTFont::floatWidthForComplexText(const TextRun&, const TextStyle&) const
{
    BALNotImplemented();
    return 0.0f;
}

int BTFont::offsetForPositionForComplexText(const TextRun&, const TextStyle&, int, bool) const
{
    BALNotImplemented();
    return 0;
}

FloatRect BTFont::selectionRectForComplexText(const TextRun&, const TextStyle&, const IntPoint&, int, int, int) const
{
    BALNotImplemented();
    return FloatRect();
}

}
