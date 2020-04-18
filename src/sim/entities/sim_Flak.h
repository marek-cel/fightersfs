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
#ifndef SIM_FLAK_H
#define SIM_FLAK_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Bullet.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Flak bullet class. */
class Flak : public Bullet
{
public:

    static const float m_r_limit;   ///< [m] range damage limit
    static const float m_r_limit_2; ///< [m^2] range damage limit squared

    /** Constructor. */
    Flak( UInt32 shooterId, float fuse_time );

    /** Destructor. */
    virtual ~Flak();

    /** Updates tracer. */
    void update( double timeStep );

private:

    float m_fuse_time;  ///< [s]

    bool m_exploded;    ///<

    void hit( Unit *target );

    void rangeDamage();

    void createBurst();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_FLAK_H
