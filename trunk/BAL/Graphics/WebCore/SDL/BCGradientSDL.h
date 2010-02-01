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

#ifndef Gradient_h
#define Gradient_h

#include "Generator.h"

#include "FloatPoint.h"
#include "BALBase.h"
#include "GraphicsTypes.h"
#include "TransformationMatrix.h"
#include <wtf/PassRefPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

    class Color;
    class String;

    class Gradient : public Generator {
    public:
        static PassRefPtr<Gradient> create(const FloatPoint& p0, const FloatPoint& p1)
        {
            return adoptRef(new Gradient(p0, p1));
        }
        static PassRefPtr<Gradient> create(const FloatPoint& p0, float r0, const FloatPoint& p1, float r1)
        {
            return adoptRef(new Gradient(p0, r0, p1, r1));
        }
        virtual ~Gradient();

        void addColorStop(float, const String&);
        void addColorStop(float, const Color&);

        void getColor(float value, float* r, float* g, float* b, float* a) const;

        PlatformGradient platformGradient();

        struct ColorStop {
            float stop;
            float red;
            float green;
            float blue;
            float alpha;
            
            ColorStop() : stop(0), red(0), green(0), blue(0), alpha(0) { }
            ColorStop(float s, float r, float g, float b, float a) : stop(s), red(r), green(g), blue(b), alpha(a) { }
        };

        void setStopsSorted(bool s) { m_stopsSorted = s; }
        void setSpreadMethod(GradientSpreadMethod);
        GradientSpreadMethod spreadMethod() { return m_spreadMethod; }
        void setGradientSpaceTransform(const TransformationMatrix& gradientSpaceTransformation);
        // Qt and CG transform the gradient at draw time
        TransformationMatrix gradientSpaceTransform() { return m_gradientSpaceTransformation; }

        virtual void fill(GraphicsContext*, const FloatRect&);
        virtual void adjustParametersForTiledDrawing(IntSize& size, FloatRect& srcRect);

        void setPlatformGradientSpaceTransform(const TransformationMatrix& gradientSpaceTransformation);

    private:
        Gradient(const FloatPoint& p0, const FloatPoint& p1);
        Gradient(const FloatPoint& p0, float r0, const FloatPoint& p1, float r1);
 
        void platformInit() { m_gradient = 0; }
        void platformDestroy();

        int findStop(float value) const;
        void sortStopsIfNecessary();

        bool m_radial;
        FloatPoint m_p0, m_p1;
        float m_r0, m_r1;
        mutable Vector<ColorStop> m_stops;
        mutable bool m_stopsSorted;
        mutable int m_lastStop;
        GradientSpreadMethod m_spreadMethod;
        TransformationMatrix m_gradientSpaceTransformation;

        PlatformGradient m_gradient;
    };

} //namespace

#endif
