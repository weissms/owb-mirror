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

#include "config.h"
#include "BALConfiguration.h"
#include "BCFontPlatformDataPrivateFreeType.h"
#include "BTFontPlatformData.h"
#include "BTLogHelper.h"
#include "CString.h"

IMPLEMENT_CREATE_DELETE(BIFontPlatformDataPrivate, BCFontPlatformDataPrivateFreeType);

using WebCore::CString;
using WebCore::StringImpl;

namespace BAL {

int BCFontPlatformDataPrivateFreeType::m_isInitialized = 0;
FT_Library BCFontPlatformDataPrivateFreeType::m_library = 0;

BCFontPlatformDataPrivateFreeType::BCFontPlatformDataPrivateFreeType()
    : m_pattern(0)
{
    m_face = 0;
}

BCFontPlatformDataPrivateFreeType::~BCFontPlatformDataPrivateFreeType()
{
    if (m_pattern && ((FcPattern*)-1 != m_pattern)) {
        FcPatternDestroy(m_pattern);
        m_pattern = NULL;
    }

    if (m_face) {
        FT_Done_Face(m_face);
        m_face = 0;
    }

    m_isInitialized--;
    DBGML(MODULE_FONTS, LEVEL_INFO, "BCFontPlatformDataPrivateFreeType::~BCFontPlatformDataPrivateFreeType(): m_isInitialized=%d\n", m_isInitialized);
    if (!m_isInitialized && m_library) {
        FT_Done_FreeType(m_library);
        m_library = 0;
    }
}

int BCFontPlatformDataPrivateFreeType::init()
{
    if (m_isInitialized) {
        m_isInitialized++;
        DBGML(MODULE_FONTS, LEVEL_INFO, "BCFontPlatformDataPrivateFreeType::init(): m_isInitialized=%d\n", m_isInitialized);
        return m_isInitialized;
    }

    FT_Error error = FT_Init_FreeType(&m_library);
    if (error) {
        DBGML(MODULE_FONTS, LEVEL_EMERGENCY, "Couldn't init FreeType engine\n");
        m_isInitialized = 0;
        m_library = 0;
        return m_isInitialized;
    }

    if (!FcInit()) {
        DBGML(MODULE_FONTS, LEVEL_EMERGENCY, "Can't init font config library\n");
        m_isInitialized = 0;
        return m_isInitialized;
    }
    m_isInitialized++;
    DBGML(MODULE_FONTS, LEVEL_INFO, "BCFontPlatformDataPrivateFreeType::init(): m_isInitialized=%d\n", m_isInitialized);
    return m_isInitialized;
}

void BCFontPlatformDataPrivateFreeType::configureFontconfig(const FontDescription& fontDescription, const AtomicString& familyName)
{
    CString familyNameString = familyName.domString().utf8();
    const char* fcfamily = familyNameString.data();
    int fcslant = FC_SLANT_ROMAN;
    int fcweight = FC_WEIGHT_NORMAL;
    float fcsize = fontDescription.computedSize();
    if (fontDescription.italic())
        fcslant =FC_SLANT_ITALIC;
    if (fontDescription.bold())
        fcweight = FC_WEIGHT_BOLD;
    FcConfig *config = FcConfigGetCurrent();
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
    if (!m_pattern)
        goto freePattern;

    FcConfigDestroy(config);

freePattern:
    FcPatternDestroy(pattern);
}

void BCFontPlatformDataPrivateFreeType::configureFreetype(const FontDescription& fontDescription)
{
    FcChar8 *fc_filename;
    char *filename;
    int id = 0;

    if (FcPatternGetString(m_pattern, FC_FILE, 0, &fc_filename) != FcResultMatch) {
        DBGML(MODULE_FONTS, LEVEL_EMERGENCY, "cannot retrieve font\n");
        return;
    }
    filename = (char *) fc_filename; //use C cast as FcChar is a fontconfig type

    if (FcPatternGetInteger(m_pattern, FC_INDEX, 0, &id) != FcResultMatch) {
        DBGML(MODULE_FONTS, LEVEL_EMERGENCY, "cannot retrieve font index\n");
        return;
    }

    if (FT_Error error = FT_New_Face(m_library, filename, id, &m_face)) {
        DBGML(MODULE_FONTS, LEVEL_EMERGENCY, "fail to open font %s with index %d (error = 0x%x)\n", filename, id, error);
        m_face = 0;
        return;
    }
    FT_Set_Pixel_Sizes(m_face, 0, static_cast<uint> (fontDescription.specifiedSize()));
    DBGML(MODULE_FONTS, LEVEL_INFO, "open font %s with size %d\n", filename, static_cast<uint> (fontDescription.specifiedSize()));
}

bool BCFontPlatformDataPrivateFreeType::isFixedPitch()
{
    int spacing;
    if (FcPatternGetInteger(m_pattern, FC_SPACING, 0, &spacing) == FcResultMatch)
        return spacing == FC_MONO;
    return false;
}

unsigned BCFontPlatformDataPrivateFreeType::hash() const
{
    uintptr_t hashCodes[1] = { reinterpret_cast<uintptr_t>(m_face) };
    return StringImpl::computeHash( reinterpret_cast<UChar*>(hashCodes), sizeof(hashCodes) / sizeof(UChar));
}

bool BCFontPlatformDataPrivateFreeType::isEqual(const BIFontPlatformDataPrivate* other) const
{
    const BCFontPlatformDataPrivateFreeType *compare = static_cast<const BCFontPlatformDataPrivateFreeType*> (other);
    if (!compare)
        return false;
    if (m_pattern == compare->m_pattern)
        return true;
    if (m_pattern == 0 || m_pattern == reinterpret_cast<FcPattern*>(-1)
        || compare->m_pattern == 0 || compare->m_pattern == reinterpret_cast<FcPattern*>(-1))
        return false;
    return FcPatternEqual(m_pattern, compare->m_pattern);
}

} //namespace BAL
