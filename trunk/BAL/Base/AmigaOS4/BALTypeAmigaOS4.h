/*
 * Copyright (C) 2008 Joerg Strohmayer
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

struct _cairo;
struct _cairo_surface;
struct _cairo_pattern;
struct SDL_Color;
struct Window;
struct Gadget;
struct OutlineFont;
struct Hook;
struct AppIcon;
struct BitMap;
class WebView;

namespace WebCore {
    struct CairoPath;
}

namespace WKAL {
    class FloatSize;
}

struct AmigaOWBWindow
{
    AmigaOWBWindow* next;
    Window* window;
    WebView* webView;
    int offsetx, offsety;
    int left, top, width, height;
    int webViewWidth, webViewHeight;
    _cairo* cr;
    _cairo_surface* surface;
    void *img_back, *img_forward, *img_stop,
         *img_search, *img_home, *img_reload,
         *img_iconify;
    Gadget *gad_toolbar, *gad_vbar, *gad_hbar,
           *gad_url, *gad_fuelgauge, *gad_stop,
           *gad_back, *gad_forward, *gad_iconify,
           *gad_search,  *gad_status, *gad_webview,
           *gad_page, *gad_hlayout;
    Hook* backfill_hook;
    char title[256];
    char url[2000];
    char search[500];
    char statusBarText[256];
    char toolTipText[256];
    char statusToolTipText[512];
    AppIcon* appicon;
    void* curentCursor;
    unsigned int fuelGaugeArgs[2];
    bool expose;
};

struct AmigaOWBResizeEvent
{
    int w, h;
};

struct AmigaConfig
{
    const char *homeURL;
    const char *searchURL;
    int minFontSize;
    int fontXDPI, fontYDPI;
    const char *unicodeFontName;
    OutlineFont *unicodeFace;
    int windowleft, windowtop, windowwidth, windowheight;
    bool confirmQuit;
};
extern struct AmigaConfig amigaConfig;

typedef int BalEventExpose;
typedef struct AmigaOWBResizeEvent BalResizeEvent;
typedef int BalQuitEvent;
typedef int BalUserEvent;
typedef struct IntuiMessage BalEventKey;
typedef struct IntuiMessage BalEventButton;
typedef struct IntuiMessage BalEventMotion;
typedef struct IntuiMessage BalEventScroll;
typedef struct OutlineFont BalFontFace;
typedef void BalFont;
typedef void BalPattern;
typedef void BalScaledFont;
typedef void BalDrawable;
typedef _cairo_surface BalSurface;
typedef struct _BalPoint{} BalPoint;
typedef void BalMenuItem;
typedef _cairo_surface BalMenu;
typedef void BalClipboard;
typedef void BalTargetList;
typedef void BalAdjustment;
typedef AmigaOWBWindow BalWidget;
typedef void BalContainer;
typedef void BalPixbuf;
typedef SDL_Color BalColor;
typedef struct _BalMatrix{} BalMatrix;


typedef _cairo PlatformGraphicsContext;
typedef BalWidget* PlatformWidget;
typedef _cairo_pattern* PlatformPatternPtr;

namespace WKAL {
    typedef _cairo_pattern *PlatformGradient;
    typedef WebCore::CairoPath PlatformPath;
    typedef void* PlatformCursor;
    typedef void* DragImageRef;
    typedef void* DragDataRef;
    typedef unsigned short GlyphBufferGlyph;
    typedef WKAL::FloatSize GlyphBufferAdvance;
}

#endif
