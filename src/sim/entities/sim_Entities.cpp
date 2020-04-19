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

#include <sim/entities/sim_Entities.h>

#include <limits>

#include <sim/sim_Log.h>
#include <sim/entities/sim_Unit.h>
#include <sim/utils/sim_String.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Entities::Entities() :
    Group( 0 )
{}

////////////////////////////////////////////////////////////////////////////////

Entities::~Entities() {}

////////////////////////////////////////////////////////////////////////////////

Unit* Entities::getUnitByName( const std::string &name )
{
    List::iterator it = m_children.begin();

    while ( it != m_children.end() )
    {
        Unit *unit = dynamic_cast< Unit* >( *it );

        if ( unit )
        {
            if ( 0 == String::icompare( unit->getName(), name ) )
            {
                return unit;
            }
        }

        ++it;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

Unit* Entities::getOwnship()
{
    List::iterator it = m_children.begin();

    while ( it != m_children.end() )
    {
        Unit *unit = dynamic_cast< Unit* >( *it );

        if ( unit )
        {
            if ( unit->isOwnship() )
            {
                return unit;
            }
        }

        ++it;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void Entities::listAll()
{
    List::iterator it = m_children.begin();

    while ( it != m_children.end() )
    {
        Log::i() << (*it)->getName() << std::endl;
        ++it;
    }
}
