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

#include "BCNativeImageSDL.h"
#include "BTRGBA32Buffer.h"

namespace BAL {

// Native Image
BCNativeImage::BCNativeImage(SDL_Surface* surface) 
    : m_surface(surface)
    , m_ownArray(NULL)
    , m_hasAlpha(true)
{
    m_size = IntSize(surface->w, surface->h);
}
BCNativeImage::BCNativeImage(SDL_Surface* surface, RGBA32Array* array) 
    : m_surface(surface)
    , m_ownArray(array)
    , m_hasAlpha(true)
{
    m_size = IntSize(surface->w, surface->h);
}

IntSize BCNativeImage::size() const
{
    return m_size;
}


BCNativeImage::~BCNativeImage()
{
    if (m_surface)
        SDL_FreeSurface(m_surface);
    if (m_ownArray)
        delete m_ownArray;
}

bool BCNativeImage::hasAlpha()
{
    return m_hasAlpha;
}

}
