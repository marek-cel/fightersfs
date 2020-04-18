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

#include <sim/missions/sim_Objective.h>

#include <sim/missions/sim_ObjectiveAirspeed.h>
#include <sim/missions/sim_ObjectiveAltitude.h>
#include <sim/missions/sim_ObjectiveDefend.h>
#include <sim/missions/sim_ObjectiveDestroy.h>
#include <sim/missions/sim_ObjectiveDistance.h>
#include <sim/missions/sim_ObjectiveHeading.h>
#include <sim/missions/sim_ObjectiveTimeout.h>
#include <sim/missions/sim_ObjectiveWaypoint.h>

#include <sim/utils/sim_String.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Objective* Objective::read( const XmlNode &node )
{
    if ( node.isValid() )
    {
        std::string nodeName = node.getName();

        if ( 0 == String::icompare( nodeName, "objective_airspeed" ) )
        {
            return ObjectiveAirspeed::read( node );
        }
        else if ( 0 == String::icompare( nodeName, "objective_altitude" ) )
        {
            return ObjectiveAltitude::read( node );
        }
        else if ( 0 == String::icompare( nodeName, "objective_defend" ) )
        {
            return ObjectiveDefend::read( node );
        }
        else if ( 0 == String::icompare( nodeName, "objective_destroy" ) )
        {
            return ObjectiveDestroy::read( node );
        }
        else if ( 0 == String::icompare( nodeName, "objective_distance" ) )
        {
            return ObjectiveDistance::read( node );
        }
        else if ( 0 == String::icompare( nodeName, "objective_heading" ) )
        {
            return ObjectiveHeading::read( node );
        }
        else if ( 0 == String::icompare( nodeName, "objective_timeout" ) )
        {
            return ObjectiveTimeout::read( node );
        }
        else if ( 0 == String::icompare( nodeName, "objective_waypoint" ) )
        {
            return ObjectiveWaypoint::read( node );
        }
    }

    return 0;
}
