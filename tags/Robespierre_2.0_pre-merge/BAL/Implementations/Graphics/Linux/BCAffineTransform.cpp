/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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
#include "AffineTransform.h"
#include "IntRect.h"
#include "FloatRect.h"
#include "BIMath.h"


using namespace WebCore;

namespace BAL {

AffineTransform::AffineTransform()
:m_m11(1),m_m12(0),m_m21(0),m_m22(1),m_dx(0),m_dy(0)
{
}

AffineTransform::AffineTransform(double a, double b, double c, double d, double tx, double ty)
:m_m11(a),m_m12(b),m_m21(c),m_m22(d),m_dx(tx),m_dy(ty)
{
}


void AffineTransform::setMatrix(double a, double b, double c, double d, double tx, double ty)
{
    m_m11 = a;
    m_m12 = b;
    m_m21 = c;
    m_m22 = d;
    m_dx = tx;
    m_dy = ty;
}

void AffineTransform::map(double x, double y, double *x2, double *y2) const
{
    *x2 = m_dx + x;
    *y2 = m_dy + y;
}

IntRect AffineTransform::mapRect(const IntRect& rect) const
{
    IntRect r ( (int)( m_dx + rect.x()), (int)(m_dy + rect.y()), rect.width(), rect.height() );

    return r;
}

FloatRect AffineTransform::mapRect(const FloatRect& rect) const
{
    FloatRect r ( m_dx + rect.x(), m_dy + rect.y(), rect.width(), rect.height() );

    return r;
}

bool AffineTransform::isIdentity() const
{
    if( ( m_m11 == 1 )
    &&  ( m_m12 == 0 )
    &&  ( m_m21 == 0 )
    &&  ( m_m22 == 1 )
    &&  ( m_dx == 0 )
    &&  ( m_dy == 0 ) )
        return true;

    return false;
}

void AffineTransform::reset()
{
    m_m11 = 1;
    m_m12 = 0;
    m_m21 = 0;
    m_m22 = 1;
    m_dx = 0;
    m_dy = 0;
}

AffineTransform &AffineTransform::scale(double sx, double sy)
{
    m_m11 = m_m11 * sx;
    m_m12 = m_m12 * sx;
    m_m21 = m_m21 * sy;
    m_m22 = m_m11 * sy;

    return *this;
}

AffineTransform &AffineTransform::rotate(double d)
{
    double c = cos(d);
    double s = sin(d);
    m_m11 = m_m11 * c + ( m_m21 * s * -1 );
    m_m12 = m_m12 * c + ( m_m22 * s * -1 );
    m_m21 = m_m11 * s + m_m21 * c;
    m_m22 = m_m12 * s + m_m22 * c;
    m_dx = m_dx * c + ( m_dy * s * -1 );
    m_dy = m_dx * s + m_dy * c;

    return *this;
}

AffineTransform &AffineTransform::translate(double tx, double ty)
{
    m_dx = m_dx + tx;
    m_dy = m_dy + ty;

    return *this;
}

AffineTransform &AffineTransform::shear(double sx, double sy)
{
    m_m11 = m_m11 * sx;
    m_m12 = m_m12 * sx;
    m_m21 = m_m21 * sy;
    m_m22 = m_m11 * sy;

    return *this;
}

double AffineTransform::det() const
{
    return m_m11 * m_m22 - m_m21 * m_m12;
}

/*bool AffineTransform::isInvertible() const
{
}*/

AffineTransform AffineTransform::inverse() const
{
    AffineTransform *ret = new AffineTransform();
    double scalar = 1/det();

    ret->m_m11 = m_m11 * scalar;
    ret->m_m21 = m_m21 * scalar;
    ret->m_m12 = m_m12 * scalar;
    ret->m_m22 = m_m22 * scalar;
    ret->m_dx = m_dx * scalar;
    ret->m_dy = m_dy * scalar;

    return *ret;
}

bool AffineTransform::operator==(const AffineTransform& at) const
{
    if( ( m_m11 == at.m_m11 )
    &&  ( m_m12 == at.m_m12 )
    &&  ( m_m21 == at.m_m21 )
    &&  ( m_m22 == at.m_m22 )
    &&  ( m_dx == at.m_dx )
    &&  ( m_dy == at.m_dy ) )
        return true;

    return false;
}

AffineTransform& AffineTransform::operator*=(const AffineTransform& b)
{
    m_m11 = m_m11 * b.m_m11 + m_m21 * b.m_m12;
    m_m21 = m_m11 * b.m_m21 + m_m21 * b.m_m22;
    m_m12 = m_m12 * b.m_m11 + m_m22 * b.m_m12;
    m_m22 = m_m12 * b.m_m21 + m_m22 * b.m_m22;
    m_dx = m_dx * b.m_m11 + m_dy * b.m_m12 + b.m_dx;
    m_dy = m_dx * b.m_m21 + m_dy * b.m_m22 + b.m_dy;

    return *this;
}

AffineTransform AffineTransform::operator*(const AffineTransform& b)
{
    m_m11 = m_m11 * b.m_m11 + m_m21 * b.m_m12;
    m_m21 = m_m11 * b.m_m21 + m_m21 * b.m_m22;
    m_m12 = m_m12 * b.m_m11 + m_m22 * b.m_m12;
    m_m22 = m_m12 * b.m_m21 + m_m22 * b.m_m22;
    m_dx = m_dx * b.m_m11 + m_dy * b.m_m12 + b.m_dx;
    m_dy = m_dx * b.m_m21 + m_dy * b.m_m22 + b.m_dy;

    return *this;
}

}
