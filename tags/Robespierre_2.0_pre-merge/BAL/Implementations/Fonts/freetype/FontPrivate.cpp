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


/**
 * @file  Font.cpp
 *
 * Implementation file for Font.
 * This work is based on SDL_ttf implementation.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef FONT_PRIVATE_CPP
#define FONT_PRIVATE_CPP


#include "config.h"

#include "BALConfiguration.h"
#include <wtf/MathExtras.h>
#include "BIGraphicsDevice.h"
#include "BINativeImage.h"
#include "BTLogHelper.h" // to be placed after math.h
#include "Color.h"
#include "IntSize.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_IDS_H

#include "FontPrivate.h"


namespace BAL
{

FontPrivate::FontPrivate () : m_ttfFont( 0 )
{
}

FontPrivate::~FontPrivate()
{
    if (initialized)
        FT_Done_FreeType(library);
}

int FontPrivate::init(void)
{
    FT_Error error;

    if (!initialized) {
        error = FT_Init_FreeType(&library);
        if (error) {
            logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Couldn't init FreeType engine", error));
            return 0;
        }
    }
    initialized = 1;
    return 1 ;
}

font_info* FontPrivate::open(const char* file, int ptsize, long index)
{
    int position;
    int freesrc;
    font_info* font;
    FT_Stream stream;
    FT_Face face;
    FT_Error error;
    FT_Fixed scale;

    index = 0; // Assume first font face
    freesrc = 1; // Cf SDL_ttf => hardcoded to 1!

    FILE* src = fopen(file, "rb");
    if (src == NULL) {
        logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Open: %s error", file));
        return NULL;
    }
    if (!initialized) {
        logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Open : Library not initialized!\n"));
        return NULL;
    }

    // Check to make sure we can seek in this stream
    position = ftell(src);
    if (position < 0) {
        logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Open : Can't seek in stream"));
        return NULL;
    }

    font = static_cast<font_info*> (malloc(sizeof * font));
    if (font == NULL) {
        logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Open : Out of memory\n"));
        return NULL;
    }
    memset(font, 0, sizeof(*font));

    font->src = src;
    font->freesrc = freesrc;
    stream = static_cast<FT_Stream> (malloc(sizeof(*stream)));
    if (stream == NULL) {
        logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Open : Out of memory\n"));
        close(font);
        return NULL;
    }
    memset(stream, 0, sizeof(*stream));

    stream->memory = NULL; // library->memory;
    stream->read = RWread;
    stream->descriptor.pointer = src;
    stream->pos = static_cast<unsigned long> (position);
    fseek(src, 0, SEEK_END);
    stream->size = static_cast<unsigned long> (ftell(src) - position);
    fseek(src, position, SEEK_SET);

    font->args.flags = FT_OPEN_STREAM;
    font->args.stream = stream;
    error = FT_Open_Face(library, &font->args, index, &font->face);
    if (error) {
        logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Open : Couldn't load font file\n", error));
        close(font);
        return NULL;
    }

    face = font->face;

    // Make sure that our font face is scalable (global metrics)
    if (FT_IS_SCALABLE(face)) {
        // Set the character size and use default DPI (72)
        error = FT_Set_Char_Size(font->face, 0, ptsize * 64, 0, 0);
        if (error) {
            logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Open : Couldn't set font size\n", error));
            close(font);
            return NULL;
        }
        // Get the scalable font metrics for this font
        scale = face->size->metrics.y_scale;
        font->ascent = FT_CEIL(FT_MulFix(face->ascender, scale));
        font->descent = FT_CEIL(FT_MulFix(face->descender, scale));
        font->height = font->ascent - font->descent + 1; /* baseline */
        font->lineskip = FT_CEIL(FT_MulFix(face->height, scale));
        font->underline_offset = FT_FLOOR(FT_MulFix(face->underline_position, scale));
        font->underline_height = FT_FLOOR(FT_MulFix(face->underline_thickness, scale));
    } else {
        /*
         * Non-scalable font case.  ptsize determines which family
         * or series of fonts to grab from the non-scalable format.
         * It is not the point size of the font.
         */
        if (ptsize >= font->face->num_fixed_sizes)
            ptsize = font->face->num_fixed_sizes - 1;
        font->font_size_family = ptsize;
        error = FT_Set_Pixel_Sizes(face,
                                    face->available_sizes[ptsize].height,
                                    face->available_sizes[ptsize].width);
        /*
         * With non-scalale fonts, Freetype2 likes to fill many of the
         * font metrics with the value of 0.  The size of the
         * non-scalable fonts must be determined differently
         * or sometimes cannot be determined.
         */
        font->ascent = face->available_sizes[ptsize].height;
        font->descent = 0;
        font->height = face->available_sizes[ptsize].height;
        font->lineskip = FT_CEIL(font->ascent);
        font->underline_offset = FT_FLOOR(face->underline_position);
        font->underline_height = FT_FLOOR(face->underline_thickness);
    }

    if (font->underline_height < 1)
        font->underline_height = 1;

#ifdef DEBUG_FONTS
    printf("Font metrics:\n");
    printf("\tascent = %d, descent = %d\n",
            font->ascent, font->descent);
    printf("\theight = %d, lineskip = %d\n",
            font->height, font->lineskip);
    printf("\tunderline_offset = %d, underline_height = %d\n",
            font->underline_offset, font->underline_height);
#endif

    // Set the default font style
    font->style = FT_STYLE_NORMAL;
    font->glyph_overhang = face->size->metrics.y_ppem / 10;
    // x offset = cos(((90.0-12)/360)*2*M_PI), or 12 degree angle
    font->glyph_italics = 0.207f;
    font->glyph_italics *= font->height;
    fflush (stdout);

    return font;
}

void FontPrivate::close(struct font_info* font)
{
    flushCache(font);
    if (font->face)
        FT_Done_Face(font->face);

    if (font->args.stream)
        free(font->args.stream);

    if (font->freesrc)
        fclose(font->src);

    free(font);
}

void FontPrivate::quit(void)
{
    if (initialized)
        FT_Done_FreeType(library);
    initialized = 0;
}

unsigned long RWread(FT_Stream stream, unsigned long offset, unsigned char* buffer, unsigned long count)
{
    FILE* src;
    src = static_cast<FILE*> (stream->descriptor.pointer);
    fseek(src, static_cast<int> (offset), SEEK_SET);
    if (count == 0)
        return 0;
    return fread(buffer, 1, static_cast<int> (count), src);
}

int FontPrivate::sizeUnicode(font_info* font, const uint16_t* text, int* w, int* h)
{
    int status;
    int swapped;
    int x, z;
    int minx, maxx;
    int miny, maxy;
    const uint16_t* ch;
    FT_Long use_kerning;
    FT_Error error = 0;
    c_glyph* glyph;
    FT_UInt prev_index = 0;

    // Initialize everything to 0
    if (!initialized) {
        logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Library not initialized"));
        return -1;
    }
    status = 0;
    minx = maxx = 0;
    miny = maxy = 0;
    swapped = 0;

    // check kerning
    use_kerning = FT_HAS_KERNING(font->face);

    // Load each character and sum it's bounding box
    x = 0;
    for (ch = text; *ch; ++ch) {
        uint16_t c = *ch;
        if (c == UNICODE_BOM_NATIVE) {
            swapped = 0;
            if (text == ch)
                ++text;
            continue;
        }
        if (c == UNICODE_BOM_SWAPPED) {
            swapped = 1;
            if (text == ch)
                ++text;
            continue;
        }
        if (swapped)
            c = c << 8 | c >> 8;
        error = findGlyph(font, c, CACHED_METRICS);
        if (error)
            return -1;
        glyph = font->current;

        // handle kerning
        if (use_kerning && prev_index && glyph->index) {
            FT_Vector delta;
            FT_Get_Kerning(font->face, prev_index, glyph->index, ft_kerning_default, &delta);
            x += delta.x >> 6;
        }

        z = x + glyph->minx;
        if (minx > z)
            minx = z;
        if (font->style & FT_STYLE_BOLD)
            x += font->glyph_overhang;
        if (glyph->advance > glyph->maxx)
            z = x + glyph->advance;
        else
            z = x + glyph->maxx;
        if (maxx < z)
            maxx = z;
        x += glyph->advance;

        if (glyph->miny < miny)
            miny = glyph->miny;
        if (glyph->maxy > maxy)
            maxy = glyph->maxy;
        prev_index = glyph->index;
    }

    // Fill the bounds rectangle
    if (w)
        *w = (maxx - minx);
    if (h)
        *h = font->height;

    return status;
}

BINativeImage* FontPrivate::renderUnicodeBlended(font_info* font, const uint16_t* text, WebCore::Color fg, uint8_t alphaLayer)
{
    int width, height, pitch;
    uint32_t* dst;
    uint32_t* dst_check;
    int xstart;
    uint32_t alpha;
    uint32_t pixel;
    const uint16_t* ch;
    uint8_t* src;
    int swapped;
    int row, col;
    c_glyph* glyph;

    FT_Error error;
    FT_UInt prev_index = 0;
    FT_Long use_kerning;
    RGBA32Array* textbuf;
    float alphaKey = alphaLayer/255.0;
    // Get the dimensions of the text surface
    if ((sizeUnicode( font, text, &width, NULL) < 0) || !width) {
        logml(MODULE_FONTS, LEVEL_EMERGENCY, make_message("Text has zero width\n"));
        return (NULL);
    }
    height = font->height;
    textbuf = new RGBA32Array();

    if (textbuf == NULL)
        return NULL;

    pitch = width * 4;
    textbuf->resize(pitch * height);

    // Adding bound checking to avoid all kinds of memory corruption errors that may occur.
    dst_check = static_cast<uint32_t*> (textbuf->data()) + pitch / 4 * height;

    // check kerning
    use_kerning = FT_HAS_KERNING(font->face);

    // Load and render each character
    xstart = 0;
    swapped = 0;
    pixel = (fg.red() << 16) | (fg.green() << 8) | fg.blue();
    textbuf->fill(pixel); // Initialize with fg and 0 alpha

    for (ch = text; *ch; ++ch) {
        uint16_t c = *ch;
        if (c == UNICODE_BOM_NATIVE) {
            swapped = 0;
            if (text == ch)
                ++text;
            continue;
        }
        if (c == UNICODE_BOM_SWAPPED) {
            swapped = 1;
            if (text == ch)
                ++text;
            continue;
        }
        if (swapped)
            c = c << 8 | c >> 8;

        error = findGlyph(font, c, CACHED_METRICS | CACHED_PIXMAP);
        if (error) {
            delete textbuf;
            return NULL;
        }
        glyph = font->current;
        // Ensure the width of the pixmap is correct. On some cases,
        // freetype may report a larger pixmap than possible.
        width = glyph->pixmap.width;
        if (width > glyph->maxx - glyph->minx)
            width = glyph->maxx - glyph->minx;

        // do kerning, if possible AC-Patch
        if (use_kerning && prev_index && glyph->index) {
            FT_Vector delta;
            FT_Get_Kerning(font->face, prev_index, glyph->index, ft_kerning_default, &delta);
            xstart += delta.x >> 6;
        }

        // Compensate for the wrap around bug with negative minx's
        if ((ch == text) && (glyph->minx < 0))
            xstart -= glyph->minx;

        for (row = 0; row < glyph->pixmap.rows; ++row) {
            //Make sure we don't go either over, or under the limit
            if (row + glyph->yoffset < 0)
                continue;
            if (row + glyph->yoffset >= height)
                continue;
            dst = static_cast<uint32_t*> (textbuf->data()) +
                    (row + glyph->yoffset) * pitch / 4 +
                    xstart + glyph->minx;

            // Added code to adjust src pointer for pixmaps to account for pitch.
            src = static_cast<uint8_t*> (glyph->pixmap.buffer + glyph->pixmap.pitch * row);
            for (col = width; col > 0 && dst < dst_check; --col) {
                alpha = *src++;
                alpha = uint32_t((alpha & 0x000000ff) * alphaKey);
                *dst++ |= pixel | (alpha << 24);
            }
        }

        xstart += glyph->advance;
        if (font->style & FT_STYLE_BOLD)
            xstart += font->glyph_overhang;
        prev_index = glyph->index;
    }

    // Handle the underline style
    if (font->style & FT_STYLE_UNDERLINE) {
        row = font->ascent - font->underline_offset - 1;
        if (row >= height)
            row = (height - 1) - font->underline_height;
        dst = static_cast<uint32_t*> (textbuf->data()) + row * pitch / 4;
        pixel |= 0xFF000000; /* Amask */
        for (row = font->underline_height; row > 0; --row) {
            for (col = 0; col < width; ++col)
                dst[col] = pixel;
            dst += width;
        }
    }
    // use pitch because width may be broken here
    return getBIGraphicsDevice()->createNativeImage(*textbuf, WebCore::IntSize(pitch / 4, height));
}

FT_Error FontPrivate::findGlyph(font_info* font, uint16_t ch, int want)
{
    int retval = 0;

    if (ch < 256)
        font->current = &font->cache[ch];
    else {
        if (font->scratch.cached != ch)
            flushGlyph(&font->scratch);
        font->current = &font->scratch;
    }
    if ((font->current->stored & want) != want)
        retval = loadGlyph(font, ch, font->current, want);
    return retval;
}

void FontPrivate::flushGlyph(c_glyph* glyph)
{
    glyph->stored = 0;
    glyph->index = 0;
    if (glyph->bitmap.buffer) {
        free(glyph->bitmap.buffer);
        glyph->bitmap.buffer = 0;
    }
    if (glyph->pixmap.buffer) {
        free(glyph->pixmap.buffer);
        glyph->pixmap.buffer = 0;
    }
    glyph->cached = 0;
}

void FontPrivate::flushCache(font_info* font)
{
    int i;
    int size = sizeof(font->cache) / sizeof(font->cache[0]);

    for (i = 0; i < size; ++i) {
        if (font->cache[i].cached)
            flushGlyph(&font->cache[i]);
    }
    if (font->scratch.cached)
        flushGlyph(&font->scratch);
}

FT_Error FontPrivate::loadGlyph(font_info* font, uint16_t ch, c_glyph* cached, int want)
{
    FT_Face face;
    FT_Error error;
    FT_GlyphSlot glyph;
    FT_Glyph_Metrics* metrics;
    FT_Outline* outline;

    if (!font || !font->face)
        return FT_Err_Invalid_Handle;

    face = font->face;

    // Load the glyph
    if (!cached->index)
        cached->index = FT_Get_Char_Index(face, ch);

    // PPi : suppression of autohinting => twice faster for big texts
    // error = FT_Load_Glyph( face, cached->index, FT_LOAD_DEFAULT );
    error = FT_Load_Glyph(face, cached->index, FT_LOAD_NO_AUTOHINT);
    if (error)
        return error;

    // Get our glyph shortcuts
    glyph = face->glyph;
    metrics = &glyph->metrics;
    outline = &glyph->outline;

    // Get the glyph metrics if desired
    if ((want & CACHED_METRICS) && !(cached->stored & CACHED_METRICS)) {
        if (FT_IS_SCALABLE(face)) {
            // Get the bounding box
            cached->minx = FT_FLOOR(metrics->horiBearingX);
            cached->maxx = cached->minx + FT_CEIL(metrics->width);
            cached->maxy = FT_FLOOR(metrics->horiBearingY);
            cached->miny = cached->maxy - FT_CEIL(metrics->height);
            cached->yoffset = font->ascent - cached->maxy;
            cached->advance = FT_CEIL(metrics->horiAdvance);
        } else {
            /*
             * Get the bounding box for non-scalable format.
             * Again, freetype2 fills in many of the font metrics
             * with the value of 0, so some of the values we
             * need must be calculated differently with certain
             * assumptions about non-scalable formats.
             */
            cached->minx = FT_FLOOR(metrics->horiBearingX);
            cached->maxx = cached->minx + FT_CEIL(metrics->horiAdvance);
            cached->maxy = FT_FLOOR(metrics->horiBearingY);
            cached->miny = cached->maxy - FT_CEIL(face->available_sizes[font->font_size_family].height);
            cached->yoffset = 0;
            cached->advance = FT_CEIL(metrics->horiAdvance);
        }

        // Adjust for bold and italic text
        if (font->style & FT_STYLE_BOLD)
            cached->maxx += font->glyph_overhang;
        if (font->style & FT_STYLE_ITALIC)
            cached->maxx += static_cast<int> (ceil(font->glyph_italics));
        cached->stored |= CACHED_METRICS;
    }

    if (((want & CACHED_BITMAP) && !(cached->stored & CACHED_BITMAP)) ||
            ((want & CACHED_PIXMAP) && !(cached->stored & CACHED_PIXMAP))) {
        int mono = (want & CACHED_BITMAP);
        int i;
        FT_Bitmap* src;
        FT_Bitmap* dst;
        // Handle the italic style
        if (font->style & FT_STYLE_ITALIC) {
            FT_Matrix shear;
            shear.xx = 1 << 16;
            shear.xy = static_cast<int> ((font->glyph_italics * (1 << 16)) / font->height);
            shear.yx = 0;
            shear.yy = 1 << 16;

            FT_Outline_Transform(outline, &shear);
        }

        // Render the glyph
        if (mono)
            error = FT_Render_Glyph( glyph, ft_render_mode_mono );
        else
            error = FT_Render_Glyph( glyph, ft_render_mode_normal );
        if (error)
            return error;

        // Copy over information to cache
        src = &glyph->bitmap;
        if (mono)
            dst = &cached->bitmap;
        else
            dst = &cached->pixmap;
        memcpy(dst, src, sizeof(*dst));

        /*
         * FT_Render_Glyph() and .fon fonts always generate a
         * two-color (black and white) glyphslot surface, even
         * when rendered in ft_render_mode_normal.  This is probably
         * a freetype2 bug because it is inconsistent with the
         * freetype2 documentation under FT_Render_Mode section.
         */
        if (mono || !FT_IS_SCALABLE(face))
            dst->pitch *= 8;

        // Adjust for bold and italic text
        if (font->style & FT_STYLE_BOLD) {
            int bump = font->glyph_overhang;
            dst->pitch += bump;
            dst->width += bump;
        }
        if (font->style & FT_STYLE_ITALIC) {
            int bump = static_cast<int> (ceil(font->glyph_italics));
            dst->pitch += bump;
            dst->width += bump;
        }

        if (dst->rows != 0) {
            dst->buffer = static_cast<unsigned char*> (malloc(dst->pitch * dst->rows));
            if (!dst->buffer)
                return FT_Err_Out_Of_Memory;
            memset(dst->buffer, 0, dst->pitch * dst->rows);

            for (i = 0; i < src->rows; i++) {
                int soffset = i * src->pitch;
                int doffset = i * dst->pitch;
                if (mono) {
                    unsigned char* srcp = src->buffer + soffset;
                    unsigned char* dstp = dst->buffer + doffset;
                    for (int j = 0; j < src->width; j += 8) {
                        unsigned char ch = *srcp++;
                        *dstp++ = (ch & 0x80) >> 7;
                        ch <<= 1;
                        *dstp++ = (ch & 0x80) >> 7;
                        ch <<= 1;
                        *dstp++ = (ch & 0x80) >> 7;
                        ch <<= 1;
                        *dstp++ = (ch & 0x80) >> 7;
                        ch <<= 1;
                        *dstp++ = (ch & 0x80) >> 7;
                        ch <<= 1;
                        *dstp++ = (ch & 0x80) >> 7;
                        ch <<= 1;
                        *dstp++ = (ch & 0x80) >> 7;
                        ch <<= 1;
                        *dstp++ = (ch & 0x80) >> 7;
                    }
                } else if (!FT_IS_SCALABLE(face)) {
                    /*
                     * This special case wouldn't
                     * be here if the FT_Render_Glyph()
                     * function wasn't buggy when it tried
                     * to render a .fon font with 256
                     * shades of gray.  Instead, it
                     * returns a black and white surface
                     * and we have to translate it back
                     * to a 256 gray shaded surface.
                     */
                    unsigned char* srcp = src->buffer + soffset;
                    unsigned char* dstp = dst->buffer + doffset;
                    unsigned char ch;
                    for (int j = 0; j < src->width; j += 8) {
                        ch = *srcp++;
                        for (int k = 0; k < 8; ++k) {
                            if ((ch & 0x80) >> 7)
                                *dstp++ = NUM_GRAYS - 1;
                            else
                                *dstp++ = 0x00;
                            ch <<= 1;
                        }
                    }
                } else
                    memcpy(dst->buffer + doffset, src->buffer + soffset, src->pitch);
            }
        }

        // Handle the bold style
        if (font->style & FT_STYLE_BOLD) {
            int row;
            int col;
            int offset;
            int pixel;
            uint8_t* pixmap;

            // The pixmap is a little hard, we have to add and clamp
            for (row = dst->rows - 1; row >= 0; --row) {
                pixmap = static_cast<uint8_t*> (dst->buffer + row * dst->pitch);
                for (offset = 1; offset <= font->glyph_overhang; ++offset) {
                    for (col = dst->width - 1; col > 0; --col) {
                        pixel = (pixmap[col] + pixmap[col - 1]);
                        if (pixel > NUM_GRAYS - 1)
                            pixel = NUM_GRAYS - 1;
                        pixmap[col] = static_cast<uint8_t> (pixel);
                    }
                }
            }
        }

        // Mark that we rendered this format
        if (mono)
            cached->stored |= CACHED_BITMAP;
        else
            cached->stored |= CACHED_PIXMAP;
    }

    // We're done, mark this glyph cached
    cached->cached = ch;
    return 0;
}

}

#endif
