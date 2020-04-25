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

#include <sim/sim_Route.h>

#include <sim/sim_Defines.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Route* Route::read( const XmlNode &node )
{
    Route *route = new Route();

    if ( node.isValid() && route->size() == 0 )
    {
        if ( 0 == String::icompare( node.getName(), "route" ) )
        {
            XmlNode waypointNode = node.getFirstChildElement( "waypoint" );

            int status = SIM_SUCCESS;

            while ( waypointNode.isValid() )
            {
                WaypointData waypoint;

                status = read( waypointNode, waypoint );

                if ( status == SIM_SUCCESS )
                {
                    route->push_back( waypoint );
                }
                else
                {
                    break;
                }

                waypointNode = waypointNode.getNextSiblingElement( "waypoint" );
            }

            if ( status == SIM_SUCCESS && route->size() > 0 )
            {
                return route;
            }
        }
    }

    DELPTR( route );

    return route;
}

////////////////////////////////////////////////////////////////////////////////

int Route::read( const XmlNode &node, WaypointData &waypoint )
{
    if ( node.isValid() )
    {
        if ( 0 == String::icompare( node.getName(), "waypoint" ) )
        {
            XmlNode positionNode = node.getFirstChildElement( "position" );
            XmlNode speedNode    = node.getFirstChildElement( "speed" );

            if ( positionNode.isValid() && speedNode.isValid() )
            {
                Vec3 position;

                if ( SIM_SUCCESS == XmlUtils::read( positionNode, position ) )
                {
                    float speed;

                    if ( SIM_SUCCESS == XmlUtils::read( speedNode, speed ) )
                    {
                        waypoint = WaypointData( position, speed );

                        return SIM_SUCCESS;
                    }
                }
            }
        }
    }

    return SIM_FAILURE;
}
