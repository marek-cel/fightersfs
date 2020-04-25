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

#include <sim/missions/sim_ObjectiveWaypoint.h>

#include <sim/sim_Ownship.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ObjectiveWaypoint* ObjectiveWaypoint::read( const XmlNode &node )
{
    ObjectiveWaypoint *objective = new ObjectiveWaypoint();

    if ( node.isValid() )
    {
        std::string waypointIndex = node.getAttribute( "index" );

        if ( waypointIndex.length() > 0 )
        {
            int index = String::toInt( waypointIndex );

            if ( index >= 0 )
            {
                objective->m_waypointIndex = index;

                return objective;
            }
        }
    }

    DELPTR( objective );

    return objective;
}

////////////////////////////////////////////////////////////////////////////////

ObjectiveWaypoint::ObjectiveWaypoint()
{}

////////////////////////////////////////////////////////////////////////////////

ObjectiveWaypoint::~ObjectiveWaypoint() {}

////////////////////////////////////////////////////////////////////////////////

void ObjectiveWaypoint::update()
{
    Aircraft *aircraft = Ownship::instance()->getAircraft();

    if ( aircraft )
    {
        if ( aircraft->getWaypointIndex() > m_waypointIndex )
        {
            m_status = Success;
        }
    }
}
