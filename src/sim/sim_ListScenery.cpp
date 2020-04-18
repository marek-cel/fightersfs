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

#include <sim/sim_ListScenery.h>

#include <sim/sim_Base.h>

#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlDoc.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ListScenery::ListScenery()
{
    readScenery();
}

////////////////////////////////////////////////////////////////////////////////

ListScenery::~ListScenery() {}

////////////////////////////////////////////////////////////////////////////////

ListScenery::SceneryData ListScenery::getData( UInt32 index )
{
    return m_data[ index ];
}

////////////////////////////////////////////////////////////////////////////////

void ListScenery::readScenery()
{
    XmlDoc doc( Base::getPath( "scenery.xml" ) );

    if ( doc.isOpen() )
    {
        XmlNode rootNode = doc.getRootNode();

        if ( rootNode.isValid() )
        {
            if ( 0 == String::icompare( rootNode.getName(), "scenery_list" ) )
            {
                XmlNode sceneryNode = rootNode.getFirstChildElement( "scenery" );

                while ( sceneryNode.isValid() )
                {
                    readScenery( sceneryNode );
                    sceneryNode = sceneryNode.getNextSiblingElement( "scenery" );
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void ListScenery::readScenery( const XmlNode &node )
{
    XmlNode terrainNode = node.getFirstChildElement( "terrain" );
    XmlNode genericNode = node.getFirstChildElement( "generic" );

    if ( terrainNode.isValid() && genericNode.isValid() )
    {
        std::string terrainFile;
        std::string genericFile;

        if ( SIM_SUCCESS == XmlUtils::read( terrainNode, terrainFile )
          && SIM_SUCCESS == XmlUtils::read( genericNode, genericFile ) )
        {
            SceneryData data;

            data.terrainFile = terrainFile;
            data.genericFile = genericFile;

            // elevation
            XmlNode elevationNode = node.getFirstChildElement( "elevation" );

            if ( elevationNode.isValid() )
            {
                std::string elevationFile;

                if ( SIM_SUCCESS == XmlUtils::read( elevationNode, elevationFile ) )
                {
                    data.elevationFile = elevationFile;
                }
            }

            // objects
            XmlNode objectsNode = node.getFirstChildElement( "objects" );

            if ( objectsNode.isValid() )
            {
                XmlNode objectNode = objectsNode.getFirstChildElement( "object" );

                while ( objectNode.isValid() )
                {
                    std::string objectFile;

                    if ( SIM_SUCCESS == XmlUtils::read( objectNode, objectFile ) )
                    {
                        data.objectFiles.push_back( objectFile );
                    }

                    objectNode = objectNode.getNextSiblingElement( "object" );
                }
            }

            XmlNode initPositionNode = node.getFirstChildElement( "initial_position" );

            XmlUtils::read( initPositionNode, data.initialPosition );

            m_data.push_back( data );
        }
    }
}
