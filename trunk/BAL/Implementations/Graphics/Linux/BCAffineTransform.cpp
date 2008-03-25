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
#include "BALConfiguration.h"
#include "BTLogHelper.h"

#include "AffineTransform.h"
#include "IntRect.h"
#include "FloatRect.h"
#include "BIMath.h"
#include "MathExtras.h"

using namespace WebCore;

namespace BAL {

AffineTransform::AffineTransform()
    : m_m11(1)
    , m_m12(0)
    , m_m21(0)
    , m_m22(1)
    , m_dx(0)
    , m_dy(0)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::AffineTransform1\n");
}

AffineTransform::AffineTransform(double a, double b, double c, double d, double tx, double ty)
    : m_m11(a)
    , m_m12(b)
    , m_m21(c)
    , m_m22(d)
    , m_dx(tx)
    , m_dy(ty)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::AffineTransform2\n");
}


void AffineTransform::setMatrix(double a, double b, double c, double d, double tx, double ty)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::setMatrix\n");
    m_m11 = a;
    m_m12 = b;
    m_m21 = c;
    m_m22 = d;
    m_dx = tx;
    m_dy = ty;
}

void AffineTransform::map(double x, double y, double *x2, double *y2) const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::map\n");
    *x2 = m_m11 * x + m_m21 * y + m_dx;
    *y2 = m_m22 * y + m_m12 * x + m_dy;
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "after AffineTransform::map x =%f y =%f\n", *x2, *y2);

}

IntRect AffineTransform::mapRect(const IntRect& rect) const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::mapRect1\n");
    IntRect result;
    if (m_m12 == 0.0F && m_m21 == 0.0F) {
        int x = lround(m_m11 * rect.x() + m_dx);
        int y = lround(m_m22 * rect.y() + m_dy);
        int w = lround(m_m11 * rect.width());
        int h = lround(m_m22 * rect.height());
        if (w < 0) {
            w = -w;
            x -= w;
        }
        if (h < 0) {
            h = -h;
            y -= h;
        }
        result = IntRect(x, y, w, h);
    } else {
        // see mapToPolygon for explanations of the algorithm.
        double x0, y0;
        double x, y;
        map(rect.location().x(), rect.location().y(), &x0, &y0);
        double xmin = x0;
        double ymin = y0;
        double xmax = x0;
        double ymax = y0;
        map(rect.right() + 1, rect.location().y(), &x, &y);
	if (x < xmin)
	    xmin = x;
        if (y < ymin)
	    ymin = y;
        if (x > xmax)
	    xmax = x;
	if (y > ymax)
	    ymax = y;
        map(rect.right() + 1, rect.bottom() + 1, &x, &y);
        if (x < xmin)
	    xmin = x;
        if (y < ymin)
	    ymin = y;
        if (x > xmax)
	    xmax = x;
	if (y > ymax)
	    ymax = y;
        map(rect.location().x(), rect.bottom() + 1, &x, &y);
        if (x < xmin)
	    xmin = x;
        if (y < ymin)
	    ymin = y;
        if (x > xmax)
	    xmax = x;
	if (y > ymax)
	    ymax = y;
        double w = xmax - xmin;
        double h = ymax - ymin;
        xmin -= (xmin - x0) / w;
    	ymin -= (ymin - y0) / h;
        xmax -= (xmax - x0) / w;
        ymax -= (ymax - y0) / h;
        result = IntRect(lround(xmin), lround(ymin), lround(xmax)-lround(xmin)+1, lround(ymax)-lround(ymin)+1);
    }
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "afert AffineTransform::mapRect int  x=%d y=%d width=%d height=%d\n", result.x(), result.y(), result.width(), result.height());
    return result;

    /*IntRect r ( (int)( m_m11*rect.x() + m_m21*rect.y() + m_dx ), (int)(m_m22*rect.y() + m_m12*rect.x() + m_dy), rect.width(), rect.height() );
    return r;*/
}

FloatRect AffineTransform::mapRect(const FloatRect& rect) const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::mapRect2\n");
    FloatRect result;
    if (m_m12 == 0.0F && m_m21 == 0.0F) {
        double x = m_m11 * rect.x() + m_dx;
        double y = m_m22 * rect.y() + m_dy;
        double w = m_m11 * rect.width();
        double h = m_m22 * rect.height();
        if (w < 0) {
            w = -w;
            x -= w;
        }
        if (h < 0) {
            h = -h;
            y -= h;
        }
        result = FloatRect(x, y, w, h);
    } else {
        double x0, y0;
        double x, y;
        map(rect.x(), rect.y(), &x0, &y0);
        double xmin = x0;
        double ymin = y0;
        double xmax = x0;
        double ymax = y0;
        map(rect.x() + rect.width(), rect.y(), &x, &y);
        if (x < xmin)
    	    xmin = x;
        if (y < ymin)
    	    ymin = y;
        if (x > xmax)
    	    xmax = x;
	if (y > ymax)
	    ymax = y;
        map(rect.x() + rect.width(), rect.y() + rect.height(), &x, &y);
        if (x < xmin)
    	    xmin = x;
        if (y < ymin)
    	    ymin = y;
        if (x > xmax)
    	    xmax = x;
	if (y > ymax)
	    ymax = y;
        map(rect.x(), rect.y() + rect.height(), &x, &y);
        if (x < xmin)
    	    xmin = x;
        if (y < ymin)
    	    ymin = y;
        if (x > xmax)
    	    xmax = x;
	if (y > ymax)
	    ymax = y;
        result = FloatRect(xmin, ymin, xmax-xmin, ymax - ymin);
    }
    return result;

    /*FloatRect r ( m_m11*rect.x() + m_m21*rect.y() + m_dx, m_m22*rect.y() + m_m12*rect.x() + m_dy, rect.width(), rect.height() );
    return r;*/
}

bool AffineTransform::isIdentity() const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::isIdentity\n");
    if ((m_m11 == 1)
    &&  (m_m12 == 0)
    &&  (m_m21 == 0)
    &&  (m_m22 == 1)
    &&  (m_dx == 0)
    &&  (m_dy == 0))
        return true;

    return false;
}
double AffineTransform::a() const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::a\n");
    return m_m11;
}

void AffineTransform::setA(double a)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::setA\n");
    setMatrix(a, b(), c(), d(), e(), f());
}

double AffineTransform::b() const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::b\n");
    return m_m12;
}

void AffineTransform::setB(double b)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::setB\n");
    setMatrix(a(), b, c(), d(), e(), f());
}

double AffineTransform::c() const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::c\n");
    return m_m21;
}

void AffineTransform::setC(double c)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::setC\n");
    setMatrix(a(), b(), c, d(), e(), f());
}

double AffineTransform::d() const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::d\n");
    return m_m22;
}

void AffineTransform::setD(double d)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::setD\n");
    setMatrix(a(), b(), c(), d, e(), f());
}

double AffineTransform::e() const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::e\n");
    return m_dx;
}

void AffineTransform::setE(double e)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::setE\n");
    setMatrix(a(), b(), c(), d(), e, f());
}

double AffineTransform::f() const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::f\n");
    return m_dy;
}

void AffineTransform::setF(double f)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::setF\n");
    setMatrix(a(), b(), c(), d(), e(), f);
}

void AffineTransform::reset()
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::reset\n");
    m_m11 = 1;
    m_m12 = 0;
    m_m21 = 0;
    m_m22 = 1;
    m_dx = 0;
    m_dy = 0;
}

AffineTransform &AffineTransform::scale(double sx, double sy)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::scale sx = %f sy = %f\n", sx, sy);
    m_m11 *= sx;
    m_m12 *= sx;
    m_m21 *= sy;
    m_m22 *= sy;

    return *this;
}

//const double deg2rad = 0.017453292519943295769;        // pi/180

AffineTransform &AffineTransform::rotate(double d)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::rotate %f\n", d);
    double sind = 0;
    double cosd = 0;
    if (d == 90. || d == -270.)
        sind = 1.;
    else if (d == 270. || d == -90.)
        sind = -1.;
    else if (d == 180.)
        cosd = -1.;
    else{
        double b = deg2rad(d);                        // convert to radians
        sind = sin(b);                // fast and convenient
        cosd = cos(b);
    }
    double tm11 = cosd * m_m11 + sind * m_m21;
    double tm12 = cosd * m_m12 + sind * m_m22;
    double tm21 = -sind * m_m11 + cosd * m_m21;
    double tm22 = -sind * m_m12 + cosd * m_m22;
    m_m11 = tm11;
    m_m12 = tm12;
    m_m21 = tm21;
    m_m22 = tm22;

	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "afert AffineTransform::rotate a=%f b=%f c=%f d=%f\n", m_m11, m_m12, m_m21, m_m22);
    return *this;
}

AffineTransform &AffineTransform::translate(double tx, double ty)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::translate x=%f y=%f\n", tx, ty);
    m_dx += tx*m_m11 + ty*m_m21;
    m_dy += ty*m_m22 + tx*m_m12;
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "after AffineTransform::translate x=%f y=%f\n", m_dx, m_dy);

    return *this;
}

AffineTransform &AffineTransform::shear(double sx, double sy)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::shear x=%f y=%f\n", sx, sy);
    double tm11 = sy * m_m21;
    double tm12 = sy * m_m22;
    double tm21 = sx * m_m11;
    double tm22 = sx * m_m12;
    m_m11 += tm11;
    m_m12 += tm12;
    m_m21 += tm21;
    m_m22 += tm22;

    return *this;
}

double AffineTransform::det() const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::det = %f\n", m_m11*m_m22 - m_m12*m_m21);
    return m_m11 * m_m22 - m_m12 * m_m21;
}

AffineTransform AffineTransform::inverse() const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::inverse \n");
    double determinant = det();
    if (determinant == 0.0)
        return AffineTransform();
    else {
        double dinv = 1.0 / determinant;
        AffineTransform imatrix((m_m22 * dinv), (-m_m12 * dinv),
                          (-m_m21 * dinv), (m_m11 * dinv),
                          ((m_m21 * m_dy - m_m22 * m_dx) * dinv),
                          ((m_m12 * m_dx - m_m11 * m_dy) * dinv));
        return imatrix;
    }
}

bool AffineTransform::operator==(const AffineTransform& at) const
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::operator== \n");
    if ((m_m11 == at.m_m11)
    &&  (m_m12 == at.m_m12)
    &&  (m_m21 == at.m_m21)
    &&  (m_m22 == at.m_m22)
    &&  (m_dx == at.m_dx)
    &&  (m_dy == at.m_dy))
        return true;

    return false;
}

AffineTransform& AffineTransform::operator*=(const AffineTransform& b)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::operator*= \n");
    double tm11 = m_m11 * b.m_m11 + m_m12 * b.m_m21;
    double tm12 = m_m11 * b.m_m12 + m_m12 * b.m_m22;
    double tm21 = m_m21 * b.m_m11 + m_m22 * b.m_m21;
    double tm22 = m_m21 * b.m_m12 + m_m22 * b.m_m22;
    double tdx  = m_dx * b.m_m11  + m_dy * b.m_m21 + b.m_dx;
    double tdy =  m_dx * b.m_m12  + m_dy * b.m_m22 + b.m_dy;

    m_m11 = tm11; m_m12 = tm12;
    m_m21 = tm21; m_m22 = tm22;
    m_dx = tdx; m_dy = tdy;

	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::operator*= a=%f b=%f c=%f d=%f dx=%f dy=%f\n", m_m11, m_m12, m_m21, m_m22, m_dx, m_dy);
    return *this;
}

AffineTransform AffineTransform::operator*(const AffineTransform& b)
{
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::operator* \n");
    AffineTransform result = *this;
    result *= b;
	DBGML(MODULE_GRAPHICS, LEVEL_INFO, "AffineTransform::operator* a=%f b=%f c=%f d=%f dx=%f dy=%f\n", result.m_m11, result.m_m12, result.m_m21, result.m_m22, result.m_dx, result.m_dy);
    return result;
}

}
