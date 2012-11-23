/*
 *    Copyright (c) 2012 David Cavazos <davido262@gmail.com>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef SCALAR_HPP
#define SCALAR_HPP

#include <cmath>
#include <cfloat>
#include <boost/math/constants/constants.hpp>

typedef double scalar_t;

const double PI = boost::math::constants::pi<double>();
const double HALF_PI = PI / 2.0;
const double DEG_TO_RAD_MULT = PI / 180.0;
const double RAD_TO_DEG_MULT = 180.0 / PI;

const scalar_t EPSILON = DBL_EPSILON;
const scalar_t ZERO = static_cast<scalar_t>(0.0);
const scalar_t ONE = static_cast<scalar_t>(1.0);
const scalar_t HALF = static_cast<scalar_t>(0.5);
const scalar_t TWO = static_cast<scalar_t>(2.0);



inline double degToRad(const double deg) {
    return deg * DEG_TO_RAD_MULT;
}

inline float degToRad(const float deg) {
    return deg * float(DEG_TO_RAD_MULT);
}

inline double radToDeg(const double rad) {
    return rad * RAD_TO_DEG_MULT;
}

inline float radToDeg(const float rad) {
    return rad * float(RAD_TO_DEG_MULT);
}

#endif // SCALAR_HPP
