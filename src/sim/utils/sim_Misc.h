/****************************************************************************//*
 * Copyright (C) 2020 Marek M. Cel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
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

/** Miscellaneous utilities. */
class Misc
{
public:

    /**
     * Checks if given varaible is Infinite.
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
     * Checks if given varaible is NaN.
     * @param val double precision value to test
     * @return function returns TRUE if tested value is NaN
     */
    inline static bool isNaN( const float &val )
    {
        return ( val != val );
    }

    /**
     * Checks if given varaible is Infinite or NaN.
     * @param val double precision value to test
     * @return function returns FALSE if tested value is Infinite or NaN
     */
    inline static bool isValid( const float &val )
    {
        return !( isNaN( val ) || isInf( val ) );
    }

    /**
     * Checks if given array is Infinite or NaN.
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

    /** Multiplication operator (by vector). */
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
     * Saturation function. Returns value limited to the given range.
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
     * Signum function.
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
     * Period of 2.0 fast sine wave approximation.
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
