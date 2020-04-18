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
#ifndef SIM_TORPEDO_H
#define SIM_TORPEDO_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Munition.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Torpedo class. */
class Torpedo : public Munition
{
public:

    /** Constructor. */
    Torpedo( UInt32 shooterId, Group *parent = 0 );

    /** Destructor. */
    virtual ~Torpedo();

    /** Updates torpedo. */
    virtual void update( double timeStep );

protected:

    float m_elevation;  ///< [m] terrain elevation

    virtual void hit( Unit *target );

    virtual void updateElevation();
    virtual void updateVelocity();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_TORPEDO_H
