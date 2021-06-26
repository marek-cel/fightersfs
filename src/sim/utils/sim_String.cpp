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

#include <sim/utils/sim_String.h>

#include <iostream>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string.h>

#include <sim/sim_Defines.h>

#include <sim/utils/sim_Misc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

int String::icompare( const std::string &str_1, const std::string &str_2 )
{
    std::string strTemp_1 = toLower( str_1 );
    std::string strTemp_2 = toLower( str_2 );

    return strTemp_1.compare( strTemp_2 );
}

////////////////////////////////////////////////////////////////////////////////

std::string String::stripLeadingSpaces( const std::string &str )
{
    char *temp = new char [ str.length() + 1 ];
    strcpy( temp, str.c_str() );

    unsigned int offset = 0;

    for ( unsigned int i = 0; i < str.size(); i++ )
    {
        if ( !isspace( temp[ i ] ) )
        {
            break;
        }

        offset++;
    }

    std::string result = &( temp[ offset ] );

    DELTAB( temp );

    return result;
}

////////////////////////////////////////////////////////////////////////////////

bool String::toBool( const std::string &str )
{
    bool result = false;
    int temp = std::numeric_limits< int >::quiet_NaN();

    std::stringstream ss( str );

    ss >> temp;

    if ( Misc::isValid( temp ) )
    {
        result = temp != 0;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

int String::toInt( const std::string &str )
{
    int result = std::numeric_limits< int >::quiet_NaN();
    int temp = std::numeric_limits< int >::quiet_NaN();

    std::stringstream ss( str );

    ss >> temp;

    if ( Misc::isValid( temp ) )
    {
        result = temp;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

float String::toFloat( const std::string &str )
{
    float result = std::numeric_limits< double >::quiet_NaN();
    float temp = std::numeric_limits< float >::quiet_NaN();

    std::stringstream ss( str );

    ss >> temp;

    if ( Misc::isValid( temp ) )
    {
        result = temp;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::string String::toString( int value )
{
    std::stringstream ss;

    ss << value;

    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

std::string String::toString( float value )
{
    std::stringstream ss;

    ss << value;

    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

std::string String::toString( const std::wstring &str )
{
    size_t length = str.length() + 1;

    char *mbstr = new char [ 2*length ];

    wcstombs( mbstr, str.c_str(), length );

    std::string result( mbstr );

    delete [] mbstr;

    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::string String::toLower( const std::string &str )
{
    std::string result;
    result.clear();

    unsigned int length = str.length();

    for ( unsigned int i = 0; i < length; i++ )
    {
        result += tolower( str.c_str()[ i ] );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::wstring String::toLower( const std::wstring &str )
{
    std::wstring result;
    result.clear();

    unsigned int length = str.length();

    for ( unsigned int i = 0; i < length; i++ )
    {
        result += tolower( str.c_str()[ i ] );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::string String::toUpper( const std::string &str )
{
    std::string result;
    result.clear();

    unsigned int length = str.length();

    for ( unsigned int i = 0; i < length; i++ )
    {
        result += toupper( str.c_str()[ i ] );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::wstring String::toUpper( const std::wstring &str )
{
    std::wstring result;
    result.clear();

    unsigned int length = str.length();

    for ( unsigned int i = 0; i < length; i++ )
    {
        result += toupper( str.c_str()[ i ] );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::wstring String::toWideString( const char *str )
{
    return toWideString( std::string( str ) );
}

////////////////////////////////////////////////////////////////////////////////

std::wstring String::toWideString( const std::string &str )
{
    size_t length = str.length() + 1;

    wchar_t *wstr = new wchar_t [ length ];

    mbstowcs( wstr, str.c_str(), length );

    std::wstring result( wstr );

    delete [] wstr;

    return result;

}
