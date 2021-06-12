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

