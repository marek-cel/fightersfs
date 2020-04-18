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
    m_kp ( kp ),
    m_ki ( ki ),
    m_kd ( kd ),

    m_min ( min ),
    m_max ( max ),

    m_error   ( 0.0f ),
    m_error_i ( 0.0f ),
    m_error_d ( 0.0f ),

    m_error_s ( 1 ),

    m_value ( 0.0f ),
    m_delta ( 0.0f ),

    m_coef ( coef ),

    m_reset ( reset )
{}

////////////////////////////////////////////////////////////////////////////////

PID::~PID() {}

////////////////////////////////////////////////////////////////////////////////

void PID::update( float timeStep, float error )
{
    if ( m_reset && Misc::sign( error ) != m_error_s )
    {
        // reseting integral sum of error (anti-windup)
        m_error_i -= m_coef * m_error_i;
        m_error_s = -m_error_s;
    }
    else
    {
        // integration with anti-windup filter
        m_error_i = m_error_i + ( error - m_delta ) * timeStep;
    }

    m_error_d = ( error - m_error ) / timeStep;

    m_error = error;

    float value = m_kp * m_error + m_ki * m_error_i + m_kd * m_error_d;

    // saturation
    m_value = std::max( std::min( value, m_max ), m_min );

    m_delta = value - m_value;
}

////////////////////////////////////////////////////////////////////////////////

void PID::setValue( float value )
{
    m_error   = 0.0f;
    m_error_i = 0.0f;
    m_error_d = 0.0f;

    m_value = value;
    m_delta = 0.0f;
}
