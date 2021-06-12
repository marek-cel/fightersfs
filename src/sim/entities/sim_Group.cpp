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
