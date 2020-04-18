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
#ifndef SIM_WARSHIP_H
#define SIM_WARSHIP_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_UnitMarine.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Warship class. */
class Warship : public UnitMarine
{
public:

    static const std::string m_tagName; ///<

    /** Constructor. */
    Warship( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~Warship();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_WARSHIP_H
