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

#include <cairo.h>

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
BCNativeImage::BCNativeImage() : m_surface(0), m_cairoSurface(NULL), m_hasAlpha(false)
{ 
}

IntSize BCNativeImage::size() const
{
  return m_size;
}

bool BCNativeImage::Create( const RGBA32Buffer& aBuffer, const IntSize& aSize )
{
    ASSERT( m_cairoSurface == NULL );
    m_cairoSurface = cairo_image_surface_create_for_data( (unsigned char*)aBuffer.bytes().data(),
        CAIRO_FORMAT_ARGB32,
        aBuffer.width(),
        aBuffer.height(),
	aBuffer.width()*4 );

    m_hasAlpha = aBuffer.hasAlpha();
    m_size = aSize;

    Uint32 rmask = 0x00ff0000;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x000000ff;
    Uint32 amask = 0xff000000;

    m_surface = SDL_CreateRGBSurfaceFrom((void *)aBuffer.bytes().data(), aBuffer.width(), aBuffer.height(), 32,
            aBuffer.width()*4, rmask, gmask, bmask, amask);

    return (m_cairoSurface != NULL);
}

bool BCNativeImage::Create(const IntSize& size)
{
    ASSERT( m_cairoSurface == NULL );
    m_cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size.width(), size.height());
    m_hasAlpha = true;
    m_size = size;

    int stride = size.width() * 4;

    unsigned char *data = (unsigned char *) calloc(stride * size.height(), 1);

    m_cairoSurface = cairo_image_surface_create_for_data(
            data, CAIRO_FORMAT_ARGB32, size.width(), size.height(), stride);

    Uint32 rmask = 0x00ff0000;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x000000ff;
    Uint32 amask = 0xff000000;

    m_surface = SDL_CreateRGBSurfaceFrom((void *)data, size.width(), size.height(), 32,
            stride, rmask, gmask, bmask, amask);

    return (m_cairoSurface != NULL);
}

BCNativeImage::~BCNativeImage()
{
  if( m_cairoSurface )
  {
    cairo_surface_destroy(m_cairoSurface);
  }
}

bool BCNativeImage::hasAlpha()
{
  return m_hasAlpha;
}

}
