/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 *               2006 Rob Buis   <buis@kde.org>
 * Copyright (C) 2007 Pleyo
 * 
 * All rights reserved.
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
#include "Path.h"

#include "BALConfiguration.h"
#include "FloatRect.h"
#include "PlatformString.h"
#include "AffineTransform.h"

#include <math.h>

namespace WebCore {

Path::Path()
{
}

Path::~Path()
{
}

Path::Path(const Path& other)
{
}

Path& Path::operator=(const Path& other)
{
    if (&other != this) {
    }

    return *this;
}

bool Path::contains(const FloatPoint& point, WindRule rule) const
{
    BALNotImplemented();
    bool contains = false;
    return contains;
}

void Path::translate(const FloatSize& size)
{
}

FloatRect Path::boundingRect() const
{
    BALNotImplemented();
    return FloatRect(0,0,0,0);
}

void Path::moveTo(const FloatPoint& point)
{
}

void Path::addLineTo(const FloatPoint& point)
{
}

void Path::addQuadCurveTo(const FloatPoint& cp, const FloatPoint& p)
{
}

void Path::addBezierCurveTo(const FloatPoint& cp1, const FloatPoint& cp2, const FloatPoint& p)
{
}

void Path::addArcTo(const FloatPoint& p1, const FloatPoint& p2, float radius)
{
}

void Path::closeSubpath()
{
}

#define DEGREES(t) ((t) * 180.0 / M_PI)
void Path::addArc(const FloatPoint& p, float r, float sar, float ear, bool anticlockwise)
{
}

void Path::addRect(const FloatRect& r)
{
}

void Path::addEllipse(const FloatRect& r)
{
    BALNotImplemented();
}

void Path::clear()
{
    BALNotImplemented();
}

bool Path::isEmpty() const
{
    return true;
}

void Path::apply(void* info, PathApplierFunction function) const
{
    BALNotImplemented();
}

void Path::transform(const BAL::BTAffineTransform& transform)
{
    BALNotImplemented();
}

}
