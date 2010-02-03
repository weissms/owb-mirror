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
#include "Path.h"

#include "TransformationMatrix.h"
#include "FloatRect.h"
#include "NotImplemented.h"
#include "PlatformString.h"

#include <math.h>
#include <wtf/MathExtras.h>

namespace WebCore {

Path::Path()
//    : m_path(new CairoPath())
{
    //printf("Path::Path\n");
}

Path::~Path()
{
    //printf("Path::~Path\n");
    //delete m_path;
}

Path::Path(const Path& other)
//    : m_path(new CairoPath())
{
    //printf("Path::Path copy\n");
}

Path& Path::operator=(const Path& other)
{
    //printf("Path::operator=\n");
    if (&other == this)
        return *this;

    clear();

    return *this;
}

void Path::clear()
{
    //printf("Path::clear\n");
}

bool Path::isEmpty() const
{
    //printf("Path::isEmpty\n");
    return true;
}

bool Path::hasCurrentPoint() const
{
    return !isEmpty();
}

void Path::translate(const FloatSize& p)
{
    //printf("Path::translate\n");
}

void Path::moveTo(const FloatPoint& p)
{
    //printf("Path::moveTo\n");
}

void Path::addLineTo(const FloatPoint& p)
{
    //printf("Path::addLineTo\n");
}

void Path::addRect(const FloatRect& rect)
{
    //printf("Path::addRect\n");
}

/*
 * inspired by libsvg-cairo
 */
void Path::addQuadCurveTo(const FloatPoint& controlPoint, const FloatPoint& point)
{
    //printf("Path::addQuadCurveTo\n");
}

void Path::addBezierCurveTo(const FloatPoint& controlPoint1, const FloatPoint& controlPoint2, const FloatPoint& controlPoint3)
{
    //printf("Path::addBezierCurveTo\n");

}

void Path::addArc(const FloatPoint& p, float r, float sa, float ea, bool anticlockwise)
{
    //printf("Path::addArc\n");
}

void Path::addArcTo(const FloatPoint& p1, const FloatPoint& p2, float radius)
{
    //printf("Path::addArcTo\n");
    // FIXME: cairo_arc_to not yet in cairo see cairo.h
    // cairo_arc_to(m_cr, p1.x(), p1.y(), p2.x(), p2.y());
    notImplemented();
}

void Path::addEllipse(const FloatRect& rect)
{
    //printf("Path::addEllipse\n");
}

void Path::closeSubpath()
{
    //printf("Path::closeSubpath\n");
}

FloatRect Path::boundingRect() const
{
    //printf("Path::boundingRect\n");
    return FloatRect();
}

FloatRect Path::strokeBoundingRect(StrokeStyleApplier* applier)
{
    return FloatRect();
}

bool Path::contains(const FloatPoint& point, WindRule rule) const
{
    //printf("Path::contains\n");
    return false;
}

bool Path::strokeContains(StrokeStyleApplier* applier, const FloatPoint& point) const
{
    ASSERT(applier);
    //cairo_t* cr = platformPath()->m_cr;
    //GraphicsContext gc(cr);
    //applier->strokeStyle(&gc);

    //return cairo_in_stroke(cr, point.x(), point.y());
    return false;
}

void Path::apply(void* info, PathApplierFunction function) const
{
    //printf("Path::apply\n");
}

void Path::transform(const AffineTransform& transform)
{
    //printf("Path::transform\n");
}

void Path::transform(const TransformationMatrix& trans)
{
    //printf("Path::transform\n");
}

String Path::debugString() const
{
    //printf("Path::debugString\n");
    String string = "";
    
    return string;
}

} // namespace WebCore
