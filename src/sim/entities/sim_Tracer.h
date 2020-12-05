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
#ifndef SIM_TRACER_H
#define SIM_TRACER_H

////////////////////////////////////////////////////////////////////////////////

#include <osgParticle/SmokeTrailEffect>

#include <sim/entities/sim_Bullet.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Tracer bullet class. */
class Tracer : public Bullet
{
public:

    /** Constructor. */
    Tracer( UInt32 shooterId, bool trail = false );

    /** Destructor. */
    virtual ~Tracer();

    /** Loads bullet. */
    virtual void load();

    /** Updates tracer. */
    void update( double timeStep );

private:

//    static UInt8 _counter_trail;
//    static UInt8 _quantity;

    float _elevation;  ///< [m] terrain elevation

//    bool _trail;        ///< specifies if tracer is a smoke trail
//    unsigned int _counter_update;
//    osg::ref_ptr<osgParticle::SmokeTrailEffect> _smokeTrail;

    /** */
    void updateElevation();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_TRACER_H
