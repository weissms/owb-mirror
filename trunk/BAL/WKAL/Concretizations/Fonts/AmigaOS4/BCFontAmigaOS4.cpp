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
#include "SDL.h"
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>

namespace WKAL {

#define IS_HIGH_SURROGATE(u)  ((UChar)(u) >= (UChar)0xd800 && (UChar)(u) <= (UChar)0xdbff)
#define IS_LOW_SURROGATE(u)  ((UChar)(u) >= (UChar)0xdc00 && (UChar)(u) <= (UChar)0xdfff)

extern SDL_Surface* applyTransparency(SDL_Surface* origin, const uint8_t alphaChannel);



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
    if (point.y() > 32700)
       return;

    // Set the text color to use for drawing.
    float red, green, blue, alpha;
    Color penColor = context->fillColor();
    penColor.getRGBA(red, green, blue, alpha);

    GlyphBufferGlyph* glyphs = const_cast<GlyphBufferGlyph*>(glyphBuffer.glyphs(from));
    float offset = point.x();
    struct OutlineFont *face = font->m_font.m_face;
    uint32 ysize = font->m_font.m_size;
    IntRect dstRect;
    IntPoint aPoint;
    uint32 rgb = (penColor.red() << 16) | (penColor.green() << 8) | penColor.blue();
    uint32 penalpha = penColor.alpha();

    double dimagewidth = 0;
    int32 imagewidth;
    uint32 onlyspaces = TRUE;
    for (int i = 0; i < numGlyphs; i++) {
        if (32 != glyphs[i])
            onlyspaces = FALSE;
        dimagewidth += glyphBuffer.advanceAt(from + i);
    }
    imagewidth = dimagewidth + 0.5;

    if (onlyspaces)
       return;

    if (imagewidth <= 0)
        return;

    imagewidth += 2 * ysize;

    Vector<unsigned> *glyphRGBABuffer = new Vector<unsigned>(imagewidth * height());
    if (!glyphRGBABuffer)
        return;

    memset(&((*glyphRGBABuffer)[0]), 0, imagewidth * height() * 4);

    float offsetinimg = 0;
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

        if (IDiskfont->EObtainInfo(&face->olf_EEngine,
                                   OT_GlyphMap8Bit, &glyph,
                                   TAG_END))
            continue; //do not handle error

#if 0 // causes wrong cursor offserts in text input
        if (i > 0)
            if (!IDiskfont->ESetInfo(&face->olf_EEngine,
                                     OT_GlyphCode, glyphs[i -1],
                                     OT_GlyphCode2, glyphs[i],
                                     TAG_END)) {
                FIXED kern = 0;
                if (!IDiskfont->EObtainInfo(&face->olf_EEngine,
                                            OT_TextKernPair, &kern,
                                            TAG_END))
                    if (kern)
                        offsetinimg -= kern / 65536.0 * ysize;
            }
#endif

        uint32 left = glyph->glm_BlackLeft;
        uint32 top = glyph->glm_BlackTop;
        uint32 width = glyph->glm_BlackWidth;
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
           for (uint32 x = 0; x < width; x++)
              if (((offsetinimg + 0.5) + imgoffsetx + x < imagewidth)
               && ((offsetinimg + 0.5) + imgoffsetx + x >= 0)) {
                 uint32 a = penalpha * glyph->glm_BitMap[top_mod_left + x] / 255;
                 if (a)
                    (*glyphRGBABuffer)[(imgoffsety + y) * imagewidth + (offsetinimg + 0.5) + imgoffsetx + x] = (a << 24) | rgb;
              }
        }

        offsetinimg += glyphBuffer.advanceAt(from + i);

        IDiskfont->EReleaseInfo(&face->olf_EEngine, OT_GlyphMap8Bit, glyph, TAG_END);
    }

    SDL_Surface* img;
    Uint32 rmask, gmask, bmask, amask;

    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
        
    img = SDL_CreateRGBSurfaceFrom((void*)glyphRGBABuffer->data(), imagewidth, height(),
                                    32, imagewidth * 4, rmask, gmask, bmask, amask);
    SDL_Rect sdlSrc, sdlDest;
    sdlDest.x = static_cast<int>(offset) - shiftleft + context->origin().width();
    sdlDest.y = static_cast<int>(point.y() - ascent()) + context->origin().height();
    sdlSrc.w = imagewidth;
    sdlSrc.h = height();
    sdlSrc.x = 0;
    sdlSrc.y = 0;


    SDL_Surface *surface = context->platformContext();
    if (context->transparencyLayer() == 1.0)
        SDL_BlitSurface(img, &sdlSrc, surface, &sdlDest);
    else {
        SDL_Surface *surfaceWithAlpha = applyTransparency(img, static_cast<int> (context->transparencyLayer() * 255));
        SDL_BlitSurface(surfaceWithAlpha, &sdlSrc, surface, &sdlDest);
        SDL_FreeSurface(surfaceWithAlpha);
    }

    SDL_FreeSurface(img);
    delete glyphRGBABuffer;
}

}
