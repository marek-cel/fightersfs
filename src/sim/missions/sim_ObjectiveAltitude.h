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
#ifndef SIM_OBJECTIVEALTITUDE_H
#define SIM_OBJECTIVEALTITUDE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/missions/sim_Objective.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Altitude objective class.
 * <p>Objective is completed if ownship altitude is in the given range.</p>
 */
class ObjectiveAltitude : public Objective
{
public:

    /** Reads objective from XML file. */
    static ObjectiveAltitude* read( const XmlNode &node );

    /** Constructor. */
    ObjectiveAltitude();

    /** Destructor. */
    virtual ~ObjectiveAltitude();

    /** Updates objective state. */
    void update();

private:

    float m_above;  ///< [m] minimum altitude
    float m_below;  ///< [m] maximum altitude
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OBJECTIVEALTITUDE_H
