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

#include <sim/sim_Captions.h>

#include <sim/sim_Base.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlDoc.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Captions::Captions()
{
    readCaptions();
}

////////////////////////////////////////////////////////////////////////////////

Captions::~Captions() {}

////////////////////////////////////////////////////////////////////////////////

void Captions::readCaptions()
{
    XmlDoc doc( Base::getPath( "captions.xml" ) );

    if ( doc.isOpen() )
    {
        XmlNode rootNode = doc.getRootNode();

        if ( rootNode.isValid() )
        {
            if ( 0 == String::icompare( rootNode.getName(), "captions" ) )
            {
                readText( rootNode, _loading         , "loading"         );
#               ifdef SIM_DESKTOP
                readText( rootNode, _resume          , "resume_desktop"  );
                readText( rootNode, _begin           , "begin_desktop"   );
#               else
                readText( rootNode, _resume          , "resume_mobile"   );
                readText( rootNode, _begin           , "begin_mobile"    );
#               endif
                readText( rootNode, _mission_success , "mission_success" );
                readText( rootNode, _mission_failure , "mission_failure" );
                readText( rootNode, _friendly_fire   , "friendly_fire"   );
                readText( rootNode, _target_killed   , "target_killed"   );
                readText( rootNode, _target_hit      , "target_hit"      );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Captions::readText( XmlNode &node, Text &text, const char *name )
{
    XmlNode textNode = node.getFirstChildElement( name );

    if ( textNode.isValid() )
    {
        Text temp;

        if ( SIM_SUCCESS == XmlUtils::read( textNode, temp ) )
        {
            text = temp;
        }
    }
}

