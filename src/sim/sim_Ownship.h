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

    static const float m_target_fov_max;    ///< [rad] target indicators visibility max FoV
    static const float m_target_fov_max_2;  ///< [rad^2] target indicators visibility max FoV squared

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
    inline Aircraft* getAircraft() const { return m_aircraft; }

    /**
     * Returns ownship entity.
     * @return ownship entity
     */
    inline Entity* getEntity() const
    {
        if ( !m_destroyed )
        {
            return m_aircraft;
        }

        return m_aircraftWreckage;
    }

    inline bool getTrigger() const { return m_trigger;  }

    inline float getCtrlRoll()  const { return m_ctrlRoll;  }
    inline float getCtrlPitch() const { return m_ctrlPitch; }
    inline float getCtrlYaw()   const { return m_ctrlYaw;   }
    inline float getThrottle()  const { return m_throttle;  }

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

    Aircraft             *m_aircraft;           ///< ownship aircraft
    Target< UnitAerial > *m_aircraftTarget;     ///< ownship auxiliary target
    WreckageAircraft     *m_aircraftWreckage;   ///< ownship wreckage
    Entity               *m_targetWreckage;     ///< target wreckage

    UInt32 m_id;                    ///< ownship aircraft ID

    double m_timeStep;              ///< [s] time step

    bool m_destroyed;               ///< specifies if ownship have been destroyed

    bool m_trigger;                 ///< ownship trigger state

    float m_ctrlRoll;               ///< [-] roll controls
    float m_ctrlPitch;              ///< [-] pitch controls
    float m_ctrlYaw;                ///< [-] yaw controls
    float m_throttle;               ///< [-] throttle

    PID *m_pid_p;                   ///< roll rate PID controller
    PID *m_pid_q;                   ///< pitch rate PID controller
    PID *m_pid_r;                   ///< yaw rate PID controller

    Quat m_att_own_inv;             ///< inverted ownship attitude

    Vec3 m_pos_cam;                 ///< [m] camera position
    Quat m_att_cam;                 ///< camera attitude expressed as quaternion
    Quat m_att_cam_inv;             ///< inverted camera attitude

    UInt32 m_waypoint_index;        ///< current waypoint index

    bool m_waypoint;                ///< specifies if waypoint indicator is visible

    Vec3 m_waypoint_pos;            ///< [m] waypoint position

    float m_waypoint_tht;           ///< [rad] waypoint relative elevation
    float m_waypoint_psi;           ///< [rad] waypoint relative bearing
    float m_waypoint_phi;           ///< [rad] waypoint relative vertical direction

    float m_waypoint_time;          ///< [s] time since last waypoint

    float m_hit_direction;          ///< [rad] direction from which ownship has been hit

    UInt8 m_ownship_hits;           ///< number of ownship hits

    float m_ownship_hit;            ///< [s] time since last ownship hit
    float m_friend_hit;             ///< [s] time since last friend hit
    float m_target_hit;             ///< [s] time since last target hit
    float m_target_kill;            ///< [s] time since last target killed

    bool  m_target;                 ///< specifies if target indicator is visible
    bool  m_target_box;             ///< specifies if target box is visible
    bool  m_target_cue;             ///< specifies if target cue is visible

    UInt32 m_target_id;             ///< target ID
    UInt16 m_target_hp;             ///< current target hit points

    float m_target_dir_phi;         ///< [rad] target box verical direction
    float m_target_box_tht;         ///< [rad] target box elevation
    float m_target_box_psi;         ///< [rad] target box bearing
    float m_target_cue_tht;         ///< [rad] target cue elevation
    float m_target_cue_psi;         ///< [rad] target cue bearing

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
