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

#include <gui/Units.h>

#include <defs.h>

////////////////////////////////////////////////////////////////////////////////

Units::Data Units::getData( UInt32 index ) const
{
    Data result;

    if ( index < _units.size() )
    {
        result = _units[ index ];
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

Units::Units()
{
    readFile( Path::get( "units.xml" ) );
}

////////////////////////////////////////////////////////////////////////////////

void Units::readFile( const std::string &file )
{
    XmlDoc doc( file );

    if ( doc.isOpen() )
    {
        XmlNode rootNode = doc.getRootNode();

        if ( rootNode.isValid() )
        {
            readUnits( rootNode );
        }
        else
        {
            Log::e() << "Reading file \"" << file << "\" failed. Invalid root node." << std::endl;
        }
    }
    else
    {
        Log::e() << "Reading file \"" << file << "\" failed." << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Units::readUnits( XmlNode &rootNode )
{
    UInt32 index = 0;

    XmlNode nodeAerial = rootNode.getFirstChildElement( "aerial" );
    XmlNode nodeMarine = rootNode.getFirstChildElement( "marine" );
    XmlNode nodeGround = rootNode.getFirstChildElement( "ground" );

    if ( nodeAerial.isValid() ) readUnits( nodeAerial, Aerial );
    if ( nodeMarine.isValid() ) readUnits( nodeMarine, Marine );
    if ( nodeGround.isValid() ) readUnits( nodeGround, Ground );
}

////////////////////////////////////////////////////////////////////////////////

void Units::readUnits( XmlNode &groupNode, Type type )
{
    XmlNode nodeUnit = groupNode.getFirstChildElement( "unit" );

    while ( nodeUnit.isValid() )
    {
        readUnit( nodeUnit, type );
        nodeUnit = nodeUnit.getNextSiblingElement( "unit" );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Units::readUnit( XmlNode &unitNode, Type type )
{
    Data data;

    data.index = _units.size();

    data.type = type;

    XmlNode nodeName = unitNode.getFirstChildElement( "name" );

    int result = XmlUtils::read( nodeName, data.name );

    if ( result == SIM_SUCCESS )
    {
        /////////////////////////
        _units.push_back( data );
        /////////////////////////
    }
}
