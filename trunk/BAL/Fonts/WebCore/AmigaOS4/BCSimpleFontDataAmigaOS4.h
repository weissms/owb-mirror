/*
 * This file is part of the internal font implementation.
 *
 * Copyright (C) 2006, 2008 Apple Computer, Inc.
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

#ifndef SimpleFontData_h
#define SimpleFontData_h

#include "FontData.h"
#include "FontPlatformData.h"
#include "GlyphMetricsMap.h"
#include "GlyphPageTreeNode.h"
#include <wtf/OwnPtr.h>

#define DOUBLE_FROM_26_6(t) ((double)(t) / 64.0)

namespace WebCore {

class FontDescription;
class FontPlatformData;
class SharedBuffer;
class SVGFontData;

enum Pitch { UnknownPitch, FixedPitch, VariablePitch };
enum GlyphMetricsMode { GlyphBoundingBox, GlyphWidthOnly };

class SimpleFontData : public FontData {
public:
    SimpleFontData(const FontPlatformData&, bool customFont = false, bool loading = false, SVGFontData* data = 0);
    virtual ~SimpleFontData();

public:
    const FontPlatformData& platformData() const { return m_platformData; }
    SimpleFontData* smallCapsFontData(const FontDescription& fontDescription) const;

    // vertical metrics
    int ascent() const { return m_ascent; }
    int descent() const { return m_descent; }
    int lineSpacing() const { return m_lineSpacing; }
    int lineGap() const { return m_lineGap; }
    float maxCharWidth() const { return m_maxCharWidth; }
    float avgCharWidth() const { return m_avgCharWidth; }
    float xHeight() const { return m_xHeight; }
    unsigned unitsPerEm() const { return m_unitsPerEm; }

    float widthForGlyph(Glyph glyph) const { return metricsForGlyph(glyph, GlyphWidthOnly).horizontalAdvance; }
    GlyphMetrics metricsForGlyph(Glyph, GlyphMetricsMode = GlyphBoundingBox) const;
    GlyphMetrics platformMetricsForGlyph(Glyph, GlyphMetricsMode) const;

    float spaceWidth() const { return m_spaceWidth; }
    float adjustedSpaceWidth() const { return m_adjustedSpaceWidth; }

    Glyph spaceGlyph() const { return m_spaceGlyph; }

    virtual const SimpleFontData* fontDataForCharacter(UChar32) const;
    virtual bool containsCharacters(const UChar*, int length) const;

    void determinePitch();
    Pitch pitch() const { return m_treatAsFixedPitch ? FixedPitch : VariablePitch; }

#if ENABLE(SVG_FONTS)
    SVGFontData* svgFontData() const { return m_svgFontData.get(); }
    bool isSVGFont() const { return m_svgFontData; }
#else
    bool isSVGFont() const { return false; }
#endif

    virtual bool isCustomFont() const { return m_isCustomFont; }
    virtual bool isLoading() const { return m_isLoading; }
    virtual bool isSegmented() const;

    const GlyphData& missingGlyphData() const { return m_missingGlyphData; }

    void setFont(BalFont*) const;

private:
    void platformInit();
    void platformGlyphInit();
    void platformCharWidthInit();
    void platformDestroy();
    
    void initCharWidths();

    void commonInit();

public:
    int m_ascent;
    int m_descent;
    int m_lineSpacing;
    int m_lineGap;
    float m_maxCharWidth;
    float m_avgCharWidth;
    float m_xHeight;
    unsigned m_unitsPerEm;

    FontPlatformData m_platformData;

    mutable GlyphMetricsMap m_glyphToMetricsMap;

    bool m_treatAsFixedPitch;

#if ENABLE(SVG_FONTS)
    OwnPtr<SVGFontData> m_svgFontData;
#endif

    bool m_isCustomFont;  // Whether or not we are custom font loaded via @font-face
    bool m_isLoading; // Whether or not this custom font is still in the act of loading.

    Glyph m_spaceGlyph;
    float m_spaceWidth;
    float m_adjustedSpaceWidth;

    Glyph m_zeroWidthSpaceGlyph;

    GlyphData m_missingGlyphData;

    mutable SimpleFontData* m_smallCapsFontData;
};

ALWAYS_INLINE GlyphMetrics SimpleFontData::metricsForGlyph(Glyph glyph, GlyphMetricsMode metricsMode) const
{
    GlyphMetrics metrics = m_glyphToMetricsMap.metricsForGlyph(glyph);
    if ((metricsMode == GlyphWidthOnly && metrics.horizontalAdvance != cGlyphSizeUnknown) || (metricsMode == GlyphBoundingBox && metrics.boundingBox.width() != cGlyphSizeUnknown))
        return metrics;

    metrics = platformMetricsForGlyph(glyph, metricsMode);
    m_glyphToMetricsMap.setMetricsForGlyph(glyph, metrics);

    return metrics;
}

} // namespace WebCore

#endif // SimpleFontData_h
