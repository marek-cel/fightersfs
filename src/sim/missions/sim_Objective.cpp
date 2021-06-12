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
