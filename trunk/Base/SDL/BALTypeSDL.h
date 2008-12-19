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

#ifndef BALType_h
#define BALType_h

struct SDL_Surface;
struct SDL_Color;
struct SDL_Cursor;
struct FT_FaceRec_;
struct _FcPattern;

namespace WKAL {
    class FloatSize;
}

typedef FT_FaceRec_ BalFontFace;
typedef void BalFont;
typedef struct _FcPattern BalPattern;
typedef void BalScaledFont;
typedef void BalDrawable;
typedef SDL_Surface BalSurface;
typedef void BalMenuItem;
typedef SDL_Surface BalMenu;
typedef void BalClipboard;
typedef void BalTargetList;
typedef void BalAdjustment;
typedef void BalContainer;
typedef void BalPixbuf;
typedef SDL_Color BalColor;
typedef struct _BalMatrix{} BalMatrix;


typedef SDL_Surface PlatformGraphicsContext;
typedef BalWidget* PlatformWidget;
typedef void* PlatformPatternPtr;

namespace WKAL {
    typedef void* PlatformGradient;
    typedef void* PlatformPath;
    typedef SDL_Cursor* PlatformCursor;
    typedef void* DragImageRef;
    typedef void* DragDataRef;
    typedef unsigned short GlyphBufferGlyph;
    typedef WKAL::FloatSize GlyphBufferAdvance;
}

#endif
