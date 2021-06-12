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
#ifndef SIM_STRING_H
#define SIM_STRING_H

////////////////////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Text string utilities class. */
class String
{
public:

    /**
     * @brief Compares strings. This function is case insensitive.
     * @param str_1 1st string to compare
     * @param str_2 2nd string to compare
     * @return integer result of the comparison (returns 0 if strings are equal)
     */
    static int icompare( const std::string &str_1, const std::string &str_2 );

    /**
     * @brief Returns string with leading white spaces removed.
     * @param str string to be processed
     * @return string with leading white spaces removed
     */
    static std::string stripLeadingSpaces( const std::string &str );

    /** @brief Converts string into variable. */
    static bool toBool( const std::string &str );

    /** @brief Converts string into variable. */
    static int toInt( const std::string &str );

    /** @brief Converts string into variable. */
    static float toFloat( const std::string &str );

    /** @brief Converts variable into string. */
    static std::string toString( int value );

    /** @brief Converts variable into string. */
    static std::string toString( float value );

    /** @brief Converts wide string into string. */
    static std::string toString( const std::wstring &str );

    /** @brief Converts string to lower case. */
    static std::string toLower( const std::string &str );

    /** @brief Converts wide string to lower case. */
    static std::wstring toLower( const std::wstring &str );

    /** @brief Converts string to upper case. */
    static std::string toUpper( const std::string &str );

    /** @brief Converts wide string to upper case. */
    static std::wstring toUpper( const std::wstring &str );

    /** @brief Converts string to wide string. */
    static std::wstring toWideString( const char *str );

    /** @brief Converts string to wide string. */
    static std::wstring toWideString( const std::string &str );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_STRING_H
