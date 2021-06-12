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
#ifndef SIM_CONVERT_H
#define SIM_CONVERT_H

////////////////////////////////////////////////////////////////////////////////

#include <cmath>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Units converter class. */
class Convert
{
public:

    /**
     * @brief Converts given angle from degrees to radians.
     * @param angle angle expressed in degrees
     * @return angle expressed in radians
     */
    static inline double deg2rad( double angle = 1.0 )
    {
        return angle * M_PI / 180.0;
    }

    /**
     * @brief Converts given angle from radians to degrees.
     * @param angle angle expressed in radians
     * @return angle expressed in degrees
     */
    static inline double rad2deg( double angle = 1.0 )
    {
        return angle * 180.0 / M_PI;
    }

    /**
     * @brief Converts given length from feet to metres.
     * @param length length expressed in feet
     * @return length expressed in metres
     */
    static inline float ft2m( float length = 1.0f )
    {
        return length * 0.3048f;
    }

    /**
     * @brief Converts given velocity from km/h to knots.
     * @param vel velocity expressed in km/h
     * @return velocity expressed in knots
     */
    static inline double kmh2kts( double vel = 1.0 )
    {
        return vel * 0.539956803;
    }

    /**
     * @brief Converts given velocity from kntos to km/h.
     * @param vel velocity expressed in knots
     * @return velocity expressed in km/h
     */
    static inline double kts2kmh( double vel = 1.0 )
    {
        return vel * 1.852;
    }

    /**
     * @brief Converts given length from kilometres to nautical miles.
     * @param len length expressed in kilometres
     * @return length expressed in nautical miles
     */
    static inline double km2nmi( double len = 1.0 )
    {
        return len * 0.539956803;
    }

    /**
     * @brief Converts given length from metres to feet.
     * @param length length expressed in metres
     * @return length expressed in feet
     */
    static inline float m2ft( float length = 1.0f )
    {
        return length * 3.2808399f;
    }

    /**
     * @brief Converts given length from metres to nautical miles.
     * @param length length expressed in metres
     * @return length expressed in nautical miles
     */
    static inline float m2nmi( float length = 1.0f )
    {
        return length * 0.000539956803f;
    }

    /**
     * @brief Converts given length from nautical miles to metres.
     * @param length length expressed in nautical miles
     * @return length expressed in metres
     */
    static inline float nmi2m( float length = 1.0 )
    {
        return length * 1852.0f;
    }

    /**
     * @brief Converts given velocity from m/s to ft/min.
     * @param velocity velocity expressed in m/s
     * @return velocity expressed in ft/min
     */
    static inline float mps2fpm( float vel = 1.0f )
    {
        return vel * 196.850394f;
    }

    /**
     * @brief Converts given velocity from m/s to knots.
     * @param velocity velocity expressed in m/s
     * @return velocity expressed in knots
     */
    static inline float mps2kts( float vel = 1.0f )
    {
        return vel * 1.943844491f;
    }

    /**
     * @brief Converts given velocity from kntos to m/s.
     * @param velocity velocity expressed in knots
     * @return velocity expressed in m/s
     */
    static inline float kts2mps( float vel = 1.0f )
    {
        return vel * 0.5144444444444f;
    }

    /**
     * @brief Converts given mass from kilograms to pounds.
     * @param mass mass expressed in kilograms
     * @return mass expressed in pounds
     */
    static inline double kg2lb( double mass = 1.0 )
    {
        return mass * 2.20462262;
    }

    /**
     * @brief Converts given mass from pounds to kilograms.
     * @param mass mass expressed in pounds
     * @return mass expressed in kilograms
     */
    static inline double lb2kg( double mass = 1.0 )
    {
        return mass * 0.45359237;
    }

    /**
     * @brief Converts given pressure from pascals to in Hg.
     * @param press pressure expressed in pascals
     * @return  pressure expressed in in Hg
     */
    static inline float pa2inhg( float press = 1.0f )
    {
        return press * 0.000295333727f;
    }
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_CONVERT_H
