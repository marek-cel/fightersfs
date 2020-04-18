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

#include <sim/utils/sim_Random.h>

#include <stdlib.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Random::Random()
{
    srand( time( NULL ) );
}

////////////////////////////////////////////////////////////////////////////////

Random::~Random() {}

////////////////////////////////////////////////////////////////////////////////

int Random::getRandom( int min, int max )
{
    if ( max > 0 && max > min && max <= RAND_MAX )
    {
        return min + rand() % ( max - min + 1 );
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

float Random::getRandom( float min, float max )
{
    int random = getRandom( 0, RAND_MAX );

    return min + ( max - min ) * ( (float)random / (float)RAND_MAX );
}
