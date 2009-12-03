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

#ifndef ApplyTransparency_h
#define ApplyTransparency_h

#include "config.h"

#include <SDL.h>

namespace WebCore {

static SDL_Surface* applyTransparency(SDL_Surface* origin, const uint8_t alphaChannel)
{
     // blend is not very optimized for now, to say the least
    SDL_Surface *final = NULL;
    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
    on the endianness (byte order) of the machine */
#if !PLATFORM(AMIGAOS4) && SDL_BYTEORDER == SDL_BIG_ENDIAN
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
    uint16_t width = origin->w;
    uint16_t height = origin->h;
    void *data = malloc(sizeof(uint32_t) * height * width);
    SDL_LockSurface(origin);
    memcpy(data, origin->pixels, sizeof(uint32_t) * height * width);
    final = SDL_CreateRGBSurfaceFrom(data, origin->w, origin->h, 32, origin->pitch, rmask, gmask, bmask, amask);
    SDL_UnlockSurface(origin);
    SDL_LockSurface(final);
    uint32_t alpha = 0;
    for (uint16_t i = 0; i < height; i++) {
        for (uint16_t j = 0; j < width; j++) {
            alpha = ((*(uint32_t*)final->pixels >> 24 ) * alphaChannel) >> 8;;
            *(uint32_t*)final->pixels = (*(uint32_t*)final->pixels & 0x00ffffff) | (alpha << 24);
            final->pixels = (uint32_t*)final->pixels + 1;
        }
    }
    final->pixels = data;
    SDL_UnlockSurface(final);
    return final;
}

} // namespace WebCore

#endif // ApplyTransparency_h
