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
#ifndef SIM_OWNSHIP_H
#define SIM_OWNSHIP_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Fighter.h>
#include <sim/entities/sim_WreckageAircraft.h>

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Ownship class.
 * <p>Ownship is a unit controlled by player. As long as there cannot be more
 * than one such a unit ownship is a singleton class.</p>
 * <p>Ownship class gives interface and features essential for the purpose
 * of controlling unit and giving necessary feedback.</p>
 */
class Ownship : public Singleton< Ownship >
{
    friend class Singleton< Ownship >;

public:

    static const float _target_fov_max;     ///< [rad] target indicators visibility max FoV
    static const float _target_fov_max_2;   ///< [rad^2] target indicators visibility max FoV squared

    /** Constructor. */
    Ownship();

    /** Destructor. */
    virtual ~Ownship();

    /** Initializes ownship. */
    void init();

    /** This function should be called when ownship has been destroyed. */
    void reportDestroyed();

    /** This function should be called when ownship has been hit. */
    void reportHit( const Munition *munition );

    /** This function should be called when ownship has hit other unit. */
    void reportTargetHit( Unit *target );

    /** This function should be called when ownship has destroyed other unit. */
    void reportTargetKill( Unit *target );

    /**
     * Updates ownship.
     * @param timeStep [s] simulation time step
     */
    void update( double timeStep );

    /**
     * Returns ownship aircraft.
     * @return ownship aircraft
     */
    inline Aircraft* getAircraft() const { return _aircraft; }

    /**
     * Returns ownship entity.
     * @return ownship entity
     */
    inline Entity* getEntity() const
    {
        if ( !_destroyed )
        {
            return _aircraft;
        }

        return _aircraftWreckage;
    }

    inline bool getTrigger() const { return _trigger;  }

    inline float getCtrlRoll()  const { return _ctrlRoll;  }
    inline float getCtrlPitch() const { return _ctrlPitch; }
    inline float getCtrlYaw()   const { return _ctrlYaw;   }
    inline float getThrottle()  const { return _throttle;  }

    /**
     * Sets current ownship aircraft.
     * @param aircraft ownship aircraft
     */
    void setAircraft( Aircraft *aircraft );

    /**
     * Sets wreckage entity after ownship has been destroyed.
     * @param wreckage ownship wreckage entity
     */
    void setWreckage( WreckageAircraft *wreckage );

private:

    Aircraft             *_aircraft;            ///< ownship aircraft
    Target< UnitAerial > *_aircraftTarget;      ///< ownship auxiliary target
    WreckageAircraft     *_aircraftWreckage;    ///< ownship wreckage
    Entity               *_targetWreckage;      ///< target wreckage

    UInt32 _id;                     ///< ownship aircraft ID

    double _timeStep;               ///< [s] time step

    bool _destroyed;                ///< specifies if ownship have been destroyed

    bool _trigger;                  ///< ownship trigger state

    float _ctrlRoll;                ///< [-] roll controls
    float _ctrlPitch;               ///< [-] pitch controls
    float _ctrlYaw;                 ///< [-] yaw controls
    float _throttle;                ///< [-] throttle

    PID *_pid_p;                    ///< roll rate PID controller
    PID *_pid_q;                    ///< pitch rate PID controller
    PID *_pid_r;                    ///< yaw rate PID controller

    Quat _att_own_inv;              ///< inverted ownship attitude

    Vec3 _pos_cam;                  ///< [m] camera position
    Quat _att_cam;                  ///< camera attitude expressed as quaternion
    Quat _att_cam_inv;              ///< inverted camera attitude

    UInt32 _waypoint_index;         ///< current waypoint index

    bool _waypoint;                 ///< specifies if waypoint indicator is visible

    Vec3 _waypoint_pos;             ///< [m] waypoint position

    float _waypoint_tht;            ///< [rad] waypoint relative elevation
    float _waypoint_psi;            ///< [rad] waypoint relative bearing
    float _waypoint_phi;            ///< [rad] waypoint relative vertical direction

    float _waypoint_time;           ///< [s] time since last waypoint

    float _hit_direction;           ///< [rad] direction from which ownship has been hit

    UInt8 _ownship_hits;            ///< number of ownship hits

    float _ownship_hit;             ///< [s] time since last ownship hit
    float _friend_hit;              ///< [s] time since last friend hit
    float _target_hit;              ///< [s] time since last target hit
    float _target_kill;             ///< [s] time since last target killed

    bool  _target;                  ///< specifies if target indicator is visible
    bool  _target_box;              ///< specifies if target box is visible
    bool  _target_cue;              ///< specifies if target cue is visible

    UInt32 _target_id;              ///< target ID
    UInt16 _target_hp;              ///< current target hit points

    float _target_dir_phi;          ///< [rad] target box verical direction
    float _target_box_tht;          ///< [rad] target box elevation
    float _target_box_psi;          ///< [rad] target box bearing
    float _target_cue_tht;          ///< [rad] target cue elevation
    float _target_cue_psi;          ///< [rad] target cue bearing

    void idleOutput();

    void reset();

    void updateCamera();
    void updateControls();
    void updateTarget();
    void updateWaypoint();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OWNSHIP_H
