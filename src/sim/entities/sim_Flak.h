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
#ifndef SIM_FLAK_H
#define SIM_FLAK_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Bullet.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Flak bullet class. */
class Flak : public Bullet
{
public:

    static const float _r_limit;    ///< [m] range damage limit
    static const float _r_limit_2;  ///< [m^2] range damage limit squared

    /** Constructor. */
    Flak( UInt32 shooterId, float fuse_time );

    /** Destructor. */
    virtual ~Flak();

    /** Updates tracer. */
    void update( double timeStep );

private:

    float _fuse_time;   ///< [s] fuse time delay

    bool _exploded;     ///<

    void hit( Unit *target );

    void rangeDamage();

    void createBurst();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_FLAK_H
