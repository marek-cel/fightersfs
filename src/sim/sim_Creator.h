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
#ifndef SIM_CREATOR_H
#define SIM_CREATOR_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Aircraft.h>
#include <sim/missions/sim_Mission.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** */
class Creator
{
public:

    /**
     * @brief Creates aircraft.
     * @param index
     * @param livery
     * @param affiliation
     * @return aircraft object
     */
    static Aircraft* createAircraft( int index, int livery, Affiliation affiliation );

    /**
     * @brief Creates unit due to unit type and file.
     * @param type unit type
     * @param file unit file
     * @return unit object
     */
    static Unit* createUnit( const std::string &type, const std::string &file,
                             Affiliation affiliation );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_CREATOR_H
