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

#include <sim/missions/sim_ObjectiveDefend.h>

#include <sim/entities/sim_Entities.h>
#include <sim/utils/sim_String.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ObjectiveDefend* ObjectiveDefend::read( const XmlNode &node )
{
    ObjectiveDefend *objective = new ObjectiveDefend();

    if ( node.isValid() )
    {
        std::string minimum = node.getAttribute( "minimum" );

        if ( minimum.length() )
        {
            objective->_minimum = String::toInt( minimum );
        }

        XmlNode unitNode = node.getFirstChildElement( "unit" );

        while ( unitNode.isValid() )
        {
            std::string unitName = unitNode.getAttribute( "name" );

            if ( unitName.length() > 0 )
            {
                Unit *unit = Entities::instance()->getUnitByName( unitName );

                if ( unit )
                {
                    objective->_unitsId.push_back( unit->getId() );
                }
            }
            else
            {
                ////////////////////
                DELPTR( objective );
                ////////////////////

                return 0;
            }

            unitNode = unitNode.getNextSiblingElement( "unit" );
        }

        if ( objective->_minimum > objective->_unitsId.size() )
        {
            objective->_minimum = objective->_unitsId.size();
        }

        return objective;
    }

    DELPTR( objective );

    return objective;
}

////////////////////////////////////////////////////////////////////////////////

ObjectiveDefend::ObjectiveDefend() {}

////////////////////////////////////////////////////////////////////////////////

ObjectiveDefend::~ObjectiveDefend() {}

////////////////////////////////////////////////////////////////////////////////

void ObjectiveDefend::update()
{
    _status = Success;

    // faillure if unit destroyed
    if ( _unitsId.size() > 0 )
    {
        UnitsId::iterator it = _unitsId.begin();

        unsigned int count = 0;

        while ( it != _unitsId.end() )
        {
            Unit *unit = dynamic_cast< Unit* >( Entities::instance()->getEntityById( *it ) );

            if ( unit )
            {
                if ( unit->getHP() > 0 )
                {
                    count++;
                }
            }

            ++it;
        }

        if ( count < _minimum )
        {
            _status = Failure;
        }
    }
}
