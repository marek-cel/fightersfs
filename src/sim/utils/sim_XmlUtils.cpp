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

#include <sim/utils/sim_XmlUtils.h>

#include <sstream>

#include <sim/sim_Defines.h>
#include <sim/sim_Languages.h>

#include <sim/utils/sim_Misc.h>
#include <sim/utils/sim_String.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

int XmlUtils::read( const XmlNode &node, int &value )
{
    if ( node.isValid() )
    {
        XmlNode textNode = node.getFirstChild();

        if ( textNode.isValid() && textNode.isText() )
        {
            std::string text = String::stripLeadingSpaces( textNode.getText() );

            value = String::toInt( text );

            if ( Misc::isValid( value ) )
            {
                return SIM_SUCCESS;
            }
        }
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int XmlUtils::read( const XmlNode &node, float &value )
{
    if ( node.isValid() )
    {
        XmlNode textNode = node.getFirstChild();

        if ( textNode.isValid() && textNode.isText() )
        {
            std::string text = String::stripLeadingSpaces( textNode.getText() );

            value = String::toFloat( text );

            if ( Misc::isValid( value ) )
            {
                return SIM_SUCCESS;
            }
        }
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int XmlUtils::read( const XmlNode &node, std::string &str )
{
    if ( node.isValid() )
    {
        XmlNode textNode = node.getFirstChild();

        if ( textNode.isValid() && textNode.isText() )
        {
            str = textNode.getText();

            return SIM_SUCCESS;
        }
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int XmlUtils::read( const XmlNode &node, Text &text )
{
    if ( node.isValid() )
    {
        XmlNode textNode = node.getFirstChildElement( "text" );

        while ( textNode.isValid() )
        {
            std::string str = "";

            if ( SIM_SUCCESS == read( textNode, str ) )
            {
                std::string lang = textNode.getAttribute( "lang" );

                int index = Languages::instance()->getIndexByCode( lang );

                if ( index >= 0 )
                {
                    text.set( index, str );
                }
            }

            textNode = textNode.getNextSiblingElement( "text" );
        }

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int XmlUtils::read( const XmlNode &node, Vec3 &vect )
{
    if ( node.isValid() )
    {
        XmlNode textNode = node.getFirstChild();

        if ( textNode.isValid() && textNode.isText() )
        {
            double x = std::numeric_limits< double >::quiet_NaN();
            double y = std::numeric_limits< double >::quiet_NaN();
            double z = std::numeric_limits< double >::quiet_NaN();

            std::stringstream ss( String::stripLeadingSpaces( textNode.getText() ) );

            ss >> x;
            ss >> y;
            ss >> z;

            if ( Misc::isValid( x ) && Misc::isValid( y ) && Misc::isValid( z ) )
            {
                vect.x() = x;
                vect.y() = y;
                vect.z() = z;

                return SIM_SUCCESS;
            }
        }
    }

    return SIM_FAILURE;
}
