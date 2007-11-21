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
 * @file  BCFontPlatformDataPrivateFT.h
 *
 * Header file for BCFontPlatformDataPrivateFT.h
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BCFontPlatformDataPrivateFreeType_h
#define BCFontPlatformDataPrivateFreeType_h

#include "BIFontPlatformDataPrivate.h"
#include "FontDescription.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig/fontconfig.h>

using WebCore::AtomicString;
using WebCore::FontDescription;

#define DOUBLE_TO_26_6(d) ((FT_F26Dot6)((d) * 64.0))
#define DOUBLE_FROM_26_6(t) ((double)(t) / 64.0)
#define DOUBLE_TO_16_16(d) ((FT_Fixed)((d) * 65536.0))
#define DOUBLE_FROM_16_16(t) ((double)(t) / 65536.0)

namespace BAL {

/**
 * @brief BCFontPlatformDataPrivateFreeType
 * Class which implements BIFontPlatformDataPrivate
 * this is a FreeType based implementation
 */

class BCFontPlatformDataPrivateFreeType : public BIFontPlatformDataPrivate {
    public:

        BCFontPlatformDataPrivateFreeType();

        ~BCFontPlatformDataPrivateFreeType();

        /**
         * This method is used to initialize FreeType and Fontconfig libraries.
         * @return a boolean: true for successful initialization else false
         */
        virtual int init();

        /**
         * This method determines wether the used font has a fixed pitch or not.
         * @return a boolean: true for fixed pitch font else false
         */
        virtual bool isFixedPitch();

        /**
         *
         * @return
         */
        virtual unsigned hash() const;

        /**
         *
         * @return
         */
        virtual bool isEqual(const BIFontPlatformDataPrivate*) const;

        /**
         * @param[in] FontDescription
         * @param[in] AtomicString
         * Method used to set m_pattern attribute
         */
        void configureFontconfig(const FontDescription& fontDescription, const AtomicString& familyName);

        /**
         * @param[in] FontDescription: to retrieve font filename path
         * Method used to set m_face attribute
         */
        void configureFreetype(const FontDescription& fontDescription);

        /**
         * getter for m_face attribute
         * @return FT_Face: a Freetype font face
         */
        FT_Face ftFace() { return m_face; }

        /**
         * getter for m_pattern attribute
         * @return FcPattern*
         */
        FcPattern* fcPattern() { return m_pattern; }

    private:
        static int m_isInitialized;
        FcPattern   *m_pattern;
        /**
         * Declared as static to avoid multiple freetype initialization
         */
        static FT_Library  m_library;
        FT_Face     m_face;
};

} //namespace BAL

#endif //BCFontPlatformDataPrivateFreeType_h
