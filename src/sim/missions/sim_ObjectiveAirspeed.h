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
#ifndef SIM_OBJECTIVEAIRSPEED_H
#define SIM_OBJECTIVEAIRSPEED_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/missions/sim_Objective.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Airspeed objective class.
 *
 * Objective is completed if ownship airspeed is in the given range.
 */
class ObjectiveAirspeed : public Objective
{
public:

    /** Reads objective from XML file. */
    static ObjectiveAirspeed* read( const XmlNode &node );

    /** Constructor. */
    ObjectiveAirspeed();

    /** Destructor. */
    virtual ~ObjectiveAirspeed();

    /** Updates objective state. */
    void update();

private:

    float _above;   ///< [m/s] minimum airspeed
    float _below;   ///< [m/s] maximum airspeed
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OBJECTIVEAIRSPEED_H
