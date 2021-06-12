/****************************************************************************//*
 * Copyright (C) 2021 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ******************************************************************************/
#ifndef SIM_MISC_H
#define SIM_MISC_H

////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <math.h>
#include <stddef.h>

#ifdef _MSC_VER
#   include <float.h>
#endif

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Miscellaneous utilities. */
class Misc
{
public:

    /**
     * @brief Checks if given varaible is Infinite.
     * @param val double precision value to test
     * @return function returns TRUE if tested value is Infinite
     */
    inline static bool isInf( const float &val )
    {
#       ifdef _MSC_VER
        return !( _finite( val ) );
#       else
        return std::isinf( val );
#       endif
    }

    /**
     * @brief Checks if given varaible is NaN.
     * @param val double precision value to test
     * @return function returns TRUE if tested value is NaN
     */
    inline static bool isNaN( const float &val )
    {
        return ( val != val );
    }

    /**
     * @brief Checks if given varaible is Infinite or NaN.
     * @param val double precision value to test
     * @return function returns FALSE if tested value is Infinite or NaN
     */
    inline static bool isValid( const float &val )
    {
        return !( isNaN( val ) || isInf( val ) );
    }

    /**
     * @brief Checks if given array is Infinite or NaN.
     * @param array double precision array to test
     * @param size the size of given array
     * @return function returns FALSE if tested array is Infinite or NaN
     */
    inline static bool isValid( const float array[], unsigned int size )
    {
        for ( unsigned int i = 0; i < size; i++ )
        {
            if ( isNaN( array[ i ] ) || isInf( array[ i ] ) ) return false;
        }

        return true;
    }

    inline static float max( const float &val1, const float &val2 )
    {
        return ( val1 > val2 ) ? val1 : val2;
    }

    /** @brief Multiplication operator (by vector). */
    template < typename MAT, typename VEC >
    static VEC multiplyMatrixByVector( const MAT &mat, const VEC &vec )
    {
        VEC result;

        for ( unsigned int r = 0; r < mat.row_count; r++ )
        {
            result._v[ r ] = 0.0;

            for ( unsigned int c = 0; c < mat.col_count; c++ )
            {
                result._v[ r ] += ( mat[ r*mat.col_count + c ] * vec._v[ c ] );
            }
        }

        return result;
    }

    /**
     * @brief Saturation function. Returns value limited to the given range.
     * @param min minimum possible value
     * @param max maximum possible value
     * @param val variable to test
     * @return min if val less than min, max if val larger than max, val if val larger than min and less than max
     */
    inline static float satur( float min, float max, float val )
    {
        if      ( val < min ) return min;
        else if ( val > max ) return max;

        return val;
    }

    /**
     * @brief Signum function.
     * @param val input value
     * @return 1 if val is possitive, -1 when val is negative, 0 if val is zero
     */
    inline static short sign( float val )
    {
        if      ( val < 0.0f ) return -1;
        else if ( val > 0.0f ) return  1;

        return 0;
    }

    /**
     * @brief Period of 2.0 fast sine wave approximation.
     * Valid range is -1.0 to 1.0.
     * @param x normalized input value <-1.0;1.0>
     * @return approximated sine wave
     */
    inline static float sineWave( float x )
    {
        float y = 4.0f * x - 4.0f * x * fabs( x );
        return 0.225f * ( y * fabs( y ) - y ) + y;
    }
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_MISC_H
