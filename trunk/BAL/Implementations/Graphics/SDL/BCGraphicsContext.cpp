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
#include "BCNativeImage.h"
#include "BTAffineTransform.h"
#include "Color.h"
#include "DeprecatedString.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <BIMath.h>

#include "BTLogHelper.h"

using namespace std;
using namespace WebCore;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define notImplemented() logm(MODULE_GRAPHICS, "not implemented");

//FIXME: Should be elsewhere.
void WebCore::setFocusRingColorChangeFunction(void (*)()) { BALNotImplemented(); }
Color WebCore::focusRingColor() { return 0x000000ff; }

namespace BAL {
    // used by some rendering stuff that use a fake GC
BIGraphicsContext* createBIGraphicsContext()
{
    return new BCGraphicsContext(true);
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
    SDL_Rect clippingRect;
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

        SDL_Surface* context;
        BINativeImage* nativeImage;
};


GraphicsContextPlatformPrivate::GraphicsContextPlatformPrivate()
    :  context(0)
{
}

GraphicsContextPlatformPrivate::~GraphicsContextPlatformPrivate()
{
    delete context;
}

// A fillRect helper
static inline void fillRectSourceOver(SDL_Surface* context, const FloatRect& rect, const Color& col)
{
    boxRGBA(context, static_cast<Sint16>(rect.x()), static_cast<Sint16>(rect.y()),
                    static_cast<Sint16>(rect.x() + rect.width()), static_cast<Sint16>(rect.y() + rect.height()),
                    col.red(), col.green(), col.blue(), col.alpha());
}

//
// BCGraphicsContext implementation
//

BCGraphicsContext::BCGraphicsContext(bool isReal)
    : m_common(createGraphicsContextPrivate())
    , m_data(new GraphicsContextPlatformPrivate)
    , m_alphaLayerValue(1.0)
{
    if (!isReal)
        m_data->nativeImage = createBCNativeImage(IntSize(0, 0));
    else
        m_data->nativeImage = createBCNativeImage(IntSize(800, 600));
    m_data->context = static_cast<BCNativeImage*>(m_data->nativeImage)->getSurface();
}

BCGraphicsContext::~BCGraphicsContext()
{
    destroyGraphicsContextPrivate(m_common);
    delete m_data;
}

SDL_Surface* BCGraphicsContext::platformContext() const
{
    return m_data->context;
}

GraphicsContextPrivate* BCGraphicsContext::createGraphicsContextPrivate()
{
    return new GraphicsContextPrivate;
}

void BCGraphicsContext::destroyGraphicsContextPrivate(GraphicsContextPrivate* deleteMe)
{
    delete deleteMe;
}

BINativeImage* BCGraphicsContext::getNativeImage()
{
    return m_data->nativeImage;
}

void BCGraphicsContext::save()
{
    if (paintingDisabled())
        return;

    m_common->stack.append(m_common->state);
}

void BCGraphicsContext::restore()
{
    if (paintingDisabled())
        return;

    if (m_common->stack.isEmpty()) {
        LOG_ERROR("ERROR void BCGraphicsContext::restore() stack is empty");
        return;
    }
    m_common->state = m_common->stack.last();
    m_common->stack.removeLast();
    SDL_SetClipRect(m_data->context, NULL);
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

void BCGraphicsContext::drawText(const TextRun& run, const IntPoint& point)
{
    drawText(run, point, TextStyle());
}

void BCGraphicsContext::drawText(const TextRun& run, const IntPoint& point, const TextStyle& style)
{
    if (paintingDisabled())
        return;

    font().drawText(this, run, style, point);
}

void BCGraphicsContext::drawHighlightForText(const TextRun& run, const IntPoint& point, int h, const TextStyle& style, const Color& backgroundColor)
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
    if (!nativeImage)
        return;
    SDL_Rect srcRect, dstRect;
    srcRect.x = static_cast<Sint16>(src.x());
    srcRect.y = static_cast<Sint16>(src.y());
    if (0 == src.width())
        srcRect.w = nativeImage->size().width();
    else
        srcRect.w = static_cast<Uint16>(src.width());
    if (0 == src.height())
        srcRect.h = nativeImage->size().height();
    else
        srcRect.h = static_cast<Uint16>(src.height());
    dstRect.x = static_cast<Sint16>(dst.x());
    dstRect.y = static_cast<Sint16>(dst.y());
    dstRect.w = static_cast<Sint16>(dst.width());
    dstRect.h = static_cast<Sint16>(dst.height());
    SDL_Surface *surface = zoomSurface(static_cast<BCNativeImage*>(nativeImage)->getSurface(),
                        (((double)dst.width()/(double)srcRect.w)),
                        ((double)dst.height()/((double)srcRect.h)),
                        SMOOTHING_OFF);
    srcRect.w = static_cast<Uint16>(dst.width());
    srcRect.h = static_cast<Uint16>(dst.height());
    srcRect.x = 0;
    srcRect.y = 0;

    SDL_BlitSurface(surface, &srcRect, m_data->context, &dstRect);
    SDL_FreeSurface(surface);
}

void realDrawTiled(BINativeImage*, const FloatRect& dstRect, const FloatRect& srcRect, BIGraphicsContext::TileRule hRule, BIGraphicsContext::TileRule vRule, CompositeOperator)
{
  BALNotImplemented();
}

void BCGraphicsContext::realDrawTiled(
  BINativeImage* image, const FloatRect& dstRect, const FloatPoint& srcPoint, const FloatSize& tileSize, CompositeOperator op)
{
    if (!image)
        return;

    SDL_Surface* ctxt = m_data->context;

    float tileWidth = image->size().width();
    float tileHeight = image->size().height();

    // save context info
    save();

    clip(IntRect(dstRect)); // don't draw outside this

    for(float x = dstRect.x(); x < dstRect.x() + dstRect.width(); x+= tileWidth) {
        for(float y = dstRect.y(); y < dstRect.y() + dstRect.height(); y+=tileHeight) {
            realDraw(image, FloatRect(x, y, tileWidth, tileHeight), FloatRect(0, 0, tileWidth, tileHeight), CompositeSourceOver);
        }
    }

    // restore clipping
    restore();
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
void BCGraphicsContext::drawRect(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    SDL_Surface* context = m_data->context;
    fillRectSourceOver(context, rect, fillColor());

    if (strokeStyle() != NoStroke) {
        FloatRect r(rect);
        r.inflate(-.5f);
        rectangleRGBA(context, static_cast<Sint16>(r.x()), static_cast<Sint16>(r.y()),
                        static_cast<Sint16>(r.x() + r.width()), static_cast<Sint16>(r.y() + r.height()),
                        strokeColor().red(), strokeColor().green(), strokeColor().blue(), strokeColor().alpha());
    }
}

// FIXME: Now that this is refactored, it should be shared by all contexts.
static void adjustLineToPixelBounderies(FloatPoint& p1, FloatPoint& p2, float strokeWidth, const StrokeStyle& strokeStyle)
{
    // For odd widths, we add in 0.5 to the appropriate x/y so that the float arithmetic
    // works out.  For example, with a border width of 3, KHTML will pass us (y1+y2)/2, e.g.,
    // (50+53)/2 = 103/2 = 51 when we want 51.5.  It is always true that an even width gave
    // us a perfect position, but an odd width gave us a position that is off by exactly 0.5.
    if (strokeStyle == DottedStroke || strokeStyle == DashedStroke) {
        if (p1.x() == p2.x()) {
            p1.setY(p1.y() + strokeWidth);
            p2.setY(p2.y() - strokeWidth);
        }
        else {
            p1.setX(p1.x() + strokeWidth);
            p2.setX(p2.x() - strokeWidth);
        }
    }

    if (((int)strokeWidth)%2) {
        if (p1.x() == p2.x()) {
            // We're a vertical line.  Adjust our x.
            p1.setX(p1.x() + 0.5);
            p2.setX(p2.x() + 0.5);
        }
        else {
            // We're a horizontal line. Adjust our y.
            p1.setY(p1.y() + 0.5);
            p2.setY(p2.y() + 0.5);
        }
    }
}

// This is only used to draw borders.
void BCGraphicsContext::drawLine(const IntPoint& point1, const IntPoint& point2)
{
    if (paintingDisabled())
        return;
    SDL_Surface* context = m_data->context;

/*    if (strokeStyle() == NoStroke)
        return;*/
    float width = strokeThickness();
    if (width < 1)
        width = 1;

    FloatPoint p1 = point1;
    FloatPoint p2 = point2;

    adjustLineToPixelBounderies(p1, p2, width, strokeStyle());

    lineRGBA(context, static_cast<Sint16>(p1.x()), static_cast<Sint16>(p1.y()),
             static_cast<Sint16>(p2.x()), static_cast<Sint16>(p2.y()),
             strokeColor().red(),
             strokeColor().green(),
             strokeColor().blue(),
             strokeColor().alpha());

}

// This method is only used to draw the little circles used in lists.
void BCGraphicsContext::drawEllipse(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    float yRadius = .5 * rect.height();
    float xRadius = .5 * rect.width();

    if (strokeStyle() != NoStroke) {
        unsigned width = static_cast<unsigned int>(strokeThickness());
        if (width == 0)
            width++;
    }
    ellipseRGBA(m_data->context, static_cast<Sint16>(rect.x() + xRadius), static_cast<Sint16>(rect.y() + yRadius),
                static_cast<Sint16>(xRadius), static_cast<Sint16>(yRadius),
                   strokeColor().red(),
                   strokeColor().green(),
                   strokeColor().blue(),
                   strokeColor().alpha());
}

// FIXME: This function needs to be adjusted to match the functionality on the Mac side.
void BCGraphicsContext::strokeArc(const IntRect& rect, int startAngle, int angleSpan)
{
    // can't draw circles with bezierRGBA
    // the drawMidpointCircle method is not accurate for small circles...
    // so trying a simple way to draw arc

    if (paintingDisabled())
        return;

    int x = rect.x();
    int y = rect.y();
    float w = (float)rect.width();

    if (strokeStyle() != NoStroke) {
        float r = w / 2;
        float fa = startAngle;
        float falen =  fa + angleSpan;

        float xc = x + r;
        float yc = y + r;
        fa *= M_PI/180;
        angleSpan *= static_cast<int>(M_PI/180);

        int x1, x2, y1, y2;
        float cosx1, cosx2, siny1, siny2;
        float angleStep = angleSpan/r;
        float angleCurrent = -fa;
        cosx2 = cos(angleCurrent);
        siny2 = sin(angleCurrent);
        x2 = static_cast<int>(xc + r*cosx2);
        y2 = static_cast<int>(yc + r*siny2);

        for(int i=0; i<r; i++) {
            cosx1 = cosx2;
            siny1 = siny2;
            cosx2 = cos(angleCurrent+angleStep);
            siny2 = sin(angleCurrent+angleStep);

            x1 = x2;
            y1 = y2;
            x2 = static_cast<int>(xc + r*cosx2);
            y2 = static_cast<int>(yc + r*siny2);

            lineRGBA(m_data->context, x1, y1, x2, y2,
                   strokeColor().red(),
                   strokeColor().green(),
                   strokeColor().blue(),
                   strokeColor().alpha());
            angleCurrent += angleStep;
        }
    }
}

void BCGraphicsContext::drawConvexPolygon(size_t npoints, const FloatPoint* points, bool shouldAntialias)
{
    if (paintingDisabled())
        return;

    if (npoints <= 1)
        return;
    Sint16 *vx = new Sint16[npoints];
    Sint16 *vy = new Sint16[npoints];
    for(unsigned int i=0; i<npoints; i++) {
        vx[i] = static_cast<Sint16>(points[i].x());
        vy[i] = static_cast<Sint16>(points[i].y());
    }
    filledPolygonRGBA(m_data->context, vx, vy, npoints,
                   fillColor().red(),
                   fillColor().green(),
                   fillColor().blue(),
                   fillColor().alpha());
    delete[] vx;
    delete[] vy;
}

void BCGraphicsContext::fillRect(const IntRect& rect, const Color& color)
{
    if (paintingDisabled())
        return;

    Color c(color.red(), color.green(), color.blue(), static_cast<int>(color.alpha()*m_alphaLayerValue));
    fillRectSourceOver(m_data->context, rect, c);
}

void BCGraphicsContext::fillRect(const FloatRect& rect, const Color& color)
{
    if (paintingDisabled())
        return;

    Color c(color.red(), color.green(), color.blue(), static_cast<int>(color.alpha()*m_alphaLayerValue));
    fillRectSourceOver(m_data->context, rect, c);
}

void BCGraphicsContext::drawFocusRing(const Color& color)
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
        focusRect.inflate(offset);
        finalFocusRect.unite(focusRect);
    }

    // NOTE These rects are rounded on Mac
    // Force the alpha to 50%.  This matches what the Mac does with outline rings.
    rectangleRGBA(m_data->context, finalFocusRect.x(), finalFocusRect.y(),
                  finalFocusRect.x() + finalFocusRect.width(), finalFocusRect.y() + finalFocusRect.height(),
                  color.red(), color.green(), color.blue(), 127);
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

void BCGraphicsContext::drawLineForText(const IntPoint& point, int width, bool /*printing*/)
{
    if (paintingDisabled())
        return;

    IntPoint endPoint = point + IntSize(width, 0);
    SDL_Surface* context = m_data->context;

    // NOTE we should adjust line to pixel boundaries
    lineRGBA(context, point.x(), point.y(), endPoint.x(), endPoint.y(),
             strokeColor().red(),
             strokeColor().green(),
             strokeColor().blue(),
             strokeColor().alpha());
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
                        const IntSize& /*bottomLeft*/, const IntSize& /*bottomRight*/) {
                            notImplemented();
}
void BCGraphicsContext::addInnerRoundedRectClip(const IntRect& /*rect*/, int /*thickness*/) { notImplemented(); }
void BCGraphicsContext::setShadow(IntSize const&,int,Color const&) { notImplemented(); }
void BCGraphicsContext::clearShadow() { notImplemented(); }
void BCGraphicsContext::beginTransparencyLayer(float f) { m_alphaLayerValue = f; }
void BCGraphicsContext::endTransparencyLayer() { m_alphaLayerValue = 1.0; }
void BCGraphicsContext::clearRect(const FloatRect& rect) {
    logml(MODULE_GRAPHICS, LEVEL_INFO, make_message("Not implemented for %fx%f+%f+%f", rect.width(), rect.height(), rect.x(), rect.y()));
}
void BCGraphicsContext::strokeRect(const FloatRect&, float) { notImplemented(); }
void BCGraphicsContext::setLineCap(LineCap) { notImplemented(); }
void BCGraphicsContext::setLineJoin(LineJoin) { notImplemented(); }
void BCGraphicsContext::setMiterLimit(float) { notImplemented(); }
void BCGraphicsContext::setAlpha(float) { notImplemented(); }
void BCGraphicsContext::setCompositeOperation(CompositeOperator) { notImplemented(); }
void BCGraphicsContext::clip(const Path&) { notImplemented(); }
void BCGraphicsContext::clip(const IntRect& rect) {

    if (paintingDisabled())
        return;
    if (rect.isEmpty()) {
        SDL_SetClipRect(m_data->context, NULL);
        m_common->state.clippingRect.x = 0;
        m_common->state.clippingRect.y = 0;
        m_common->state.clippingRect.w = 0;
        m_common->state.clippingRect.h = 0;
    }
    else {
        SDL_Rect sdlRect;
        SDL_GetClipRect(m_data->context, &sdlRect);
        IntRect r(sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
        r.intersect(rect);
        sdlRect.x = r.x();
        sdlRect.y = r.y();
        sdlRect.w = r.width();
        sdlRect.h = r.height();
        SDL_SetClipRect(m_data->context, &sdlRect);
        m_common->state.clippingRect = sdlRect;
    }
}
void BCGraphicsContext::translate(float x, float y) { notImplemented(); }
void BCGraphicsContext::rotate(float) { notImplemented(); }
void BCGraphicsContext::scale(const FloatSize&) { notImplemented(); }

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
IntPoint BCGraphicsContext::origin()
{
}
void BCGraphicsContext::setURLForRect(WebCore::KURL const&, WebCore::IntRect const&)
{
}

}
