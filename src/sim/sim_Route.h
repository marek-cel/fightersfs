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
 * @brief Aircraft route class.
 *
 * First element is waypoint position, second element is waypoint desired speed.
 */
class Route : public std::vector< WaypointData >
{
public:

    /** @brief Reads route from XML file. */
    static Route* read( const XmlNode &node );

    /**
     * @brief Reads waypoint from XML file.
     * @return FDM_SUCCESS on success or FDM_FAILURE on failure.
     */
    static int read( const XmlNode &node, WaypointData &waypoint );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_ROUTE_H
