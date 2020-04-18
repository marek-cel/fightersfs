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

#include <sim/sim_Statistics.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Statistics::Statistics() :
    m_activated ( 0 ),
    m_destroyed ( 0 ),

    m_friendsActivated ( 0 ),
    m_friendsDestroyed ( 0 ),
    m_enemiesActivated ( 0 ),
    m_enemiesDestroyed ( 0 ),
    m_ownshipDestroyed ( 0 ),

    m_flightTime ( 0.0f )
{
    reset();
}

////////////////////////////////////////////////////////////////////////////////

Statistics::~Statistics()
{
    reset();
}

////////////////////////////////////////////////////////////////////////////////

void Statistics::reportActivated( Affiliation affiliation )
{
    if ( affiliation == Hostile )
    {
        m_enemiesActivated++;
    }
    else if ( affiliation == Friend )
    {
        m_friendsActivated++;
    }

    m_activated++;
}

////////////////////////////////////////////////////////////////////////////////

void Statistics::reportDestroyed( Affiliation affiliation )
{
    if ( affiliation == Hostile )
    {
        m_enemiesDestroyed++;
    }
    else if ( affiliation == Friend )
    {
        m_friendsDestroyed++;
    }

    m_destroyed++;
}

////////////////////////////////////////////////////////////////////////////////

void Statistics::reset()
{
    m_activated = 0;
    m_destroyed = 0;

    m_friendsActivated = 0;
    m_friendsDestroyed = 0;
    m_enemiesActivated = 0;
    m_enemiesDestroyed = 0;
    m_ownshipDestroyed = 0;

    m_flightTime = 0.0f;
}
