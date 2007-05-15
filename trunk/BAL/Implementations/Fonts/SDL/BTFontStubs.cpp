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
 * Implementation file for Font.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BTFONT_CPP
#define BTFONT_CPP


#include "config.h"
#include "BTFont.h"
#include "FloatRect.h"
#include "FloatPoint.h"
#include "IntPoint.h"
#include "BTFontFallbackList.h"

using WebCore::FloatPoint;
using WebCore::FloatRect;
using WebCore::IntPoint;

namespace BAL {

    FontFallbackList::FontFallbackList()
    {
    }

    FontFallbackList::~FontFallbackList()
    {
    }
// ============================================================================================
// Font Implementation (Cross-Platform Portion)
// ============================================================================================

Font::Font() :m_fontList(0), m_letterSpacing(0), m_wordSpacing(0) {}
Font::Font(const WebCore::FontDescription& fd, short letterSpacing, short wordSpacing)
    : m_fontDescription(fd),
  m_fontList(0),
  m_letterSpacing(letterSpacing),
  m_wordSpacing(wordSpacing)
  {}

Font::Font(const Font& other)
{
}

Font& Font::operator=(const Font& other)
{
    return *this;
}

Font::~Font()
{
}

const BIFontData* Font::primaryFont() const
{
}

const BIFontData* Font::fontDataAt(unsigned index) const
{
}

const BIFontData* Font::fontDataForCharacters(const UChar* characters, int length) const
{
}

void Font::update() const
{
}

int Font::width(const TextRun& run, const TextStyle& style) const
{
    return 0;
}

int Font::ascent() const
{
    return 0;
}

int Font::descent() const
{
    return 0;
}

int Font::lineSpacing() const
{
    return 0;
}

float Font::xHeight() const
{
    return 0;
}

bool Font::isFixedPitch() const
{
    return 0;
}

// FIXME: These methods will eventually be cross-platform, but to keep Windows compiling we'll make this Apple-only for now.
bool Font::gAlwaysUseComplexPath = false;
void Font::setAlwaysUseComplexPath(bool alwaysUse)
{
    gAlwaysUseComplexPath = alwaysUse;
}

bool Font::canUseGlyphCache(const TextRun& run) const
{
    return true;
}

void Font::drawSimpleText(BIGraphicsContext* context, const TextRun& run, const TextStyle& style, const FloatPoint& point) const
{
}

void Font::drawText(BIGraphicsContext* context, const TextRun& run, const TextStyle& style, const FloatPoint& point) const
{
}

float Font::floatWidth(const TextRun& run, const TextStyle& style) const
{
    return 0;
}

float Font::floatWidthForSimpleText(const TextRun& run, const TextStyle& style,
                                      const BIFontData* substituteFont, float* startPosition, BIGlyphBuffer* glyphBuffer) const {
    return 0;
}

FloatRect Font::selectionRectForText(const TextRun& run, const TextStyle& style, const IntPoint& point, int h) const
{
}

FloatRect Font::selectionRectForSimpleText(const TextRun& run, const TextStyle& style, const IntPoint& point, int h) const
{
}

int Font::offsetForPosition(const TextRun& run, const TextStyle& style, int x, bool includePartialGlyphs) const
{
}

int Font::offsetForPositionForSimpleText(const TextRun& run, const TextStyle& style, int x, bool includePartialGlyphs) const
{
}

void Font::drawGlyphs(BIGraphicsContext* graphicsContext, const BIFontData* font,
                      const BIGlyphBuffer *glyphBuffer,
                      int from, int numGlyphs, const FloatPoint& point) const
{
}

FloatRect Font::selectionRectForComplexText(const TextRun&, const TextStyle&, const IntPoint&, int /*h*/) const
{
}

void Font::drawComplexText(BIGraphicsContext*, const TextRun&, const TextStyle&, const FloatPoint&) const
{
}

float Font::floatWidthForComplexText(const TextRun&, const TextStyle&) const
{
    return 0;
}

int Font::offsetForPositionForComplexText(const TextRun&, const TextStyle&, int /*x*/, bool /*includePartialGlyphs*/) const
{
    return 0;
}

}
#endif // BTFONT_CPP


