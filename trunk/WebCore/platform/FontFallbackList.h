/*
 * This file is part of the internal font implementation.  It should not be included by anyone other than
 * FontMac.cpp, FontWin.cpp and Font.cpp.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// This file has no guards on purpose in order to detect redundant includes. This is a private header
// and so this should catch anyone trying to include this file in public cpp files.

#ifdef __OWB__
#include "Font.h"
#endif //__OWB__
#include "FontData.h"
#include "Shared.h"
#include "FontSelector.h"
#include <wtf/Vector.h>

#ifdef __OWB__
using BAL::Pitch;
using BAL::BTFont;
#endif //__OWB__

namespace WebCore {

#ifndef __OWB__
class Font;
#endif //__OWB__
class GraphicsContext;
class IntRect;
class FontDescription;
class FontPlatformData;
class FontSelector;

const int cAllFamiliesScanned = -1;

class FontFallbackList : public Shared<FontFallbackList> {
public:
    FontFallbackList();

    void invalidate(PassRefPtr<FontSelector>);
    
#ifdef __OWB__
    bool isFixedPitch(const BTFont* f) const { if (m_pitch == BAL::UnknownPitch) determinePitch(f); return m_pitch == BAL::FixedPitch; };
#else
    bool isFixedPitch(const Font* f) const { if (m_pitch == UnknownPitch) determinePitch(f); return m_pitch == FixedPitch; };
#endif
    void determinePitch(const Font*) const;

	bool loadingCustomFonts() const { return m_loadingCustomFonts; }
	
	FontSelector* fontSelector() const { return m_fontSelector.get(); }

private:
    const FontData* primaryFont(const Font* f) const { return fontDataAt(f, 0); }
    const FontData* fontDataAt(const Font*, unsigned index) const;
    const FontData* fontDataForCharacters(const Font*, const UChar*, int length) const;
    
    void setPlatformFont(const FontPlatformData&);

    mutable Vector<const FontData*, 1> m_fontList;
    mutable int m_familyIndex;
    mutable Pitch m_pitch;
    mutable bool m_loadingCustomFonts;
    RefPtr<FontSelector> m_fontSelector;

    friend class Font;
};

}

