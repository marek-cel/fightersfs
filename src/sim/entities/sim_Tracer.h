/****************************************************************************//*
 * Copyright (C) 2021 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
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

    bool _trail;        ///< specifies if tracer is a smoke trail
//    unsigned int _counter_update;
//    osg::ref_ptr<osgParticle::SmokeTrailEffect> _smokeTrail;

    /** */
    void updateElevation();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_TRACER_H
