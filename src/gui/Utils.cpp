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

#include <gui/Utils.h>

#include <sim/sim_Languages.h>

////////////////////////////////////////////////////////////////////////////////

int Utils::read( const QDomElement &node, Text &text )
{
    if ( !node.isNull() )
    {
        QDomElement textNode = node.firstChildElement( "text" );

        while ( !textNode.isNull() )
        {
            std::string str = textNode.text().toStdString();

            if ( str.length() > 0 )
            {
                std::string lang = textNode.attribute( "lang" ).toStdString();

                int index = sim::Languages::instance()->getIndexByCode( lang );

                if ( index >= 0 )
                {
                    text.set( index, str );
                }
            }

            textNode = textNode.nextSiblingElement( "text" );
        }

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void Utils::m2ftin( double len_m, int *len_ft, int *len_in )
{
    double temp_ft = Convert::m2ft( len_m );

    *len_ft = floor( temp_ft );
    *len_in = floor( 12.0 * ( temp_ft - *len_ft ) + 0.5 );

    if ( *len_in == 12 )
    {
        *len_ft += 1;
        *len_in = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////

QString Utils::m2ftin( double len_m )
{
    int len_ft = 0;
    int len_in = 0;

    m2ftin( len_m, &len_ft, &len_in );

    return QString::number( len_ft ) + " ft " + QString::number( len_in ) + " in";
}
