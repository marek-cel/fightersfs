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
    return _data[ index ];
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

            _data.push_back( data );
        }
    }
}
