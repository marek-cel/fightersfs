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

#include <sim/utils/sim_Text.h>

#include <sim/sim_Languages.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Text::Text() {}

////////////////////////////////////////////////////////////////////////////////

Text::Text( const Text &text )
{
    _strings = text._strings;
}

////////////////////////////////////////////////////////////////////////////////

Text::~Text() {}

////////////////////////////////////////////////////////////////////////////////

void Text::append( const char *str )
{
    for ( Strings::iterator it = _strings.begin(); it != _strings.end(); it++ )
    {
        it->second += str;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Text::append( const std::string &str )
{
    append( str.c_str() );
}

////////////////////////////////////////////////////////////////////////////////

void Text::append( const Text &text )
{
    if ( _strings.size() == text._strings.size() )
    {
        Strings::iterator it = _strings.begin();
        Strings::const_iterator it_src = text._strings.begin();

        while ( it != _strings.end() && it_src != text._strings.end() )
        {
            it->second += it_src->second;

            it++;
            it_src++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

std::string Text::get() const
{
    Strings::const_iterator it = _strings.find( Languages::instance()->getCurrent() );

    if ( it != _strings.end() )
    {
        return it->second;
    }

    return std::string();
}

////////////////////////////////////////////////////////////////////////////////

void Text::set( UInt8 index, const std::string &str )
{
    Strings::iterator it = _strings.find( index );

    if ( it != _strings.end() )
    {
        it->second = str;
    }
    else
    {
        _strings.insert( std::pair< UInt8, std::string >( index, str ) );
    }
}
