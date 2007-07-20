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
 * @file  BCGraphicsDeviceSDL.h
 *
 * Header file for BCGraphicsDeviceSDL.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#include "config.h"
#include "BALConfiguration.h"
#include "BCGraphicsDeviceSDL.h"
#include "BCNativeImageSDL.h"
#include "Color.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include "BTLogHelper.h"
#include "BTWidget.h"

using WebCore::Image;

IMPLEMENT_GET_DELETE(BIGraphicsDevice, BCGraphicsDeviceSDL);

#define GET_NATIVE(image) ((static_cast<const BCNativeImage*>(image))->getNativeSDL())

namespace BAL {

BCGraphicsDeviceSDL::BCGraphicsDeviceSDL() : m_screen(0)
{
}

void BCGraphicsDeviceSDL::initialize(uint16_t width, uint16_t height, uint8_t depth)
{
#ifndef NDEBUG
    if (getenv("DISABLE_DISPLAY")) {
        m_layoutTests = true;
        return;
    }
    else
        m_layoutTests = false;
#endif
    const SDL_VideoInfo* vi;

    vi = SDL_GetVideoInfo();
    if(vi && vi->wm_available) /* Change les titres */
        SDL_WM_SetCaption("Origyn Web Browser", "Origyn Web Browser");

    /* Initialize only once with main frame size */
    if (!m_screen)
        m_screen = SDL_SetVideoMode(width, height, depth, 0/*SDL_HWSURFACE|SDL_NOFRAME|SDL_SRCALPHA*/);

    if (!m_screen) {
        logml(MODULE_GRAPHICS, LEVEL_CRITICAL, make_message("SDL_SetVideoMode failed: %s", SDL_GetError()));
        exit(1);
    } else
        logm(MODULE_GRAPHICS, "SDL_SetVideoMode succeeded");
}

void BCGraphicsDeviceSDL::initialize(const BAL::BISurface&)
{
}

void BCGraphicsDeviceSDL::finalize()
{
}
    
BINativeImage* BCGraphicsDeviceSDL::createNativeImage(const IntSize size)
{
    SDL_Surface* surface;
    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
    surface = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, size.width(), size.height(), 32,
                                  rmask, gmask, bmask, amask);

    return new BCNativeImage(surface);
}

BINativeImage* BCGraphicsDeviceSDL::createNativeImage(RGBA32Array& array, const WebCore::IntSize size)
{
    SDL_Surface* surface;
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

    surface = SDL_CreateRGBSurfaceFrom((void*)array.data(),
        size.width(),
        size.height(),
        32, size.width()*4, rmask, gmask, bmask, amask);
//     SDL_SetAlpha(surface, SDL_SRCALPHA, SDL_ALPHA_TRANSPARENT);
        
    return new BCNativeImage(surface, &array);
}

void BCGraphicsDeviceSDL::copy(const BTWidget& widget, const BINativeImage& image, IntRect zone, IntPoint point, const uint8_t alphaChannel)
{
    if (!widget.backingStore())
        return;
    SDL_Rect sdlSrc, sdlDest;
    sdlDest.x = point.x();
    sdlDest.y = point.y();
    sdlSrc.w = zone.width();
    sdlSrc.h = zone.height();
    sdlSrc.x = zone.x();
    sdlSrc.y = zone.y();

    if (alphaChannel != 255) {
        SDL_Surface *surfaceWithAlpha = applyTransparency(image, alphaChannel);
        SDL_BlitSurface(surfaceWithAlpha, &sdlSrc, GET_NATIVE(widget.backingStore()), &sdlDest);
        free(surfaceWithAlpha->pixels);
        SDL_FreeSurface(surfaceWithAlpha);
    } else {
        // now blit the updated rect
        SDL_BlitSurface(GET_NATIVE(&image), &sdlSrc,
                        GET_NATIVE(widget.backingStore()), &sdlDest);
    }
}

void BCGraphicsDeviceSDL::fillRect(const BTWidget& widget, IntRect rect, const WebCore::Color color)
{
    if (!widget.backingStore())
        return;

    boxRGBA(GET_NATIVE(widget.backingStore()), static_cast<Sint16>(rect.x()), 
                    static_cast<Sint16>(rect.y()),
                    static_cast<Sint16>(rect.x() + rect.width()), 
                    static_cast<Sint16>(rect.y() + rect.height()),
                    color.red(), color.green(), color.blue(), color.alpha());
}

//TODO: draw points instead of lines for nicer circles
inline void BCGraphicsDeviceSDL::drawArc(const BTWidget& widget, const WebCore::Color color, int zone, int xc, int yc, float& x0, float& y0, float x1, float y1, bool doSwap)
{
    // Mean First draw => will not draw just a point.
    if (x0 == x1)
        return;

    switch(zone) {
        case 0:
            lineRGBA(GET_NATIVE(widget.backingStore()),
                     static_cast<int> (xc - ceilf(x0)), static_cast<int> (yc - ceilf(y0)),
                     static_cast<int> (xc - ceilf(x1)), static_cast<int> (yc - ceilf(y1)),
                     color.red(),
                     color.green(),
                     color.blue(),
                     color.alpha());

            break;
        case 1:
            lineRGBA(GET_NATIVE(widget.backingStore()),
                     static_cast<int> (ceilf(y0) + xc), static_cast<int> (yc - ceilf(x0)),
                     static_cast<int> (ceilf(y1) + xc), static_cast<int> (yc - ceilf(x1)),
                     color.red(),
                     color.green(),
                     color.blue(),
                     color.alpha());
            break;
        case 2:
            lineRGBA(GET_NATIVE(widget.backingStore()),
                     static_cast<int> (ceilf(x0) + xc), static_cast<int> (ceilf(y0) + yc),
                     static_cast<int> (ceilf(x1) + xc), static_cast<int> (ceilf(y1) + yc),
                     color.red(),
                     color.green(),
                     color.blue(),
                     color.alpha());
            break;
        case 3:
            lineRGBA(GET_NATIVE(widget.backingStore()),
                     static_cast<int> (xc - ceilf(y0)), static_cast<int> (ceilf(x0) + yc),
                     static_cast<int> (xc - ceilf(y1)), static_cast<int> (ceilf(x1) + yc),
                     color.red(),
                     color.green(),
                     color.blue(),
                     color.alpha());
            break;
    }

    if (doSwap) {
        x0 = x1;
        y0 = y1;
    }
}

void BCGraphicsDeviceSDL::drawArc(const BTWidget& widget, const IntRect rect, uint16_t startAngle, uint16_t angleSpan, const WebCore::Color color)
{
    //
    //        |y          (This diagram is supposed to be a circle).
    //        |
    //       ...
    // z=1 .  |  . z=0      We know that there are 360 degrees in a circle. First we
    //   .    |    .        see that a circle is symetrical about the x axis, so
    //  .     |     .       only the first 180 degrees need to be calculated. Next
    //--.-----+-----.--     we see that it's also symetrical about the y axis, so now
    //  .     |     . x     we only need to calculate the first 90 degrees.
    //   .    |    .        Each 90° region is associated to a zone (z value). Zone where
    // z=2 .  |  . z=3      we will draw is defined from startAngle and angleSpan.
    //       ...            Thus after coord computation, we draw in the zone(s).
    //        |             Note: 0 <= alpha0, alpha1 <= 90
    //        |

    int r = rect.width() / 2;
    int xc = rect.x() + r;
    int yc = rect.y() + rect.height() / 2;
    int z0 = startAngle / 90;
    int z1 = (startAngle + angleSpan) / 90;
    int alpha0 = startAngle % 90;
    float xalpha0 = r * cosf((alpha0 * M_PI / 180));
    int alpha1 = (startAngle + angleSpan) % 90;
    float xalpha1 = r * cosf((alpha1 * M_PI / 180));

    float x0, y0, x1, y1;

    if (z0 == z1) {
        // Draw in the same zone from angle = z0 * 90 + alpha0 to angle = z0 * 90 + alpha1
        x0 = xalpha0;
        y0 = sqrt(pow(r, 2) - pow(x0, 2));
        for (x1 = xalpha0; x1 >= xalpha1; x1--) {
            y1 = sqrt(pow(r, 2) - pow(x1, 2));
            drawArc(widget, color, z0, xc, yc, x0, y0, x1, y1);
        }
    } else if ((z1 - z0) == 1) {
        // Begin to draw in zone Z and end in zone Z + 1
        if (alpha1 < alpha0) {
            // Draw from angle = z1 * 90 to angle = z1 * 90 + alpha1
            // Then from angle = z0 * 90 + alpha0 to angle = z1 * 90
            x0 = r;
            y0 = 0;
            for (x1 = r; x1 >= xalpha1; x1--) {
                y1 = sqrt(pow(r, 2) - pow(x1, 2));
                drawArc(widget, color, z1, xc, yc, x0, y0, x1, y1);
            }
            x0 = xalpha0;
            y0 = sqrt(pow(r, 2) - pow(x0, 2));
            for (x1 = xalpha0; x1 >= 0; x1--) {
                y1 = sqrt(pow(r, 2) - pow(x1, 2));
                drawArc(widget, color, z0, xc, yc, x0, y0, x1, y1);
            }
        } else {
            // Compute a complete quarter of circle.
            // Draw in zone1 from 0 to alpha0
            // Draw in zone0 and zone1 from alpha0 to alpha1
            // Draw in zone0 from alpha1 to 90
            x0 = r;
            y0 = 0;
            for (x1 = r; x1 >= 0; x1--) {
                y1 = sqrt(pow(r, 2) - pow(x1, 2));
                if (x1 < xalpha1)
                    drawArc(widget, color, z0, xc, yc, x0, y0, x1, y1);
                else if (x1 > xalpha0)
                    drawArc(widget, color, z1, xc, yc, x0, y0, x1, y1);
                else {
                    drawArc(widget, color, z0, xc, yc, x0, y0, x1, y1, false);
                    drawArc(widget, color, z1, xc, yc, x0, y0, x1, y1);
                }
            }
        }
    } else {
        // Draw at least a complete quarter of circle and probably many more
        x0 = r;
        y0 = 0;
        for (x1 = r; x1 >= 0; x1--) {
            y1 = sqrt(pow(r, 2) - pow(x1, 2));
            if ((z1 - z0) >= 3)
                drawArc(widget, color, z1 - 2, xc, yc, x0, y0, x1, y1, false);
            if (x1 < xalpha1)
                drawArc(widget, color, z1 % 3, xc, yc, x0, y0, x1, y1, false);
            if (x1 < xalpha0)
                drawArc(widget, color, z0, xc, yc, x0, y0, x1, y1, false);
            drawArc(widget, color, z1 - 1, xc, yc, x0, y0, x1, y1);
        }
    }
}

void BCGraphicsDeviceSDL::drawLine(const BTWidget& widget, IntPoint p1, IntPoint p2, const WebCore::Color color)
{
    if (!widget.backingStore())
        return;

    lineRGBA(GET_NATIVE(widget.backingStore()),
             static_cast<Sint16>(p1.x()), static_cast<Sint16>(p1.y()),
             static_cast<Sint16>(p2.x()), static_cast<Sint16>(p2.y()),
             color.red(),
             color.green(),
             color.blue(),
             color.alpha());
}

void BCGraphicsDeviceSDL::drawEllipse(const BTWidget& widget, WebCore::IntRect rect, const WebCore::Color color)
{
    float yRadius = .5 * rect.height();
    float xRadius = .5 * rect.width();
    ellipseRGBA(GET_NATIVE(widget.backingStore()), static_cast<Sint16>(rect.x() + xRadius), 
                static_cast<Sint16>(rect.y() + yRadius),
                static_cast<Sint16>(xRadius), 
                static_cast<Sint16>(yRadius),
                color.red(),
                color.green(),
                color.blue(),
                color.alpha());
}

void BCGraphicsDeviceSDL::drawRect(const BTWidget& widget, IntRect rect, const WebCore::Color color)
{
    if (!widget.backingStore())
        return;
    FloatRect r(rect);
    r.inflate(-.5f);

    rectangleRGBA(GET_NATIVE(widget.backingStore()),
                    static_cast<Sint16>(r.x()), static_cast<Sint16>(r.y()),
                    static_cast<Sint16>(r.x() + r.width()), static_cast<Sint16>(r.y() + r.height()),
                    color.red(), color.green(), color.blue(), color.alpha());
}

void BCGraphicsDeviceSDL::stretchBlit(const BTWidget& widget, const BINativeImage& nativeImage, IntRect src, IntRect dst, const uint8_t alphaChannel)
{
    SDL_Rect srcRect, dstRect;
    srcRect.x = static_cast<Sint16>(src.x());
    srcRect.y = static_cast<Sint16>(src.y());
    if (0 == src.width())
        srcRect.w = nativeImage.size().width();
    else
        srcRect.w = static_cast<Uint16>(src.width());
    if (0 == src.height())
        srcRect.h = nativeImage.size().height();
    else
        srcRect.h = static_cast<Uint16>(src.height());
    dstRect.x = static_cast<Sint16>(dst.x());
    dstRect.y = static_cast<Sint16>(dst.y());
    dstRect.w = static_cast<Sint16>(dst.width());
    dstRect.h = static_cast<Sint16>(dst.height());

    SDL_Surface *surface = NULL;
    if (alphaChannel != 255) {
        SDL_Surface *surfaceWithAlpha = applyTransparency(nativeImage, alphaChannel);
        surface = zoomSurface(surfaceWithAlpha,
                            (((double)dst.width()/(double)srcRect.w)),
                            ((double)dst.height()/((double)srcRect.h)),
                            SMOOTHING_OFF);
        free(surfaceWithAlpha->pixels);
        SDL_FreeSurface(surfaceWithAlpha);
    } else {
    	surface = zoomSurface(GET_NATIVE(&nativeImage),
                            (((double)dst.width()/(double)srcRect.w)),
                            ((double)dst.height()/((double)srcRect.h)),
                             SMOOTHING_OFF);
    }
    srcRect.w = static_cast<Uint16>(dst.width());
    srcRect.h = static_cast<Uint16>(dst.height());
    srcRect.x = 0;
    srcRect.y = 0;

    SDL_BlitSurface(surface, &srcRect, GET_NATIVE(widget.backingStore()), &dstRect);
    SDL_FreeSurface(surface);
}

void BCGraphicsDeviceSDL::fillConvexPolygon(const BTWidget& widget, size_t npoints, const WebCore::IntPoint* points, const WebCore::Color color)
{
    if (!widget.backingStore())
        return;
    if (npoints <= 1)
        return;
    Sint16 *vx = new Sint16[npoints];
    Sint16 *vy = new Sint16[npoints];
    for(unsigned int i=0; i<npoints; i++) {
        vx[i] = static_cast<Sint16>(points[i].x());
        vy[i] = static_cast<Sint16>(points[i].y());
    }
    filledPolygonRGBA(GET_NATIVE(widget.backingStore()), vx, vy, npoints,
                   color.red(),
                   color.green(),
                   color.blue(),
                   color.alpha());
    delete[] vx;
    delete[] vy;
}

void BCGraphicsDeviceSDL::setClip(const BTWidget& widget, IntRect r)
{
    if (!widget.backingStore())
        return;
    if (r.isEmpty())
        SDL_SetClipRect(GET_NATIVE(widget.backingStore()), NULL);
    else {
        SDL_Rect sdlRect;
        sdlRect.x = r.x();
        sdlRect.y = r.y();
        sdlRect.w = r.width();
        sdlRect.h = r.height();
        SDL_SetClipRect(GET_NATIVE(widget.backingStore()), &sdlRect);
    }
}

IntRect BCGraphicsDeviceSDL::clip(const BTWidget& widget)
{
    if (!widget.backingStore())
        return IntRect(0,0,0,0);
    SDL_Rect sdlRect;
    SDL_GetClipRect(GET_NATIVE(widget.backingStore()), &sdlRect);
    IntRect r(sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
    return r;
}

IntSize BCGraphicsDeviceSDL::size() const
{
#ifndef NDEBUG
    if (m_layoutTests)
        return IntSize(800, 600); // fixed for layout tests
#endif
    return IntSize(m_screen->w, m_screen->h);
}

void BCGraphicsDeviceSDL::update(const BTWidget& widget, const IntRect rect)
{
#ifndef NDEBUG
    if (m_layoutTests)
        return;
#endif

    if (!widget.backingStore() || rect.isEmpty())
        return;
    SDL_Rect sdlRect, sdlDest;
    sdlRect.x = rect.x();
    sdlRect.y = rect.y();
    sdlRect.h = rect.height();
    sdlRect.w = rect.width();

    sdlDest.x = sdlRect.x + widget.x();
    sdlDest.y = sdlRect.y + widget.y();
#ifndef NDEBUG
    if (getenv("FLASHING_RECTS")) {
        // flash a red rect
        SDL_FillRect(m_screen, &sdlRect, 0x00ff0000);
        SDL_UpdateRect(m_screen, sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
        SDL_Delay(10); // wait 10ms in order to see flashing rect
    }
#endif
    SDL_BlitSurface(GET_NATIVE(widget.backingStore()), &sdlRect, m_screen, &sdlDest);
    SDL_UpdateRect(m_screen, sdlDest.x, sdlDest.y, sdlRect.w, sdlRect.h);
}

void BCGraphicsDeviceSDL::clear(const BTWidget& widget, const IntRect rect)
{
    if (!widget.backingStore() || rect.isEmpty())
        return;

    // FIXME should redraw with background color
    // i.e. transparent for transparent pages
    Color white(0xff, 0xff, 0xff, 0xff);
    fillRect(widget, rect, white);
}

SDL_Surface* BCGraphicsDeviceSDL::applyTransparency(const BINativeImage& image, const uint8_t alphaChannel)
{
     // blend is not very optimized for now, to say the least
    SDL_Surface *final = NULL;
    SDL_Surface *origin = GET_NATIVE(&image);
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

} // namespace BAL
