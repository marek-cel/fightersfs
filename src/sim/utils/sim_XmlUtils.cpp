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

#include <sim/utils/sim_XmlUtils.h>

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
            std::string text = String::stripLeadingSpaces( textNode.getText() );

            double x = std::numeric_limits< double >::quiet_NaN();
            double y = std::numeric_limits< double >::quiet_NaN();
            double z = std::numeric_limits< double >::quiet_NaN();

            int valRead = sscanf( text.c_str(),
                                  "%lf %lf %lf",
                                  &x, &y, &z );

            if ( valRead == 3 )
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
