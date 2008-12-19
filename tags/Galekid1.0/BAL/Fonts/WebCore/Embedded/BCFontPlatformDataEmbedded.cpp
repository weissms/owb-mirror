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
#include "Logging.h"
#include "FontPlatformData.h"

#include "CString.h"
#include "PlatformString.h"
#include "FontDescription.h"
#include "fonts/pixelfont.h"

namespace WKAL {

FontPlatformData::FontPlatformData(WTF::HashTableDeletedValueType)
        : m_pattern(hashTableDeletedFontValue())
        , m_scaledFont(0)
{
}

FontPlatformData::FontPlatformData()
        : m_pattern(0)
        , m_scaledFont(0)
{
}

FontPlatformData::FontPlatformData(const FontDescription& fontDescription, const AtomicString& familyName)
    : m_pattern(0)
    , m_size(fontDescription.computedSize())
    , m_syntheticBold(false)
    , m_syntheticOblique(false)
    , m_scaledFont(0)
{
    if (fontDescription.italic()) {
        m_pixelFont = &NormalItalicPixelFont;
        m_syntheticOblique = true;
    } else if (fontDescription.weight() >= FontWeightBold) {
        m_pixelFont = &NormalBoldPixelFont;
        m_syntheticBold = true;
    } else 
        m_pixelFont = &NormalPixelFont;
    FontPlatformData::init();
}

FontPlatformData::FontPlatformData(float size, bool bold, bool italic)
    : m_pattern(0)
    , m_size(size)
    , m_syntheticBold(bold)
    , m_syntheticOblique(italic)
    , m_scaledFont(0)
    , m_pixelFont(0)
{
    if (italic)
        m_pixelFont = &NormalItalicPixelFont;
    else if (bold)
        m_pixelFont = &NormalBoldPixelFont;
    else 
        m_pixelFont = &NormalPixelFont;
}

FontPlatformData::FontPlatformData(BalFontFace* fontFace, int size, bool bold, bool italic)
    : m_pattern(0)
    , m_size(size)
    , m_syntheticBold(bold)
    , m_syntheticOblique(italic)
    , m_scaledFont(0)
    , m_pixelFont(0)
{
    if (italic)
        m_pixelFont = &NormalItalicPixelFont;
    else if (bold)
        m_pixelFont = &NormalBoldPixelFont;
    else 
        m_pixelFont = &NormalPixelFont;
}

bool FontPlatformData::init()
{
    return true;
}

FontPlatformData::~FontPlatformData()
{
}

bool FontPlatformData::isFixedPitch()
{
    return true;
}

void FontPlatformData::setFont(BalFont* cr) const
{
    ASSERT(m_scaledFont);

    BalNotImplemented();
}

bool FontPlatformData::operator==(const FontPlatformData& other) const
{
    if (m_pixelFont == 0 && other.m_pixelFont == 0)
        return true;
    if (m_pixelFont == 0 || other.m_pixelFont == 0)
        return false;
    if (m_pixelFont->width == other.m_pixelFont->width)
        return true;
    return false;

}

}
