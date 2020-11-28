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

#include <sim/sim_Languages.h>

#include <sim/sim_Base.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlDoc.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Languages::Languages() :
    _current ( 0 ),
    _default ( 0 )
{
    readLanguages();
}

////////////////////////////////////////////////////////////////////////////////

Languages::~Languages() {}

////////////////////////////////////////////////////////////////////////////////

std::string Languages::getCodeByIndex( UInt8 index ) const
{
    if ( index < _list.size() )
    {
        return _list.at( index );
    }

    return std::string();
}

////////////////////////////////////////////////////////////////////////////////

int Languages::getIndexByCode( const std::string &code ) const
{
    for ( UInt8 i = 0; i < _list.size(); i++ )
    {
        if ( 0 == String::icompare( _list[ i ], code ) )
        {
            return i;
        }
    }

    return -1;
}

////////////////////////////////////////////////////////////////////////////////

bool Languages::isCodeValid( const std::string &code ) const
{
    return -1 != getIndexByCode( code );
}

////////////////////////////////////////////////////////////////////////////////

void Languages::setCurrent( UInt8 current )
{
    if ( current < _list.size() )
    {
        _current = current;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Languages::readLanguages()
{
    XmlDoc doc( Base::getPath( "languages.xml" ) );

    if ( doc.isOpen() )
    {
        XmlNode rootNode = doc.getRootNode();

        if ( rootNode.isValid() )
        {
            if ( 0 == String::icompare( rootNode.getName(), "languages" ) )
            {
                XmlNode languageNode = rootNode.getFirstChildElement( "language" );

                while ( languageNode.isValid() )
                {
                    _list.push_back( languageNode.getAttribute( "code" ) );

                    if ( String::toBool( languageNode.getAttribute( "default" ) ) )
                    {
                        _default = _list.size() - 1;
                    }

                    languageNode = languageNode.getNextSiblingElement( "language" );
                }
            }
        }
    }

    _current = _default;
}
