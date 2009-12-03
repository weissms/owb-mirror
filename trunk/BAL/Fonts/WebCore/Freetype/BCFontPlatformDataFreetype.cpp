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
#include "FontPlatformData.h"

#include "CString.h"
#include "Logging.h"
#include "FontDescription.h"
#include "NotImplemented.h"
#include "PlatformString.h"
#include "UnusedParam.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig/fontconfig.h>

namespace WebCore {

FT_Library FontPlatformData::m_library = 0;

FontPlatformData::FontPlatformData(const FontDescription& fontDescription, const AtomicString& familyName)
    : m_pattern(0)
    , m_syntheticBold(false)
    , m_syntheticOblique(false)
    , m_fallbacks(0)
    , m_size(fontDescription.computedSize())
    , m_scaledFont(0)
    , m_face(0)
{
    FontPlatformData::init();

    CString familyNameString = familyName.string().utf8();
    const char* fcfamily = familyNameString.data();

    int fcslant = FC_SLANT_ROMAN;
    // FIXME: Map all FontWeight values to fontconfig weights.
    int fcweight = FC_WEIGHT_NORMAL;
    float fcsize = fontDescription.computedSize();
    if (fontDescription.italic())
        fcslant = FC_SLANT_ITALIC;
    if (fontDescription.weight() >= FontWeight600)
        fcweight = FC_WEIGHT_BOLD;

    FcConfig *config = FcConfigGetCurrent();

    //printf("family = %s\n", fcfamily);

    int type = fontDescription.genericFamily();

    FcPattern* pattern = FcPatternCreate();
    
    if (!FcPatternAddString(pattern, FC_FAMILY, reinterpret_cast<const FcChar8*>(fcfamily)))
        goto freePattern;

    switch (type) {
        case FontDescription::SerifFamily:
            fcfamily = "serif";
            break;
        case FontDescription::SansSerifFamily:
            fcfamily = "sans-serif";
            break;
        case FontDescription::MonospaceFamily:
            fcfamily = "monospace";
            break;
        case FontDescription::NoFamily:
        case FontDescription::StandardFamily:
        default:
            fcfamily = "sans-serif";
    }

    if (!FcPatternAddString(pattern, FC_FAMILY, reinterpret_cast<const FcChar8*>(fcfamily)))
        goto freePattern;
    if (!FcPatternAddInteger(pattern, FC_SLANT, fcslant))
        goto freePattern;
    if (!FcPatternAddInteger(pattern, FC_WEIGHT, fcweight))
        goto freePattern;
    if (!FcPatternAddDouble(pattern, FC_PIXEL_SIZE, fcsize))
        goto freePattern;

    FcConfigSubstitute(config, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    FcResult fcresult;
    m_pattern = FcFontMatch(config, pattern, &fcresult);
    FcPatternReference(m_pattern);
    // FIXME: should we set some default font?
    if (!m_pattern)
        goto freePattern;

    FcChar8 *fc_filename;
    char *filename;
    int id;
    id = 0;

    if (FcPatternGetString(m_pattern, FC_FILE, 0, &fc_filename) != FcResultMatch) {
        LOG(FontEngine, "cannot retrieve font\n");
        goto freePattern;
    }

    filename = (char *) fc_filename; //use C cast as FcChar is a fontconfig type
    //printf("filename = %s\n", filename);

    if (FcPatternGetInteger(m_pattern, FC_INDEX, 0, &id) != FcResultMatch) {
        LOG(FontEngine, "cannot retrieve font index\n");
        goto freePattern;
    }
    
    if (FT_Error error = FT_New_Face(m_library, filename, id, &m_face)) {
#ifdef NDEBUG
        UNUSED_PARAM(error);
#endif
        LOG(FontEngine, "fail to open fonti %s with index %d (error = 0x%x)\n", filename, id, error);
        m_face = 0;
        goto freePattern;
    }

    FT_Set_Pixel_Sizes(m_face, 0, static_cast<unsigned int> (fontDescription.computedSize()));
    //DBGML(MODULE_FONTS, LEVEL_INFO, "open font %s with size %d\n", filename, static_cast<uint> (fontDescription.specifiedSize()));

freePattern:
    FcPatternDestroy(pattern);
    FcConfigDestroy(config);
}

FontPlatformData::FontPlatformData(float size, bool bold, bool italic)
    : m_pattern(0)
    , m_syntheticBold(bold)
    , m_syntheticOblique(italic)
    , m_fallbacks(0)
    , m_size(size)
    , m_scaledFont(0)
{
}

FontPlatformData::FontPlatformData(BalFontFace* fontFace, int size, bool bold, bool italic)
    : m_pattern(0)
    , m_syntheticBold(bold)
    , m_syntheticOblique(italic)
    , m_fallbacks(0)
    , m_size(size)
    , m_scaledFont(0)
{
    m_face = fontFace;
}

bool FontPlatformData::init()
{
    static bool initialized = false;
    if (initialized)
        return true;

    FT_Error error = FT_Init_FreeType(&m_library);
    if (error) {
        LOG(FontEngine, "Couldn't init FreeType engine\n");
        initialized = false;
        m_library = 0;
        return initialized;
    }

    if (!FcInit()) {
        fprintf(stderr, "Can't init font config library\n");
        return false;
    }
    initialized = true;
    return true;
}

FontPlatformData::~FontPlatformData()
{
}

bool FontPlatformData::isFixedPitch()
{
    // TODO: Support isFixedPitch() for custom fonts.
    if (!m_pattern)
        return false;

    int spacing;
    if (FcPatternGetInteger(m_pattern, FC_SPACING, 0, &spacing) == FcResultMatch)
        return spacing == FC_MONO;
    return false;
}

void FontPlatformData::setFont(BalFont* cr) const
{
    //ASSERT(m_scaledFont);

    balNotImplemented();
}

bool FontPlatformData::operator==(const FontPlatformData& other) const
{
    if (m_pattern == other.m_pattern)
        return true;
    if (m_pattern == 0 || m_pattern == reinterpret_cast<FcPattern*>(-1)
            || other.m_pattern == 0 || other.m_pattern == reinterpret_cast<FcPattern*>(-1))
        return false;
    return FcPatternEqual(m_pattern, other.m_pattern);
}

#ifndef NDEBUG
String FontPlatformData::description() const
{
    return String();
}
#endif

}
