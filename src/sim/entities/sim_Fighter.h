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
