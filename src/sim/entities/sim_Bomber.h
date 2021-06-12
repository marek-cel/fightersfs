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
#ifndef SIM_BOMBER_H
#define SIM_BOMBER_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Target.h>

#include <sim/entities/sim_Aircraft.h>
#include <sim/entities/sim_Munition.h>
#include <sim/entities/sim_UnitSurface.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Bomber aircraft base class. */
class Bomber : public Aircraft
{
public:

    /** Constructor. */
    Bomber( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~Bomber();

    /** Updates entity. */
    virtual void update( double timeStep );

    /** Returns aircraft target unit pointer. */
    virtual UnitSurface* getTarget() const;

protected:

    Target< UnitSurface > *_target;         ///< target

    std::vector< Munition* > _ordnance;     ///< bomber ordnance

    UInt8 _ordnanceIndex;   ///< current ordnance index

    Vec3  _target_pos;      ///< [m] target position expressed ine ENU
    float _target_dist;     ///< [m] horizontal distance to the current target
    float _target_alt;      ///< [m] relative altitude above target
    float _target_tht;      ///< [rad] relative elevation of the current target
    float _target_psi;      ///< [rad] relative azimuth of the current target

    bool _engaged;          ///< specifies if aircraft is engaged into combat
    bool _trigger;          ///< trigger

    virtual void readOrdnance( const XmlNode &node );
    virtual void readOrdnance( const XmlNode &node, Munition *ordnance );

    virtual void releaseWeapon();

    virtual void updateDestination();
    virtual void updateTarget();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_BOMBER_H
