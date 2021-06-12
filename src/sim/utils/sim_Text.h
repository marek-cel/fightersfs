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
#ifndef SIM_TEXT_H
#define SIM_TEXT_H

////////////////////////////////////////////////////////////////////////////////

#include <map>
#include <string>

#include <sim/sim_Types.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Text class. */
class Text
{
public:

    typedef std::map< UInt8, std::string > Strings;

    /** @brief Default constructor. */
    Text();

    /** @brief Copy constructor. */
    Text( const Text &text );

    /** @brief Destructor. */
    virtual ~Text();

    void append( const char *str );

    void append( const std::string &str );

    void append( const Text &text );

    /** */
    std::string get() const;

    /** */
    void set( UInt8 index, const std::string &str );

private:

    Strings _strings;
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_TEXT_H
