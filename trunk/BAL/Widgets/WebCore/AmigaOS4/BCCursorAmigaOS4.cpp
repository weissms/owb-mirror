/*
 * Copyright (C) 2008 Joerg Strohmayer.
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

#include <wtf/Assertions.h>
#include <cstdio>
#include "BALBase.h"
#include "Cursor.h"
#include <string.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <intuition/pointerclass.h>
#include <workbench/icon.h>

namespace WKAL {

Cursor::Cursor(Image*, const IntPoint&)
    : m_impl(0)
    , m_dispose(false)
    , m_data(0)
{
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    NotImplemented();
}

Cursor::Cursor(PlatformCursor c)
    : m_impl(c)
    , m_dispose(false)
    , m_data(0)
{
    fprintf(stderr, "%s (%p)\n", __PRETTY_FUNCTION__, c);
}

Cursor::Cursor(const char* name)
    : m_impl(0)
    , m_dispose(true)
    , m_data(0)
{
    DiskObject *icon = IIcon->GetIconTags(name, ICONGETA_UseFriendBitMap, TRUE, TAG_DONE);
    if (icon) {
        uint32 format = IDFMT_BITMAPPED;
        if (1 == IIcon->IconControl(icon, ICONCTRLA_GetImageDataFormat, &format, TAG_DONE))
            if (IDFMT_DIRECTMAPPED == format) {
                int32 width = 0, height = 0;
                uint8* data = 0;
                IIcon->IconControl(icon,
                                   ICONCTRLA_GetWidth, &width,
                                   ICONCTRLA_GetHeight, &height,
                                   ICONCTRLA_GetImageData1, &data,
                                   TAG_DONE);
                if (width > 0 && width <= 64 && height > 0 && height <= 64 && data) {
                    int32 xoffset = 0, yoffset = 0;
                    STRPTR tooltype;

                    tooltype = IIcon->FindToolType(icon->do_ToolTypes, "XOFFSET");
                    if (tooltype)
                        xoffset = atoi(tooltype);
                    tooltype = IIcon->FindToolType(icon->do_ToolTypes, "YOFFSET");
                    if (tooltype)
                        yoffset = atoi(tooltype);

                    if (xoffset < 0 || xoffset >= width)
                        xoffset = 0;
                    if (yoffset < 0 || yoffset >= height)
                        yoffset = 0;

                    m_data = calloc(width * height * 4 + 32 / 8 * 64 + sizeof(BitMap), 1);
                    if (m_data) {
                        memcpy(m_data, data, width * height * 4);

                        PLANEPTR singleColourPlane = (PLANEPTR)((uint32)m_data + width * height * 4);
                        BitMap* singleColourBitMap = (BitMap*)((uint32)m_data + width * height * 4 + 32 / 8 * 64);
                        singleColourBitMap->BytesPerRow = 32 / 8;
                        singleColourBitMap->Rows = height;
                        singleColourBitMap->Depth = 1;
                        singleColourBitMap->Planes[0] /*= singleColourBitMap->Planes[1]*/ = singleColourPlane;

                        uint32* data32 = (uint32*)data;
                        for (int y = 0 ; y < height ; y++)
                            for (int x = 0 ; x < width && x < 32 ; x++)
                                if (0xFF000000 == (data32[y * width + x] & 0xFF000000))
                                    singleColourPlane[y * 32 / 8 + x / 8] |= 1 << (7 - (x & 7));

                        m_impl = IIntuition->NewObject(NULL, POINTERCLASS,
                                                       POINTERA_BitMap, singleColourBitMap,
                                                       POINTERA_XOffset, -xoffset,
                                                       POINTERA_YOffset, -yoffset,
                                                       POINTERA_WordWidth, 32 / 16,
                                                       POINTERA_XResolution, POINTERXRESN_SCREENRES,
                                                       POINTERA_YResolution, POINTERYRESN_SCREENRESASPECT,
                                                       POINTERA_Dummy + 0x07, m_data,
                                                       POINTERA_Dummy + 0x08, width,
                                                       POINTERA_Dummy + 0x09, height,
                                                       TAG_DONE);
                    }
                }
            }

        IIcon->FreeDiskObject(icon);
    }
}

Cursor::~Cursor()
{
    if (m_dispose) {
        IIntuition->DisposeObject(m_impl);
        free(m_data);
    }
}

Cursor& Cursor::operator=(const Cursor& other)
{
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    m_impl = other.m_impl;
    return *this;
}

const Cursor& pointerCursor()
{
    static Cursor c("PROGDIR:Resources/pointerCursor");
    return c;
}

const Cursor& crossCursor()
{
    static Cursor c("PROGDIR:Resources/crossCursor");
    return c;
}

const Cursor& handCursor()
{
    static Cursor c("PROGDIR:Resources/handCursor");
    return c;
}

const Cursor& moveCursor()
{
    static Cursor c("PROGDIR:Resources/moveCursor");
    return c;
}

const Cursor& iBeamCursor()
{
    static Cursor c("PROGDIR:Resources/iBeamCursor");
    return c;
}

const Cursor& waitCursor()
{
    static Cursor c("PROGDIR:Resources/waitCursor");
    return c;
}

const Cursor& helpCursor()
{
    static Cursor c("PROGDIR:Resources/helpCursor");
    return c;
}

const Cursor& eastResizeCursor()
{
    static Cursor c("PROGDIR:Resources/eastResizeCursor");
    return c;
}

const Cursor& northResizeCursor()
{
    static Cursor c("PROGDIR:Resources/northResizeCursor");
    return c;
}

const Cursor& northEastResizeCursor()
{
    static Cursor c("PROGDIR:Resources/northEastResizeCursor");
    return c;
}

const Cursor& northWestResizeCursor()
{
    static Cursor c("PROGDIR:Resources/northWestResizeCursor");
    return c;
}

const Cursor& southResizeCursor()
{
    static Cursor c("PROGDIR:Resources/southResizeCursor");
    return c;
}

const Cursor& southEastResizeCursor()
{
    static Cursor c("PROGDIR:Resources/southEastResizeCursor");
    return c;
}

const Cursor& southWestResizeCursor()
{
    static Cursor c("PROGDIR:Resources/southWestResizeCursor");
    return c;
}

const Cursor& westResizeCursor()
{
    static Cursor c("PROGDIR:Resources/westResizeCursor");
    return c;
}

const Cursor& northSouthResizeCursor()
{
    static Cursor c("PROGDIR:Resources/northSouthResizeCursor");
    return c;
}

const Cursor& eastWestResizeCursor()
{
    static Cursor c("PROGDIR:Resources/eastWestResizeCursor");
    return c;
}

const Cursor& northEastSouthWestResizeCursor()
{
    static Cursor c("PROGDIR:Resources/northEastSouthWestResizeCursor");
    return c;
}

const Cursor& northWestSouthEastResizeCursor()
{
    static Cursor c("PROGDIR:Resources/northWestSouthEastResizeCursor");
    return c;
}
const Cursor& columnResizeCursor()
{
    static Cursor c("PROGDIR:Resources/columnResizeCursor");
    return c;
}

const Cursor& rowResizeCursor()
{
    static Cursor c("PROGDIR:Resources/rowResizeCursor");
    return c;
}

const Cursor& middlePanningCursor()
{
    static Cursor c("PROGDIR:Resources/middlePanningCursor"); // missing
    return c;
}

const Cursor& eastPanningCursor()
{
    static Cursor c("PROGDIR:Resources/eastPanningCursor"); // missing
    return c;
}

const Cursor& northPanningCursor()
{
    static Cursor c("PROGDIR:Resources/northPanningCursor"); // missing
    return c;
}

const Cursor& northEastPanningCursor()
{
    static Cursor c("PROGDIR:Resources/northEastPanningCursor"); // missing
    return c;
}

const Cursor& northWestPanningCursor()
{
    static Cursor c("PROGDIR:Resources/northWestPanningCursor"); // missing
    return c;
}

const Cursor& southPanningCursor()
{
    static Cursor c("PROGDIR:Resources/southPanningCursor"); // missing
    return c;
}

const Cursor& southEastPanningCursor()
{
    static Cursor c("PROGDIR:Resources/southEastPanningCursor"); // missing
    return c;
}

const Cursor& southWestPanningCursor()
{
    static Cursor c("PROGDIR:Resources/southWestPanningCursor"); // missing
    return c;
}

const Cursor& westPanningCursor()
{
    static Cursor c("PROGDIR:Resources/westPanningCursor"); // missing
    return c;
}

const Cursor& verticalTextCursor()
{
    static Cursor c("PROGDIR:Resources/verticalTextCursor");
    return c;
}

const Cursor& cellCursor()
{
    static Cursor c("PROGDIR:Resources/cellCursor");
    return c;
}

const Cursor& contextMenuCursor()
{
    static Cursor c("PROGDIR:Resources/contextMenuCursor");
    return c;
}

const Cursor& noDropCursor()
{
    static Cursor c("PROGDIR:Resources/noDropCursor");
    return c;
}

const Cursor& copyCursor()
{
    static Cursor c("PROGDIR:Resources/copyCursor");
    return c;
}

const Cursor& progressCursor()
{
    static Cursor c("PROGDIR:Resources/progressCursor");
    return c;
}

const Cursor& aliasCursor()
{
    static Cursor c("PROGDIR:Resources/aliasCursor");
    return c;
}

const Cursor& noneCursor()
{
    static Cursor c("PROGDIR:Resources/noneCursor");
    return c;
}

const Cursor& notAllowedCursor()
{
    static Cursor c("PROGDIR:Resources/notAllowedCursor");
    return c;
}

const Cursor& zoomInCursor()
{
    static Cursor c("PROGDIR:Resources/zoomInCursor");
    return c;
}

const Cursor& zoomOutCursor()
{
    static Cursor c("PROGDIR:Resources/zoomOutCursor");
    return c;
}

}
