/*
 * Copyright (C) 2003, 2006, 2007 Apple Inc.  All rights reserved.
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

#ifndef BIGraphicsContext_h
#define BIGraphicsContext_h

#include "BINativeImage.h"
#include "FloatRect.h"
#include "GraphicsTypes.h"
#include "Image.h"
#include "IntRect.h"
#include "Path.h"
#include "TextDirection.h"
#include <wtf/Noncopyable.h>
#include <wtf/Platform.h>

namespace WebCore {

    class AffineTransform;
    class GraphicsContextPrivate;
    class GraphicsContextPlatformPrivate;
    class KURL;
    class Path;
    class TextStyle;
}

using WebCore::LineCap;
using WebCore::LineJoin;
using WebCore::Color;
using WebCore::CompositeOperator;
using WebCore::CompositeSourceOver;
using WebCore::FloatPoint;
using WebCore::TextStyle;
using WebCore::IntPoint;
using WebCore::IntRect;
using WebCore::Path;
using WebCore::FloatRect;
using WebCore::FloatSize;
using WebCore::KURL;
//using WebCore::AffineTransform;

namespace BAL {

    class BINativeImage;
    class Font;
    class TextRun;
    class AffineTransform;

    const int cMisspellingLineThickness = 3;
    const int cMisspellingLinePatternWidth = 4;
    const int cMisspellingLinePatternGapWidth = 1;

    // These bits can be ORed together for a total of 8 possible text drawing modes.
    const int cTextInvisible = 0;
    const int cTextFill = 1;
    const int cTextStroke = 2;
    const int cTextClip = 4;

    enum StrokeStyle {
        NoStroke,
        SolidStroke,
        DottedStroke,
        DashedStroke
    };

    class BIGraphicsContext : Noncopyable {
    public:

        enum TileRule { StretchTile, RepeatTile };

        virtual ~BIGraphicsContext() {}

        virtual BINativeImage* getNativeImage() = 0;

        virtual const Font& font() const = 0;
        virtual void setFont(const Font&) = 0;

        virtual float strokeThickness() const = 0;
        virtual void setStrokeThickness(float) = 0;
        virtual StrokeStyle strokeStyle() const = 0;
        virtual void setStrokeStyle(const StrokeStyle& style) = 0;
        virtual Color strokeColor() const = 0;
        virtual void setStrokeColor(const Color&) = 0;

        virtual Color fillColor() const = 0;
        virtual void setFillColor(const Color&) = 0;

        virtual void save() = 0;
        virtual void restore() = 0;

        // These draw methods will do both stroking and filling.
        virtual void drawRect(const IntRect&) = 0;
        virtual void drawLine(const IntPoint&, const IntPoint&) = 0;
        virtual void drawEllipse(const IntRect&) = 0;
        virtual void drawConvexPolygon(size_t numPoints, const FloatPoint*, bool shouldAntialias = false) = 0;

        // Arc drawing (used by border-radius in CSS) just supports stroking at the moment.
        virtual void strokeArc(const IntRect&, int startAngle, int angleSpan) = 0;

        virtual void fillRect(const IntRect&, const Color&) = 0;
        virtual void fillRect(const FloatRect&, const Color&) = 0;
        virtual void fillRoundedRect(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight, const Color&) = 0;
        virtual void clearRect(const FloatRect&) = 0;
        virtual void strokeRect(const FloatRect&, float lineWidth) = 0;

        virtual void drawImage(BINativeImage*, const IntPoint&, CompositeOperator = CompositeSourceOver) = 0;
        virtual void drawImage(BINativeImage*, const IntRect&, CompositeOperator = CompositeSourceOver) = 0;
        virtual void drawImage(BINativeImage*, const IntPoint& destPoint, const IntRect& srcRect, CompositeOperator = CompositeSourceOver) = 0;
        virtual void drawImage(BINativeImage*, const IntRect& destRect, const IntRect& srcRect, CompositeOperator = CompositeSourceOver) = 0;
        virtual void drawImage(BINativeImage*, const FloatRect& destRect, const FloatRect& srcRect = FloatRect(0, 0, -1, -1),
                       CompositeOperator = CompositeSourceOver) = 0;
        virtual void drawTiledImage(BINativeImage*, const IntRect& destRect, const IntPoint& srcPoint, const IntSize& tileSize,
                       CompositeOperator = CompositeSourceOver) = 0;
        virtual void drawTiledImage(BINativeImage*, const IntRect& destRect, const IntRect& srcRect,
                            TileRule hRule = StretchTile, TileRule vRule = StretchTile,
                            CompositeOperator = CompositeSourceOver) = 0;

        virtual void clip(const IntRect&) = 0;
        virtual void addRoundedRectClip(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight) = 0;
        virtual void addInnerRoundedRectClip(const IntRect&, int thickness) = 0;
        virtual void clipOut(const IntRect&) = 0;
        virtual void clipOutEllipseInRect(const IntRect&) = 0;
        virtual void clipOutRoundedRect(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight) = 0;

        // Functions to work around bugs in focus ring clipping on Mac.
        virtual void setFocusRingClip(const IntRect&) = 0;
        virtual void clearFocusRingClip() = 0;

        virtual int textDrawingMode() = 0;
        virtual void setTextDrawingMode(int) = 0;

        virtual void drawText(const TextRun&, const IntPoint&) = 0;
        virtual void drawText(const TextRun&, const IntPoint&, const TextStyle&) = 0;
        virtual void drawHighlightForText(const TextRun&, const IntPoint&, int h, const TextStyle&, const Color& backgroundColor) = 0;

        virtual FloatRect roundToDevicePixels(const FloatRect&) = 0;

        virtual void drawLineForText(const IntPoint&, int width, bool printing) = 0;
        virtual void drawLineForMisspellingOrBadGrammar(const IntPoint&, int width, bool grammar) = 0;

        virtual bool paintingDisabled() const = 0;
        virtual void setPaintingDisabled(bool) = 0;

        virtual bool updatingControlTints() const = 0;
        virtual void setUpdatingControlTints(bool) = 0;

        virtual void beginTransparencyLayer(float opacity) = 0;
        virtual void endTransparencyLayer() = 0;

        virtual void setShadow(const IntSize&, int blur, const Color&) = 0;
        virtual void clearShadow() = 0;

        virtual void initFocusRing(int width, int offset) = 0;
        virtual void addFocusRingRect(const IntRect&) = 0;
        virtual void drawFocusRing(const Color&) = 0;
        virtual void clearFocusRing() = 0;
        virtual IntRect focusRingBoundingRect() = 0;

        virtual void setLineCap(LineCap) = 0;
        virtual void setLineJoin(LineJoin) = 0;
        virtual void setMiterLimit(float) = 0;

        virtual void setAlpha(float) = 0;

        virtual void setCompositeOperation(CompositeOperator) = 0;

        virtual void beginPath() = 0;
        virtual void addPath(const Path& path) = 0;

        virtual void clip(const Path&) = 0;

        virtual void scale(const FloatSize&) = 0;
        virtual void rotate(float angleInRadians) = 0;
        virtual void translate(float x, float y) = 0;
        virtual IntPoint origin() = 0;

        virtual void setURLForRect(const KURL&, const IntRect&) = 0;

        virtual void concatCTM(const AffineTransform&) = 0;

    };

} // namespace BAL

#define IMPLEMENT_BIGRAPHICSCONTEXT  \
    public: \
        virtual BINativeImage* getNativeImage(); \
        virtual const Font& font() const; \
        virtual void setFont(const Font&); \
        virtual float strokeThickness() const; \
        virtual void setStrokeThickness(float); \
        virtual StrokeStyle strokeStyle() const; \
        virtual void setStrokeStyle(const StrokeStyle& style); \
        virtual Color strokeColor() const; \
        virtual void setStrokeColor(const Color&); \
        virtual Color fillColor() const; \
        virtual void setFillColor(const Color&); \
        virtual void save(); \
        virtual void restore(); \
        virtual void drawRect(const IntRect&); \
        virtual void drawLine(const IntPoint&, const IntPoint&); \
        virtual void drawEllipse(const IntRect&); \
        virtual void drawConvexPolygon(size_t numPoints, const FloatPoint*, bool shouldAntialias = false); \
        virtual void strokeArc(const IntRect&, int startAngle, int angleSpan); \
        virtual void fillRect(const IntRect&, const Color&); \
        virtual void fillRect(const FloatRect&, const Color&); \
        virtual void fillRoundedRect(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight, const Color&); \
        virtual void clearRect(const FloatRect&); \
        virtual void strokeRect(const FloatRect&, float lineWidth); \
        virtual void drawImage(BINativeImage*, const IntPoint&, CompositeOperator = CompositeSourceOver); \
        virtual void drawImage(BINativeImage*, const IntRect&, CompositeOperator = CompositeSourceOver); \
        virtual void drawImage(BINativeImage*, const IntPoint& destPoint, const IntRect& srcRect, CompositeOperator = CompositeSourceOver); \
        virtual void drawImage(BINativeImage*, const IntRect& destRect, const IntRect& srcRect, CompositeOperator = CompositeSourceOver); \
        virtual void drawImage(BINativeImage*, const FloatRect& destRect, const FloatRect& srcRect = FloatRect(0, 0, -1, -1), CompositeOperator = CompositeSourceOver); \
        virtual void drawTiledImage(BINativeImage*, const IntRect& destRect, const IntPoint& srcPoint, const IntSize& tileSize, CompositeOperator = CompositeSourceOver); \
        virtual void drawTiledImage(BINativeImage*, const IntRect& destRect, const IntRect& srcRect, TileRule hRule = StretchTile, TileRule vRule = StretchTile, CompositeOperator = CompositeSourceOver); \
        virtual void clip(const IntRect&); \
        virtual void addRoundedRectClip(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight); \
        virtual void addInnerRoundedRectClip(const IntRect&, int thickness); \
        virtual void clipOut(const IntRect&); \
        virtual void clipOutEllipseInRect(const IntRect&); \
        virtual void clipOutRoundedRect(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight); \
        virtual void setFocusRingClip(const IntRect&); \
        virtual void clearFocusRingClip(); \
        virtual int textDrawingMode(); \
        virtual void setTextDrawingMode(int); \
        virtual void drawText(const TextRun&, const IntPoint&); \
        virtual void drawText(const TextRun&, const IntPoint&, const TextStyle&); \
        virtual void drawHighlightForText(const TextRun&, const IntPoint&, int h, const TextStyle&, const Color& backgroundColor); \
        virtual FloatRect roundToDevicePixels(const FloatRect&); \
        virtual void drawLineForText(const IntPoint&, int width, bool printing); \
        virtual void drawLineForMisspellingOrBadGrammar(const IntPoint&, int width, bool grammar); \
        virtual bool paintingDisabled() const; \
        virtual void setPaintingDisabled(bool); \
        virtual bool updatingControlTints() const; \
        virtual void setUpdatingControlTints(bool); \
        virtual void beginTransparencyLayer(float opacity); \
        virtual void endTransparencyLayer(); \
        virtual void setShadow(const IntSize&, int blur, const Color&); \
        virtual void clearShadow(); \
        virtual void initFocusRing(int width, int offset); \
        virtual void addFocusRingRect(const IntRect&); \
        virtual void drawFocusRing(const Color&); \
        virtual void clearFocusRing(); \
        virtual IntRect focusRingBoundingRect(); \
        virtual void setLineCap(LineCap); \
        virtual void setLineJoin(LineJoin); \
        virtual void setMiterLimit(float); \
        virtual void setAlpha(float); \
        virtual void setCompositeOperation(CompositeOperator); \
        virtual void beginPath(); \
        virtual void addPath(const Path& path); \
        virtual void clip(const Path&); \
        virtual void scale(const FloatSize&); \
        virtual void rotate(float angleInRadians); \
        virtual void translate(float x, float y); \
        virtual IntPoint origin(); \
        virtual void setURLForRect(const KURL&, const IntRect&); \
        virtual void concatCTM(const AffineTransform&);

#endif // BIGraphicsContext_h
