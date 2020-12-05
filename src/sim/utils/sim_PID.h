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
