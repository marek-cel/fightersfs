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

#include <sim/missions/sim_ObjectiveDistance.h>

#include <sim/sim_Ownship.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ObjectiveDistance* ObjectiveDistance::read( const XmlNode &node )
{
    ObjectiveDistance *objective = new ObjectiveDistance();

    if ( node.isValid() )
    {
        std::string limit = node.getAttribute( "limit" );
        std::string unitName = node.getAttribute( "leader" );

        if ( limit.length() > 0 && unitName.length() > 0 )
        {
            objective->_limit = String::toFloat( limit );
            objective->_limit_2 = objective->_limit * objective->_limit;

            Unit *unit = Entities::instance()->getUnitByName( unitName );

            if ( unit && objective->_limit > 0.0f )
            {
                objective->_unitId = unit->getId();

                return objective;
            }
        }
    }

    DELPTR( objective );

    return objective;
}

////////////////////////////////////////////////////////////////////////////////

ObjectiveDistance::ObjectiveDistance() :
    _unitId ( 0 ),
    _limit ( 0.0f ),
    _limit_2 ( 0.0f )
{}

////////////////////////////////////////////////////////////////////////////////

ObjectiveDistance::~ObjectiveDistance() {}

////////////////////////////////////////////////////////////////////////////////

void ObjectiveDistance::update()
{
    if ( _status != Failure )
    {
        Unit *unit = dynamic_cast< Unit* >( Entities::instance()->getEntityById( _unitId ) );

        Aircraft *ownship = Ownship::instance()->getAircraft();

        if ( unit && ownship )
        {
            if ( ( unit->getAbsPos() - ownship->getAbsPos() ).length2() < _limit_2 )
            {
                _status = Success;
            }
            else
            {
                _status = Failure;
            }
        }
        else
        {
            _status = Failure;
        }
    }
}
