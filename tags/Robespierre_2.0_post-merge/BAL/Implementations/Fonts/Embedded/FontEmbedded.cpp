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

#include "BCFontPlatformDataPrivateEmbedded.h"
#include "BIGraphicsDevice.h"
#include "FontData.h"
#include "GraphicsContext.h"
#include "RGBA32Array.h"

namespace BAL {

void BTFont::drawGlyphs(GraphicsContext* graphicsContext, const FontData* font, const GlyphBuffer& glyphBuffer,
                      int from, int numGlyphs, const FloatPoint& point) const
{
    // Set the text color to use for drawing.
    Color penColor = graphicsContext->fillColor();
	unsigned pixelColor = (penColor.alpha() << 24) | (penColor.red() << 16) | (penColor.green() << 8) | penColor.blue();
    GlyphBufferGlyph* glyphs = const_cast<GlyphBufferGlyph*>(glyphBuffer.glyphs(from));
    PixelFontType *fontPixel = static_cast<BCFontPlatformDataPrivateEmbedded *> (font->platformData().impl())->fontType();
	uint8_t w = fontPixel->width;
	uint8_t h = fontPixel->height;
    IntRect dstRect(0, 0, w, h);

	for (int i = 0; i < numGlyphs; i++) {
		IntPoint aPoint(static_cast<uint16_t> (point.x() + i * w), static_cast<uint16_t> (point.y() - h));
		RGBA32Array *glyphRGBABuffer = new RGBA32Array(h * w);
		for (int j = 0; j < h; j++) {
			for (int k = 0; k < w; k++) {
				uint16_t glyphOffset = glyphs[i] * h + j;
				if (fontPixel->buffer[glyphOffset] & (1 << (w - k)))
					glyphRGBABuffer->insert(j * w + k, pixelColor);
				else
					glyphRGBABuffer->insert(j * w + k, 0);
			}
		}
		BINativeImage *img = getBIGraphicsDevice()->createNativeImage(*glyphRGBABuffer, WebCore::IntSize(w, h));
		getBIGraphicsDevice()->copy(*(graphicsContext->widget()), *img, dstRect, aPoint, graphicsContext->alphaLayer());
		delete img;
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
