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

#include <sim/utils/sim_PID.h>

#include <algorithm>

#include <sim/utils/sim_Misc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

PID::PID( float kp, float ki, float kd, float min, float max,
          bool reset, float coef ) :
    _kp ( kp ),
    _ki ( ki ),
    _kd ( kd ),

    _min ( min ),
    _max ( max ),

    _error   ( 0.0f ),
    _error_i ( 0.0f ),
    _error_d ( 0.0f ),

    _error_s ( 1 ),

    _value ( 0.0f ),
    _delta ( 0.0f ),

    _coef ( coef ),

    _reset ( reset )
{}

////////////////////////////////////////////////////////////////////////////////

PID::~PID() {}

////////////////////////////////////////////////////////////////////////////////

void PID::update( float timeStep, float error )
{
    if ( _reset && Misc::sign( error ) != _error_s )
    {
        // reseting integral sum of error (anti-windup)
        _error_i -= _coef * _error_i;
        _error_s = -_error_s;
    }
    else
    {
        // integration with anti-windup filter
        _error_i = _error_i + ( error - _delta ) * timeStep;
    }

    _error_d = ( error - _error ) / timeStep;

    _error = error;

    float value = _kp * _error + _ki * _error_i + _kd * _error_d;

    // saturation
    _value = std::max( std::min( value, _max ), _min );

    _delta = value - _value;
}

////////////////////////////////////////////////////////////////////////////////

void PID::setValue( float value )
{
    _error   = 0.0f;
    _error_i = 0.0f;
    _error_d = 0.0f;

    _value = value;
    _delta = 0.0f;
}
