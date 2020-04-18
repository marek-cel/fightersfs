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
#ifndef SIM_ROUTE_H
#define SIM_ROUTE_H

////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include <sim/sim_Types.h>

#include <sim/utils/sim_XmlNode.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

typedef std::pair< Vec3, float > WaypointData;  ///< aircraft route waypoint data

////////////////////////////////////////////////////////////////////////////////

/**
 * Aircraft route class.
 * First element is waypoint position, second element is waypoint desired speed.
 */
class Route : public std::vector< WaypointData >
{
public:

    /** Reads route from XML file. */
    static Route* read( const XmlNode &node );

    /** Reads waypoint from XML file.
     * @return FDM_SUCCESS on success or FDM_FAILURE on failure.
     */
    static int read( const XmlNode &node, WaypointData &waypoint );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_ROUTE_H
