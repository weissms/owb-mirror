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
 * @file  BCFontPlatformDataPrivateEmbedded.h
 *
 * Header file for BCFontPlatformDataPrivateEmbedded.h
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BCFontPlatformDataPrivateEmbedded_h
#define BCFontPlatformDataPrivateEmbedded_h

#include "BIFontPlatformDataPrivate.h"
#include "FontDescription.h"
#include "fonts/pixelfont.h"

using WebCore::FontDescription;

namespace BAL {

/**
 * @brief BCFontPlatformDataPrivateEmbedded
 * Class which implements BIFontPlatformDataPrivate
 * this is a FreeType based implementation
 */

class BCFontPlatformDataPrivateEmbedded : public BIFontPlatformDataPrivate {
    public:

        BCFontPlatformDataPrivateEmbedded();

        ~BCFontPlatformDataPrivateEmbedded();

        /**
         * This method is used to initialize FreeType and Fontconfig libraries.
         * @return a boolean: true for successful initialization else false
         */
        virtual uint8_t init();

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

		void setFont(const FontDescription& fontDescription);
		PixelFontType* fontType() { return m_pixelFont; }
    private:
		PixelFontType *m_pixelFont;
};

} //namespace BAL

#endif //BCFontPlatformDataPrivateEmbedded_h
