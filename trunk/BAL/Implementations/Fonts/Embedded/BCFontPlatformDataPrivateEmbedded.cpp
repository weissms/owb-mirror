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
#include "BCFontPlatformDataPrivateEmbedded.h"
#include "BTFontPlatformData.h"
#include "CString.h"

IMPLEMENT_CREATE_DELETE(BIFontPlatformDataPrivate, BCFontPlatformDataPrivateEmbedded);

using WebCore::CString;
using WebCore::StringImpl;

namespace BAL {

BCFontPlatformDataPrivateEmbedded::BCFontPlatformDataPrivateEmbedded()
    :m_pixelFont(0)
{
}

BCFontPlatformDataPrivateEmbedded::~BCFontPlatformDataPrivateEmbedded()
{
    if (m_pixelFont)
        delete m_pixelFont;
    m_pixelFont = NULL;
}

int BCFontPlatformDataPrivateEmbedded::init()
{
    m_pixelFont = &NormalPixelFont;
    return 1;
}

void BCFontPlatformDataPrivateEmbedded::setFont(const FontDescription& fontDescription)
{
    //Current policy: bold > italic
    if (fontDescription.italic())
        m_pixelFont = &NormalItalicPixelFont;
    if (fontDescription.bold())
        m_pixelFont = &NormalBoldPixelFont;
}

bool BCFontPlatformDataPrivateEmbedded::isFixedPitch()
{
    //Always with our embedded fonts
    return true;
}

unsigned BCFontPlatformDataPrivateEmbedded::hash() const
{
    uintptr_t hashCodes[1] = { reinterpret_cast<uintptr_t>(m_pixelFont) };
    return StringImpl::computeHash( reinterpret_cast<UChar*>(hashCodes), sizeof(hashCodes) / sizeof(UChar));
}

bool BCFontPlatformDataPrivateEmbedded::isEqual(const BIFontPlatformDataPrivate* other) const
{
    const BCFontPlatformDataPrivateEmbedded *compare = static_cast<const BCFontPlatformDataPrivateEmbedded*> (other);
    if (!compare)
        return false;
    if (m_pixelFont == 0 || compare->m_pixelFont == 0)
        return false;
    if (m_pixelFont->width == compare->m_pixelFont->width)
        return true;
    return false;
}

} //namespace BAL
