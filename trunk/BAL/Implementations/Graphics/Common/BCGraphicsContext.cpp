/*
 * Copyright (C) 2003, 2004, 2005, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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
#include "BCGraphicsContext.h"
#include "BINativeImage.h"
#include "BTAffineTransform.h"
#include "Color.h"
#include "DeprecatedString.h"
#include <BIMath.h>
#include "BIGraphicsDevice.h"
#include "BTLogHelper.h"

using namespace std;
using namespace WebCore;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define notImplemented() DBGM(MODULE_GRAPHICS, "not implemented\n");

//FIXME: Should be elsewhere.
void WebCore::setFocusRingColorChangeFunction(void (*)()) { BALNotImplemented(); }
Color WebCore::focusRingColor() { return 0x7f0000ff; }

namespace BAL {
// used by some rendering stuff that use a fake GC
BIGraphicsContext* createBIGraphicsContext()
{
    return new BCGraphicsContext(true);
}

void deleteBIGraphicsContext(BIGraphicsContext* context)
{
    delete context;
}

BIGraphicsContext* createFakeBIGraphicsContext()
{
    return new BCGraphicsContext(false);
}
//
// GraphicsContextState
//


struct GraphicsContextState {
    GraphicsContextState()
    : strokeStyle(SolidStroke)
    , strokeThickness(0)
    , strokeColor(Color::black)
    , fillColor(Color::transparent)
    , textDrawingMode(cTextStroke|cTextFill)
    , paintingDisabled(false)
    {}

    Font font;
    StrokeStyle strokeStyle;
    float strokeThickness;
    Color strokeColor;
    Color fillColor;
    int textDrawingMode;
    bool paintingDisabled;
    IntRect clippingRect;
    IntPoint origin;
};

//
// GraphicsContextPrivate implementation
//

class GraphicsContextPrivate {
public:
    GraphicsContextPrivate();

    GraphicsContextState state;
    Vector<GraphicsContextState> stack;
    Vector<IntRect> m_focusRingRects;
    int m_focusRingWidth;
    int m_focusRingOffset;
    bool m_updatingControlTints;
};

GraphicsContextPrivate::GraphicsContextPrivate()
    : m_focusRingWidth(0)
    , m_focusRingOffset(0)
    , m_updatingControlTints(false)
{
}

//
// GraphicsContextPlatformPrivate implementation
//

class GraphicsContextPlatformPrivate {
    public:
        GraphicsContextPlatformPrivate();
        ~GraphicsContextPlatformPrivate();

        BINativeImage* nativeImage;
};


GraphicsContextPlatformPrivate::GraphicsContextPlatformPrivate()
{
}

GraphicsContextPlatformPrivate::~GraphicsContextPlatformPrivate()
{
}
//
// BCGraphicsContext implementation
//

BCGraphicsContext::BCGraphicsContext(bool isReal)
    : m_common(createGraphicsContextPrivate())
    , m_data(new GraphicsContextPlatformPrivate)
    , m_alphaLayerValue(1.0)
    , m_widget(NULL)
{
}

BCGraphicsContext::~BCGraphicsContext()
{
    destroyGraphicsContextPrivate(m_common);
    delete m_data;
}

GraphicsContextPrivate* BCGraphicsContext::createGraphicsContextPrivate()
{
    return new GraphicsContextPrivate;
}

void BCGraphicsContext::destroyGraphicsContextPrivate(GraphicsContextPrivate* deleteMe)
{
    delete deleteMe;
}

void BCGraphicsContext::setWidget(const BTWidget* widget)
{
    m_widget = widget;
}

const BTWidget* BCGraphicsContext::widget()
{
    return m_widget;
}

void BCGraphicsContext::save()
{
    if (paintingDisabled())
        return;

    m_common->stack.append(m_common->state);
}

void BCGraphicsContext::restore()
{
    if (!m_widget)
        return;
    if (paintingDisabled())
        return;

    if (m_common->stack.isEmpty()) {
        LOG_ERROR("ERROR void BCGraphicsContext::restore() stack is empty");
        return;
    }
    m_common->state = m_common->stack.last();
    m_common->stack.removeLast();
    getBIGraphicsDevice()->setClip(*m_widget, m_common->state.clippingRect);
}

const Font& BCGraphicsContext::font() const
{
    return m_common->state.font;
}

void BCGraphicsContext::setFont(const Font& aFont)
{
    m_common->state.font = aFont;
}

void BCGraphicsContext::setStrokeThickness(float thickness)
{
    m_common->state.strokeThickness = thickness;
}

void BCGraphicsContext::setStrokeStyle(const StrokeStyle& style)
{
    m_common->state.strokeStyle = style;
}

void BCGraphicsContext::setStrokeColor(const Color& color)
{
    m_common->state.strokeColor = color;
}

float BCGraphicsContext::strokeThickness() const
{
    return m_common->state.strokeThickness;
}

StrokeStyle BCGraphicsContext::strokeStyle() const
{
    return m_common->state.strokeStyle;
}

Color BCGraphicsContext::strokeColor() const
{
    return m_common->state.strokeColor;
}

void BCGraphicsContext::setFillColor(const Color& color)
{
    m_common->state.fillColor = color;
}

Color BCGraphicsContext::fillColor() const
{
    return m_common->state.fillColor;
}


bool BCGraphicsContext::updatingControlTints() const
{
    return m_common->m_updatingControlTints;
}

void BCGraphicsContext::setUpdatingControlTints(bool b)
{
    setPaintingDisabled(b);
    m_common->m_updatingControlTints = b;
}

void BCGraphicsContext::setPaintingDisabled(bool f)
{
    m_common->state.paintingDisabled = f;
}

bool BCGraphicsContext::paintingDisabled() const
{
    return m_common->state.paintingDisabled;
}

void BCGraphicsContext::drawImage(BINativeImage* image, const IntPoint& p, CompositeOperator op)
{
    if (image)
        drawImage(image, p, IntRect(0, 0, -1, -1), op);
    return;
}

void BCGraphicsContext::drawImage(BINativeImage* image, const IntRect& r, CompositeOperator op)
{
    if (image)
        drawImage(image, r, IntRect(0, 0, -1, -1), op);
    return;
}

void BCGraphicsContext::drawImage(BINativeImage* image, const IntPoint& dest, const IntRect& srcRect, CompositeOperator op)
{
    if (image)
        drawImage(image, IntRect(dest, srcRect.size()), srcRect, op);
    return;
}

void BCGraphicsContext::drawImage(BINativeImage* image, const IntRect& dest, const IntRect& srcRect, CompositeOperator op)
{
    if (image)
        drawImage(image, FloatRect(dest), srcRect, op);
    return;
}

void BCGraphicsContext::drawText(const TextRun& run, const IntPoint& point, int from = 0, int to = -1)
{
    drawText(run, point, TextStyle(), from, to);
}

void BCGraphicsContext::drawText(const TextRun& run, const IntPoint& point, const TextStyle& style, int from = 0, int to = -1)
{
    if (paintingDisabled())
        return;

    font().drawText(this, run, style, point + origin());
}

void BCGraphicsContext::drawBidiText(const TextRun& run, const IntPoint& point, const TextStyle& style)
{
}

void BCGraphicsContext::drawHighlightForText(const TextRun& run, const IntPoint& point, int h, const TextStyle& style, const Color& backgroundColor, int from = 0, int to = -1)
{
    if (paintingDisabled())
        return;

    fillRect(font().selectionRectForText(run, style, point, h), backgroundColor);
}

void BCGraphicsContext::initFocusRing(int width, int offset)
{
    if (paintingDisabled())
        return;
    clearFocusRing();

    m_common->m_focusRingWidth = width;
    m_common->m_focusRingOffset = offset;
}

void BCGraphicsContext::clearFocusRing()
{
    m_common->m_focusRingRects.clear();
}

void BCGraphicsContext::addFocusRingRect(const IntRect& rect)
{
    if (paintingDisabled() || rect.isEmpty())
        return;
    m_common->m_focusRingRects.append(rect);
}

int BCGraphicsContext::focusRingWidth() const
{
    return m_common->m_focusRingWidth;
}

int BCGraphicsContext::focusRingOffset() const
{
    return m_common->m_focusRingOffset;
}

const Vector<IntRect>& BCGraphicsContext::focusRingRects() const
{
    return m_common->m_focusRingRects;
}

void BCGraphicsContext::drawImage(BINativeImage* image, const FloatRect& dest, const FloatRect& src, CompositeOperator op)
{
    if (paintingDisabled())
        return;

    float tsw = src.width();
    float tsh = src.height();
    float tw = dest.width();
    float th = dest.height();

    if (tsw == -1)
        tsw = image->size().width();
    if (tsh == -1)
        tsh = image->size().height();

    if (tw == -1)
        tw = image->size().width();
    if (th == -1)
        th = image->size().height();

    this->realDraw(image, FloatRect(dest.location(), FloatSize(tw, th)), FloatRect(src.location(), FloatSize(tsw, tsh)), op);
}

void BCGraphicsContext::drawTiledImage(BINativeImage* image, const IntRect& rect, const IntPoint& srcPoint, const IntSize& tileSize, CompositeOperator op)
{
    if (paintingDisabled())
        return;

    this->realDrawTiled(image, rect, srcPoint, tileSize, op);
}

void BCGraphicsContext::drawTiledImage(BINativeImage* image, const IntRect& dest, const IntRect& srcRect, TileRule hRule, TileRule vRule, CompositeOperator op)
{
    if (paintingDisabled())
        return;

    if (hRule == BIGraphicsContext::StretchTile && vRule == BIGraphicsContext::StretchTile)
        // Just do a scale.
        return drawImage(image, dest, srcRect);
}

void BCGraphicsContext::realDraw(
  BINativeImage* nativeImage, const FloatRect& dst, const FloatRect& src, CompositeOperator op)
{
    if (!m_widget)
        return;
    if (!nativeImage)
        return;

    IntRect source(src);
    IntRect dest(dst);

    dest.setLocation(dest.location() + origin());

    if (source.size() != dest.size())
        getBIGraphicsDevice()->stretchBlit(*m_widget, *nativeImage, source, dest, static_cast<int> (m_alphaLayerValue * 255));
    else
        getBIGraphicsDevice()->copy(*m_widget, *nativeImage, source, dest.location(), static_cast<int> (m_alphaLayerValue * 255));
}

void realDrawTiled(BINativeImage*, const FloatRect& dstRect, const FloatRect& srcRect, BIGraphicsContext::TileRule hRule, BIGraphicsContext::TileRule vRule, CompositeOperator)
{
    BALNotImplemented();
}

void BCGraphicsContext::realDrawTiled(
  BINativeImage* image, const FloatRect& destRect, const FloatPoint& srcPoint, const FloatSize& scaledTileSize, CompositeOperator op)
{
    if (!m_widget)
        return;
    if (!image)
        return;

    FloatSize intrinsicTileSize = image->size();
    FloatSize scale(scaledTileSize.width() / intrinsicTileSize.width(),
                    scaledTileSize.height() / intrinsicTileSize.height());
    AffineTransform patternTransform = AffineTransform().scale(scale.width(), scale.height());

    FloatRect oneTileRect;
    oneTileRect.setX(destRect.x() + fmodf(fmodf(-srcPoint.x(), scaledTileSize.width()) - scaledTileSize.width(), scaledTileSize.width()));
    oneTileRect.setY(destRect.y() + fmodf(fmodf(-srcPoint.y(), scaledTileSize.height()) - scaledTileSize.height(), scaledTileSize.height()));
    oneTileRect.setSize(scaledTileSize);
   
    // Check and see if a single draw of the image can cover the entire area we are supposed to tile.   
    if (oneTileRect.contains(destRect)) {
        FloatRect visibleSrcRect;
        visibleSrcRect.setX((destRect.x() - oneTileRect.x()) / scale.width());
        visibleSrcRect.setY((destRect.y() - oneTileRect.y()) / scale.height());
        visibleSrcRect.setWidth(destRect.width() / scale.width());
        visibleSrcRect.setHeight(destRect.height() / scale.height());
        realDraw(image, destRect, visibleSrcRect, op);
        return;
    }
    else {
        // save context info
        if (!destRect.isEmpty()) {
            save();
            clip(IntRect(destRect)); // don't draw outside this
         }
        // draw image pattern inside destRect 
        IntRect dest(IntPoint(), image->size());
        IntRect src(static_cast<int>(oneTileRect.x()), static_cast<int>(oneTileRect.y()), static_cast<int>(oneTileRect.size().width()), static_cast<int>(oneTileRect.size().height()));

        int xMax = static_cast<int>(destRect.x() + destRect.width());
        int yMax = static_cast<int>(destRect.y() + destRect.height());

        for(int x = static_cast<int>(oneTileRect.x()); x <= xMax; x+= image->size().width()) {
            for(int y = static_cast<int>(oneTileRect.y()); y <= yMax; y+=image->size().height()) {
                dest.setLocation(IntPoint(x, y) + origin());
                getBIGraphicsDevice()->stretchBlit(*m_widget, *image, src, dest, alphaLayer());
            }
        }
        if (!destRect.isEmpty())
            restore();
    }
}

void BCGraphicsContext::clipOutRoundedRect(const IntRect& rect, const IntSize& topLeft, const IntSize& topRight,
                                         const IntSize& bottomLeft, const IntSize& bottomRight)
{
    if (paintingDisabled())
        return;

    // Need sufficient width and height to contain these curves.  Sanity check our
    // corner radii and our width/height values to make sure the curves can all fit.
    if (static_cast<unsigned>(rect.width()) < static_cast<unsigned>(topLeft.width()) + static_cast<unsigned>(topRight.width()) ||
        static_cast<unsigned>(rect.width()) < static_cast<unsigned>(bottomLeft.width()) + static_cast<unsigned>(bottomRight.width()) ||
        static_cast<unsigned>(rect.height()) < static_cast<unsigned>(topLeft.height()) + static_cast<unsigned>(bottomLeft.height()) ||
        static_cast<unsigned>(rect.height()) < static_cast<unsigned>(topRight.height()) + static_cast<unsigned>(bottomRight.height()))
        return;

    // Clip out each shape one by one.
    clipOutEllipseInRect(IntRect(rect.x(), rect.y(), topLeft.width() * 2, topLeft.height() * 2));
    clipOutEllipseInRect(IntRect(rect.right() - topRight.width() * 2, rect.y(), topRight.width() * 2, topRight.height() * 2));
    clipOutEllipseInRect(IntRect(rect.x(), rect.bottom() - bottomLeft.height() * 2, bottomLeft.width() * 2, bottomLeft.height() * 2));
    clipOutEllipseInRect(IntRect(rect.right() - bottomRight.width() * 2, rect.bottom() - bottomRight.height() * 2, bottomRight.width() * 2, bottomRight.height() * 2));
    clipOut(IntRect(rect.x() + topLeft.width(), rect.y(),
                    rect.width() - topLeft.width() - topRight.width(),
                    max(topLeft.height(), topRight.height())));
    clipOut(IntRect(rect.x() + bottomLeft.width(),
                    rect.bottom() - max(bottomLeft.height(), bottomRight.height()),
                    rect.width() - bottomLeft.width() - bottomRight.width(),
                    max(bottomLeft.height(), bottomRight.height())));
    clipOut(IntRect(rect.x(), rect.y() + topLeft.height(),
                    max(topLeft.width(), bottomLeft.width()), rect.height() - topLeft.height() - bottomLeft.height()));
    clipOut(IntRect(rect.right() - max(topRight.width(), bottomRight.width()),
                    rect.y() + topRight.height(),
                    max(topRight.width(), bottomRight.width()), rect.height() - topRight.height() - bottomRight.height()));
    clipOut(IntRect(rect.x() + max(topLeft.width(), bottomLeft.width()),
                    rect.y() + max(topLeft.height(), topRight.height()),
                    rect.width() - max(topLeft.width(), bottomLeft.width()) - max(topRight.width(), bottomRight.width()),
                    rect.height() - max(topLeft.height(), topRight.height()) - max(bottomLeft.height(), bottomRight.height())));
}

int BCGraphicsContext::textDrawingMode()
{
    return m_common->state.textDrawingMode;
}

void BCGraphicsContext::setTextDrawingMode(int mode)
{
    m_common->state.textDrawingMode = mode;
    if (paintingDisabled())
        return;
}

//
// Code formerly in platform/cairo/GraphicsContextCairo.cpp
//

// Draws a filled rectangle with a stroked border.
void BCGraphicsContext::drawRect(const IntRect& rectangle)
{
    if (!m_widget)
    return;

    if (rectangle.isEmpty())
        return;

    if (paintingDisabled())
        return;

    IntRect rect(rectangle);
    rect.setLocation(rectangle.location() + origin());

    if (m_alphaLayerValue != 1.0) {
        Color rectFillColor(fillColor().red(), fillColor().green(), fillColor().blue(), static_cast<int> (fillColor().alpha() * m_alphaLayerValue));
        getBIGraphicsDevice()->fillRect(*m_widget, rect, rectFillColor);
    } else
        getBIGraphicsDevice()->fillRect(*m_widget, rect, fillColor());

    if (strokeStyle() != NoStroke) {
        if (m_alphaLayerValue != 1.0) {
            Color rectColor(strokeColor().red(), strokeColor().green(), strokeColor().blue(), static_cast<int> (strokeColor().alpha() * m_alphaLayerValue));
            getBIGraphicsDevice()->drawRect(*m_widget, rect, rectColor);
        } else
            getBIGraphicsDevice()->drawRect(*m_widget, rect, strokeColor());
    }
}

// This is only used to draw borders.
void BCGraphicsContext::drawLine(const IntPoint& point1, const IntPoint& point2)
{
    if (!m_widget)
    return;

    if (paintingDisabled())
        return;

    float width = strokeThickness();
    if (width < 1)
        width = 1;

    if (m_alphaLayerValue != 1.0) {
        Color lineColor(strokeColor().red(), strokeColor().green(), strokeColor().blue(), static_cast<int> (strokeColor().alpha() * m_alphaLayerValue));
        getBIGraphicsDevice()->drawLine(*m_widget, point1 + origin(), point2 + origin(), lineColor);
    } else
        getBIGraphicsDevice()->drawLine(*m_widget, point1 + origin(), point2 + origin(), strokeColor());
}

// This method is only used to draw the little circles used in lists.
void BCGraphicsContext::drawEllipse(const IntRect& rect)
{
    if (!m_widget)
        return;
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

    getBIGraphicsDevice()->drawEllipse(*m_widget, rect, strokeColor());
}

// FIXME: This function needs to be adjusted to match the functionality on the Mac side.
void BCGraphicsContext::strokeArc(const IntRect& rect, int startAngle, int angleSpan)
{
    if (!m_widget)
        return;

    if (paintingDisabled())
        return;

    getBIGraphicsDevice()->drawArc(*m_widget, rect, startAngle, angleSpan, strokeColor());
}

void BCGraphicsContext::drawConvexPolygon(size_t npoints, const FloatPoint* points, bool shouldAntialias)
{
    if (!m_widget)
        return;

    if (paintingDisabled())
        return;

    IntPoint* intPoints = new IntPoint[npoints];
    for(size_t i=0; i < npoints; i++) {
        intPoints[i].setX(static_cast<int>(points[i].x()) + origin().x());
        intPoints[i].setY(static_cast<int>(points[i].y()) + origin().y());
    }
    getBIGraphicsDevice()->fillConvexPolygon(*m_widget, npoints, intPoints, fillColor());
    delete[] intPoints;
}

void BCGraphicsContext::fillRect(const IntRect& rectangle, const Color& color)
{
    if (paintingDisabled())
        return;
    if (!m_widget)
        return;

    IntRect rect(rectangle);
    rect.setLocation(rectangle.location() + origin());
    Color c(color.red(), color.green(), color.blue(), static_cast<int>(color.alpha()*m_alphaLayerValue));
    getBIGraphicsDevice()->fillRect(*m_widget, rect, c);
}

void BCGraphicsContext::fillRect(const FloatRect& rect, const Color& color)
{
    fillRect(IntRect(rect), color);
}

void BCGraphicsContext::drawFocusRing(const Color& color)
{
    if (!m_widget)
        return;

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
    getBIGraphicsDevice()->drawRect(*m_widget, finalFocusRect, color);
}

void BCGraphicsContext::setFocusRingClip(const IntRect&)
{
    // FIXME occurs too frequently
    //notImplemented();
}

void BCGraphicsContext::clearFocusRingClip()
{
    // FIXME occurs too frequently
    //notImplemented();
}

void BCGraphicsContext::drawLineForText(const IntPoint& startPoint, int width, bool /*printing*/)
{
    if (!m_widget)
        return;

    if (paintingDisabled())
        return;

    IntPoint point(startPoint + origin());

    IntPoint endPoint = point + IntSize(width, 0);

    // NOTE we should adjust line to pixel boundaries
    getBIGraphicsDevice()->drawLine(*m_widget, point, endPoint, strokeColor());
}

void BCGraphicsContext::drawLineForMisspellingOrBadGrammar(const IntPoint&, int /*width*/, bool /*grammar*/)
{
    notImplemented(); // FIXME Implement.
}

FloatRect BCGraphicsContext::roundToDevicePixels(const FloatRect& frect)
{
    return frect;
}

IntRect BCGraphicsContext::focusRingBoundingRect()
{
    IntRect result = IntRect(0, 0, 0, 0);

    const Vector<IntRect>& rects = focusRingRects();
    unsigned rectCount = rects.size();
    for (unsigned i = 0; i < rectCount; i++)
        result.unite(rects[i]);

    return result;
}

void BCGraphicsContext::concatCTM(const BAL::AffineTransform&)
{
    notImplemented();
}


// some stubs

void BCGraphicsContext::addRoundedRectClip(const IntRect& /*rect*/,
                                           const IntSize& /*topLeft*/, const IntSize& /*topRight*/,
                        const IntSize& /*bottomLeft*/, const IntSize& /*bottomRight*/)
{
    notImplemented();
}

void BCGraphicsContext::addInnerRoundedRectClip(const IntRect& /*rect*/, int /*thickness*/)
{
    notImplemented();
}

void BCGraphicsContext::setShadow(IntSize const&,int,Color const&)
{
    notImplemented();
}

void BCGraphicsContext::clearShadow()
{
    notImplemented();
}

void BCGraphicsContext::beginTransparencyLayer(float f)
{
    m_alphaLayerValue = f;
}

void BCGraphicsContext::endTransparencyLayer()
{
    m_alphaLayerValue = 1.0;
}

uint8_t BCGraphicsContext::alphaLayer() const 
{
   return static_cast<uint8_t> (m_alphaLayerValue * 255);
}

void BCGraphicsContext::clearRect(const FloatRect& rect)
{
    DBGML(MODULE_GRAPHICS, LEVEL_INFO, "clearRect (%fx%f) at position (%f,%f)\n", rect.width(), rect.height(), rect.x(), rect.y());
    IntRect rectangle(rect);
    rectangle.setLocation(rectangle.location() + origin());

    getBIGraphicsDevice()->clear(*m_widget, rectangle);
}

void BCGraphicsContext::strokeRect(const FloatRect&, float)
{
    notImplemented();
}

void BCGraphicsContext::setLineCap(LineCap)
{
    notImplemented();
}

void BCGraphicsContext::setLineJoin(LineJoin)
{
    notImplemented();
}

void BCGraphicsContext::setMiterLimit(float)
{
    notImplemented();
}

void BCGraphicsContext::setAlpha(float)
{
    notImplemented();
}

void BCGraphicsContext::setCompositeOperation(CompositeOperator)
{
    notImplemented();
}

void BCGraphicsContext::clip(const Path&)
{
    notImplemented();
}

void BCGraphicsContext::clip(const IntRect& rectangle)
{
    if (!m_widget)
        return;

    if (paintingDisabled())
        return;

    IntRect rect(rectangle);
    if (rect.isEmpty()) {
        getBIGraphicsDevice()->setClip(*m_widget, rect);
        m_common->state.clippingRect = rect;
    } else {
        IntRect r(getBIGraphicsDevice()->clip(*m_widget));
        rect.setLocation(rectangle.location() + origin());
        if (r.intersects(rect))
            r.intersect(rect);

        getBIGraphicsDevice()->setClip(*m_widget, r);
        m_common->state.clippingRect = r;
    }
}

void BCGraphicsContext::translate(float x, float y)
{
    m_common->state.origin = m_common->state.origin + IntPoint(static_cast<int>(x), static_cast<int>(y));
}

void BCGraphicsContext::rotate(float)
{
    notImplemented();
}

void BCGraphicsContext::scale(const FloatSize&)
{
    notImplemented();
}

void BCGraphicsContext::addPath(WebCore::Path const&)
{
}

void BCGraphicsContext::beginPath()
{
}

void BCGraphicsContext::clipOutEllipseInRect(WebCore::IntRect const&)
{
}

void BCGraphicsContext::clipOut(WebCore::IntRect const&)
{
}

void BCGraphicsContext::fillRoundedRect(WebCore::IntRect const&, WebCore::IntSize const&, WebCore::IntSize const&, WebCore::IntSize const&, WebCore::IntSize const&, WebCore::Color const&)
{
}

/**
 * Origin may move in the context according to translates that are done
 */
IntPoint BCGraphicsContext::origin()
{
    return m_common->state.origin;   
}

void BCGraphicsContext::setURLForRect(WebCore::KURL const&, WebCore::IntRect const&)
{
}

}
