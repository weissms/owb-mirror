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
#ifndef BIMATH_H_
#define BIMATH_H_

#ifndef __BAL_MATH_POSIX__
#include <math.h>
#else

#ifdef _MATH_H
# error "Never use <math.h> directly; include <BIMath.h> instead."
#endif

# define M_PI           3.14159265358979323846  /* pi */

namespace BAL {

    /**
    * @brief the BIMath
    *
    * The math base class
    *
    */
    class BIMath {
        public:
            // this is mandatory
            /**
            * BIKeyboardEvent destructor
            */
            ~BIMath(){}

            /**
             * These functions round x up to the nearest integer.
             */
            static float ceilf(float );
            static double ceil(double );

            /**
             * The  fmod() function computes the remainder of dividing x by y.  The return value is x - n * y, where n is the quotient of x / y, rounded towards zero to an integer.
             */
            static float fmodf(float , float );

            /**
             * These  functions  round their argument to the nearest integer value, rounding away from zero, regardless of the current rounding direction.  If x is infinite or NaN, or if the rounded value is outside the range of the return type, the numeric result is unspecified.  A domain error may occur if the magnitude of x is too large.
             */
            static long int lroundf(float );
            static long int lround(double );

            /**
             * These functions round x down to the nearest integer.
             */
            static float floorf(float );
            static double floor(double );

            /**
             * These functions round x to the nearest integer, but round halfway cases away from zero (regardless of the current rounding direction), instead of to the nearest even integer like rint().
             */
            static float roundf(float );
            static double round(double );

            /**
             * The  nextafter() functions return the next representable neighbor of x in the direction towards y.  The size of the step between x and the result depends on the type of the result.  If x = y the function simply returns y.  If either value is NaN, then NaN is  returned.   Otherwise  a  value corresponding to the value of the least significant bit in the mantissa is added or subtracted, depending on the direction.
             */
            static double nextafter(double , double );
            static float nextafterf(float , float );

            /**
             * Returns a non-zero value if (fpclassify(x) != FP_NAN && fpclassify(x) != FP_INFINITE)
             */
            static int isfinite(double );

            /**
             * The fabs() functions return the absolute value of the floating-point number x.
             */
            static double fabs(double );

            /**
             * The copysign() functions return a value whose absolute value matches that of x, but whose sign matches that of y.  If x is a NaN, then a NaN with the sign of y is returned.
             */
            static double copysign(double , double );

            /**
             * The exp() function returns the value of e (the base of natural logarithms) raised to the power of x.
             */
            static double exp(double );

            /**
             * The log() function returns the natural logarithm of x.
             */
            static double log(double );

            /**
             * The sqrt() function returns the non-negative square root of x.  It fails and sets errno to EDOM, if x is negative.
             */
            static double sqrt (double );

            /**
             * The  acos() function calculates the arc cosine of x; that is the value whose cosine is x.  If x falls outside the range -1 to 1, acos() fails and errno is set.
             */
            static double acos(double );

            /**
             * The  asin()  function  calculates  the  arc sine of x; that is the value whose sine is x.  If x falls outside the range -1 to 1, asin() fails and errno is set.
             */
            static double asin(double );

            /**
             * The atan() function calculates the arc tangent of x; that is the value whose tangent is x.
             */
            static double atan(double );

            /**
             * The atan2() function calculates the arc tangent of the two variables x and y.  It is similar to calculating the arc tangent of y / x, except that the signs of both arguments are used to determine the quadrant of the result.
             */
            static double atan2(double , double );

            /**
             * The cos() function returns the cosine of x, where x is given in radians.
             */
            static double cos(double );

            /**
             * The sin() function returns the sine of x, where x is given in radians.
             */
            static double sin(double );

            /**
             * The tan() function returns the tangent of x, where x is given in radians.
             */
            static double tan(double );

            /**
             * `signbit'  is  a  generic  macro which can work on all real floating-point types.  It returns a non-zero value if the value of X has its sign bit set.
             * This is not the same as `x < 0.0', because IEEE 754 floating point allows zero to be signed.  The comparison `-0.0 < 0.0' is false, but  `signbit (-0.0)' will return a non-zero value.
             */
            static int signbit( double  );

            /**
             * The pow() function  returns the value of x raised to the power of y.
             */
            static double pow(double , double );

            /**
             * The  fmod() function computes the remainder of dividing x by y.  The return value is x - n * y, where n is the quotient of x / y, rounded towards zero to an integer.
             */
            static double fmod(double , double );

            /**
             * The log10() function returns the base 10 logarithm of x.
             */
            static double log10(double );

            /**
             * returns a non-zero value if (fpclassify(x) == FP_NAN)
             */
            static int isnan( double );

            /**
             * returns 1 if x is positive infinity, and -1 if x is negative infinity.
             */
            static int isinf( double );

            /**
             * The  remainder()  function  computes the remainder of dividing x by y.  The return value is x - n * y, where n is the value x / y, rounded to the nearest integer.  If this quotient is 1/2 (mod 1), it is rounded to the nearest even number (independent of the current rounding mode).   If  the return value is 0, it has the sign of x.
             */
            static double remainder(double , double );
    };
}

inline float ceilf(float x)
{
    return BAL::BIMath::ceilf( x );
}

inline float fmodf(float x, float y)
{
    return BAL::BIMath::fmodf( x, y );
}

inline long int lroundf(float x)
{
    return BAL::BIMath::lroundf( x );
}

inline float floorf(float x)
{
    return BAL::BIMath::floorf( x );
}

inline float roundf(float x)
{
    return BAL::BIMath::roundf( x );
}

inline double round(double x)
{
    return BAL::BIMath::round( x );
}

inline long int lround(double x)
{
    return BAL::BIMath::lround( x );
}


inline double nextafter(double x, double y)
{
    return BAL::BIMath::nextafter( x, y );
}

inline float nextafterf(float x, float y)
{
    return BAL::BIMath::nextafterf( x, y );
}

inline double floor(double x)
{
    return BAL::BIMath::floor( x );
}

inline int isfinite(double x)
{
    return BAL::BIMath::isfinite( x );
}

inline double fabs(double x)
{
    return BAL::BIMath::fabs( x );
}

inline double copysign(double x, double y)
{
    return BAL::BIMath::copysign( x, y );
}


inline double exp(double x)
{
    return BAL::BIMath::exp( x );
}

inline double log(double x)
{
    return BAL::BIMath::log( x );
}

inline double sqrt (double x)
{
    return BAL::BIMath::sqrt( x );
}

inline double acos(double x)
{
    return BAL::BIMath::acos( x );
}

inline double asin(double x)
{
    return BAL::BIMath::asin( x );
}

inline double atan(double x)
{
    return BAL::BIMath::atan( x );
}

inline double atan2(double y, double x)
{
    return BAL::BIMath::atan2( y, x );
}

inline double ceil(double x)
{
    return BAL::BIMath::ceil( x );
}

inline double cos(double x)
{
    return BAL::BIMath::cos( x );
}

inline double sin(double x)
{
    return BAL::BIMath::sin( x );
}

inline double tan(double x)
{
    return BAL::BIMath::tan( x );
}

inline int signbit( double x )
{
    return BAL::BIMath::signbit( x );
}

inline double pow(double x, double y)
{
    return BAL::BIMath::pow( x, y );
}

inline double fmod(double x, double y)
{
    return BAL::BIMath::fmod( x, y );
}


inline double log10(double x)
{
    return BAL::BIMath::log10( x );
}

inline int isnan( double x)
{
    return BAL::BIMath::isnan( x );
}

inline int isinf( double x)
{
    return BAL::BIMath::isinf( x );
}

inline double remainder(double x, double y)
{
    return BAL::BIMath::remainder( x, y );
}


const double HUGE_VAL = 0x7FF00000UL;
const double NAN = 0x7FC00000UL;

#endif // __MATH__
#endif // BIMATH_H_
