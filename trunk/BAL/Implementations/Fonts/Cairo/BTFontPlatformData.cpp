/*
 * This file is part of the internal font implementation.  It should not be included by anyone other than
 * FontMac.cpp, FontWin.cpp and Font.cpp.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Copyright (C) 2007 Pleyo
 * All rights reserved.
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

/**
 * @file BTFontPlatformData.cpp
 *
 * Cairo implementation for FontPlatformData
 */
#include "config.h"
#include "BTFontPlatformData.h"
#include "BTLogHelper.h"
#include "PlatformString.h"
#include "DeprecatedString.h"
#include "FontDescription.h"
#include "cairo-ft.h"
#include "cairo.h"
#include <fontconfig/fcfreetype.h>
#include <assert.h>

namespace BAL
{

static FcBool FCInitialized;
static FcEndian endian;


FontPlatformData::FontPlatformData(const WebCore::FontDescription& fontDescription, const WebCore::AtomicString& familyName ) :
        m_pattern(0), m_fontDescription(fontDescription), m_fontMatrix(0),
        m_fontFace(0), m_options(0), m_scaledFont(0)
{
    init();
    FcResult fcresult;
    FcPattern *pattern;
    FcChar8 *family;
    family = ( FcChar8 *)((const char *)(familyName.deprecatedString().ascii()));
    int fcslant=FC_SLANT_ROMAN;
    int fcweight=FC_WEIGHT_NORMAL;
    int fcsize= (int)fontDescription.computedSize();
    if( fontDescription.italic() ) {
        //FC_SLANT_ROMAN == normal FC_SLANT_OBLIQUE ??
        fcslant=FC_SLANT_ITALIC;
    }
    if( fontDescription.weight() == WebCore::cBoldWeight ) {
        fcweight = FC_WEIGHT_BOLD;
    }

    if( !family ) {
        //FIXME use generic for now
        int type=fontDescription.genericFamily();
        switch(type ) {
            case WebCore::FontDescription::SerifFamily:
                family=(FcChar8 *)"sans-serif";
                break;
            case WebCore::FontDescription::SansSerifFamily:
                family=(FcChar8 *)"sans-serif";
                break;
            case WebCore::FontDescription::NoFamily:
            case WebCore::FontDescription::StandardFamily:
            default:
                family=(FcChar8 *)"sans-serif";
        }
    }
    pattern = FcPatternCreate();
    assert(pattern != 0);

    pattern = FcPatternBuild(0,
     FC_FAMILY, FcTypeString,  family,
     FC_WEIGHT, FcTypeInteger, fcweight,
     FC_SLANT,  FcTypeInteger, fcslant,
     FC_PIXEL_SIZE, FcTypeInteger,(int)fcsize,
     FC_DPI, FcTypeDouble, 92.0,
     FC_SCALE, FcTypeDouble, 1.0,
     FC_SPACING,FcTypeInteger,FC_PROPORTIONAL,
     NULL);
    assert(pattern != 0);

    if (!FcPatternAddString (pattern, FC_FAMILY, family))
        goto FREE_PATTERN;
    if (!FcPatternAddInteger (pattern, FC_SLANT, fcslant))
        goto FREE_PATTERN;
    if (!FcPatternAddInteger (pattern, FC_WEIGHT, fcweight))
        goto FREE_PATTERN;
    if (!FcPatternAddInteger (pattern, FC_PIXEL_SIZE, fcsize))
        goto FREE_PATTERN;


    FcConfigSubstitute (NULL, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);
    m_pattern = FcFontMatch (NULL, pattern, &fcresult);
    if (!m_pattern) {
        logml(MODULE_FONTS, LEVEL_WARNING, "No matching font");
        goto FREE_PATTERN;
    }

    m_fontFace=cairo_ft_font_face_create_for_pattern(m_pattern);
    m_fontMatrix = (cairo_matrix_t *)malloc(sizeof(cairo_matrix_t));
    cairo_matrix_t ctm;
    cairo_font_extents_t font_extents;
    cairo_text_extents_t text_extents;
    cairo_matrix_init_scale (m_fontMatrix,m_fontDescription.computedPixelSize(),
                             m_fontDescription.computedPixelSize());
    cairo_matrix_init_identity (&ctm);
    m_options = cairo_font_options_create ();
    m_scaledFont = cairo_scaled_font_create (m_fontFace,m_fontMatrix,&ctm,m_options);

    assert( m_scaledFont != 0 );
    FcPatternDestroy (pattern);
    return;

FREE_PATTERN:
    logml(MODULE_FONTS, LEVEL_WARNING, "Fontconfig error");
    FcPatternDestroy (pattern);

}

bool FontPlatformData::init ()
{
    int i=1;
    char *p;
    if (FCInitialized)
        return true;
    FCInitialized = FcTrue;
    p = (char *) &i;
    if( p[0] == 1 )
        endian = FcEndianLittle;
    else
        endian = FcEndianBig;
    if(!FcInit ()) {
        fprintf (stderr, "Can't init font config library\n");
        return false;
    }
    return true;
}

FontPlatformData::~FontPlatformData()
{
    //XXX FIXME should this arrive ?
    if ((FcPattern *)-1 != m_pattern && 0 != m_pattern) {
        FcPatternDestroy(m_pattern);
    }
    cairo_font_face_destroy(m_fontFace);
    cairo_scaled_font_destroy(m_scaledFont);
    cairo_font_options_destroy(m_options);
}

bool FontPlatformData::isFixedPitch()
{
    int spacing;
    if ( FcPatternGetInteger(m_pattern, FC_SPACING, 0, &spacing)
         == FcResultMatch) {
        return (spacing == FC_MONO);
    }
    return false;
}

/*
 * length is a out value giving the length of the list
 *
 *
 */
cairo_font_face_t **
        FontPlatformData::list(WebCore::FontDescription& fontDescription,
                               const WebCore::AtomicString& familyName,int *length )
{

    FcFontSet       *fs;
    FcObjectSet     *os;
    FcResult fc_result;
    FcChar8 *family;
    FcPattern *pattern;
    if( familyName != 0  ) {
        family= (FcChar8 *)((const char *)(familyName.deprecatedString().utf8()));
    }
    init();
    FontPlatformData data = FontPlatformData(fontDescription,familyName);
    pattern= data.m_pattern;
    int i;
    cairo_font_face_t **result;


    if( !(pattern =  FcPatternCreate ()) ) {
        return NULL;
    }
    os =  FcObjectSetCreate ();
    FcObjectSetAdd(os,FC_FAMILY);
    FcObjectSetAdd(os,FC_STYLE);
    /*
            if(pattern) {
            FcConfigSubstitute (NULL, pattern, FcMatchPattern);
            FcDefaultSubstitute (pattern);
            //cairo_ft_font_options_substitute (font_options, pattern);
            }
    */

    fs=FcFontList (0,pattern,os);
    if(length)
        *length = fs->nfont;
    if( pattern)
        FcPatternDestroy (pattern);
    if( os)
        FcObjectSetDestroy (os);
    result = (cairo_font_face_t **)malloc(fs->nfont*sizeof(cairo_font_face_t *)+1);
    for(i = 0; i < fs->nfont; i++ ) {
        FcChar8 *font;
        font = FcNameUnparse (fs->fonts[i]);
        result[i] =cairo_ft_font_face_create_for_pattern (fs->fonts[i]);
    }

    result[fs->nfont] = NULL;
    return result;
}

/**
 * Maps a Unicode char to a glyph index.
 * This function uses information from several possible underlying encoding
 * tables to work around broken fonts.  As a result, this function isn't
 * designed to be used in performance sensitive areas; results from this
 * function are intended to be cached by higher level functions.
 */
Glyph FontPlatformData::index(unsigned int ucs4) const
{
    // FIXME PPI why limit index to 255 chars ??
    // ucs4 = (0xff & ucs4);
    FT_Face face = cairo_ft_scaled_font_lock_face (m_scaledFont);
    assert( face !=0 );
    int index =FcFreeTypeCharIndex (face, ucs4);
    cairo_ft_scaled_font_unlock_face (m_scaledFont);
    return index;
}


void FontPlatformData::setFont(cairo_t *cr) const
{
    cairo_set_font_face(cr,m_fontFace);
    cairo_set_font_matrix (cr,m_fontMatrix);
    cairo_set_font_options (cr,m_options);
}

bool FontPlatformData::operator==(const FontPlatformData& other) const
{
    if( m_pattern == other.m_pattern)
        return true;
    if( m_pattern ==  (FcPattern *)0
        || m_pattern == (FcPattern *)-1
        || other.m_pattern == (FcPattern *)0
        || other.m_pattern == (FcPattern *)-1)
        return false;

    return FcPatternEqual(m_pattern,other.m_pattern);
}



}
