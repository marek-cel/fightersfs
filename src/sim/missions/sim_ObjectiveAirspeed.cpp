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

#include <sim/missions/sim_ObjectiveAirspeed.h>

#include <sim/sim_Ownship.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ObjectiveAirspeed* ObjectiveAirspeed::read( const XmlNode &node )
{
    ObjectiveAirspeed *objective = new ObjectiveAirspeed();

    if ( node.isValid() )
    {
        std::string above = node.getAttribute( "above" );
        std::string below = node.getAttribute( "below" );

        if ( above.length() > 0 && below.length() > 0 )
        {
            objective->_above = String::toFloat( above );
            objective->_below = String::toFloat( below );

            return objective;
        }
    }

    DELPTR( objective );

    return objective;
}

////////////////////////////////////////////////////////////////////////////////

ObjectiveAirspeed::ObjectiveAirspeed() :
    _above ( 0.0f ),
    _below ( 0.0f )
{}

////////////////////////////////////////////////////////////////////////////////

ObjectiveAirspeed::~ObjectiveAirspeed() {}

////////////////////////////////////////////////////////////////////////////////

void ObjectiveAirspeed::update()
{
    Aircraft *aircraft = Ownship::instance()->getAircraft();

    if ( aircraft )
    {
        float airspeed = aircraft->getAirspeed();

        if ( airspeed > _above && airspeed < _below )
        {
            _status = Success;
        }
    }
}
