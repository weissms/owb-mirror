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
#include "GraphicsContext.h"

#include "AffineTransform.h"
#include "FloatRect.h"
#include "Font.h"
#include "ImageBuffer.h"
#include "IntRect.h"
#include "NotImplemented.h"
#include "Path.h"
#include "Pattern.h"
#include "SimpleFontData.h"
#include <math.h>
#include <stdio.h>
#include <wtf/MathExtras.h>

#include "GraphicsContextPrivate.h"
#include "GraphicsContextPlatformPrivateSDL.h"

#include "SDL.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using std::min;
using std::max;

namespace WKAL {

GraphicsContext::GraphicsContext(PlatformGraphicsContext* cr)
    : m_common(createGraphicsContextPrivate())
    , m_data(new GraphicsContextPlatformPrivate)
{
    m_data->surface = cr;
    setPaintingDisabled(!cr);
}

GraphicsContext::~GraphicsContext()
{
     destroyGraphicsContextPrivate(m_common);
     delete m_data;
}

AffineTransform GraphicsContext::getCTM() const
{
    NotImplemented();
    AffineTransform a;
    return a;
}

PlatformGraphicsContext* GraphicsContext::platformContext() const
{
    return m_data->surface;
}

void GraphicsContext::savePlatformState()
{
    if (paintingDisabled())
        return;

    m_common->stack.append(m_common->state);
}

void GraphicsContext::restorePlatformState()
{
    if (paintingDisabled())
        return;

    if (m_common->stack.isEmpty()) {
        DS_WAR("ERROR void BCGraphicsContext::restore() stack is empty");
        return;
    }

    m_common->state = m_common->stack.last();
    m_common->stack.removeLast();
    clip(clippingRect);
}

// Draws a filled rectangle with a stroked border.
void GraphicsContext::drawRect(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    SDL_Surface* surf = m_data->surface;

    SDL_Rect dstRect;
    dstRect.x = rect.x() + origin().width();
    dstRect.y = rect.y() + origin().height();
    dstRect.w = rect.width();
    dstRect.h = rect.height();

    if (fillColor().alpha()) {
        if (!m_data->layers.isEmpty()) {
            uint8_t alpha = static_cast<uint8_t> (fillColor().alpha() * m_data->layers.last());
            boxRGBA(surf, dstRect.x, dstRect.y,
                dstRect.w + dstRect.x - 1, dstRect.h + dstRect.y -1,
                fillColor().red(), fillColor().green(), fillColor().blue(), alpha);
        } else
            SDL_FillRect(surf, &dstRect, fillColor().rgb());
    }

    if (strokeStyle() != NoStroke) {
        if (!m_data->layers.isEmpty())
            rectangleRGBA(surf,
                static_cast<Sint16>(rect.x() + origin().width()), static_cast<Sint16>(rect.y() + origin().height()),
                static_cast<Sint16>(rect.x() + origin().width() + rect.width() - 1), 
                static_cast<Sint16>(rect.y() + origin().height() + rect.height() - 1),
                strokeColor().red(), strokeColor().green(), strokeColor().blue(), static_cast<int> (strokeColor().alpha() * m_data->layers.last()));
        else
            rectangleRGBA(surf,
                static_cast<Sint16>(rect.x() + origin().width()), static_cast<Sint16>(rect.y() + origin().height()),
                static_cast<Sint16>(rect.x() + origin().width() + rect.width() - 1), 
                static_cast<Sint16>(rect.y() + origin().height() + rect.height() - 1),
                strokeColor().red(), strokeColor().green(), strokeColor().blue(), strokeColor().alpha());
    }
}


// This is only used to draw borders.
void GraphicsContext::drawLine(const IntPoint& point1, const IntPoint& point2)
{
    if (paintingDisabled())
        return;

    StrokeStyle style = strokeStyle();
    if (style == NoStroke)
        return;
    
    float width = strokeThickness();
    if (width < 1)
        width = 1;

    IntPoint p1(point1);
    IntPoint p2(point2);
    Color color = strokeColor();

    int alpha;
    if (!m_data->layers.isEmpty())
        alpha = static_cast<int> (strokeColor().alpha() * m_data->layers.last());
    else
        alpha = strokeColor().alpha();

    if (p1.y() == p2.y())
        lineRGBA(m_data->surface,
                static_cast<Sint16>(p1.x()), static_cast<Sint16>(p1.y()),
                static_cast<Sint16>(p2.x() - 1), static_cast<Sint16>(p2.y()),
                color.red(),
                color.green(),
                color.blue(),
                alpha);
    else
        lineRGBA(m_data->surface,
                static_cast<Sint16>(p1.x()), static_cast<Sint16>(p1.y()),
                static_cast<Sint16>(p2.x()), static_cast<Sint16>(p2.y()),
                color.red(),
                color.green(),
                color.blue(),
                alpha);
}

// This method is only used to draw the little circles used in lists.
void GraphicsContext::drawEllipse(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    float yRadius = .5 * rect.height();
    float xRadius = .5 * rect.width();
    // Mean that we will draw a circle
    if (xRadius == yRadius) {
        strokeArc(rect, 0, 360);
        return;
    }

    if (strokeStyle() != NoStroke) {
        unsigned width = static_cast<unsigned int>(strokeThickness());
        if (width == 0)
            width++;
    }

    Color color = strokeColor();

    int alpha;
    if (!m_data->layers.isEmpty())
        alpha = static_cast<int> (strokeColor().alpha() * m_data->layers.last());
    else
        alpha = strokeColor().alpha();

    ellipseRGBA(m_data->surface, static_cast<Sint16>(rect.x() + origin().width() + xRadius),
                static_cast<Sint16>(rect.y() + origin().height() + yRadius),
                static_cast<Sint16>(xRadius),
                static_cast<Sint16>(yRadius),
                color.red(),
                color.green(),
                color.blue(),
                alpha);
}

//TODO: draw points instead of lines for nicer circles
inline void drawArc(SDL_Surface* surf, const WebCore::Color color, int zone, int xc, int yc, float& x0, float& y0, float x1, float y1, bool doSwap = true)
{
    // Mean First draw => will not draw just a point.
    if (x0 == x1)
        return;

    switch(zone) {
    case 0:
        lineRGBA(surf,
                    static_cast<int> (xc + ceilf(x0)), static_cast<int> (yc - ceilf(y0)),
                    static_cast<int> (xc + ceilf(x1)), static_cast<int> (yc - ceilf(y1)),
                    color.red(),
                    color.green(),
                    color.blue(),
                    color.alpha());
        break;
    case 1:
        lineRGBA(surf,
                    static_cast<int> (xc - ceilf(y0)), static_cast<int> (yc - ceilf(x0)),
                    static_cast<int> (xc - ceilf(y1)), static_cast<int> (yc - ceilf(x1)),
                    color.red(),
                    color.green(),
                    color.blue(),
                    color.alpha());
        break;
    case 2:
        lineRGBA(surf,
                    static_cast<int> (xc - ceilf(x0)), static_cast<int> (yc + ceilf(y0)),
                    static_cast<int> (xc - ceilf(x1)), static_cast<int> (yc + ceilf(y1)),
                    color.red(),
                    color.green(),
                    color.blue(),
                    color.alpha());
        break;
    case 3:
        lineRGBA(surf,
                    static_cast<int> (xc + ceilf(y0)), static_cast<int> (yc + ceilf(x0)),
                    static_cast<int> (xc + ceilf(y1)), static_cast<int> (yc + ceilf(x1)),
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

void drawArc(SDL_Surface* surf, const IntRect rect, uint16_t startAngle, uint16_t angleSpan, const WebCore::Color color)
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

    int r = (rect.width() - 1) / 2;
    int xc = rect.x() + r;
    int yc = rect.y() + (rect.height() - 1)/ 2;
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
            drawArc(surf, color, z0, xc, yc, x0, y0, x1, y1);
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
                drawArc(surf, color, z1, xc, yc, x0, y0, x1, y1);
            }
            x0 = xalpha0;
            y0 = sqrt(pow(r, 2) - pow(x0, 2));
            for (x1 = xalpha0; x1 >= 0; x1--) {
                y1 = sqrt(pow(r, 2) - pow(x1, 2));
                drawArc(surf, color, z0, xc, yc, x0, y0, x1, y1);
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
                    drawArc(surf, color, z0, xc, yc, x0, y0, x1, y1);
                else if (x1 > xalpha0)
                    drawArc(surf, color, z1, xc, yc, x0, y0, x1, y1);
                else {
                    drawArc(surf, color, z0, xc, yc, x0, y0, x1, y1, false);
                    drawArc(surf, color, z1, xc, yc, x0, y0, x1, y1);
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
                drawArc(surf, color, z1 - 2, xc, yc, x0, y0, x1, y1, false);
            if (x1 < xalpha1)
                drawArc(surf, color, z1 % 3, xc, yc, x0, y0, x1, y1, false);
            if (x1 < xalpha0)
                drawArc(surf, color, z0, xc, yc, x0, y0, x1, y1, false);
            drawArc(surf, color, z1 - 1, xc, yc, x0, y0, x1, y1);
        }
    }
}



// FIXME: This function needs to be adjusted to match the functionality on the Mac side.
void GraphicsContext::strokeArc(const IntRect& rect, int startAngle, int angleSpan)
{
    if (paintingDisabled())
        return;

    if (strokeStyle() == NoStroke)
        return;

//draw as many arcs as needed for the thickness
    //this is a quick hack until we re-implement the drawArc in lower level.
    float thick = strokeThickness() / 2.0;
    IntRect rectWork = rect;
    int a0 = (startAngle) / 90;
    int a1 = (startAngle + angleSpan) / 90;

    int alpha;
    if (!m_data->layers.isEmpty())
        alpha = static_cast<int> (strokeColor().alpha() * m_data->layers.last());
    else
        alpha = strokeColor().alpha();

    WebCore::Color color(strokeColor().red(), strokeColor().green(), strokeColor().blue(), alpha);

    switch (a0) {

    case 0:
        rectWork.setX(rect.x() + origin().width());

        for (int i = 0; i < (int)thick; i++) {
            rectWork.setY(rect.y() + origin().height() + i);
            rectWork.setWidth(rect.width() - i);
            rectWork.setHeight(rect.height() - i);
            drawArc(m_data->surface, rectWork, startAngle, angleSpan, color);
        }
        break;

    case 1:
        for (int i = 0; i < (int)thick; i++) {
            rectWork.setX(rect.x() + origin().width() + i);
            rectWork.setY(rect.y() + origin().height() + i);
            rectWork.setWidth(rect.width() - i*2);
            rectWork.setHeight(rect.height() - i*2);
            drawArc(m_data->surface, rectWork, startAngle, angleSpan, color);
        }
        break;

    case 2:
        rectWork.setY(rect.y() + origin().height());

        for (int i = 0; i < (int)thick; i++) {
            rectWork.setX(rect.x() + origin().width() + i);
            rectWork.setWidth(rect.width() - i);
            rectWork.setHeight(rect.height() - i);
            drawArc(m_data->surface, rectWork, startAngle, angleSpan, color);
        }
        break;

    case 3:
        for (int i = 0; i < (int)thick; i++) {
            rectWork.setX(rect.x() + origin().width() + i);
            rectWork.setY(rect.y() + origin().height() + i);
            rectWork.setWidth(rect.width() - i*2);
            rectWork.setHeight(rect.height() - i*2);
            drawArc(m_data->surface, rectWork, startAngle, angleSpan, color);
        }
        break;
    }
}

void GraphicsContext::drawConvexPolygon(size_t npoints, const FloatPoint* points, bool shouldAntialias)
{
    if (paintingDisabled())
        return;

    if (npoints <= 1)
        return;

    Sint16 *vx = new Sint16[npoints];
    Sint16 *vy = new Sint16[npoints];
    Sint16 x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    for(size_t i=0; i < npoints; i++) {
        vx[i] = static_cast<Sint16>(points[i].x() + origin().width());
        vy[i] = static_cast<Sint16>(points[i].y() + origin().height());
        x1 = min(x1, vx[i]);
        y1 = min(y1, vy[i]);
        x2 = max(x2, vx[i]);
        y2 = max(y2, vy[i]);
    }

    int alpha;
    if (!m_data->layers.isEmpty())
        alpha = static_cast<int> (strokeColor().alpha() * m_data->layers.last());
    else
        alpha = strokeColor().alpha();

    Color color = fillColor();

    filledPolygonRGBA(m_data->surface, vx, vy, npoints,
                   color.red(),
                   color.green(),
                   color.blue(),
                   alpha);
    delete[] vx;
    delete[] vy;
}

void GraphicsContext::fillRect(const IntRect& rectangle, const Color& color)
{
    if (paintingDisabled())
        return;

    IntRect rect(rectangle);
    
    rect.setLocation(rectangle.location() + origin());

    SDL_Surface* surf = m_data->surface;

    SDL_Rect sdlRect;
    sdlRect.x = static_cast<Sint16>(rect.x());
    sdlRect.y = static_cast<Sint16>(rect.y());
    sdlRect.w = static_cast<Sint16>(rect.width());
    sdlRect.h = static_cast<Sint16>(rect.height());

    if (color.alpha()) {
        int alpha;
        if (!m_data->layers.isEmpty()) {
            alpha = static_cast<int> (strokeColor().alpha() * m_data->layers.last());
            boxRGBA(surf,
                static_cast<Sint16>(rect.x()), 
                static_cast<Sint16>(rect.y()),
                static_cast<Sint16>(rect.x() + rect.width() - 1), 
                static_cast<Sint16>(rect.y() + rect.height() - 1),
                color.red(), color.green(), color.blue(), alpha);
        } else {
            alpha = strokeColor().alpha();
            WebCore::Color c(color.red(), color.green(), color.blue(), alpha);
            SDL_FillRect(surf, &sdlRect, c.rgb());
        }
    }
}

void GraphicsContext::fillRect(const FloatRect& rect, const Color& color)
{
    fillRect(IntRect(rect), color);
}

void GraphicsContext::clip(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    if (rect.isEmpty()) {
        SDL_SetClipRect(m_data->surface, NULL);
    } else {
        float x = rect.x() + origin().width();
        x= x > 0 ? x : 0;
        float y = rect.y() + origin().height();
        y= y > 0 ? y : 0;
        SDL_Rect dstRect;
        dstRect.x = static_cast<Sint16>(x);
        dstRect.y = static_cast<Sint16>(y);
        dstRect.w = static_cast<Sint16>(rect.width());
        dstRect.h = static_cast<Sint16>(rect.height());

        SDL_SetClipRect(m_data->surface, &dstRect);
    }
}

void GraphicsContext::drawFocusRing(const Color& color)
{
    if (paintingDisabled())
        return;
    int radius = (focusRingWidth() - 1) / 2;
    int offset = radius + focusRingOffset();

    const Vector<IntRect>& rects = focusRingRects();
    unsigned rectCount = rects.size();
    IntRect finalFocusRect;
    for (unsigned i = 0; i < rectCount; i++) {
        IntRect focusRect = rects[i];
        focusRect.setLocation(focusRect.location() + origin());
        focusRect.inflate(offset);
        finalFocusRect.unite(focusRect);
    }

    // NOTE These rects are rounded on Mac
    // Force the alpha to 50%.  This matches what the Mac does with outline rings.
    //fillRect(finalFocusRect, Color::gray);
    //Color c = fillColor();
    Color ringColor(color.red(), color.green(), color.blue(), 127);

    rectangleRGBA(m_data->surface,
                static_cast<Sint16>(finalFocusRect.x()), 
                static_cast<Sint16>(finalFocusRect.y()),
                static_cast<Sint16>(finalFocusRect.x() + finalFocusRect.width() - 1), 
                static_cast<Sint16>(finalFocusRect.y() + finalFocusRect.height() - 1),
                ringColor.red(), ringColor.green(), ringColor.blue(), ringColor.alpha());
}

void GraphicsContext::drawLineForText(const IntPoint& startPoint, int width, bool printing)
{
    if (paintingDisabled())
        return;

    // This is a workaround for http://bugs.webkit.org/show_bug.cgi?id=15659
    StrokeStyle savedStrokeStyle = strokeStyle();
    setStrokeStyle(SolidStroke);

    IntPoint point(startPoint + origin());

    IntPoint endPoint = point + IntSize(width, 0);

    // NOTE we should adjust line to pixel boundaries
    drawLine(point, endPoint);

    setStrokeStyle(savedStrokeStyle);
}

void GraphicsContext::drawLineForMisspellingOrBadGrammar(const IntPoint& origin, int width, bool grammar)
{
    NotImplemented();
}

FloatRect GraphicsContext::roundToDevicePixels(const FloatRect& frect)
{
    return frect;
}

void GraphicsContext::translate(float x, float y)
{
    if (paintingDisabled())
        return;
    m_common->state.origin += IntSize(static_cast<int>(x), static_cast<int>(y));
}

IntSize GraphicsContext::origin()
{
    return m_common->state.origin;
}

void GraphicsContext::applyStrokePattern(const Pattern& pattern)
{
}

void GraphicsContext::applyFillPattern(const Pattern& pattern)
{
}


void GraphicsContext::setPlatformFillColor(const Color& col)
{
    // FIXME: this is probably a no-op but I'm not sure
    // notImplemented(); // commented-out because it's chatty and clutters output
	m_common->state.fillColor = col;
}

void GraphicsContext::setPlatformStrokeColor(const Color& col)
{
    // FIXME: this is probably a no-op but I'm not sure
    //notImplemented(); // commented-out because it's chatty and clutters output
	m_common->state.strokeColor = col;
}

void GraphicsContext::setPlatformStrokeThickness(float strokeThickness)
{
    m_common->state.strokeThickness = strokeThickness;
}

void GraphicsContext::setPlatformStrokeStyle(const StrokeStyle& strokeStyle)
{
	m_common->state.strokeStyle = strokeStyle;
}

void GraphicsContext::setURLForRect(const KURL& link, const IntRect& destRect)
{
    notImplemented();
}

void GraphicsContext::concatCTM(const AffineTransform& transform)
{
    NotImplemented();
}

void GraphicsContext::addInnerRoundedRectClip(const IntRect& rect, int thickness)
{
    NotImplemented();
}

void GraphicsContext::clipToImageBuffer(const FloatRect& rect, const ImageBuffer* imageBuffer)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

void GraphicsContext::setPlatformShadow(IntSize const&, int, Color const&)
{
    notImplemented();
}

void GraphicsContext::clearPlatformShadow()
{
    notImplemented();
}

void GraphicsContext::beginTransparencyLayer(float opacity)
{
    if (paintingDisabled())
        return;
    m_data->layers.append(opacity);
    m_data->beginTransparencyLayer();
}

void GraphicsContext::endTransparencyLayer()
{
    if (paintingDisabled())
        return;

    m_data->layers.removeLast();
    m_data->endTransparencyLayer();
}

void GraphicsContext::clearRect(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    IntRect rectangle(rect);
    rectangle.setLocation(rectangle.location() + origin());
    fillRect(rectangle, Color::white);
}

void GraphicsContext::strokeRect(const FloatRect& rect, float width)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::setLineCap(LineCap lineCap)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::setLineJoin(LineJoin lineJoin)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::setMiterLimit(float miter)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::setAlpha(float)
{
    notImplemented();
}

void GraphicsContext::setCompositeOperation(CompositeOperator op)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::beginPath()
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::addPath(const Path& path)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::clip(const Path& path)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::clipOut(const Path& path)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::rotate(float radians)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::scale(const FloatSize& size)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::clipOut(const IntRect& r)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::clipOutEllipseInRect(const IntRect& r)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::fillRoundedRect(const IntRect& r, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight, const Color& color)
{
    if (paintingDisabled())
        return;

    NotImplemented();
}

void GraphicsContext::setBalExposeEvent(BalEventExpose* expose)
{
    m_expose = expose;
}

BalEventExpose* GraphicsContext::balExposeEvent() const
{
    return m_expose;
}

BalDrawable* GraphicsContext::balDrawable() const
{
    //if (!m_data->expose)
    //    return 0;

    return 0;//GDK_DRAWABLE(m_data->expose->window);
}

float GraphicsContext::transparencyLayer()
{
    if (!m_data->layers.isEmpty())
        return m_data->layers.last();
    else
        return 1.0;
}

IntPoint GraphicsContext::translatePoint(const IntPoint& point) const
{
    NotImplemented();
    return point;
}

void GraphicsContext::setUseAntialiasing(bool enable)
{
    if (paintingDisabled())
        return;

    // When true, use the default Cairo backend antialias mode (usually this
    // enables standard 'grayscale' antialiasing); false to explicitly disable
    // antialiasing. This is the same strategy as used in drawConvexPolygon().
    //cairo_set_antialias(m_data->cr, enable ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);
}

} // namespace WebCore

