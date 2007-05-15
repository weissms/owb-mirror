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
 * @file
 * Implementation of native image
 */

#include "BCNativeImage.h"
#include "BTRGBA32Buffer.h"

namespace BAL {

// Configuration
BINativeImage* createBCNativeImage(const RGBA32Buffer& buffer, const IntSize& size)
{
    BCNativeImage* nativeImage = new BCNativeImage();
    nativeImage->Create(buffer, size);
    return nativeImage;
}
BINativeImage* createBCNativeImage(const IntSize& size)
{
    BCNativeImage* nativeImage = new BCNativeImage();
    nativeImage->Create(size);
    return nativeImage;
}

// Native Image
BCNativeImage::BCNativeImage() : m_surface(NULL), m_hasAlpha(false)
{
}

IntSize BCNativeImage::size() const
{
    return m_size;
}

bool BCNativeImage::Create( const RGBA32Buffer& buffer, const IntSize& size )
{
    ASSERT( m_surface == NULL );
    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
#endif

    m_surface = SDL_CreateRGBSurfaceFrom((void*)buffer.bytes().data(),
        buffer.width(),
        buffer.height(),
        32, buffer.width()*4, rmask, gmask, bmask, amask);
    m_hasAlpha = true;
    m_size = size;
    return (m_surface != NULL);
}

bool BCNativeImage::Create(const IntSize& size)
{
    ASSERT( m_surface == NULL );
    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
    m_surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, size.width(), size.height(), 32,
                                  rmask, gmask, bmask, amask);

    m_hasAlpha = true;
    m_size = size;
    return (m_surface != NULL);
}

BCNativeImage::~BCNativeImage()
{
    if (m_surface)
    {
        SDL_FreeSurface(m_surface);
    }
}

bool BCNativeImage::hasAlpha()
{
    return m_hasAlpha;
}

}
