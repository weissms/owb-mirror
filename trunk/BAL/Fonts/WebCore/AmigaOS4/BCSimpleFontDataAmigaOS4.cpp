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
#include "SimpleFontData.h"

#include "FloatRect.h"
#include "Font.h"
#include "FontCache.h"
#include "FontDescription.h"
#include "GlyphBuffer.h"
#include "BALBase.h"
#include <unicode/uchar.h>
#include <unicode/unorm.h>
#include <wtf/MathExtras.h>
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>
#include <diskfont/oterrors.h>

namespace WKAL {

void SimpleFontData::platformInit()
{
    struct OutlineFont *face = m_font.m_face;
    if (face) {
        uint32 baseline;
        if (!IDiskfont->EObtainInfo(&face->olf_EEngine,
                                    OT_BaseLine, &baseline,
                                    TAG_END)
         && (baseline & 0xffff0000) != 0) {
            m_ascent = baseline >> 16;
            m_descent = baseline & 0xffff;
        }
        else {
            // For fonts not returning the baseline we have to find it out
            // the hard way outself.

            int max_ascent = 0, max_descent = 0;

            for (int i = 33 ; i < 256 ; i++) {
                if (128 == i)
                    i = 161;

                if (!IDiskfont->ESetInfo(&face->olf_EEngine,
                                         OT_GlyphCode, i,
                                         TAG_END)) {
                    struct GlyphMap *glyph;

                    if (!IDiskfont->EObtainInfo(&face->olf_EEngine,
                                                OT_GlyphMap8Bit, &glyph,
                                                TAG_END)) {
                        int ascent = glyph->glm_Y0 - glyph->glm_BlackTop;
                        if (ascent > max_ascent)
                            max_ascent = ascent;

                        int descent = glyph->glm_BlackTop + glyph->glm_BlackHeight - glyph->glm_Y0;
                        if (descent > max_descent)
                            max_descent = descent;

                        IDiskfont->EReleaseInfo(&face->olf_EEngine, OT_GlyphMap8Bit, glyph, TAG_END);
                    }
                }
            }

            m_ascent = max_ascent;
            m_descent = max_descent;
        }

        m_lineSpacing = m_ascent + m_descent;

        m_xHeight = (int)(m_font.m_size * amigaConfig.fontYDPI / 72);
        if (!IDiskfont->ESetInfo(&face->olf_EEngine,
                                 OT_GlyphCode, 'x',
                                 TAG_END)) {
           struct GlyphMap *glyph;

           if (!IDiskfont->EObtainInfo(&face->olf_EEngine,
                                       OT_GlyphMap8Bit, &glyph,
                                       TAG_END)) {
              m_xHeight = glyph->glm_BlackHeight;

              IDiskfont->EReleaseInfo(&face->olf_EEngine, OT_GlyphMap8Bit, glyph, TAG_END);
           }
        }

        FIXED spacewidth = 0x00010000;
        if (!IDiskfont->ESetInfo(&face->olf_EEngine,
                                 OT_GlyphCode, ' ',
                                 OT_GlyphCode2, ' ',
                                 TAG_END)) {
            struct MinList *widthlist = NULL;

            if (!IDiskfont->EObtainInfo(&face->olf_EEngine,
                                        OT_WidthList, (uint32)&widthlist,
                                        TAG_END)) {
               struct GlyphWidthEntry *widthentry = (struct GlyphWidthEntry *)widthlist->mlh_Head;

               if (' ' == widthentry->gwe_Code)
                   spacewidth = widthentry->gwe_Width;

               IDiskfont->EReleaseInfo(&face->olf_EEngine, OT_WidthList, widthlist, TAG_END);
            }
        }

        m_spaceWidth = (int)(spacewidth / 65536.0 * m_font.m_size * amigaConfig.fontXDPI / 72);
        m_lineGap = m_lineSpacing - m_ascent + m_descent;
    }
}

void SimpleFontData::platformDestroy()
{
    fprintf(stderr, "%s: m_face = %p\n", __PRETTY_FUNCTION__, m_font.m_face); // never called!
    if (m_smallCapsFontData)
        delete m_smallCapsFontData;
    m_smallCapsFontData = NULL;
}

SimpleFontData* SimpleFontData::smallCapsFontData(const FontDescription& fontDescription) const
{
    if (!m_smallCapsFontData) {
        FontDescription desc = FontDescription(fontDescription);
        desc.setSpecifiedSize(0.70f*fontDescription.computedSize());
        const FontPlatformData* pdata = new FontPlatformData(desc, desc.family().family());
        m_smallCapsFontData = new SimpleFontData(*pdata);
    }
    return m_smallCapsFontData;
}

bool SimpleFontData::containsCharacters(const UChar* characters, int length) const
{
    fprintf(stderr, "SimpleFontData::containsCharacters()\n"); // never called?
    return true;
}

void SimpleFontData::determinePitch()
{
    m_treatAsFixedPitch = m_font.isFixedPitch();
}

float SimpleFontData::platformWidthForGlyph(Glyph glyph) const
{
    if (32 == glyph)
        return m_spaceWidth;

    struct OutlineFont *face = m_font.m_face;

    if (!IDiskfont->ESetInfo(&face->olf_EEngine,
                             OT_GlyphCode, glyph,
                             OT_GlyphCode2, glyph,
                             TAG_END)) {
        struct MinList *widthlist;
        double width = 0;

        if (!IDiskfont->EObtainInfo(&face->olf_EEngine,
                                    OT_WidthList, (uint32)&widthlist,
                                    TAG_END)) {
            struct GlyphWidthEntry *widthentry = (struct GlyphWidthEntry *)widthlist->mlh_Head;

            if (widthentry->gwe_Code == glyph)
                width = widthentry->gwe_Width / 65536.0 * m_font.m_size;

            IDiskfont->EReleaseInfo(&face->olf_EEngine, OT_WidthList, widthlist, TAG_END);
        }

        if (0 == width
         && amigaConfig.unicodeFace
         && !IDiskfont->ESetInfo(&amigaConfig.unicodeFace->olf_EEngine,
                                 OT_PointHeight, ((int)(m_font.m_size)) << 16,
                                 OT_GlyphCode, glyph,
                                 OT_GlyphCode2, glyph,
                                 TAG_END)
         && !IDiskfont->EObtainInfo(&amigaConfig.unicodeFace->olf_EEngine,
                                    OT_WidthList, (uint32)&widthlist,
                                    TAG_END)) {
            struct GlyphWidthEntry *widthentry = (struct GlyphWidthEntry *)widthlist->mlh_Head;

            if (widthentry->gwe_Code == glyph)
                width = widthentry->gwe_Width / 65536.0 * m_font.m_size;

            IDiskfont->EReleaseInfo(&amigaConfig.unicodeFace->olf_EEngine, OT_WidthList, widthlist, TAG_END);
        }

        if (0 == width
         && !IDiskfont->ESetInfo(&face->olf_EEngine,
                                 OT_GlyphCode, 0xFFFD,
                                 OT_GlyphCode2, 0xFFFD,
                                 TAG_END)
         && !IDiskfont->EObtainInfo(&face->olf_EEngine,
                                    OT_WidthList, (uint32)&widthlist,
                                    TAG_END)) {
             struct GlyphWidthEntry *widthentry = (struct GlyphWidthEntry *)widthlist->mlh_Head;

             if (0xFFFD == widthentry->gwe_Code)
                 width = widthentry->gwe_Width / 65536.0 * m_font.m_size;

             IDiskfont->EReleaseInfo(&face->olf_EEngine, OT_WidthList, widthlist, TAG_END);
        }

        if (0 == width
         && !IDiskfont->ESetInfo(&face->olf_EEngine,
                                 OT_GlyphCode, '?',
                                 OT_GlyphCode2, '?',
                                 TAG_END)
         && !IDiskfont->EObtainInfo(&face->olf_EEngine,
                                    OT_WidthList, (uint32)&widthlist,
                                    TAG_END)) {
             struct GlyphWidthEntry *widthentry = (struct GlyphWidthEntry *)widthlist->mlh_Head;

             if ('?' == widthentry->gwe_Code)
                 width = widthentry->gwe_Width / 65536.0 * m_font.m_size;

             IDiskfont->EReleaseInfo(&face->olf_EEngine, OT_WidthList, widthlist, TAG_END);
        }

        if (width > 0.1)
           return (int)(width * amigaConfig.fontXDPI / 72);
    }
    return m_spaceWidth;
}

void SimpleFontData::setFont(BalFont* cr) const
{
    ASSERT(cr);
    m_font.setFont(cr);
}

}
