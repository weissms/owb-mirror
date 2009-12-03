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
#include "CursorSDL.h"

#include "BALBase.h"
#include "Logging.h"
#include "NotImplemented.h"
#include <wtf/Assertions.h>

#include "stdio.h"
// include for cursor files
#include "handCursor.h"
#include "iBeamCursor.h"
#include "moveCursor.h"
#include "pointerCursor.h"
#include "SDL.h"

namespace WebCore {

Cursor::Cursor(const Cursor& other)
    : m_impl(other.m_impl)
{
    if (m_impl) {
        if (SDL_GetCursor() != m_impl)
            SDL_SetCursor(m_impl);
    } else
        SDL_SetCursor(NULL);
}

Cursor::Cursor(Image*, const IntPoint&)
{
    balNotImplemented();
}

Cursor::Cursor(const char* xpmCursor[])
{
    const int width = 16, height = 24;
    int i, row, col;
    Uint8 data[4 * height];
    Uint8 mask[4 * height];
    int hot_x, hot_y;

    i = -1;
    for (row = 0; row < height; ++row) {
        for (col = 0; col < width; ++col) {
            if (col % 8) {
                data[i] <<= 1;
                mask[i] <<= 1;
            } else {
                ++i;
                data[i] = mask[i] = 0;
            }
            switch (xpmCursor[4+row][col]) {
                case 'X':
                    data[i] |= 0x01;
                    mask[i] |= 0x01;
                    break;
                case '.':
                    mask[i] |= 0x01;
                    break;
                case ' ':
                    break;
            }
        }
    }
    sscanf(xpmCursor[4+row], "%d,%d", &hot_x, &hot_y);
    m_impl = SDL_CreateCursor(data, mask, width, height, hot_x, hot_y);
}


Cursor::~Cursor()
{
}

Cursor& Cursor::operator=(const Cursor& other)
{
    m_impl = other.m_impl;
    return *this;
}

Cursor::Cursor(PlatformCursor c)
    : m_impl(c)
{
    m_impl = c;
    ASSERT(c);
    SDL_SetCursor(c);
}

const Cursor& pointerCursor()
{
    static Cursor c(m_pointerCursor);
    return c;
}

const Cursor& crossCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& handCursor()
{
    static Cursor c(m_handCursor);
    return c;
}

const Cursor& moveCursor()
{
    static Cursor c(m_moveCursor);
    return c;
}

const Cursor& iBeamCursor()
{
    static Cursor c(m_iBeamCursor);
    return c;
}

const Cursor& waitCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& helpCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& eastResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northEastResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northWestResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& southResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& southEastResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& southWestResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& westResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northSouthResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& eastWestResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northEastSouthWestResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northWestSouthEastResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}
const Cursor& columnResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& rowResizeCursor()
{
    balNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& middlePanningCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& eastPanningCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& northPanningCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& northEastPanningCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& northWestPanningCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& southPanningCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& southEastPanningCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& southWestPanningCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& westPanningCursor()
{
    balNotImplemented();
    return pointerCursor();
}

// FIXME: should find better GDK cursors for those than a generic GDK_LEFT_PTR
const Cursor& verticalTextCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& cellCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& contextMenuCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& noDropCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& copyCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& progressCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& aliasCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& noneCursor()
{
    balNotImplemented();
    //FIXME: use SDL_ShowCursor(SDL_DISABLE) ?
    return pointerCursor();
}

const Cursor& notAllowedCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& zoomInCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& zoomOutCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& grabCursor()
{
    balNotImplemented();
    return pointerCursor();
}

const Cursor& grabbingCursor()
{
    balNotImplemented();
    return pointerCursor();
}


}
