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
#include "DeprecatedString.h"
#include <cairo.h>

#include <BIMath.h>

#include "BTLogHelper.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define notImplemented() logm(MODULE_GRAPHICS, "not implemented");

// XXX SRO create called only from WindowBal and with 1 arg !
//IMPLEMENT_CREATE_DELETE(BIGraphicsContext, BCGraphicsContext);

namespace BAL {
    // used by some rendering stuff that use a fake GC
BIGraphicsContext* createBIGraphicsContext()
{
    return new BCGraphicsContext((PlatformGraphicsContext*)0);
}

//
// GraphicsContextState
//


struct GraphicsContextState {
    GraphicsContextState() : fillColor(Color::black), paintingDisabled(false) { }
    Font font;
    Pen pen;
    Color fillColor;
    bool paintingDisabled;
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
    bool m_usesInactiveTextBackgroundColor;
    bool m_updatingControlTints;
};

GraphicsContextPrivate::GraphicsContextPrivate()
    : m_focusRingWidth(0)
    , m_focusRingOffset(0)
    , m_usesInactiveTextBackgroundColor(false)
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

        cairo_t* context;
	BINativeImage* nativeImage;
};


GraphicsContextPlatformPrivate::GraphicsContextPlatformPrivate()
    :  context(0)
{
}

GraphicsContextPlatformPrivate::~GraphicsContextPlatformPrivate()
{
    cairo_destroy(context);
}

static inline void setColor(cairo_t* cr, const Color& col)
{
    float red, green, blue, alpha;
    col.getRGBA(red, green, blue, alpha);
    cairo_set_source_rgba(cr, red, green, blue, alpha);
}

// A fillRect helper
static inline void fillRectSourceOver(cairo_t* cr, const FloatRect& rect, const Color& col)
{
    setColor(cr, col);
    cairo_rectangle(cr, rect.x(), rect.y(), rect.width(), rect.height());
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_fill(cr);
}

//
// BCGraphicsContext implementation
//

BCGraphicsContext::BCGraphicsContext(PlatformGraphicsContext* context)
    : m_common(createGraphicsContextPrivate())
    , m_data(new GraphicsContextPlatformPrivate)
{
    m_data->nativeImage = createBCNativeImage(IntSize(800, 600));
    m_data->context = cairo_create(static_cast<BCNativeImage*>(m_data->nativeImage)->m_cairoSurface);
}

BCGraphicsContext::~BCGraphicsContext()
{
    destroyGraphicsContextPrivate(m_common);
    delete m_data;
}

cairo_t* BCGraphicsContext::platformContext() const
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

    savePlatformState();
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

    restorePlatformState();
}

const Font& BCGraphicsContext::font() const
{
    return m_common->state.font;
}

void BCGraphicsContext::setFont(const Font& aFont)
{
    m_common->state.font = aFont;
}

const Pen& BCGraphicsContext::pen() const
{
    return m_common->state.pen;
}

void BCGraphicsContext::setPen(const Pen& pen)
{
    m_common->state.pen = pen;
}

void BCGraphicsContext::setPen(Pen::PenStyle style)
{
    m_common->state.pen.setStyle(style);
    m_common->state.pen.setColor(Color::black);
    m_common->state.pen.setWidth(0);
}

void BCGraphicsContext::setPen(RGBA32 rgb)
{
    m_common->state.pen.setStyle(Pen::SolidLine);
    m_common->state.pen.setColor(rgb);
    m_common->state.pen.setWidth(0);
}

void BCGraphicsContext::setFillColor(const Color& color)
{
    m_common->state.fillColor = color;
}

Color BCGraphicsContext::fillColor() const
{
    return m_common->state.fillColor;
}

void BCGraphicsContext::setUsesInactiveTextBackgroundColor(bool u)
{
    m_common->m_usesInactiveTextBackgroundColor = u;
}

bool BCGraphicsContext::usesInactiveTextBackgroundColor() const
{
    return m_common->m_usesInactiveTextBackgroundColor;
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

void BCGraphicsContext::drawImage(BINativeImage* image, const IntPoint& dest, const IntRect& srcRect, CompositeOperator op)
{
    drawImage(image, IntRect(dest, srcRect.size()), srcRect, op);
}

void BCGraphicsContext::drawImage(BINativeImage* image, const IntRect& dest, const IntRect& srcRect, CompositeOperator op)
{
    drawImage(image, FloatRect(dest), srcRect, op);
}

void BCGraphicsContext::drawText(const TextRun& run, const IntPoint& point, const TextStyle& style)
{
    if (paintingDisabled())
        return;
    // Set the text color to use for drawing.
    float red, green, blue, alpha;
    Color penColor = pen().color();
    penColor.getRGBA(red, green, blue, alpha);
    cairo_set_source_rgba(m_data->context, red, green, blue, alpha);

    font().drawText(this, run, style, point);

    cairo_status_t crStatus = cairo_status(m_data->context);
    if (CAIRO_STATUS_SUCCESS != crStatus) {
        logml(MODULE_GRAPHICS, LEVEL_WARNING, make_message("cairo status : %s", cairo_status_to_string(crStatus)));
    }
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

    ASSERT(tsw != -1);
    ASSERT(tsh != -1);
    ASSERT(tw != -1);
    ASSERT(th != -1);

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

    if (hRule == WebCore::StretchTile && vRule == WebCore::StretchTile)
        // Just do a scale.
        return drawImage(image, dest, srcRect);

    // FIXME RME this->realDrawTiled(image, dest, srcRect, hRule, vRule, op);
}

static void setCompositingOperation(cairo_t* context, CompositeOperator op, bool hasAlpha)
{
    // FIXME: Add support for more operators.
    // FIXME: This should really move to be a graphics context function once we have
    // a C++ abstraction for GraphicsContext.
    if (op == CompositeSourceOver && !hasAlpha)
        op = WebCore::CompositeCopy;

    if (op == WebCore::CompositeCopy)
        cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
    else
        cairo_set_operator(context, CAIRO_OPERATOR_OVER);
}

void BCGraphicsContext::realDraw(
  BINativeImage* aNativeImage, const FloatRect& dst, const FloatRect& src, CompositeOperator op)
{
    if (!aNativeImage)
        return;

    cairo_t* context = m_data->context;

    FloatRect srcRect(src);
    FloatRect dstRect(dst);

    cairo_save(context);

    // Set the compositing operation.
    setCompositingOperation(context, op, aNativeImage->hasAlpha());

    BCNativeImage* aCairoImage = static_cast<BCNativeImage*>(aNativeImage);

    // If we're drawing a sub portion of the image or scaling then create
    // a pattern transformation on the image and draw the transformed pattern.
    // Test using example site at http://www.meyerweb.com/eric/css/edge/complexspiral/demo.html
    cairo_pattern_t* pattern = cairo_pattern_create_for_surface(aCairoImage->m_cairoSurface);
    float scaleX = srcRect.width() / dstRect.width();
    float scaleY = srcRect.height() / dstRect.height();
    cairo_matrix_t mat = { scaleX,  0, 0 , scaleY, srcRect.x(), srcRect.y() };
    cairo_pattern_set_matrix(pattern, &mat);

    // Draw the image.
    cairo_translate(context, dstRect.x(), dstRect.y());
    cairo_set_source(context, pattern);
    cairo_rectangle(context, 0, 0, dstRect.width(), dstRect.height());
    cairo_fill(context);

    cairo_restore(context);
}

void realDrawTiled(BINativeImage*, const FloatRect& dstRect, const FloatRect& srcRect, WebCore::TileRule hRule,   WebCore::TileRule vRule, CompositeOperator)
{
  BALNotImplemented();
}

void BCGraphicsContext::realDrawTiled(
  BINativeImage* image, const FloatRect& dstRect, const FloatPoint& srcPoint, const FloatSize& tileSize, CompositeOperator op)
{
  if (!image)
    return;

  cairo_t* ctxt = m_data->context;

  IntSize intrinsicImageSize = image->size();
  FloatRect srcRect(srcPoint, intrinsicImageSize);
  FloatPoint point = srcPoint;

  // Check and see if a single draw of the image can cover the entire area we are supposed to tile.
  float tileWidth = intrinsicImageSize.width();
  float tileHeight = intrinsicImageSize.height();

  // If the scale is not equal to the intrinsic size of the image, set transform matrix
  // to the appropriate scalar matrix, scale the source point, and set the size of the
  // scaled tile.
  float scaleX = 1.0;
  float scaleY = 1.0;
  cairo_matrix_t mat;
  cairo_matrix_init_identity(&mat);
  if (tileSize.width() != intrinsicImageSize.width() || tileSize.height() != intrinsicImageSize.height()) {
      scaleX = intrinsicImageSize.width() / tileSize.width();
      scaleY = intrinsicImageSize.height() / tileSize.height();
      cairo_matrix_init_scale(&mat, scaleX, scaleY);

      tileWidth = tileSize.width();
      tileHeight = tileSize.height();
  }

  // We could get interesting source offsets (negative ones or positive ones.  Deal with both
  // out of bounds cases.
  float dstTileX = dstRect.x() + fmodf(fmodf(-point.x(), tileWidth) - tileWidth, tileWidth);
  float dstTileY = dstRect.y() + fmodf(fmodf(-point.y(), tileHeight) - tileHeight, tileHeight);
  FloatRect dstTileRect(dstTileX, dstTileY, tileWidth, tileHeight);

  float srcX = dstRect.x() - dstTileRect.x();
  float srcY = dstRect.y() - dstTileRect.y();

  // If the single image draw covers the whole area, then just draw once.
  if (dstTileRect.contains(dstRect)) {
      realDraw(image, dstRect,
            FloatRect(srcX * scaleX, srcY * scaleY, dstRect.width() * scaleX, dstRect.height() * scaleY),
            CompositeSourceOver);
      return;
  }

  cairo_save(ctxt);

  // Set the compositing operation.
  // FIXME: This should be part of the drawTiled API.
  setCompositingOperation(ctxt, CompositeSourceOver, image->hasAlpha());

  BCNativeImage* aBCImage = static_cast<BCNativeImage*>(image);
  cairo_translate(ctxt, dstTileRect.x(), dstTileRect.y());
  cairo_pattern_t* pattern = cairo_pattern_create_for_surface(aBCImage->m_cairoSurface);
  cairo_pattern_set_matrix(pattern, &mat);
  cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

  // Draw the image.
  cairo_set_source(ctxt, pattern);
  cairo_rectangle(ctxt, srcX, srcY, dstRect.width(), dstRect.height());
  cairo_fill(ctxt);
  cairo_restore(ctxt);
}

//
// Code formerly in platform/cairo/GraphicsContextCairo.cpp
//

void BCGraphicsContext::savePlatformState()
{
    cairo_save(m_data->context);
}

void BCGraphicsContext::restorePlatformState()
{
    cairo_restore(m_data->context);
}

// Draws a filled rectangle with a stroked border.
void BCGraphicsContext::drawRect(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    cairo_t* context = m_data->context;
    if (fillColor().alpha())
        fillRectSourceOver(context, rect, fillColor());

    if (pen().style() != Pen::NoPen) {
        setColor(context, pen().color());
        FloatRect r(rect);
        r.inflate(-.5f);
        cairo_rectangle(context, r.x(), r.y(), r.width(), r.height());
        cairo_set_line_width(context, 1.0);
        cairo_stroke(context);
    }
}

// FIXME: Now that this is refactored, it should be shared by all contexts.
static void adjustLineToPixelBounderies(FloatPoint& p1, FloatPoint& p2, float strokeWidth, const Pen::PenStyle& penStyle)
{
    // For odd widths, we add in 0.5 to the appropriate x/y so that the float arithmetic
    // works out.  For example, with a border width of 3, KHTML will pass us (y1+y2)/2, e.g.,
    // (50+53)/2 = 103/2 = 51 when we want 51.5.  It is always true that an even width gave
    // us a perfect position, but an odd width gave us a position that is off by exactly 0.5.
    if (penStyle == Pen::DotLine || penStyle == Pen::DashLine) {
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

    cairo_t* context = m_data->context;
    cairo_save(context);

    Pen::PenStyle penStyle = pen().style();
    if (penStyle == Pen::NoPen)
        return;
    float width = pen().width();
    if (width < 1)
        width = 1;

    FloatPoint p1 = point1;
    FloatPoint p2 = point2;
    bool isVerticalLine = (p1.x() == p2.x());

    adjustLineToPixelBounderies(p1, p2, width, penStyle);
    cairo_set_line_width(context, width);

    int patWidth = 0;
    switch (penStyle) {
        case Pen::NoPen:
        case Pen::SolidLine:
            break;
        case Pen::DotLine:
            patWidth = (int)width;
            break;
        case Pen::DashLine:
            patWidth = 3*(int)width;
            break;
    }

    setColor(context, pen().color());

    cairo_set_antialias(context, CAIRO_ANTIALIAS_NONE);

    if (patWidth) {
        // Do a rect fill of our endpoints.  This ensures we always have the
        // appearance of being a border.  We then draw the actual dotted/dashed line.
        if (isVerticalLine) {
            fillRectSourceOver(context, FloatRect(p1.x()-width/2, p1.y()-width, width, width), pen().color());
            fillRectSourceOver(context, FloatRect(p2.x()-width/2, p2.y(), width, width), pen().color());
        } else {
            fillRectSourceOver(context, FloatRect(p1.x()-width, p1.y()-width/2, width, width), pen().color());
            fillRectSourceOver(context, FloatRect(p2.x(), p2.y()-width/2, width, width), pen().color());
        }

        // Example: 80 pixels with a width of 30 pixels.
        // Remainder is 20.  The maximum pixels of line we could paint
        // will be 50 pixels.
        int distance = (isVerticalLine ? (point2.y() - point1.y()) : (point2.x() - point1.x())) - 2*(int)width;
        int remainder = distance%patWidth;
        int coverage = distance-remainder;
        int numSegments = coverage/patWidth;

        float patternOffset = 0;
        // Special case 1px dotted borders for speed.
        if (patWidth == 1)
            patternOffset = 1.0;
        else {
            bool evenNumberOfSegments = numSegments%2 == 0;
            if (remainder)
                evenNumberOfSegments = !evenNumberOfSegments;
            if (evenNumberOfSegments) {
                if (remainder) {
                    patternOffset += patWidth - remainder;
                    patternOffset += remainder/2;
                }
                else
                    patternOffset = patWidth/2;
            }
            else if (!evenNumberOfSegments) {
                if (remainder)
                    patternOffset = (patWidth - remainder)/2;
            }
        }

        double dash = patWidth;
        cairo_set_dash(context, &dash, 1, patternOffset);
    }

    cairo_move_to(context, p1.x(), p1.y());
    cairo_line_to(context, p2.x(), p2.y());

    cairo_stroke(context);
    cairo_restore(context);
}

// This method is only used to draw the little circles used in lists.
void BCGraphicsContext::drawEllipse(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    cairo_t* context = m_data->context;
    cairo_save(context);
    float yRadius = .5 * rect.height();
    float xRadius = .5 * rect.width();
    cairo_translate(context, rect.x() + xRadius, rect.y() + yRadius);
    cairo_scale(context, xRadius, yRadius);
    cairo_arc(context, 0., 0., 1., 0., 2 * M_PI);
    cairo_restore(context);

    if (fillColor().alpha()) {
        setColor(context, fillColor());
        cairo_fill(context);
    }
    if (pen().style() != Pen::NoPen) {
        setColor(context, pen().color());
        unsigned penWidth = pen().width();
        if (penWidth == 0)
            penWidth++;
        cairo_set_line_width(context, penWidth);
        cairo_stroke(context);
    }
}

// FIXME: This function needs to be adjusted to match the functionality on the Mac side.
void BCGraphicsContext::drawArc(const IntRect& rect, float /*thickness*/, int startAngle, int angleSpan)
{
    if (paintingDisabled())
        return;

    int x = rect.x();
    int y = rect.y();
    float w = (float)rect.width();

    cairo_t* context = m_data->context;
    if (pen().style() != Pen::NoPen) {
        float r = w / 2;
        float fa = startAngle;
        float falen =  fa + angleSpan;
        cairo_arc_negative(context, x + r, y + r, r, -fa * M_PI/180, -falen * M_PI/180);

        setColor(context, pen().color());
        cairo_set_line_width(context, pen().width());
        cairo_stroke(context);
    }
}

void BCGraphicsContext::drawConvexPolygon(size_t npoints, const FloatPoint* points, bool shouldAntialias)
{
    if (paintingDisabled())
        return;

    if (npoints <= 1)
        return;

    cairo_t* context = m_data->context;

    cairo_save(context);
    cairo_set_antialias(context, shouldAntialias ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);
    cairo_move_to(context, points[0].x(), points[0].y());
    for (size_t i = 1; i < npoints; i++)
        cairo_line_to(context, points[i].x(), points[i].y());
    cairo_close_path(context);

    if (fillColor().alpha()) {
        setColor(context, fillColor());
        cairo_set_fill_rule(context, CAIRO_FILL_RULE_EVEN_ODD);
        cairo_fill(context);
    }

    if (pen().style() != Pen::NoPen) {
        setColor(context, pen().color());
        cairo_set_line_width(context, pen().width());
        cairo_stroke(context);
    }
    cairo_restore(context);
}

void BCGraphicsContext::fillRect(const IntRect& rect, const Color& color)
{
    if (paintingDisabled())
        return;

    if (color.alpha())
        fillRectSourceOver(m_data->context, rect, color);
}

void BCGraphicsContext::fillRect(const FloatRect& rect, const Color& color)
{
    if (paintingDisabled())
        return;

    if (color.alpha())
        fillRectSourceOver(m_data->context, rect, color);
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
    // FIXME: These rects should be rounded
    cairo_rectangle(m_data->context, finalFocusRect.x(), finalFocusRect.y(), finalFocusRect.width(), finalFocusRect.height());

    // Force the alpha to 50%.  This matches what the Mac does with outline rings.
    Color ringColor(color.red(), color.green(), color.blue(), 127);
    setColor(m_data->context, ringColor);
    cairo_stroke(m_data->context);
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

void BCGraphicsContext::drawLineForText(const IntPoint& point, int yOffset, int width, bool /*printing*/)
{
    if (paintingDisabled())
        return;

    IntPoint origin = point + IntSize(0, yOffset + 1);
    IntPoint endPoint = origin + IntSize(width, 0);
    drawLine(origin, endPoint);
}

void BCGraphicsContext::drawLineForMisspelling(const IntPoint&, int /*width*/)
{
    notImplemented(); // FIXME: Implement.
}

FloatRect BCGraphicsContext::roundToDevicePixels(const FloatRect& frect)
{
    FloatRect result;
    double x =frect.x();
    double y = frect.y();
    cairo_t* context = m_data->context;
    cairo_user_to_device(context,&x,&y);
    x = round(x);
    y = round(y);
    cairo_device_to_user(context,&x,&y);
    result.setX((float)x);
    result.setY((float)y);
    x = frect.width();
    y = frect.height();
    cairo_user_to_device_distance(context,&x,&y);
    x = round(x);
    y = round(y);
    cairo_device_to_user_distance(context,&x,&y);
    result.setWidth((float)x);
    result.setHeight((float)y);
    return result;
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

void BCGraphicsContext::concatCTM(const BAL::BTAffineTransform&)
{
    /*if (paintingDisabled())
        return;

    m_data->p().setMatrix(transform, true);*/
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
void BCGraphicsContext::beginTransparencyLayer(float) { notImplemented(); }
void BCGraphicsContext::endTransparencyLayer() { notImplemented(); }
Color BCGraphicsContext::selectedTextBackgroundColor() const { return Color(0,0,255); }
void BCGraphicsContext::clearRect(const FloatRect&) { notImplemented(); }
void BCGraphicsContext::strokeRect(const FloatRect&, float) { notImplemented(); }
void BCGraphicsContext::setLineWidth(float) { notImplemented(); }
void BCGraphicsContext::setLineCap(LineCap) { notImplemented(); }
void BCGraphicsContext::setLineJoin(LineJoin) { notImplemented(); }
void BCGraphicsContext::setMiterLimit(float) { notImplemented(); }
void BCGraphicsContext::setAlpha(float) { notImplemented(); }
void BCGraphicsContext::setCompositeOperation(CompositeOperator) { notImplemented(); }
void BCGraphicsContext::clip(const Path&) { notImplemented(); }
void BCGraphicsContext::clip(const IntRect& rect) {
    if (paintingDisabled())
       return;

    cairo_t* context = m_data->context;
    cairo_rectangle(context, rect.x(), rect.y(), rect.width(), rect.height());
    cairo_clip(context);
}
void BCGraphicsContext::translate(const FloatSize&) { notImplemented(); }
void BCGraphicsContext::rotate(float) { notImplemented(); }

KRenderingDeviceContext* BCGraphicsContext::createRenderingDeviceContext() { notImplemented(); };
void BCGraphicsContext::scale(const FloatSize&) { notImplemented(); }

}
