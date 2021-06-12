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
