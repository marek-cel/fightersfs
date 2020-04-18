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
#ifndef SIM_OBJECTIVEDEFEND_H
#define SIM_OBJECTIVEDEFEND_H

////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include <sim/missions/sim_Objective.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Defend objective class.
 * <p>Objective is completed if there are at least minimum not destroyed
 * specified units.</p>
 * <p>This objective should be used with another type of objective, because
 * it is assumed to be completed as long as there are minimum not destroyed
 * units so if there were no other objective mission stage would be immediately
 * completed.</p>
 */
class ObjectiveDefend : public Objective
{
public:

    typedef std::vector< UInt32 > UnitsId;

    /** Reads objective from XML file. */
    static ObjectiveDefend* read( const XmlNode &node );

    /** Constructor. */
    ObjectiveDefend();

    /** Destructor. */
    virtual ~ObjectiveDefend();

    /** Updates objective state. */
    void update();

private:

    UnitsId m_unitsId;      ///< IDs of units to be defended

    unsigned int m_minimum; ///< minimum number of units
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OBJECTIVEDEFEND_H
