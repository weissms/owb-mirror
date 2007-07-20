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
 * @file  BTFont.cpp
 *
 * SDL Implementation file for Font.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BTFONT_CPP
#define BTFONT_CPP


#include "config.h"
#include "BALConfiguration.h"
#include <wtf/MathExtras.h>

#include "../Implementations/Graphics/SDL/BCNativeImageSDL.h"
#include "BIGraphicsContext.h"
#include "BIGraphicsDevice.h"
#include "BTLogHelper.h" // to be placed after math.h
#include "FontFallbackList.h"
#include "Font.h"
#include "Color.h"
#include "FloatPoint.h"
#include "FloatRect.h"
#include "IntPoint.h"
#include <SDL/SDL_gfxPrimitives.h>
#include "TextStyle.h"
#include "fonts/pixelfont.h"

using WebCore::IntPoint;
using WebCore::FloatPoint;
using WebCore::FloatRect;

namespace BAL {

// hide declaration of d->m_pixelFont from header Font.h
class FontPrivate {
    public:
        PixelFontType *m_pixelFont;
};

#define notImplemented() printf("%s:%d:%s() Not implemented\n", __FILE__, __LINE__, __func__);

// ============================================================================================
// Font Implementation (Cross-Platform Portion)
// ============================================================================================

Font::Font() :m_fontList(0), m_letterSpacing(0), m_wordSpacing(0), d(new FontPrivate()) {}
Font::Font(const WebCore::FontDescription& fd, short letterSpacing, short wordSpacing)
    : m_fontDescription(fd),
    m_fontList(0),
    m_letterSpacing(letterSpacing),
    m_wordSpacing(wordSpacing),
    d(new FontPrivate())
{
    // Process font size
    int size = static_cast<int>(m_fontDescription.specifiedSize());

    // Init with default font
    d->m_pixelFont = &NormalPixelFont;

    // then change if needed
    if (size < NORMAL_SIZE_LOW_LIMIT)
      d->m_pixelFont = &SmallPixelFont;

    if (size > NORMAL_SIZE_HIGH_LIMIT)
      d->m_pixelFont = &BigPixelFont;
}

Font::Font(const Font& other) :
    d(new FontPrivate())
{
    m_fontDescription = other.m_fontDescription;
    m_fontList = other.m_fontList;
    m_letterSpacing = other.m_letterSpacing;
    m_wordSpacing = other.m_wordSpacing;
    d->m_pixelFont = other.d->m_pixelFont;
}

Font& Font::operator=(const Font& other)
{
    if (&other != this) {
        m_fontDescription = other.m_fontDescription;
        m_fontList = other.m_fontList;
        m_letterSpacing = other.m_letterSpacing;
        m_wordSpacing = other.m_wordSpacing;
        d->m_pixelFont = other.d->m_pixelFont;
    }
    return *this;
}

Font::~Font()
{
}

const BIFontData* Font::primaryFont() const
{
    return 0;
}

const BIFontData* Font::fontDataAt(unsigned index) const
{
    return 0;
}

const BIFontData* Font::fontDataForCharacters(const UChar* characters, int length) const
{
    return 0;
}

void Font::update() const
{
}

int Font::width(const TextRun& run) const
{
    return width(run, TextStyle());
}

int Font::width(const TextRun& run, const TextStyle& style) const
{
    return lroundf(floatWidth(run, style));
}

/// between baseline and top of letter
/// ascent + descent = height of letter
int Font::ascent() const
{
    return d->m_pixelFont->height;
}

/// between baseline and bottom of letter
/// ascent + descent = height of letter
int Font::descent() const
{
    return 0;
}

/// Height of a line
int Font::lineSpacing() const
{
    return d->m_pixelFont->height;
}
/// between baseline ascent
float Font::xHeight() const
{
    return 1;
}

bool Font::isFixedPitch() const
{
    return true;
}

bool Font::canUseGlyphCache(const TextRun& run) const
{
    // Start from 0 since drawing and highlighting also measure the characters before run->from
    for (int i = 0; i < run.to(); i++) {
        const UChar c = run[i];
        if (c < 0x300)      // U+0300 through U+036F Combining diacritical marks
            continue;
        if (c <= 0x36F)
            return false;

        if (c < 0x0591 || c == 0x05BE)     // U+0591 through U+05CF excluding U+05BE Hebrew combining marks, Hebrew punctuation Paseq, Sof Pasuq and Nun Hafukha
            continue;
        if (c <= 0x05CF)
            return false;

        if (c < 0x0600)     // U+0600 through U+1059 Arabic, Syriac, Thaana, Devanagari, Bengali, Gurmukhi, Gujarati, Oriya, Tamil, Telugu, Kannada, Malayalam, Sinhala, Thai, Lao, Tibetan, Myanmar
            continue;
        if (c <= 0x1059)
            return false;

        if (c < 0x1100)     // U+1100 through U+11FF Hangul Jamo (only Ancient Korean should be left here if you precompose; Modern Korean will be precomposed as a result of step A)
            continue;
        if (c <= 0x11FF)
            return false;

        if (c < 0x1780)     // U+1780 through U+18AF Khmer, Mongolian
            continue;
        if (c <= 0x18AF)
            return false;

        if (c < 0x1900)     // U+1900 through U+194F Limbu (Unicode 4.0)
            continue;
        if (c <= 0x194F)
            return false;

        if (c < 0x20D0)     // U+20D0 through U+20FF Combining marks for symbols
            continue;
        if (c <= 0x20FF)
            return false;

        if (c < 0xFE20)     // U+FE20 through U+FE2F Combining half marks
            continue;
        if (c <= 0xFE2F)
            return false;
    }

    return true;

}

void Font::drawSimpleText(BIGraphicsContext* context, const TextRun& run, const TextStyle& style, const FloatPoint& point) const
{
    WebCore::Color color = context->strokeColor();

    // Process normal, bold, italic styles
    if (m_fontDescription.italic()) {
      if (m_fontDescription.bold())// Bold && italic
	gfxPrimitivesSetFont(d->m_pixelFont->bold, d->m_pixelFont->width, d->m_pixelFont->height);
      else // Only italic
	gfxPrimitivesSetFont(d->m_pixelFont->italic, d->m_pixelFont->width, d->m_pixelFont->height);
    }
    else if (m_fontDescription.bold())// Only bold
      gfxPrimitivesSetFont(d->m_pixelFont->bold, d->m_pixelFont->width, d->m_pixelFont->height);
    else
      gfxPrimitivesSetFont(d->m_pixelFont->normal, d->m_pixelFont->width, d->m_pixelFont->height);
    
    IntPoint intPoint;
    IntRect rect(0, 0, d->m_pixelFont->width, d->m_pixelFont->height); 
    intPoint.setX(static_cast<uint16_t> (point.x()));
    intPoint.setY(static_cast<uint16_t> (point.y()) - ascent());
    
    SDL_Surface* surface;
    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
    surface = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, d->m_pixelFont->width, d->m_pixelFont->height, 32,
                                  rmask, gmask, bmask, amask);
    BCNativeImage image(surface);
    for(int len=0; len<run.length(); len++) {
        if (run[len] <= 0xff && run[len] >= 32)
            // FIXME we should fill transparent black, but characterRGBA doesn't paint in it
            SDL_FillRect(surface, NULL, 0xffffffff);                      
            characterRGBA(surface, 0, 0, run[len], color.red(), color.green(), color.blue(), color.alpha());
           
	        getBIGraphicsDevice()->copy(*(context->widget()), image, rect, intPoint, context->alphaLayer());
            intPoint.move(d->m_pixelFont->width, 0);
    }
}

void Font::drawText(BIGraphicsContext* context, const TextRun& run, const TextStyle& style, const FloatPoint& point) const
{
    if (canUseGlyphCache(run))
        drawSimpleText(context, run, style, point);
    else
        drawComplexText(context, run, style, point);
}

float Font::floatWidth(const TextRun& run, const TextStyle& style) const
{
    if (canUseGlyphCache(run))
        return floatWidthForSimpleText(run, style, 0, 0);
    else
        return floatWidthForComplexText(run, style);
}

float Font::floatWidthForSimpleText(const TextRun& run, const TextStyle& style, float* startPosition, BIGlyphBuffer* glyphBuffer) const
{
    if (startPosition) {
        if (style.ltr())
            *startPosition = run.from() * d->m_pixelFont->width;
        else
            *startPosition = (run.to()-run.from())* d->m_pixelFont->width;
    }
    return run.to() * d->m_pixelFont->width;
}

FloatRect Font::selectionRectForText(const TextRun& run, const TextStyle& style, const IntPoint& point, int h) const
{
    if (canUseGlyphCache(run))
        return selectionRectForSimpleText(run, style, point, h);
    return selectionRectForComplexText(run, style, point, h);
}

FloatRect Font::selectionRectForSimpleText(const TextRun& textrun, const TextStyle& style, const IntPoint& point, int h) const
{
    TextRun run(textrun);
    run.makeComplete();

    return FloatRect(point.x() + run.from() * d->m_pixelFont->width, point.y(), (run.to() - run.from()) * d->m_pixelFont->width, h);
}

int Font::offsetForPosition(const TextRun& run, const TextStyle& style, int x, bool includePartialGlyphs) const
{
    if (canUseGlyphCache(run))
        return offsetForPositionForSimpleText(run, style, x, includePartialGlyphs);
    return offsetForPositionForComplexText(run, style, x, includePartialGlyphs);
}

int Font::offsetForPositionForSimpleText(const TextRun& run, const TextStyle& style, int x, bool includePartialGlyphs) const
{
    return  run.to() - run.from();
}

void Font::drawGlyphs(BIGraphicsContext* graphicsContext, const BIFontData* font,
                      const BIGlyphBuffer& glyphBuffer,
                      int from, int numGlyphs, const FloatPoint& point) const
{
}

FloatRect Font::selectionRectForComplexText(const TextRun&, const TextStyle&, const IntPoint&, int /*h*/) const
{
    notImplemented();
    return FloatRect();
}

void Font::drawComplexText(BIGraphicsContext*, const TextRun&, const TextStyle&, const FloatPoint&) const
{
    notImplemented();
}

float Font::floatWidthForComplexText(const TextRun&, const TextStyle&) const
{
    notImplemented();
    return 0;
}

int Font::offsetForPositionForComplexText(const TextRun&, const TextStyle&, int /*x*/, bool /*includePartialGlyphs*/) const
{
    notImplemented();
    return 0;
}

}
#endif // BTFONT_CPP


