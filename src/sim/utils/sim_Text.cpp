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
    for ( Strings::iterator it = _strings.begin(); it != _strings.end(); ++it )
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

            ++it;
            ++it_src;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Text::isEmpty() const
{
    bool result = false;

    for ( Strings::const_iterator it = _strings.begin(); it != _strings.end(); ++it )
    {
        result |= it->second.length() > 0;
    }

    return result;
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

////////////////////////////////////////////////////////////////////////////////

const Text& Text::operator=( const Text &text )
{
    _strings = text._strings;

    return (*this);
}
