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

#include <sim/entities/sim_Group.h>

#include <sim/entities/sim_Entity.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Group::Group( Group *parent ) :
    m_parent ( parent )
{
    m_root = new osg::Group();

    m_children.clear();
}

////////////////////////////////////////////////////////////////////////////////

Group::~Group()
{
    deleteAllEntities();
}

////////////////////////////////////////////////////////////////////////////////

void Group::attachEntity( Entity *entity )
{
    if ( entity )
    {
        m_children.push_back( entity );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Group::deleteAllEntities()
{
    List::iterator it = m_children.begin();

    while ( it != m_children.end() )
    {
        DELPTR( *it );

        it = m_children.erase( it );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Group::deleteEntity( Entity *entity )
{
    if ( entity )
    {
        List::iterator it = m_children.begin();

        while ( it != m_children.end() )
        {
            if ( (*it) == entity )
            {
                DELPTR( *it );
                m_children.erase( it );

                return;
            }

            ++it;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Group::dettachEntity( Entity *entity )
{
    if ( entity )
    {
        List::iterator it = m_children.begin();

        while ( it != m_children.end() )
        {
            if ( (*it) == entity )
            {
                m_children.erase( it );

                return;
            }

            ++it;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

Group::List* Group::getEntities()
{
    return &( m_children );
}

////////////////////////////////////////////////////////////////////////////////

Entity* Group::getEntityById( UInt32 id )
{
    List::iterator it = m_children.begin();

    while ( it != m_children.end() )
    {
        if ( (*it)->getId() == id )
        {
            return (*it);
        }

        ++it;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void Group::load()
{
    List::iterator it = m_children.begin();

    while ( it != m_children.end() )
    {
        if ( (*it) )
        {
            (*it)->load();
        }

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Group::update( double timeStep )
{
    List::iterator it = m_children.begin();

    // delete innactive entities (before updating)
    while ( it != m_children.end() )
    {
        if ( (*it) )
        {
            if ( (*it)->getState() == Inactive )
            {
                DELPTR( *it );
                it = m_children.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }

    // due to the fact that vector can be modified in update loop.
    List children = m_children;

    // updating after deleting due to keep innactive entities for one more step
    for ( List::iterator it = children.begin(); it != children.end(); ++it )
    {
        if ( (*it)->isActive() ) (*it)->update( timeStep );
    }
}
