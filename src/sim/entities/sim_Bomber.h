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

/** Bomber aircraft base class. */
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

    Target< UnitSurface > *m_target;        ///< target

    std::vector< Munition* > m_ordnance;    ///< bomber ordnance

    UInt8 m_ordnanceIndex;  ///< current ordnance index

    Vec3  m_target_pos;     ///< [m] target position expressed ine ENU
    float m_target_dist;    ///< [m] horizontal distance to the current target
    float m_target_alt;     ///< [m] relative altitude above target
    float m_target_tht;     ///< [rad] relative elevation of the current target
    float m_target_psi;     ///< [rad] relative azimuth of the current target

    bool m_engaged;         ///< specifies if aircraft is engaged into combat
    bool m_trigger;         ///< trigger

    virtual void readOrdnance( const XmlNode &node );
    virtual void readOrdnance( const XmlNode &node, Munition *ordnance );

    virtual void releaseWeapon();

    virtual void updateDestination();
    virtual void updateTarget();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_BOMBER_H
