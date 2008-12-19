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
#ifndef GlyphWidthMap_h
#define GlyphWidthMap_h
/**
 *  @file  GlyphWidthMap.t
 *  GlyphWidthMap description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include <wtf/unicode/Unicode.h>
#include <wtf/Noncopyable.h>
#include <wtf/HashMap.h>

namespace WKAL {

typedef unsigned short Glyph;

const float cGlyphWidthUnknown = -1;

class GlyphWidthMap : public WKALBase, Noncopyable {
public:
    /**
     * GlyphWidthMap default constructor
     * @code
     * GlyphWidthMap *gwm = new GlyphWidthMap();
     * @endcode
     */
    GlyphWidthMap();

    /**
     * ~GlyphWidthMap destructor
     * @code
     * delete gwm;
     * @endcode
     */
    ~GlyphWidthMap() ;

    /**
     * width for a glyph
     * @param[in] : glyph
     * @param[out] : width
     * @code
     * float width = gwm->widthForGlyph(g);
     * @endcode
     */
    float widthForGlyph(Glyph);

    /**
     * set width for a glyph
     * @param[in] : glyph
     * @param[in] : width
     * @code
     * gwm->setWidthForGlyph(g, w);
     * @endcode
     */
    void setWidthForGlyph(Glyph, float);

private:
    struct GlyphWidthPage {
        static const size_t size = 256; // Usually covers Latin-1 in a single page.
        float m_widths[size];

    /**
     * width for a glyph 
     * @param[in] : glyph
     * @param[out] : width
     */
        float widthForGlyph(Glyph g) const ;

    /**
     * set width for a glyph 
     * @param[in] : glyph
     * @param[in] : width
     */
        void setWidthForGlyph(Glyph g, float w);

    /**
     * set width for an index
     * @param[in] : index
     * @param[in] : width
     */
        void setWidthForIndex(unsigned index, float w);
    };
    
    /**
     * locate glyph width page
     * @param[in] : page
     * @param[out] : glyph width page
     */
    GlyphWidthPage* locatePage(unsigned page);
    
    bool m_filledPrimaryPage;
    GlyphWidthPage m_primaryPage; // We optimize for the page that contains glyph indices 0-255.
    HashMap<int, GlyphWidthPage*>* m_pages;
};

}
#endif




