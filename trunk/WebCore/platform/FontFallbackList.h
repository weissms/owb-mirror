/*
 * This file is part of the internal font implementation.  It should not be included by anyone other than
 * FontMac.cpp, FontWin.cpp and Font.cpp.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2007 Pleyo.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

// This file has no guards on purpose in order to detect redundant includes. This is a private header
// and so this should catch anyone trying to include this file in public cpp files.

#include "FontData.h"
#include "Shared.h"
#include <wtf/Vector.h>

#ifdef __OWB__
namespace WebCore {

class IntRect;
class FontDescription;
class FontPlatformData;

}
using WebCore::Shared;

namespace BAL {

class Font;
class GraphicsContext;
#else
namespace WebCore {

class Font;
class GraphicsContext;
class IntRect;
class FontDescription;
class FontPlatformData;
#endif

const int cAllFamiliesScanned = -1;

class FontFallbackList : public Shared<FontFallbackList> {
public:
    FontFallbackList();

    void invalidate();

#ifdef __OWB__
    bool isFixedPitch(const Font* f) const { if (m_pitch == BIFontData::UnknownPitch) determinePitch(f); return m_pitch == BIFontData::FixedPitch; };
#else
    bool isFixedPitch(const Font* f) const { if (m_pitch == UnknownPitch) determinePitch(f); return m_pitch == FixedPitch; };
#endif

    void determinePitch(const Font*) const;

private:
    const FontData* primaryFont(const Font* f) const { return fontDataAt(f, 0); }
    const FontData* fontDataAt(const Font*, unsigned index) const;
    const FontData* fontDataForCharacters(const Font*, const UChar*, int length) const;

    void setPlatformFont(const FontPlatformData&);

    mutable Vector<const FontData*, 1> m_fontList;
    mutable int m_familyIndex;
#ifdef __OWB__
    mutable BIFontData::Pitch m_pitch;
#else
    mutable Pitch m_pitch;
#endif

    friend class Font;
};

}

