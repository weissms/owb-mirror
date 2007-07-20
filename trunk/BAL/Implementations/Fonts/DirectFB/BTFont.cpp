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
 * @file  Font.cpp
 *
 * SDL_TTF Implementation file for Font.
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

#include "../Implementations/Graphics/SDL/BCNativeImage.h"
#include "BIGraphicsContext.h"
#include "BTLogHelper.h" // to be placed after math.h
#include "Color.h"
#include "FloatPoint.h"
#include "FloatRect.h"
#include "Font.h"
#include "FontFallbackList.h"
#include "IntPoint.h"
#include <directfb.h>
#include "TextStyle.h"
#include <map>

using WebCore::IntPoint;
using WebCore::FloatPoint;
using WebCore::FloatRect;

namespace BAL
{

const UChar nonBreakingSpace = 0x00A0;
const UChar zeroWidthSpace = 0x200B;

#define DFBCHECK(x...)                                         \
{                                                            \
    DFBResult err = x;                                         \
                                                               \
    if (err != DFB_OK)                                         \
{                                                        \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
        DirectFBErrorFatal( #x, err );                         \
}                                                        \
}

// FIXME SRO we need to implement cache for fonts
std::map<int, IDirectFBFont*> m_fontMap;

// hide declaration of IDirectFBFont from header Font.h
class FontPrivate {
    public:
        IDirectFBFont *m_ttfFont;
};

Font::Font() : m_fontList(0), m_letterSpacing(0), m_wordSpacing(0), d(new FontPrivate())
{
    //FIXME: retrieve dfb correctly
    if (m_fontMap[12] == 0) {
        DFBFontDescription font_dsc;

        font_dsc.flags = DFDESC_ATTRIBUTES | DFDESC_HEIGHT;
        font_dsc.attributes = DFFA_NONE;
        font_dsc.height = 12;
        DFBCHECK(dfb->CreateFont(dfb, "/usr/share/fonts/owb.ttf", &font_dsc, &m_fontMap[12]));
    }
    d->m_ttfFont = m_fontMap[12];
}

Font::Font(const WebCore::FontDescription& fd, short letterSpacing, short wordSpacing)
        : m_fontDescription(fd),
        m_fontList(0),
        m_letterSpacing(letterSpacing),
        m_wordSpacing(wordSpacing),
        d(new FontPrivate())
{
    // Process font size
    int size = static_cast<int> (m_fontDescription.specifiedSize());

    // FIXME: retrieve correctly interface name
    if (m_fontMap[size] == 0) {
        DFBFontDescription font_dsc;

        font_dsc.flags = DFDESC_ATTRIBUTES | DFDESC_HEIGHT;
        font_dsc.attributes = DFFA_NONE;
        font_dsc.height = size;
        DFBCHECK(dfb->CreateFont(dfb, "/usr/share/fonts/owb.ttf", &font_dsc, &m_fontMap[size]));
    }
    d->m_ttfFont = m_fontMap[size];

    // FIXME: Process normal, bold, italic styles
//     if (m_fontDescription.italic())
//     {
//         if (m_fontDescription.bold())
//         { // Bold && italic
//             TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_BOLD | TTF_STYLE_ITALIC);
//         }
//         else
//         { // Only italic
//             TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_ITALIC);
//         }
//     }
//     else if (m_fontDescription.bold())
//     { // Only bold
//         TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_BOLD);
//     }
//     else
//     {
//         TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_NORMAL);
//     }
}

Font::Font(const Font& other) : d(new FontPrivate())
{
    m_fontDescription = other.m_fontDescription;
    m_fontList = other.m_fontList;
    m_letterSpacing = other.m_letterSpacing;
    m_wordSpacing = other.m_wordSpacing;
    d->m_ttfFont = other.d->m_ttfFont;
}

Font& Font::operator=(const Font& other)
{
    if (&other != this)
    {
        m_fontDescription = other.m_fontDescription;
        m_fontList = other.m_fontList;
        m_letterSpacing = other.m_letterSpacing;
        m_wordSpacing = other.m_wordSpacing;
        d->m_ttfFont = other.d->m_ttfFont;
    }
    return *this;

}

Font::~Font()
{
//     font->Release (font);

    delete d;
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
    int ascent = 0;
    if (d->m_ttfFont)
        DFBCHECK(font->GetAscender(font, &ascent));
    return ascent;
}

/// between baseline and bottom of letter
/// ascent + descent = height of letter
int Font::descent() const
{
    int descent = 0;
    if (d->m_ttfFont)
        DFBCHECK(font->GetDescender(font, &descent));
    return descent; // This is a negative value
}

/// Height of a line
int Font::lineSpacing() const
{
    int height = 0;
    if (d->m_ttfFont)
        DFBCHECK(font->GetHeight(font, &height));
    return height;
}

/// between baseline ascent
// TODO: this a lack in implementation. So FIX it!!!
float Font::xHeight() const
{
    if (d->m_ttfFont)
        return DFBCHECK(font->GetHeight(font, &height)); // In fact this is line spacing!!!
    else
        return 0;
}

// TODO: this a lack in implementation. So FIX it!!!
bool Font::isFixedPitch() const
{
    if (d->m_ttfFont)
        return false;
    else
        return true;
}

bool Font::canUseGlyphCache(const TextRun& run) const
{
    // Start from 0 since drawing and highlighting also measure the characters before run->from
    for (int i = 0; i < run.to(); i++)
    {
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

void copyTextRunTo(const TextRun& run, UChar *word)
{
    int word_size = run.length() - run.from(), j = 0;
    for (int i = run.from(); i < run.length(); i++)
    {
        if (Font::treatAsSpace(run[i]))
        {
            word[j] = ' ';
            j++;
        }
        else
        {
            word[j] = run[i];
            j++;
        }
    }
    word[j]='\0';
}

/* SDL ttf implementation may be lighter ??? */
void Font::drawSimpleText(BIGraphicsContext* context, const TextRun& run, const TextStyle& style, const FloatPoint& point) const
{
    WebCore::Color  color = context->strokeColor();
    BCNativeImage*  text_surface = static_cast<BCNativeImage*>(context->getNativeImage())->getSurface();

    // FIXME: Process normal, bold, italic styles
//     if (m_fontDescription.italic())
//     {
//         if (m_fontDescription.bold())
//         { // Bold && italic
//             TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_BOLD | TTF_STYLE_ITALIC);
//         }
//         else
//         { // Only italic
//             TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_ITALIC);
//         }
//     }
//     else if (m_fontDescription.bold())
//     { // Only bold
//         TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_BOLD);
//     }
//     else
//     {
//         TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_NORMAL);
//     }

    DFBCHECK(text_surface->SetFont(text_surface, d->m_ttfFont));

    // Set font color
    DFBCHECK(text_surface->SetColor(text_surface, color.red(), color.green(), color.blue(), color.alpha()));

    // Draw font
    int wordSize = run.length() - run.from();
    UChar word[wordSize];
    copyTextRunTo(run, word);

    DFBCHECK(text_surface->DrawString(text_surface, word, -1, point.x(), point.y(), DSTF_TOP | DSTF_LEFT));
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

float Font::floatWidthForSimpleText(const TextRun& run, const TextStyle& style, float* startPosition, GlyphBuffer* glyphBuffer) const
{
    int     text_width;
    // FIXME: Process normal, bold, italic styles
//     if (m_fontDescription.italic()) {
//         if (m_fontDescription.bold()) {
//             // Bold && italic
//             TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_BOLD | TTF_STYLE_ITALIC);
//         } else {
//             // Only italic
//             TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_ITALIC);
//         }
//     } else if (m_fontDescription.bold()) {
//         // Only bold
//         TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_BOLD);
//     } else
//         TTF_SetFontStyle(d->m_ttfFont, TTF_STYLE_NORMAL);

    int wordSize = run.length() - run.from();
    UChar word[wordSize];
    copyTextRunTo(run, word);

    DFBCHECK(d->m_ttfFont->GetStringWidth(d->m_ttfFont, word, -1, &text_width));
    if (startPosition) {
        if (style.ltr())
            *startPosition = text_width ;
        else
            *startPosition = text_width;
    }
    return static_cast<float> (text_width);
}

FloatRect Font::selectionRectForText(const TextRun& run, const TextStyle& style, const IntPoint& point, int h) const
{
    if (canUseGlyphCache(run))
        return selectionRectForSimpleText(run, style, point, h);
    return selectionRectForComplexText(run, style, point, h);
}

FloatRect Font::selectionRectForSimpleText(const TextRun& textrun, const TextStyle& style, const IntPoint& point, int h) const
{
    int height, width;

    TextRun run(textrun);
    run.makeComplete();
    UChar word[run.length() - run.from()];
    copyTextRunTo(run, word);
    DFBCHECK(d->m_ttfFont->GetStringWidth(d->m_ttfFont, word, -1, &width));
    return FloatRect(point.x(), point.y(), width, height);

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
                        const GlyphBuffer& glyphBuffer,
                        int from, int numGlyphs, const FloatPoint& point) const
{
    BALNotImplemented();
}

FloatRect Font::selectionRectForComplexText(const TextRun&, const TextStyle&, const IntPoint&, int /*h*/) const
{
    BALNotImplemented();
    return FloatRect();
}

void Font::drawComplexText(BIGraphicsContext*, const TextRun&, const TextStyle&, const FloatPoint&) const
{
    BALNotImplemented();
}

float Font::floatWidthForComplexText(const TextRun&, const TextStyle&) const
{
    BALNotImplemented();
    return 0;
}

int Font::offsetForPositionForComplexText(const TextRun&, const TextStyle&, int /*x*/, bool /*includePartialGlyphs*/) const
{
    BALNotImplemented();
    return 0;
}

}
#endif // BTFONT_CPP

