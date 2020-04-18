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
#ifndef SIM_GUNNER_H
#define SIM_GUNNER_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Target.h>

#include <sim/entities/sim_UnitAerial.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Gunner class. */
class Gunner : public Entity
{
public:

    static const float m_10deg_rad;     ///< [rad] 10 deg
    static const float m_20deg_rad;     ///< [rad] 20 deg
    static const float m_40deg_rad;     ///< [rad] 40 deg
    static const float m_45deg_rad;     ///< [rad] 45 deg
    static const float m_60deg_rad;     ///< [rad] 60 deg
    static const float m_80deg_rad;     ///< [rad] 80 deg

    /** Constructor. */
    Gunner( Affiliation affiliation, Group *parent = 0, float range = 1000.0f );

    /** Destructor. */
    virtual ~Gunner();

    /** Updates gunner. */
    virtual void update( double timeStep );

protected:

    Affiliation m_affiliation;  ///< gunner affiliation

    bool m_parent_valid;        ///<
    UInt32 m_parent_id;         ///<

    float m_range;              ///< [m] range

    float m_inertia;            ///< [s] gunner inertia time constant
    bool  m_advanced;           ///< specifies if gunner predetermines target position

    Target< UnitAerial > *m_target; ///< target

    Quat m_target_dir;          ///< absolute direction to target

    float m_target_dist;        ///< [m] distance to target
    float m_target_tht;         ///< [rad] target relative elevation
    float m_target_psi;         ///< [rad] target relative bearing

    bool m_trigger;

    bool m_target_valid;        ///< specifies if target is valid

    Vec3 m_pos_abs;             ///< [m] gunner absolute position
    Quat m_att_abs;             ///< gunner absolute attitude

    float m_shoot_time;         ///< [s] time since last shot

    virtual void updateWeapons();
};

////////////////////////////////////////////////////////////////////////////////

/** Flak gunner class. */
class GunnerFlak : public Gunner
{
public:

    /** Constructor. */
    GunnerFlak( Affiliation affiliation, Group *parent = 0 );

    /** Destructor. */
    virtual ~GunnerFlak();

protected:

    UInt8 m_burst_count;
    float m_burst_time;         ///< [s] time since last shot

    virtual void updateWeapons();
};

////////////////////////////////////////////////////////////////////////////////

/**
 * Rear gunner class. This gunner shoots only backwards and tries to avoid
 * horizontal and vertical stabilizers of his own aircraft.
 */
class GunnerRear : public Gunner
{
public:

    /** Constructor. */
    GunnerRear( Affiliation affiliation, Group *parent = 0 );

    /** Destructor. */
    virtual ~GunnerRear();

private:

    virtual void updateWeapons();
};

////////////////////////////////////////////////////////////////////////////////

/** Zone gunner class. This gunner shoots only in defined azimuth zone. */
class GunnerZone : public Gunner
{
public:

    /** Constructor. */
    GunnerZone( Affiliation affiliation, float psi_min, float psi_max,
                Group *parent = 0 );

    /** Destructor. */
    virtual ~GunnerZone();

protected:

    const float m_psi_min;      ///< [rad]
    const float m_psi_max;      ///< [rad]

    virtual void updateWeapons();
};

////////////////////////////////////////////////////////////////////////////////

/** Back gunner class. This gunner shoots only backwards. */
class GunnerBack : public GunnerZone
{
public:

    /** Constructor. */
    GunnerBack( Affiliation affiliation, Group *parent = 0 );

    /** Destructor. */
    virtual ~GunnerBack();
};

////////////////////////////////////////////////////////////////////////////////

/** Front gunner class. This gunner shoots only forward. */
class GunnerFront : public GunnerZone
{
public:

    /** Constructor. */
    GunnerFront( Affiliation affiliation, Group *parent = 0 );

    /** Destructor. */
    virtual ~GunnerFront();
};

////////////////////////////////////////////////////////////////////////////////

/** Left gunner class. This gunner shoots only left backwards. */
class GunnerLeft : public GunnerZone
{
public:

    /** Constructor. */
    GunnerLeft( Affiliation affiliation, Group *parent = 0 );

    /** Destructor. */
    virtual ~GunnerLeft();
};

////////////////////////////////////////////////////////////////////////////////

/** Right gunner class. This gunner shoots only right backwards. */
class GunnerRight : public GunnerZone
{
public:

    /** Constructor. */
    GunnerRight( Affiliation affiliation, Group *parent = 0 );

    /** Destructor. */
    virtual ~GunnerRight();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_GUNNER_H
