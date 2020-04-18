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
    m_root ( root ),
    m_parent ( parent )
{
    m_children.clear();
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
        m_children.push_back( child );
        m_root->addChild( child->getNode() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Module::load()
{
    List::iterator it = m_children.begin();

    while ( it != m_children.end() )
    {
        (*it)->load();

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Module::update()
{
    List::iterator it = m_children.begin();

    while ( it != m_children.end() )
    {
        (*it)->update();

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Module::removeAllChildren()
{
    if ( m_root->getNumChildren() > 0 )
    {
        m_root->removeChildren( 0, m_root->getNumChildren() );
    }

    for ( unsigned int i = 0; i < m_children.size(); i++ )
    {
        if ( m_children[ i ] ) delete m_children[ i ];
        m_children[ i ] = 0;
    }

    m_children.clear();
}
