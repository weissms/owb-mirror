/*
 * Copyright (C) 2005, 2006 Apple Computer, Inc.  All rights reserved.
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

#ifndef BTAffineTransform_h
#define BTAffineTransform_h

#if PLATFORM(CG)
#include <ApplicationServices/ApplicationServices.h>
#elif PLATFORM(QT)
#include <QMatrix>
#elif PLATFORM(CAIRO)
#include "cairo.h"
#endif

namespace WebCore {

class IntRect;
class FloatRect;

#ifdef __OWB__
}
using WebCore::IntRect;
using WebCore::FloatRect;
namespace BAL {
#endif

class BTAffineTransform {
public:
    BTAffineTransform();
    BTAffineTransform(double a, double b, double c, double d, double e, double f);
#if PLATFORM(CG)
    BTAffineTransform(CGAffineTransform transform);
#elif PLATFORM(QT)
    BTAffineTransform(const QMatrix &matrix);
#elif PLATFORM(CAIRO)
    BTAffineTransform(const cairo_matrix_t &matrix);
#endif

    void setMatrix(double a, double b, double c, double d, double e, double f);
    void map(double x, double y, double *x2, double *y2) const;
    IntRect mapRect(const IntRect&) const;
    FloatRect mapRect(const FloatRect&) const;

    bool isIdentity() const;

    double a() const;
    void setA(double a);

    double b() const;
    void setB(double b);

    double c() const;
    void setC(double c);

    double d() const;
    void setD(double d);

    double e() const;
    void setE(double e);

    double f() const;
    void setF(double f);

    void reset();

    BTAffineTransform& multiply(const BTAffineTransform&);
    BTAffineTransform& scale(double);
    BTAffineTransform& scale(double sx, double sy);
    BTAffineTransform& scaleNonUniform(double sx, double sy);
    BTAffineTransform& rotate(double d);
    BTAffineTransform& rotateFromVector(double x, double y);
    BTAffineTransform& translate(double tx, double ty);
    BTAffineTransform& shear(double sx, double sy);
    BTAffineTransform& flipX();
    BTAffineTransform& flipY();
    BTAffineTransform& skewX(double angle);
    BTAffineTransform& skewY(double angle);

    double det() const;
    bool isInvertible() const;
    BTAffineTransform inverse() const;

#if PLATFORM(CG)
    operator CGAffineTransform() const;
#elif PLATFORM(QT)
    operator QMatrix() const;
#elif PLATFORM(CAIRO)
    operator cairo_matrix_t() const;
#endif

    bool operator==(const BTAffineTransform&) const;
    BTAffineTransform& operator*=(const BTAffineTransform&);
    BTAffineTransform operator*(const BTAffineTransform&);

private:
#if PLATFORM(CG)
    CGAffineTransform m_transform;
#elif PLATFORM(QT)
    QMatrix m_transform;
#elif PLATFORM(CAIRO)
    cairo_matrix_t m_transform;
#elif defined __OWB__
    double m_m11;
    double m_m12;
    double m_m21;
    double m_m22;
    double m_dx;
    double m_dy;
#endif
};

} // namespace WebCore

#endif // BTAffineTransform_h
