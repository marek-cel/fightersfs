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
#ifndef SIM_BOMB_H
#define SIM_BOMB_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Munition.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Bomb class. */
class Bomb : public Munition
{
public:

    static const float _r_limit;    ///< [m] range damage limit
    static const float _r_limit_2;  ///< [m^2] range damage limit squared

    /** Constructor. */
    Bomb( UInt32 shooterId, Group *parent = 0 );

    /** Destructor. */
    virtual ~Bomb();

    /** Updates bomb. */
    virtual void update( double timeStep );

protected:

    float _elevation;   ///< [m] terrain elevation

    virtual void hit( Unit *target );

    virtual void rangeDamage();

    virtual void updateElevation();
    virtual void updateVelocity();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_BOMB_H
