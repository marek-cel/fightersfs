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
