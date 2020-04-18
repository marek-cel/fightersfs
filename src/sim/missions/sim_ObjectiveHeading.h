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
#ifndef SIM_OBJECTIVEHEADING_H
#define SIM_OBJECTIVEHEADING_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/missions/sim_Objective.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Heading objective class.
 * <p>Objective is completed if ownship heading is in the given range.</p>
 */
class ObjectiveHeading : public Objective
{
public:

    /** Reads objective from XML file. */
    static ObjectiveHeading* read( const XmlNode &node );

    /** Constructor. */
    ObjectiveHeading();

    /** Destructor. */
    virtual ~ObjectiveHeading();

    /** Updates objective state. */
    void update();

private:

    float m_above;  ///< [rad] minimum heading
    float m_below;  ///< [rad] maximum heading
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OBJECTIVEHEADING_H
