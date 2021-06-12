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
#ifndef SIM_PID_H
#define SIM_PID_H

////////////////////////////////////////////////////////////////////////////////

#include <limits>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Proportional-Integral-Derivative controller with anti-windup filter class.
 *
 * @see Duzinkiewicz K., et al.: Zadania do cwiczen laboratoryjnych T10: Sterowanie predkoscia obrotowa silnika pradu stalego, Gdansk, 2016
 */
class PID
{
public:

    /** @brief Constructor. */
    PID( float kp, float ki, float kd,
         float min = std::numeric_limits< float >::min(),
         float max = std::numeric_limits< float >::max(),
         bool reset = false, float coef = 1.0f );

    /** @brief Destructor. */
    virtual ~PID();

    /** @brief Updates controller. */
    void update( float timeStep, float error );

    /** @brief Returns controller output. */
    inline float getValue() const { return _value; }

    /** @brief Sets controller gains. */
    inline void set( float kp, float ki, float kd )
    {
        _kp = kp;
        _ki = ki;
        _kd = kd;
    }

    inline void setKp( float kp ) { _kp = kp; }
    inline void setKi( float ki ) { _ki = ki; }
    inline void setKd( float kd ) { _kd = kd; }

    inline void setMin( float min ) { _min = min; }
    inline void setMax( float max ) { _max = max; }

    inline void setReset( bool reset ) { _reset = reset; }

    /** @brief Sets controller output (resets integer). */
    void setValue( float value );

private:

    float _kp;          ///< proportional gain
    float _ki;          ///< integral gain
    float _kd;          ///< derivative gain

    float _min;         ///< minimum output value
    float _max;         ///< maximum output value

    float _error;       ///< error
    float _error_i;     ///< error integral sum
    float _error_d;     ///< error derivative

    short _error_s;     ///< error sign

    float _value;       ///< output value
    float _delta;       ///< difference between raw and saturated output values

    float _coef;        ///< reset coefficient

    bool _reset;        ///< reset integral sum of error on overshoot
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_PID_H
