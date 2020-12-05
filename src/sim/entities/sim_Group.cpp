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
    _parent ( parent )
{
    _root = new osg::Group();

    _children.clear();
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
        _children.push_back( entity );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Group::deleteAllEntities()
{
    List::iterator it = _children.begin();

    while ( it != _children.end() )
    {
        DELPTR( *it );

        it = _children.erase( it );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Group::deleteEntity( Entity *entity )
{
    if ( entity )
    {
        List::iterator it = _children.begin();

        while ( it != _children.end() )
        {
            if ( (*it) == entity )
            {
                DELPTR( *it );
                _children.erase( it );

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
        List::iterator it = _children.begin();

        while ( it != _children.end() )
        {
            if ( (*it) == entity )
            {
                _children.erase( it );

                return;
            }

            ++it;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

Group::List* Group::getEntities()
{
    return &( _children );
}

////////////////////////////////////////////////////////////////////////////////

Entity* Group::getEntityById( UInt32 id )
{
    List::iterator it = _children.begin();

    while ( it != _children.end() )
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
    List::iterator it = _children.begin();

    while ( it != _children.end() )
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
    List::iterator it = _children.begin();

    // delete innactive entities (before updating)
    while ( it != _children.end() )
    {
        if ( (*it) )
        {
            if ( (*it)->getState() == Inactive )
            {
                DELPTR( *it );
                it = _children.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }

    // due to the fact that vector can be modified in update loop.
    List children = _children;

    // updating after deleting due to keep innactive entities for one more step
    for ( List::iterator it = children.begin(); it != children.end(); ++it )
    {
        if ( (*it)->isActive() ) (*it)->update( timeStep );
    }
}
