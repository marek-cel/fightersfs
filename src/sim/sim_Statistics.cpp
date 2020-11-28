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
    _activated ( 0 ),
    _destroyed ( 0 ),

    _friendsActivated ( 0 ),
    _friendsDestroyed ( 0 ),
    _enemiesActivated ( 0 ),
    _enemiesDestroyed ( 0 ),
    _ownshipDestroyed ( 0 ),

    _flightTime ( 0.0f )
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
        _enemiesActivated++;
    }
    else if ( affiliation == Friend )
    {
        _friendsActivated++;
    }

    _activated++;
}

////////////////////////////////////////////////////////////////////////////////

void Statistics::reportDestroyed( Affiliation affiliation )
{
    if ( affiliation == Hostile )
    {
        _enemiesDestroyed++;
    }
    else if ( affiliation == Friend )
    {
        _friendsDestroyed++;
    }

    _destroyed++;
}

////////////////////////////////////////////////////////////////////////////////

void Statistics::reset()
{
    _activated = 0;
    _destroyed = 0;

    _friendsActivated = 0;
    _friendsDestroyed = 0;
    _enemiesActivated = 0;
    _enemiesDestroyed = 0;
    _ownshipDestroyed = 0;

    _flightTime = 0.0f;
}
