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
            objective->m_minimum = String::toInt( minimum );
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
                    objective->m_unitsId.push_back( unit->getId() );
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

        if ( objective->m_minimum > objective->m_unitsId.size() )
        {
            objective->m_minimum = objective->m_unitsId.size();
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
    m_status = Success;

    // faillure if unit destroyed
    if ( m_unitsId.size() > 0 )
    {
        UnitsId::iterator it = m_unitsId.begin();

        unsigned int count = 0;

        while ( it != m_unitsId.end() )
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

        if ( count < m_minimum )
        {
            m_status = Failure;
        }
    }
}
