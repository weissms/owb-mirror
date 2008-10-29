/*
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
#include "SDL.h"
#include "SimpleFontData.h"
#include <limits.h>
#include <ft2build.h>

#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <fontconfig/fontconfig.h>

namespace WKAL {

#define IS_HIGH_SURROGATE(u)  ((UChar)(u) >= (UChar)0xd800 && (UChar)(u) <= (UChar)0xdbff)
#define IS_LOW_SURROGATE(u)  ((UChar)(u) >= (UChar)0xdc00 && (UChar)(u) <= (UChar)0xdfff)

#if PLATFORM(AMIGAOS4)
static
#endif
SDL_Surface* applyTransparency(SDL_Surface* origin, const uint8_t alphaChannel)
{
    // Blend is not very optimized for now, to say the least.
    SDL_Surface *final = NULL;
    Uint32 rmask, gmask, bmask, amask;
    // SDL interprets each pixel as a 32-bit number, so our masks must depend
    // on the endianness (byte order) of the machine
#if !PLATFORM(AMIGAOS4) && SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
#endif
    uint16_t width = origin->w;
    uint16_t height = origin->h;
    void *data = malloc(sizeof(uint32_t) * height * width);
    SDL_LockSurface(origin);
    memcpy(data, origin->pixels, sizeof(uint32_t) * height * width);
    final = SDL_CreateRGBSurfaceFrom(data, origin->w, origin->h, 32, origin->pitch, rmask, gmask, bmask, amask);
    final->flags &= ~SDL_PREALLOC;
    SDL_UnlockSurface(origin);
    SDL_LockSurface(final);
    uint32_t alpha = 0;
    for (uint16_t i = 0; i < height; i++) {
        for (uint16_t j = 0; j < width; j++) {
            alpha = ((*(uint32_t*)final->pixels >> 24 ) * alphaChannel) >> 8;;
            *(uint32_t*)final->pixels = (*(uint32_t*)final->pixels & 0x00ffffff) | (alpha << 24);
            final->pixels = (uint32_t*)final->pixels + 1;
        }
    }
    final->pixels = data;
    SDL_UnlockSurface(final);
    return final;
}



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
    GlyphBufferGlyph* glyphs = const_cast<GlyphBufferGlyph*>(glyphBuffer.glyphs(from));
    FT_BitmapGlyph ftBitmapGlyph[numGlyphs];

    // glyphBoxX[i].x() = xGlyphMin for glyph #i
    // glyphBoxX[i].y() = xGlyphMax for glyph #i
    Vector<IntPoint> glyphBoxX(numGlyphs);

    // glyphBoxY[i].x() = yGlyphMin for glyph #i
    // glyphBoxY[i].y() = yGlyphMax for glyph #i
    Vector<IntPoint> glyphBoxY(numGlyphs);

    float offset = 0; 
    int xMin = INT_MAX;
    int xMax = INT_MIN;
    int yMin = INT_MAX;
    int yMax = INT_MIN;
    FT_Face face = font->m_font.m_face;
    for (int i = 0; i < numGlyphs; i++) {
        FT_Glyph ftGlyph;

        if (FT_Load_Glyph(face, glyphs[i], FT_LOAD_DEFAULT)) {
            printf("FT_Load_Glyph fail for face=%p with glyphs[i]=%d\n", face, glyphs[i]);
            continue; //do not handle error
        }

        if (FT_Get_Glyph(face->glyph, &ftGlyph)) {
            printf("FT_Render_Glyph fail\n");
            continue; //do not handle error
        }
        FT_Glyph_To_Bitmap(&ftGlyph, FT_RENDER_MODE_NORMAL, 0, 1);
        ftBitmapGlyph[i] = (FT_BitmapGlyph)ftGlyph;

        if (ftBitmapGlyph[i]->bitmap.buffer) {
            if (i > 0 && FT_HAS_KERNING(face)) {
                FT_Vector kerning;
                FT_Get_Kerning(face, glyphs[i - 1], glyphs[i], FT_KERNING_DEFAULT, &kerning);
                offset += DOUBLE_FROM_26_6(kerning.x);
            }

            FT_BBox bbox;
            FT_Glyph_Get_CBox(ftGlyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);

            // Calculate the glyph box minimum / maximum.
            int xGlyphMin = static_cast<int>(offset + DOUBLE_FROM_26_6(bbox.xMin));
            int xGlyphMax = static_cast<int>(offset + DOUBLE_FROM_26_6(bbox.xMax));
            int yGlyphMin = static_cast<int>(DOUBLE_FROM_26_6(bbox.yMin));
            int yGlyphMax = static_cast<int>(DOUBLE_FROM_26_6(bbox.yMax));

            // Update the global minimum and maximum.
            if (xGlyphMin < xMin)
                xMin = xGlyphMin;
            if (xGlyphMax > xMax)
                xMax = xGlyphMax;
            if (yGlyphMin < yMin)
                yMin = yGlyphMin;
            if (yGlyphMax > yMax)
                yMax = yGlyphMax;

            glyphBoxX[i].setX(xGlyphMin);
            glyphBoxX[i].setY(xGlyphMax);
            glyphBoxY[i].setX(yGlyphMin);
            glyphBoxY[i].setY(yGlyphMax);
        }
        offset += glyphBuffer.advanceAt(from + i);
    }

    int x_offset = 0;
    if (glyphBoxX[0].x() < 0) {
        x_offset = glyphBoxX[0].x();
        glyphBoxX[0].setX(0);
    }

    // Calculate the enclosing box height and width.
    uint16_t height = yMax - yMin;
    // FIXME: It should be xMax - xMin but with it, we would assert in acid3.
    uint16_t width = xMin < 0 ? xMax : xMax + xMin;
    
    Vector<unsigned>* glyphRGBABuffer = new Vector<unsigned>(width * height);
    glyphRGBABuffer->fill(0);

    bool isMono = (ftBitmapGlyph[0]->bitmap.pixel_mode == FT_PIXEL_MODE_MONO); 
    // Set the text color to use for drawing.
    Color penColor = context->fillColor();
    for (int i = 0; i < numGlyphs; i++) {
        int yOffset = height - glyphBoxY[i].y() + yMin;
        int baseIndex = yOffset * width + glyphBoxX[i].x();
        unsigned char* bitmapAddr = ftBitmapGlyph[i]->bitmap.buffer;

        // Check for overflow.
        // If baseIndex overflows, then lastIndex will too so it is ok to only check for overflow on lastIndex.
        long lastIndex = baseIndex + (ftBitmapGlyph[i]->bitmap.rows - 1) * width + ftBitmapGlyph[i]->bitmap.width - 1;
        if (baseIndex < 0 || lastIndex < 0 || lastIndex > UINT_MAX)
            continue;

        if (isMono) {
            unsigned pixelColor = (penColor.alpha() << 24) | (penColor.red() << 16) | (penColor.green() << 8) | penColor.blue();
            for (int j = 0; j < ftBitmapGlyph[i]->bitmap.rows; j++) {
                unsigned char* bufferAddr = bitmapAddr;
                for (int k = 0; k < ftBitmapGlyph[i]->bitmap.width; k++) {
                    // We can safely cast to unsigned int as we have already checked this glyph.
                    unsigned int index = static_cast<unsigned int>(baseIndex + j * width + k);

                    if ((*bitmapAddr) & (1 << (7 - k % 8)))
                        (*glyphRGBABuffer)[index] = pixelColor;

                    if (k > 0 && (k % 8) == 0)
                        *bitmapAddr++;
                }
                bitmapAddr = bufferAddr + ftBitmapGlyph[i]->bitmap.pitch;
            }
        } else {
            for (int j = 0; j < ftBitmapGlyph[i]->bitmap.rows; j++)
                for (int k = 0; k < ftBitmapGlyph[i]->bitmap.width; k++) {
                    // We can safely cast to unsigned int as we have already checked this glyph.
                    unsigned int index = static_cast<unsigned int>(baseIndex + j * width + k);
                    (*glyphRGBABuffer)[index] = (static_cast<unsigned>((penColor.alpha() * *bitmapAddr++) / 255) << 24) | (penColor.red() << 16) | (penColor.green() << 8) | penColor.blue();
                }
        }
        FT_Done_Glyph((FT_Glyph) ftBitmapGlyph[i]);
    }

    SDL_Surface* img;
    Uint32 rmask, gmask, bmask, amask;

#if !PLATFORM(AMIGAOS4) && SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
#endif
        
    img = SDL_CreateRGBSurfaceFrom((void*)glyphRGBABuffer->data(), width, height,
                                    32, width * 4, rmask, gmask, bmask, amask);
    SDL_Rect sdlSrc, sdlDest;
    sdlDest.x = static_cast<int>(point.x()) + x_offset + context->origin().x();
    sdlDest.y = static_cast<int>(point.y()) - yMax + context->origin().y();
    sdlSrc.w = width;
    sdlSrc.h = height;
    sdlSrc.x = 0;
    sdlSrc.y = 0;

    SDL_Surface* surface = context->platformContext();
    if (context->transparencyLayer() == 1.0) {
        SDL_BlitSurface(img, &sdlSrc, surface, &sdlDest);
    }
    else {
        SDL_Surface* surfaceWithAlpha = applyTransparency(img, static_cast<int> (context->transparencyLayer() * 255));
        SDL_BlitSurface(surfaceWithAlpha, &sdlSrc, surface, &sdlDest);
        SDL_FreeSurface(surfaceWithAlpha);
    }

    SDL_FreeSurface(img);
    delete glyphRGBABuffer;
}

} // namespace WKAL
