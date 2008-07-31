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
#include "PlatformString.h"
#include "FontDescription.h"
#include <proto/exec.h>
#include <diskfont/diskfonttag.h>
#include <diskfont/oterrors.h>
#include <proto/utility.h>

namespace WKAL {
extern "C" {
// OWB never calls anything which could free the fonts, have to track
// and release them on exit.
static List font_list, tracking_list;
static void add_to_fontlist(struct OutlineFont *face) 
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    if (node) {
       node->ln_Name = (STRPTR)face;
       IExec->AddHead(&tracking_list, node);
    }
}

static void amiga_font_init(void) __attribute__((constructor));
static void amiga_font_init(void)
{
    IExec->NewList(&tracking_list);
}

static void amiga_font_cleanup(void) __attribute__((destructor));
static void amiga_font_cleanup(void)
{
    struct Node *node = IExec->GetHead(&tracking_list);
    while (node) {
        if (node->ln_Name) {
            IDiskfont->CloseOutlineFont((struct OutlineFont *)node->ln_Name, &font_list);
            node->ln_Name = 0;
        }
        IExec->Remove(node);
        node = IExec->GetHead(&tracking_list);
    }
}
};

static char *get_font_name(const char *family, const int type, const bool bold, const bool italic, struct List *list)
{
    char *fontname = NULL;

    if (0 != family && 0 != *family) {
        struct OutlineFont *face = IDiskfont->OpenOutlineFont(family, list, 0);
        if (face) {
            if (!bold && !italic) {
                char *rname = (char *)IUtility->GetTagData(OT_RName, NULL, face->olf_OTagList);
                if (rname)
                    fontname = strdup(rname);
                else
                    fontname = strdup(family);
            }
            else if (bold && !italic) {
                char *bname = (char *)IUtility->GetTagData(OT_BName, NULL, face->olf_OTagList);
                if (bname)
                    fontname = strdup(bname);
                else
                    fprintf(stderr, "font family '%s' has no bold font\n", family);
            }
            else if (!bold && italic) {
                char *iname = (char *)IUtility->GetTagData(OT_IName, NULL, face->olf_OTagList);
                if (iname)
                    fontname = strdup(iname);
                else
                    fprintf(stderr, "font family '%s' has no italic font\n", family);
            }
            else { // (bold && italic)
                char *biname = (char *)IUtility->GetTagData(OT_BIName, NULL, face->olf_OTagList);
                if (biname)
                    fontname = strdup(biname);
                else
                    fprintf(stderr, "font family '%s' has no bold italic font\n", family);
            }
            IDiskfont->CloseOutlineFont(face, list);
        } else
            fprintf(stderr, "font family '%s' not found\n", family);
    }

    if (0 == fontname) {
        const char *boldstr = bold ? " Bold" : "";
        const char *italicstr = italic ? " Italic" : "";
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
    }

    return fontname;
}

static char *get_font_name_fallback(const char *family, const int type, const bool bold, const bool italic, struct List *list)
{
    char *fontname = NULL;
    const char *boldstr = bold ? " Bold" : "";
    const char *italicstr = italic ? " Oblique" : "";
    char tempfontname[256];

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

FontPlatformData::FontPlatformData(WTF::HashTableDeletedValueType)
    : m_fontname(hashTableDeletedFontValue())
{ }

FontPlatformData::FontPlatformData()
    : m_fontname(0)
{ }

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

    int type = fontDescription.genericFamily();
    bool bold = fontDescription.weight() >= FontWeight600;
    bool italic = fontDescription.italic();
    char *fontname = get_font_name(family, type, bold, italic, &font_list);
    if (fontname) {
        m_face = IDiskfont->OpenOutlineFont(fontname, &font_list, OFF_OPEN);
        if (!m_face)
            free(fontname);
    }
    if (!m_face) {
        fontname = get_font_name_fallback(family, type, bold, italic, &font_list);
        if (fontname)
            m_face = IDiskfont->OpenOutlineFont(fontname, &font_list, OFF_OPEN);
    }
    if (m_face) {
        if (IDiskfont->ESetInfo(&m_face->olf_EEngine,
                                OT_DeviceDPI, (80 << 16) | 72,
                                OT_PointHeight, (int)m_size << 16,
                                TAG_END) != OTERR_Success) {
            IDiskfont->CloseOutlineFont(m_face, &font_list);
            m_face = 0;
        }
        else {
            add_to_fontlist(m_face);
            m_fontname = fontname;
            fontname = 0;
        }
    }
    free(fontname);
}

FontPlatformData::FontPlatformData(float size, bool bold, bool italic)
    : m_size(size)
    , m_syntheticBold(bold)
    , m_syntheticOblique(italic)
    , m_fontname(0)
{
    fprintf(stderr, "%s: m_size = %f %s%s\n", __PRETTY_FUNCTION__, size, bold ? "b" : "", italic ? "i" : "");
}

FontPlatformData::FontPlatformData(BalFontFace* fontFace, int size, bool bold, bool italic)
    : m_size(size)
    , m_syntheticBold(bold)
    , m_syntheticOblique(italic)
    , m_fontname(0)
{
    fprintf(stderr, "%s: fontFace = %p, size = %d %s%s\n", __PRETTY_FUNCTION__, fontFace, size, bold ? "b" : "", italic ? "i" : "");
    m_face = fontFace;
}

bool FontPlatformData::init()
{
    static bool initialized = false;
    if (!initialized) {
        IExec->NewList(&font_list);
        initialized = true;
    }
    return true;
}

FontPlatformData::~FontPlatformData()
{
//    fprintf(stderr, "%s: m_face = %p\n", __PRETTY_FUNCTION__, m_face); // only called once, with 0 m_face!?
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

    BalNotImplemented();
}

bool FontPlatformData::operator==(const FontPlatformData& other) const
{
    if (m_face == other.m_face)
        return true;
    if (m_size == other.m_size
     && m_fontname && other.m_fontname
     && (m_fontname == other.m_fontname || !strcmp(m_fontname, other.m_fontname)))
        return true;

    return false;
}

}
