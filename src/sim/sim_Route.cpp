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
