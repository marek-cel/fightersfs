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
    _doc  ( 0 ),
    _open ( false ),
    _root ( 0 )
{
    readFile( fileName );
}

////////////////////////////////////////////////////////////////////////////////

XmlDoc::~XmlDoc()
{
    DELPTR( _root );

    xmlFreeDoc( _doc );
}

////////////////////////////////////////////////////////////////////////////////

int XmlDoc::readFile( const std::string &fileName )
{
    _doc = xmlParseFile( fileName.c_str() );

    if ( _doc == 0 )
    {
        xmlFreeDoc( _doc );
        return SIM_FAILURE;
    }

    xmlNodePtr root = xmlDocGetRootElement( _doc );

    if ( root == 0 )
    {
        xmlFreeNode( root );
        xmlFreeDoc( _doc );
        return SIM_FAILURE;
    }

    _root = new XmlNode( root, fileName );

    _open = true;

    return SIM_SUCCESS;
}
