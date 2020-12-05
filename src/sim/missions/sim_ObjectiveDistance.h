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
#ifndef SIM_OBJECTIVEDISTANCE_H
#define SIM_OBJECTIVEDISTANCE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/missions/sim_Objective.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Distance objective class.
 *
 * Objective is completed if ownship distance to the specified unit
 * is less than given limit.
 *
 * This objective should be used with another type of objective, because it is
 * assumed to be completed as long as ownship is closer to the specified unit
 * than given distance so if there were no other objective mission stage would
 * be immediately completed.
 */
class ObjectiveDistance : public Objective
{
public:

    /** Reads objective from XML file. */
    static ObjectiveDistance* read( const XmlNode &node );

    /** Constructor. */
    ObjectiveDistance();

    /** Destructor. */
    virtual ~ObjectiveDistance();

    /** Updates objective state. */
    void update();

private:

    UInt32 _unitId;     ///< unit ID

    float _limit;       ///< [m] maximum distance
    float _limit_2;     ///< [m] maximum distance squared
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OBJECTIVEDISTANCE_H
