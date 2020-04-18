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

#include <sim/missions/sim_ObjectiveDestroy.h>

#include <sim/entities/sim_Entities.h>

#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ObjectiveDestroy* ObjectiveDestroy::read( const XmlNode &node )
{
    ObjectiveDestroy *objective = new ObjectiveDestroy();

    if ( node.isValid() )
    {
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
                //////////////////////////////////
                if ( objective ) delete objective;
                objective = 0;
                //////////////////////////////////

                return 0;
            }

            unitNode = unitNode.getNextSiblingElement( "unit" );
        }

        return objective;
    }

    if ( objective ) delete objective;
    objective = 0;

    return objective;
}

////////////////////////////////////////////////////////////////////////////////

ObjectiveDestroy::ObjectiveDestroy() {}

////////////////////////////////////////////////////////////////////////////////

ObjectiveDestroy::~ObjectiveDestroy() {}

////////////////////////////////////////////////////////////////////////////////

void ObjectiveDestroy::addUnitId( UInt32 unitId )
{
    m_unitsId.push_back( unitId );
}

////////////////////////////////////////////////////////////////////////////////

void ObjectiveDestroy::update()
{
    if ( m_unitsId.size() > 0 )
    {
        UnitsId::iterator it = m_unitsId.begin();

        while ( it != m_unitsId.end() )
        {
            Unit *unit = dynamic_cast< Unit* >( Entities::instance()->getEntityById( *it ) );

            if ( unit )
            {
                if ( unit->getHP() <= 0 )
                {
                    it = m_unitsId.erase( it );
                    continue;
                }
            }
            else
            {
                it = m_unitsId.erase( it );
                continue;
            }

            ++it;
        }
    }
    else
    {
        m_status = Success;
    }
}
