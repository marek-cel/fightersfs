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
#ifndef SIM_OBJECTIVEDESTROY_H
#define SIM_OBJECTIVEDESTROY_H

////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include <sim/missions/sim_Objective.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Destroy objective class.
 *
 * Objective is completed if all specified units are destroyed.
 */
class ObjectiveDestroy : public Objective
{
public:

    typedef std::vector< UInt32 > UnitsId;

    /** Reads objective from XML file. */
    static ObjectiveDestroy* read( const XmlNode &node );

    /** Constructor. */
    ObjectiveDestroy();

    /** Destructor. */
    virtual ~ObjectiveDestroy();

    /** Adds a unit id to be destroyed. */
    void addUnitId( UInt32 unitId );

    /** Updates objective state. */
    void update();

private:

    UnitsId _unitsId;   ///< IDs of units to be destroyed
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OBJECTIVEDESTROY_H
