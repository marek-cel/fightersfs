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
