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
#ifndef SIM_FIGHTER_H
#define SIM_FIGHTER_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Target.h>

#include <sim/entities/sim_Aircraft.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Fighter aircraft class. */
class Fighter : public Aircraft
{
public:

    static const std::string _tagName;  ///< unit tag name

    /** Constructor. */
    Fighter( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~Fighter();

    /**
     * Demages fighter.
     * Sets shooter as new target when current target is far.
     * @param dp demage points
     * @param munition munition which hit unit
     */
    virtual void hit( UInt16 dp, const Munition *munition );

    /**
     * Reports hit by fighter's munition.
     * Sets target as a new current target if it's affiliation is opposite.
     * @param target target unit
     */
    virtual void reportTargetHit( Unit *target );

    /** Updates entity. */
    virtual void update( double timeStep );

    /** Returns aircraft target unit pointer. */
    virtual UnitAerial* getTarget() const;

    /**
     * Sets new current target.
     * @param target new current target
     */
    virtual void setTarget( UnitAerial *target );

protected:

    Target< UnitAerial > *_target;          ///< target

    float _target_dist;     ///< [m] distance to the current target
    float _target_bear;     ///< [rad] bearing of the current target
    float _target_tht;      ///< [rad] relative elevation of the current target
    float _target_psi;      ///< [rad] relative azimuth of the current target
    float _target_rad;      ///< [m] target radius

    bool _engaged;          ///< specifies if aircraft is engaged into combat

    virtual void updateControls();
    virtual void updateDestination();
    virtual void updateTarget();
    virtual void updateTrigger();

};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_FIGHTER_H
