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
