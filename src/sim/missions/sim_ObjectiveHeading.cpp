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

#include <sim/missions/sim_ObjectiveHeading.h>

#include <sim/sim_Ownship.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ObjectiveHeading* ObjectiveHeading::read( const XmlNode &node )
{
    ObjectiveHeading *objective = new ObjectiveHeading();

    if ( node.isValid() )
    {
        std::string above = node.getAttribute( "above" );
        std::string below = node.getAttribute( "below" );

        if ( above.length() > 0 && below.length() > 0 )
        {
            objective->m_above = Convert::deg2rad( String::toFloat( above ) );
            objective->m_below = Convert::deg2rad( String::toFloat( below ) );

            return objective;
        }
    }

    DELPTR( objective );

    return objective;
}

////////////////////////////////////////////////////////////////////////////////

ObjectiveHeading::ObjectiveHeading() :
    m_above ( 0.0f ),
    m_below ( 0.0f )
{}

////////////////////////////////////////////////////////////////////////////////

ObjectiveHeading::~ObjectiveHeading() {}

////////////////////////////////////////////////////////////////////////////////

void ObjectiveHeading::update()
{
    Aircraft *aircraft = Ownship::instance()->getAircraft();

    if ( aircraft )
    {
        float heading = aircraft->getHeading();

        if ( heading > m_above && heading < m_below )
        {
            m_status = Success;
        }
    }
}
