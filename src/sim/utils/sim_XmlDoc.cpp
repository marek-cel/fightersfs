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

#include <sim/utils/sim_XmlDoc.h>

#include <sim/sim_Defines.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

XmlDoc::XmlDoc( const std::string &fileName ) :
    m_doc  ( 0 ),
    m_open ( false ),
    m_root ( 0 )
{
    readFile( fileName );
}

////////////////////////////////////////////////////////////////////////////////

XmlDoc::~XmlDoc()
{
    DELPTR( m_root );

    xmlFreeDoc( m_doc );
}

////////////////////////////////////////////////////////////////////////////////

int XmlDoc::readFile( const std::string &fileName )
{
    m_doc = xmlParseFile( fileName.c_str() );

    if ( m_doc == 0 )
    {
        xmlFreeDoc( m_doc );
        return SIM_FAILURE;
    }

    xmlNodePtr root = xmlDocGetRootElement( m_doc );

    if ( root == 0 )
    {
        xmlFreeNode( root );
        xmlFreeDoc( m_doc );
        return SIM_FAILURE;
    }

    m_root = new XmlNode( root, fileName );

    m_open = true;

    return SIM_SUCCESS;
}
