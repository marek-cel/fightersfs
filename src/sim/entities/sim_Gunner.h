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
#ifndef SIM_GUNNER_H
#define SIM_GUNNER_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Target.h>

#include <sim/entities/sim_UnitAerial.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Gunner class. */
class Gunner : public Entity
{
public:

    static const float _10deg_rad;      ///< [rad] 10 deg
    static const float _20deg_rad;      ///< [rad] 20 deg
    static const float _40deg_rad;      ///< [rad] 40 deg
    static const float _45deg_rad;      ///< [rad] 45 deg
    static const float _60deg_rad;      ///< [rad] 60 deg
    static const float _80deg_rad;      ///< [rad] 80 deg

    /** Constructor. */
    Gunner( Affiliation affiliation, Group *parent = 0, float range = 1000.0f );

    /** Destructor. */
    virtual ~Gunner();

    /** Updates gunner. */
    virtual void update( double timeStep );

protected:

    Affiliation _affiliation;   ///< gunner affiliation

    bool _parent_valid;         ///<
    UInt32 _parent_id;          ///<

    float _range;               ///< [m] range

    float _inertia;             ///< [s] gunner inertia time constant
    bool  _advanced;            ///< specifies if gunner predetermines target position

    Target< UnitAerial > *_target;  ///< target

    Quat _target_dir;           ///< absolute direction to target

    float _target_dist;         ///< [m] distance to target
    float _target_tht;          ///< [rad] target relative elevation
    float _target_psi;          ///< [rad] target relative bearing

    bool _trigger;

    bool _target_valid;         ///< specifies if target is valid

    Vec3 _pos_abs;              ///< [m] gunner absolute position
    Quat _att_abs;              ///< gunner absolute attitude

    float _shoot_time;          ///< [s] time since last shot

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

    UInt8 _burst_count;
    float _burst_time;          ///< [s] time since last shot

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

    const float _psi_min;       ///< [rad]
    const float _psi_max;       ///< [rad]

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
