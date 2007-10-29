/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "BALConfiguration.h"
#include "Cursor.h"

#include "DeprecatedString.h"
#include <wtf/Assertions.h>

// include for cursor files
#include "handCursor.h"
#include "iBeamCursor.h"
#include "moveCursor.h"
#include "pointerCursor.h"

//FIXME: do a real and clean implementation
//FIXME: change namespace WebCore to namespace BAL
namespace WebCore {

Cursor::Cursor(const Cursor& other)
    : m_impl(other.m_impl)
{
    if (m_impl)
        SDL_SetCursor(m_impl);
    else
        SDL_SetCursor(NULL);
}

Cursor::Cursor(Image*, const IntPoint&)
{
    BALNotImplemented();
}

Cursor::Cursor(PlatformCursor c)
    : m_impl(c)
{
    m_impl = c;
    ASSERT(c);
    SDL_SetCursor(c);
}

Cursor::Cursor(char* xpmCursor[])
{
    int width = 16, height = 24;
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
    BALNotImplemented();
    return *this;
}

const Cursor& pointerCursor()
{
    static Cursor c(m_pointerCursor);
    SDL_SetCursor(c.impl());
    return c;
}

const Cursor& crossCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& handCursor()
{
    static Cursor c(m_handCursor);
    SDL_SetCursor(c.impl());
    return c;
}

const Cursor& moveCursor()
{
    static Cursor c(m_moveCursor);
    SDL_SetCursor(c.impl());
    return c;
}

const Cursor& iBeamCursor()
{
    static Cursor c(m_iBeamCursor);
    SDL_SetCursor(c.impl());
    return c;
}

const Cursor& waitCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& helpCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& eastResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northEastResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northWestResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& southResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& southEastResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& southWestResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& westResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northSouthResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& eastWestResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northEastSouthWestResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& northWestSouthEastResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& columnResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

const Cursor& rowResizeCursor()
{
    BALNotImplemented();
    static Cursor c = static_cast<const Cursor&> (SDL_GetCursor());
    return c;
}

// FIXME: should find better GDK cursors for those than a generic GDK_LEFT_PTR
const Cursor& verticalTextCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

const Cursor& cellCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

const Cursor& contextMenuCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

const Cursor& noDropCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

const Cursor& copyCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

const Cursor& progressCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

const Cursor& aliasCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

const Cursor& noneCursor()
{
    BALNotImplemented();
    //FIXME: use SDL_ShowCursor(SDL_DISABLE) ?
    return pointerCursor();
}

const Cursor& notAllowedCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

const Cursor& zoomInCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

const Cursor& zoomOutCursor()
{
    BALNotImplemented();
    return pointerCursor();
}

}
