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

#include <sim/cgi/sim_Module.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Module::Module( osg::Group *root, Module *parent ) :
    _root ( root ),
    _parent ( parent )
{
    _children.clear();
}

////////////////////////////////////////////////////////////////////////////////

Module::~Module()
{
    removeAllChildren();
}

////////////////////////////////////////////////////////////////////////////////

void Module::addChild( Module *child )
{
    if ( child )
    {
        _children.push_back( child );
        _root->addChild( child->getNode() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Module::load()
{
    List::iterator it = _children.begin();

    while ( it != _children.end() )
    {
        (*it)->load();

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Module::update()
{
    List::iterator it = _children.begin();

    while ( it != _children.end() )
    {
        (*it)->update();

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Module::removeAllChildren()
{
    if ( _root->getNumChildren() > 0 )
    {
        _root->removeChildren( 0, _root->getNumChildren() );
    }

    for ( unsigned int i = 0; i < _children.size(); i++ )
    {
        DELPTR( _children[ i ] );
    }

    _children.clear();
}
