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
#include "FontPlatformData.h"

#include "CString.h"
#include "FontDescription.h"
#include "NotImplemented.h"
#include "PlatformString.h"
#include <proto/exec.h>
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>
#include <diskfont/oterrors.h>
#include <proto/utility.h>

namespace WebCore {
extern "C" {
// OWB never calls anything which could free the fonts, have to track
// and release them on exit.
static List font_list, tracking_list;
static void add_to_fontlist(OutlineFont* face) 
{
    Node* node = (Node*)malloc(sizeof(struct Node));
    if (node) {
       node->ln_Name = (STRPTR)face;
       IExec->AddHead(&tracking_list, node);
    }
}

struct amigaFontFamily
{
    amigaFontFamily* next;
    char *familyName, *regularName, *boldName, *italicName, *boldItalicName;
};
static amigaFontFamily* fontFamilies = 0;

static void addFont(char* name, OutlineFont* olf)
{
    TagItem* tag;

    if (strstr(name, "Condensed"))
        return;

    tag = IUtility->FindTagItem(OT_Engine, olf->olf_OTagList);
    if (!tag || strcasecmp((char*)tag->ti_Data, "ft2"))
        return;
    tag = IUtility->FindTagItem(OT_Family, olf->olf_OTagList);
    if (!tag)
        return;

    amigaFontFamily* family = fontFamilies;
    while(family) {
        if (!strcasecmp((char*)tag->ti_Data, family->familyName))
            break;
        family = family->next;
    }

    if (!family) {
        family = (amigaFontFamily*)calloc(1, sizeof(struct amigaFontFamily));
        if (!family)
            return;
        family->familyName = strdup((char*)tag->ti_Data);
        family->next = fontFamilies;
        fontFamilies = family;
    }

    if (family->regularName && family->boldName && family->italicName && family->boldItalicName)
        return;

    tag = IUtility->FindTagItem(OT_StemWeight, olf->olf_OTagList);
    if (!tag)
        return;
    if (tag->ti_Data < OTS_Light)
       return;
    bool bold = tag->ti_Data >= OTS_SemiBold || strstr(name, "Demi") || strstr(name, "Medium");

    tag = IUtility->FindTagItem(OT_SlantStyle, olf->olf_OTagList);
    if (!tag)
        return;
    bool italic = tag->ti_Data == OTS_Italic || strstr(name, "Italic");

    if (!bold && !italic) {
        if (family->regularName && strstr(family->regularName, "Medium")) {
            free(family->regularName);
            family->regularName = strdup(name);
        }
        else if (!family->regularName)
            family->regularName = strdup(name);
    }
    else if (bold && !italic) {
        if (!family->boldName)
            family->boldName = strdup(name);
    }
    else if (!bold && italic) {
        if (family->italicName && strstr(family->italicName, "Medium")) {
            free(family->italicName);
            family->italicName = strdup(name);
        }
        else if (!family->italicName)
            family->italicName = strdup(name);
    }
    else // (bold && italic)
        if (!family->boldItalicName)
           family->boldItalicName = strdup(name);
}

static void amiga_font_init(void) __attribute__((constructor));
static void amiga_font_init(void)
{
    IExec->NewList(&tracking_list);
    IExec->NewList(&font_list);

    int buffersize = 64 * 1024;
    char* buffer = (char*)malloc(buffersize);
    if (!buffer)
        return;
    int32 error;
    do {
        error = IDiskfont->AvailFonts(buffer, buffersize, AFF_DISK | AFF_TAGGED | AFF_OTAG | AFF_CHARSET | AFF_TYPE);
        if (error) {
            buffersize += error;
            buffer = (char*)realloc(buffer, buffersize);
            if (!buffer)
                return;
        }
    } while (error);

    AvailFontsHeader* afh = (AvailFontsHeader*)buffer;
    TAvailFonts* taf = (TAvailFonts*)(buffer + sizeof(struct AvailFontsHeader));

    for (int i = 0 ; i < afh->afh_NumEntries ; i++) {
        if ((AFF_DISK|AFF_OTAG|AFF_SCALED) == taf[i].taf_Type) {
            TagItem* tags = taf[i].taf_Attr.tta_Tags;
            while (tags && tags->ti_Tag != TAG_DONE) {
                if (TAG_MORE == tags->ti_Tag)
                    break;
                tags++;
            }
            OutlineFont* olf = IDiskfont->OpenOutlineFont(taf[i].taf_Attr.tta_Name, &font_list, OFF_OPEN);
            if (olf) {
                tags = olf->olf_OTagList;
                addFont(taf[i].taf_Attr.tta_Name, olf);
                IDiskfont->CloseOutlineFont(olf, &font_list);
            }
        }
    }

    free(buffer);
}

static void amiga_font_cleanup(void) __attribute__((destructor));
static void amiga_font_cleanup(void)
{
    struct Node* node = IExec->GetHead(&tracking_list);
    while (node) {
        if (node->ln_Name) {
            IDiskfont->CloseOutlineFont((OutlineFont*)node->ln_Name, &font_list);
            node->ln_Name = 0;
        }
        IExec->Remove(node);
        node = IExec->GetHead(&tracking_list);
    }
}
};

static char* get_font_name(const char* family, const int type, const bool bold, const bool italic, List* list)
{
    char* fontname = 0;

    if (0 != family && 0 != *family) {
        if (!strcasecmp(family, "-webkit-serif"))
            family = "DejaVu Serif";
        else if (!strcasecmp(family, "-webkit-sans-serif"))
            family = "DejaVu Sans";
        else if (!strcasecmp(family, "-webkit-monospace"))
            family = "DejaVu Sans Mono";
        // else -webkit-cursive
        // else -webkit-fantasy
        // else -webkit-standard
        else if (!strncasecmp(family, "-webkit-", strlen("-webkit-")))
           family = "DejaVu Serif";

        amigaFontFamily* fontFamily = fontFamilies;
        while (fontFamily) {
            if (!strcasecmp(fontFamily->familyName, family))
                break;
            fontFamily = fontFamily->next;
        }
        if (fontFamily) {
            if (!bold && !italic) {
                if (fontFamily->regularName)
                    fontname = strdup(fontFamily->regularName);
                else
                    fprintf(stderr, "font family '%s' has no regular font\n", family);
            }
            else if (bold && !italic) {
                if (fontFamily->boldName)
                    fontname = strdup(fontFamily->boldName);
                else
                    fprintf(stderr, "font family '%s' has no bold font\n", family);
            }
            else if (!bold && italic) {
                if (fontFamily->italicName)
                    fontname = strdup(fontFamily->italicName);
                else
                    fprintf(stderr, "font family '%s' has no italic font\n", family);
            }
            else { // (bold && italic)
                if (fontFamily->boldItalicName)
                    fontname = strdup(fontFamily->boldItalicName);
                else
                    fprintf(stderr, "font family '%s' has no bold italic font\n", family);
            }
        } else
            fprintf(stderr, "font family '%s' not found\n", family);
    }

    return fontname;
}

static char* get_font_name_fallback(const int type, const bool bold, const bool italic, List* list)
{
    char* fontname = 0;
    const char* boldstr = bold ? " Bold" : "";
    const char* italicstr = italic ? " Italic" : "";
    char tempfontname[256];

    switch (type) {
    case FontDescription::SerifFamily:
        snprintf(tempfontname, sizeof(tempfontname), "Times New Roman%s%s", boldstr, italicstr);
        fontname = strdup(tempfontname);
    break;

    case FontDescription::SansSerifFamily:
        snprintf(tempfontname, sizeof(tempfontname), "Arial%s%s", boldstr, italicstr);
        fontname = strdup(tempfontname);
    break;

    case FontDescription::MonospaceFamily:
        snprintf(tempfontname, sizeof(tempfontname), "Courier New%s%s", boldstr, italicstr);
        fontname = strdup(tempfontname);
    break;

    case FontDescription::NoFamily:
    case FontDescription::StandardFamily:
    default:
        snprintf(tempfontname, sizeof(tempfontname), "Arial%s%s", boldstr, italicstr);
        fontname = strdup(tempfontname);
    }

    OutlineFont* face = IDiskfont->OpenOutlineFont(fontname, list, OFF_OPEN);
    if (face) {
        IDiskfont->CloseOutlineFont(face, list);
        return fontname;
    }

    free(fontname);


    italicstr = italic ? " Oblique" : "";

    switch (type) {
    case FontDescription::SerifFamily:
        snprintf(tempfontname, sizeof(tempfontname), "DejaVu Serif%s%s", boldstr, italicstr);
        fontname = strdup(tempfontname);
    break;

    case FontDescription::SansSerifFamily:
        snprintf(tempfontname, sizeof(tempfontname), "DejaVu Sans%s%s", boldstr, italicstr);
        fontname = strdup(tempfontname);
    break;

    case FontDescription::MonospaceFamily:
        snprintf(tempfontname, sizeof(tempfontname), "DejaVu Sans Mono%s%s", boldstr, italicstr);
        fontname = strdup(tempfontname);
    break;

    case FontDescription::NoFamily:
    case FontDescription::StandardFamily:
    default:
        snprintf(tempfontname, sizeof(tempfontname), "DejaVu Sans%s%s", boldstr, italicstr);
        fontname = strdup(tempfontname);
    }

    return fontname;
}

static OutlineFont* openFont(const char* family, int type, bool bold, bool italic, float &size, char* &m_fontname, double &m_sizefactor)
{
    OutlineFont* fontFace = 0;

    char* fontname = 0;
    if (family && *family)
        fontname = get_font_name(family, type, bold, italic, &font_list);
    if (fontname) {
        fontFace = IDiskfont->OpenOutlineFont(fontname, &font_list, OFF_OPEN);
        if (!fontFace) {
            free(fontname);
            fontname = 0;
        }
    }
    if (!fontFace && (!family || !*family)) {
        fontname = get_font_name_fallback(type, bold, italic, &font_list);
        if (fontname)
            fontFace = IDiskfont->OpenOutlineFont(fontname, &font_list, OFF_OPEN);
    }
    if (fontFace) {
        uint32 ysizefactor = IUtility->GetTagData(OT_YSizeFactor, 0x00010001, fontFace->olf_OTagList);
//        m_sizefactor = (double)(ysizefactor & 0xFFFF) / (ysizefactor >> 16);
        size = size * (ysizefactor >> 16) / (ysizefactor & 0xFFFF) + 0.5;

        if (size < amigaConfig.minFontSize)
            size = amigaConfig.minFontSize;

        if (amigaConfig.fontHinter >= 0 && amigaConfig.fontHinter <= 4)
            IDiskfont->ESetInfo(&fontFace->olf_EEngine,
                                OT_Spec + 6, amigaConfig.fontHinter,
                                TAG_END);

        if (IDiskfont->ESetInfo(&fontFace->olf_EEngine,
                                OT_DeviceDPI, (amigaConfig.fontXDPI << 16) | amigaConfig.fontYDPI,
                                OT_PointHeight, ((int)size) << 16,
                                TAG_END) != OTERR_Success) {
            IDiskfont->CloseOutlineFont(fontFace, &font_list);
            fontFace = 0;
        }
        else {
            add_to_fontlist(fontFace);
            m_fontname = fontname;
            fontname = 0;
        }
    }
    free(fontname);

    return fontFace;
}

FontPlatformData::FontPlatformData(WTF::HashTableDeletedValueType)
    : m_size(-1)
    , m_sizefactor(-1)
    , m_syntheticBold(false)
    , m_syntheticOblique(false)
    , m_face((OutlineFont*)-1)
    , m_fontname(hashTableDeletedFontValue())
{
}

FontPlatformData::FontPlatformData()
    : m_size(0)
    , m_sizefactor(0)
    , m_syntheticBold(false)
    , m_syntheticOblique(false)
    , m_face(0)
    , m_fontname(0)
{
}

FontPlatformData::FontPlatformData(const FontDescription& fontDescription, const AtomicString& familyName)
    : m_size(fontDescription.computedSize())
    , m_syntheticBold(false)
    , m_syntheticOblique(false)
    , m_face(0)
    , m_fontname(0)
{
    FontPlatformData::init();

    CString familyNameString = familyName.string().utf8();
    const char* family = familyNameString.data();

    if (family && strchr(family, ':'))
        family = 0;

    if (m_size < amigaConfig.minFontSize)
        m_size = amigaConfig.minFontSize;

    if (m_size > 256)
        m_size = 256;

    int type = fontDescription.genericFamily();
    m_syntheticBold = fontDescription.weight() >= FontWeightBold;
    m_syntheticOblique = fontDescription.italic();
    
    m_face = openFont(family, type, m_syntheticBold, m_syntheticOblique, m_size, m_fontname, m_sizefactor);
    if (!m_face)
        m_size = 0;
}

FontPlatformData::FontPlatformData(float size, bool bold, bool italic)
    : m_size(size)
    , m_syntheticBold(bold)
    , m_syntheticOblique(italic)
    , m_fontname(0)
{
    if (0 == size) {
        m_sizefactor = 0;
        m_face = 0;
    }
    else
        m_face = openFont(0, FontDescription::NoFamily, bold, italic, m_size, m_fontname, m_sizefactor);
}

FontPlatformData::FontPlatformData(BalFontFace* fontFace, int size, bool bold, bool italic)
    : m_size(size)
    , m_syntheticBold(bold)
    , m_syntheticOblique(italic)
    , m_fontname(0)
{
    m_face = openFont(0, FontDescription::NoFamily, bold, italic, m_size, m_fontname, m_sizefactor);
}

FontPlatformData::FontPlatformData(const FontPlatformData& other)
    : m_size(0)
    , m_sizefactor(0)
    , m_syntheticBold(false)
    , m_syntheticOblique(false)
    , m_face(0)
    , m_fontname(0)
{
    if (0 != &other)
        *this = other;
}

bool FontPlatformData::init()
{
    static bool initialized = false;
    if (!initialized) {
        amigaConfig.unicodeFace = IDiskfont->OpenOutlineFont(amigaConfig.unicodeFontName, &font_list, OFF_OPEN);
        if (amigaConfig.unicodeFace)
            if (IDiskfont->ESetInfo(&amigaConfig.unicodeFace->olf_EEngine,
                                    OT_DeviceDPI, (amigaConfig.fontXDPI << 16) | amigaConfig.fontYDPI,
                                    TAG_END) != OTERR_Success) {
                IDiskfont->CloseOutlineFont(amigaConfig.unicodeFace, &font_list);
                amigaConfig.unicodeFace = 0;
            }
            else
                add_to_fontlist(amigaConfig.unicodeFace);

        initialized = true;
    }
    return true;
}

FontPlatformData::~FontPlatformData()
{
    free(m_fontname);
}

bool FontPlatformData::isFixedPitch()
{
    if (!m_face)
       return false;

    if (m_face->olf_OTagList)
        return FALSE != IUtility->GetTagData(OT_IsFixed, FALSE, m_face->olf_OTagList);

    return false;
}

void FontPlatformData::setFont(BalFont* cr) const
{
    //ASSERT(m_scaledFont);

    fprintf(stderr, "%s(%p)\n", __PRETTY_FUNCTION__, cr);
    balNotImplemented();
}

FontPlatformData& FontPlatformData::operator=(const FontPlatformData& other)
{
    if (0 == &other)
        return *(new FontPlatformData());

    // Check for self-assignment.
    if (this == &other)
        return *this;

    m_size = other.m_size;
    m_syntheticBold = other.m_syntheticBold;
    m_syntheticOblique = other.m_syntheticOblique;
    m_face = other.m_face;

    if (other.m_fontname && other.m_fontname != hashTableDeletedFontValue())
        m_fontname = strdup(other.m_fontname);
    else
        m_fontname = other.m_fontname;

    return *this;
}

bool FontPlatformData::operator==(const FontPlatformData& other) const
{
    if (m_face == other.m_face)
        return true;
    if (m_size == other.m_size
     && m_syntheticBold == other.m_syntheticBold
     && m_syntheticOblique == other.m_syntheticOblique
     && m_fontname && other.m_fontname
     && (m_fontname == other.m_fontname || !strcasecmp(m_fontname, other.m_fontname)))
        return true;

    return false;
}

}
