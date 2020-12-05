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
#ifndef SIM_BULLET_H
#define SIM_BULLET_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Munition.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Bullet base class. */
class Bullet : public Munition
{
public:

    static const float _vel_m;      ///< [m/s] muzzle velocity

    /** Constructor. */
    Bullet( UInt16 dp, UInt32 shooterId, float life_span, Group *parent );

    /** Destructor. */
    virtual ~Bullet();

    /** Loads bullet. */
    virtual void load();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_BULLET_H
