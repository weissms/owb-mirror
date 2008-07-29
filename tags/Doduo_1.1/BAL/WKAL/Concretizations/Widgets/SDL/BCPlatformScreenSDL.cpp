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


#include "config.h"
#include "PlatformScreen.h"

#include "Widget.h"
#include "SDL.h"

namespace WKAL {

int screenDepth(Widget* widget)
{
    ASSERT(widget->containingWindow());
#if PLATFORM(AMIGAOS4)
    SDL_Surface *surface = widget->containingWindow()->surface;
    return surface ? surface->format->BitsPerPixel : 32;
#else
    return widget->containingWindow()->format->BitsPerPixel;
#endif
}

int screenDepthPerComponent(Widget*)
{
    NotImplemented();
    return 8;
}

bool screenIsMonochrome(Widget* widget)
{
    return screenDepth(widget) < 2;
}

FloatRect screenRect(Widget* widget)
{
    ASSERT(widget->containingWindow());
#if PLATFORM(AMIGAOS4)
    SDL_Rect sdlRect = widget->containingWindow()->surface->clip_rect;
#else
    SDL_Rect sdlRect = widget->containingWindow()->clip_rect;
#endif
    return FloatRect(sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
}

FloatRect screenAvailableRect(Widget*)
{
    NotImplemented();
    return FloatRect();
}

} // namespace WebCore
