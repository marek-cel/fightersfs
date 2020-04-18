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
#ifndef SIM_XMLUTILS_H
#define SIM_XMLUTILS_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Types.h>

#include <sim/utils/sim_Text.h>
#include <sim/utils/sim_XmlNode.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** XML utilities class. */
class XmlUtils
{
public:

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    static int read( const XmlNode &node, int &value );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    static int read(const XmlNode &node, float &value );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    static int read( const XmlNode &node, std::string &str );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    static int read( const XmlNode &node, Text &text );

    /** @return FDM_SUCCESS on success or FDM_FAILURE on failure. */
    static int read( const XmlNode &node, Vec3 &vect );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_XMLUTILS_H
