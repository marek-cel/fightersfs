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

#include <sim/missions/sim_ObjectiveTimeout.h>

#include <sim/sim_Ownship.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ObjectiveTimeout* ObjectiveTimeout::read( const XmlNode &node )
{
    ObjectiveTimeout *objective = new ObjectiveTimeout();

    if ( node.isValid() )
    {
        std::string limit = node.getAttribute( "limit" );

        if ( limit.length() > 0 )
        {
            objective->_limit = String::toFloat( limit );

            return objective;
        }
    }

    DELPTR( objective );

    return objective;
}

////////////////////////////////////////////////////////////////////////////////

ObjectiveTimeout::ObjectiveTimeout() :
    _inited ( false ),
    _limit ( 0.0f )
{}

////////////////////////////////////////////////////////////////////////////////

ObjectiveTimeout::~ObjectiveTimeout() {}

////////////////////////////////////////////////////////////////////////////////

void ObjectiveTimeout::update()
{
    if ( !_inited )
    {
        _limit += Data::get()->mission.real_time;
        _inited = true;
    }

    if ( Data::get()->mission.real_time > _limit )
    {
        _status = Success;
    }
}

////////////////////////////////////////////////////////////////////////////////

void ObjectiveTimeout::setLimit( float limit )
{
    _limit = limit;
}
