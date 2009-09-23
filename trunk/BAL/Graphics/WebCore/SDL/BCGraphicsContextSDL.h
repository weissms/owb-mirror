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

#ifndef GraphicsContext_h
#define GraphicsContext_h

#include "DashArray.h"
#include "FloatRect.h"
#include "Image.h"
#include "IntRect.h"
#include "Path.h"
#include "TextDirection.h"
#include <wtf/Noncopyable.h>
#include <wtf/Platform.h>
#include "BALBase.h"

namespace WebCore {

    const int cMisspellingLineThickness = 3;
    const int cMisspellingLinePatternWidth = 4;
    const int cMisspellingLinePatternGapWidth = 1;

    class TransformationMatrix;
    class Font;
    class Generator;
    class Gradient;
    class GraphicsContextPrivate;
    class GraphicsContextPlatformPrivate;
    class ImageBuffer;
    class KURL;
    class Path;
    class Pattern;
    class TextRun;

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

// FIXME: This is a place-holder until we decide to add
// real color space support to WebCore.  At that time, ColorSpace will be a
// class and instances will be held  off of Colors.   There will be
// special singleton Gradient and Pattern color spaces to mark when
// a fill or stroke is using a gradient or pattern instead of a solid color.
// https://bugs.webkit.org/show_bug.cgi?id=20558
    enum ColorSpace {
        SolidColorSpace,
        PatternColorSpace,
        GradientColorSpace
    };

    enum InterpolationQuality {
        InterpolationDefault,
        InterpolationNone,
        InterpolationLow,
        InterpolationMedium,
        InterpolationHigh
    };

    class GraphicsContext : public Noncopyable {
    public:
        GraphicsContext(PlatformGraphicsContext*);
        ~GraphicsContext();
       
        PlatformGraphicsContext* platformContext() const;

        float strokeThickness() const;
        void setStrokeThickness(float);
        StrokeStyle strokeStyle() const;
        void setStrokeStyle(const StrokeStyle& style);
        Color strokeColor() const;
        void setStrokeColor(const Color&);

        ColorSpace strokeColorSpace() const;

        void setStrokePattern(PassRefPtr<Pattern>);
        Pattern* strokePattern() const;
        void setStrokeGradient(PassRefPtr<Gradient>);
        Gradient* strokeGradient() const;

        WindRule fillRule() const;
        void setFillRule(WindRule);
        GradientSpreadMethod spreadMethod() const;
        void setSpreadMethod(GradientSpreadMethod);
        Color fillColor() const;
        void setFillColor(const Color&);
        void setFillPattern(PassRefPtr<Pattern>);
        Pattern* fillPattern() const;
        void setFillGradient(PassRefPtr<Gradient>);
        Gradient* fillGradient() const;

        ColorSpace fillColorSpace() const;

        void setShadowsIgnoreTransforms(bool);

        void setShouldAntialias(bool);
        bool shouldAntialias() const;
       
        void save();
        void restore();

        // These draw methods will do both stroking and filling.
        void drawRect(const IntRect&);
        void drawLine(const IntPoint&, const IntPoint&);
        void drawEllipse(const IntRect&);
        void drawConvexPolygon(size_t numPoints, const FloatPoint*, bool shouldAntialias = false);

        void drawPath();
        void fillPath();
        void strokePath();

        // Arc drawing (used by border-radius in CSS) just supports stroking at the moment.
        void strokeArc(const IntRect&, int startAngle, int angleSpan);
        
        void fillRect(const FloatRect&);
        void fillRect(const FloatRect&, const Color&);
        void fillRect(const FloatRect&, Generator&);
        void fillRoundedRect(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight, const Color&);

        void clearRect(const FloatRect&);

        void strokeRect(const FloatRect&);
        void strokeRect(const FloatRect&, float lineWidth);

        void drawImage(Image*, const IntPoint&, CompositeOperator = CompositeSourceOver);
        void drawImage(Image*, const IntRect&, CompositeOperator = CompositeSourceOver, bool useLowQualityScale = false);
        void drawImage(Image*, const IntPoint& destPoint, const IntRect& srcRect, CompositeOperator = CompositeSourceOver);
        void drawImage(Image*, const IntRect& destRect, const IntRect& srcRect, CompositeOperator = CompositeSourceOver, bool useLowQualityScale = false);
        void drawImage(Image*, const FloatRect& destRect, const FloatRect& srcRect = FloatRect(0, 0, -1, -1),
                       CompositeOperator = CompositeSourceOver, bool useLowQualityScale = false);
        void drawTiledImage(Image*, const IntRect& destRect, const IntPoint& srcPoint, const IntSize& tileSize,
                       CompositeOperator = CompositeSourceOver);
        void drawTiledImage(Image*, const IntRect& destRect, const IntRect& srcRect, 
                            Image::TileRule hRule = Image::StretchTile, Image::TileRule vRule = Image::StretchTile,
                            CompositeOperator = CompositeSourceOver);

        void setImageInterpolationQuality(InterpolationQuality) {}
        InterpolationQuality imageInterpolationQuality() const { return InterpolationDefault; }

        void clip(const FloatRect&);
        void addRoundedRectClip(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight);
        void addInnerRoundedRectClip(const IntRect&, int thickness);
        void clipOut(const IntRect&);
        void clipOutEllipseInRect(const IntRect&);
        void clipOutRoundedRect(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight);
        void clipPath(WindRule);

        void clipToImageBuffer(const FloatRect&, const ImageBuffer*);

        int textDrawingMode();
        void setTextDrawingMode(int);

        void drawText(const Font&, const TextRun&, const IntPoint&, int from = 0, int to = -1);
        void drawBidiText(const Font&, const TextRun&, const FloatPoint&);
        void drawHighlightForText(const Font&, const TextRun&, const IntPoint&, int h, const Color& backgroundColor, int from = 0, int to = -1);

        FloatRect roundToDevicePixels(const FloatRect&);
        
        void drawLineForText(const IntPoint&, int width, bool printing);
        void drawLineForMisspellingOrBadGrammar(const IntPoint&, int width, bool grammar);
        
        bool paintingDisabled() const;
        void setPaintingDisabled(bool);
        
        bool updatingControlTints() const;
        void setUpdatingControlTints(bool);

        void beginTransparencyLayer(float opacity);
        void endTransparencyLayer();

        void setShadow(const IntSize&, int blur, const Color&);
        bool getShadow(IntSize&, int&, Color&) const;
        void clearShadow();

        void initFocusRing(int width, int offset);
        void addFocusRingRect(const IntRect&);
        void drawFocusRing(const Color&);
        void clearFocusRing();
        IntRect focusRingBoundingRect();

        void setLineCap(LineCap);
        void setLineDash(const DashArray&, float dashOffset);
        void setLineJoin(LineJoin);
        void setMiterLimit(float);

        void setAlpha(float);

        void setCompositeOperation(CompositeOperator);

        void beginPath();
        void addPath(const Path&);

        void clip(const Path&);
        void clipOut(const Path&);

        void scale(const FloatSize&);
        void rotate(float angleInRadians);
        void translate(float x, float y);
        IntPoint origin();
        
        void setURLForRect(const KURL&, const IntRect&);

        void concatCTM(const TransformationMatrix&);
        TransformationMatrix getCTM() const;

        void setBalExposeEvent(BalEventExpose*);
        BalDrawable* balDrawable() const;
        BalEventExpose* balExposeEvent() const;
        IntPoint translatePoint(const IntPoint&) const;
        float transparencyLayer();
    private:
        void savePlatformState();
        void restorePlatformState();

        void setPlatformTextDrawingMode(int);
        void setPlatformFont(const Font& font);

        void setPlatformStrokeColor(const Color&);
        void setPlatformStrokePattern(Pattern*);
        void setPlatformStrokeGradient(Gradient*);
        void setPlatformStrokeStyle(const StrokeStyle&);
        void setPlatformStrokeThickness(float);

        void setPlatformFillColor(const Color&);
        void setPlatformFillPattern(Pattern*);
        void setPlatformFillGradient(Gradient*);

        void setPlatformShouldAntialias(bool b);

        void setPlatformShadow(const IntSize&, int blur, const Color&);
        void clearPlatformShadow();

        static void adjustLineToPixelBoundaries(FloatPoint& p1, FloatPoint& p2, float strokeWidth, const StrokeStyle&);

        int focusRingWidth() const;
        int focusRingOffset() const;
        const Vector<IntRect>& focusRingRects() const;

        static GraphicsContextPrivate* createGraphicsContextPrivate();
        static void destroyGraphicsContextPrivate(GraphicsContextPrivate*);

        GraphicsContextPrivate* m_common;
        GraphicsContextPlatformPrivate* m_data; // Deprecated; m_commmon can just be downcasted. To be removed.
        BalEventExpose* m_expose;
    };

} // namespace WebCore

#endif // GraphicsContext_h
