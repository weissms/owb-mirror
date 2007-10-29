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

#ifndef FONTPRIVATE_H
#define FONTPRIVATE_H



#define FT_FLOOR(X) ((X & -64) / 64)
#define FT_CEIL(X)  (((X + 63) & -64) / 64)

#define FT_STYLE_NORMAL     0x00
#define FT_STYLE_BOLD       0x01
#define FT_STYLE_ITALIC     0x02
#define FT_STYLE_UNDERLINE  0x04


#define CACHED_METRICS  0x10
#define CACHED_BITMAP   0x01
#define CACHED_PIXMAP   0x02

#define NUM_GRAYS       256

// ZERO WIDTH NO-BREAKSPACE (Unicode byte order mark)
#define UNICODE_BOM_NATIVE  0xFEFF
#define UNICODE_BOM_SWAPPED 0xFFFE



typedef struct cached_glyph
{
    int stored;
    FT_UInt index;
    FT_Bitmap bitmap;
    FT_Bitmap pixmap;
    int minx;
    int maxx;
    int miny;
    int maxy;
    int yoffset;
    int advance;
    uint16_t cached;
}
c_glyph;


struct font_info
{
    // Freetype2 maintains all sorts of useful info itself
    FT_Face face;

    // We'll cache these ourselves
    int height;
    int ascent;
    int descent;
    int lineskip;

    // The font style
    int style;

    // Extra width in glyph bounds for text styles
    int glyph_overhang;
    float glyph_italics;

    // Information in the font for underlining
    int underline_offset;
    int underline_height;

    // Cache for style-transformed glyphs
    c_glyph* current;
    c_glyph cache[256];
    c_glyph scratch;

    // We are responsible for closing the font stream
    FILE* src;
    int freesrc;
    FT_Open_Args args;

    // For non-scalable formats, we must remember which font index size
    int font_size_family;
};


namespace BAL
{

    class FontPrivate
    {
        public:
            FontPrivate();
            ~FontPrivate();

            static  int initialized;

            font_info* m_ttfFont; // TODO rename



            static FT_Library library;
            int init();
            font_info* open(const char* file, int ptsize, long index);    // TODO return type !!!
            void close(struct font_info* font);
            void quit();
            int sizeUnicode(font_info* font, const uint16_t* text, int* w, int* h);


            BINativeImage* renderUnicodeBlended(font_info* font,const uint16_t* text, WebCore::Color fg, uint8_t alphaLayer);
            FT_Error findGlyph(font_info* font, uint16_t ch, int want);
            void flushGlyph(c_glyph* glyph);
            void flushCache(font_info* font);
            FT_Error loadGlyph(font_info* font, uint16_t ch, c_glyph* cached, int want);

    };

    unsigned long RWread(FT_Stream stream, unsigned long offset, unsigned char* buffer, unsigned long count);

}

#endif // FONTPRIVATE_H
