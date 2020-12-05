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
#ifndef SIM_UNITGROUND_H
#define SIM_UNITGROUND_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_UnitSurface.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Ground unit base class. */
class UnitGround : public UnitSurface
{
public:

    /** Constructor. */
    UnitGround( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~UnitGround();

    /** Destroys unit. */
    virtual void destroy();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_UNITGROUND_H
