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
#ifndef SIM_DATA_H
#define SIM_DATA_H

////////////////////////////////////////////////////////////////////////////////

#include <memory.h>

#include <sim/sim_Defines.h>
#include <sim/sim_Types.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Simulation data access class. */
class Data
{
public:

    /** Simulation data struct. */
    struct SimulationData
    {
        /** Camera data struct. */
        struct Camera
        {
            ViewType type;              ///< view type

            float altitude_asl;         ///< [m] altitude above sea level

            float pos_x;                ///< [m] position x-coordinate
            float pos_y;                ///< [m] position y-coordinate
            float pos_z;                ///< [m] position z-coordinate

            float att_w;                ///< attitude w-coordinate expressed as quaternion
            float att_x;                ///< attitude x-coordinate expressed as quaternion
            float att_y;                ///< attitude y-coordinate expressed as quaternion
            float att_z;                ///< attitude z-coordinate expressed as quaternion

            float att_phi;              ///< [rad] attitude phi angle
            float att_tht;              ///< [rad] attitude theta angle
            float att_psi;              ///< [rad] attitude psi angle

            float d_x;                  ///< [m] camera offset x-coordinate
            float d_y;                  ///< [m] camera offset y-coordinate
            float d_z;                  ///< [m] camera offset z-coordinate

            float d_phi;                ///< [rad] camera offset phi angle
            float d_tht;                ///< [rad] camera offset theta angle
            float d_psi;                ///< [rad] camera offset psi angle
        };

        /** Controls data struct. */
        struct Controls
        {
            bool autopilot;             ///< autopilot

            bool trigger;               ///< trigger

            float ctrlRoll;             ///< [-1.0,1.0] roll controls
            float ctrlPitch;            ///< [-1.0,1.0] pitch controls
            float ctrlYaw;              ///< [-1.0,1.0] yaw controls
            float throttle;             ///< [ 0.0,1.0] throttle
        };

        /** Message data struct. */
        struct Message
        {
            bool visible;               ///< specifies if message is visible
            bool overlay;               ///< specifies if message overlays screen

            char text[ SIM_MSG_LEN ];   ///< message text

            bool pointer_custom;        ///< specifies if custom pointer is visible
            bool pointer_target;        ///< specifies if target pointer is visible
            bool pointer_rpm_dec;       ///< specifies if decrease RPM pointer is visible
            bool pointer_rpm_inc;       ///< specifies if increase RPM pointer is visible
            bool pointer_trigger;       ///< specifies if trigger pointer is visible

            float pointer_x;            ///< [pt] custom pointer position x-coordinate
            float pointer_y;            ///< [pt] custom pointer position y-coordinate
            float pointer_phi;          ///< [rad] custom pointer rotation angle

            TutorialTip tip;            ///< tutorial tip
        };

        /** Mission data struct. */
        struct Mission
        {
            Status status;              ///< mission status

            float time_step;            ///< [s] mission time step
            float real_time;            ///< [s] mission real time
            float time_end;             ///< [s] time since mission end
            float time_left;            ///< [s] time left to finish stage
        };

        /** Ownship data struct. */
        struct Ownship
        {
            UInt32 ownship_id;          ///< ownship ID
            UInt16 hit_points;          ///< hit points

            bool destroyed;             ///< specifies if ownship entity has been destroyed

            bool gunfire;               ///< specifies if ownship entity is shooting guns

            float init_throttle;        ///< [0.0,1.0] initial throttle position

            float altitude_asl;         ///< [m] altitude above mean sea level
            float altitude_agl;         ///< [m] altitude above mean sea level

            float airspeed;             ///< [m/s] airspeed
            float climbRate;            ///< [m/s] climb rate

            float angleOfAttack;        ///< [rad] angle of attack
            float sideslipAngle;        ///< [rad] sideslip angle

            float rollAngle;            ///< [rad] roll angle
            float pitchAngle;           ///< [rad] pitch angle
            float heading;              ///< [rad] true heading

            float rollRate;             ///< [rad/s] roll rate
            float pitchRate;            ///< [rad/s] pitch rate
            float yawRate;              ///< [rad/s] yaw rate
            float turnRate;             ///< [rad/s] turn rate

            float pos_x;                ///< [m] position x-coordinate
            float pos_y;                ///< [m] position y-coordinate
            float pos_z;                ///< [m] position z-coordinate

            float att_w;                ///< attitude w-coordinate expressed as quaternion
            float att_x;                ///< attitude x-coordinate expressed as quaternion
            float att_y;                ///< attitude y-coordinate expressed as quaternion
            float att_z;                ///< attitude z-coordinate expressed as quaternion

            bool  waypoint;             ///< waypoint visible

            float waypoint_x;           ///< [m] waypoint x-coordinate
            float waypoint_y;           ///< [m] waypoint y-coordinate
            float waypoint_z;           ///< [m] waypoint z-coordinate

            float waypoint_tht;         ///< [rad] waypoint relative elevation
            float waypoint_psi;         ///< [rad] waypoint relative bearing
            float waypoint_phi;         ///< [rad] waypoint relative vertical direction

            float waypoint_dist;        ///< [m] distance to the waypoint
            float waypoint_time;        ///< [s] time since last waypoint

            float hit_direction;        ///< [rad] direction from which ownship has been hit

            UInt8 ownship_hits;         ///< number of ownship hits

            float ownship_hit;          ///< [s] time since last ownship hit
            float friend_hit;           ///< [s] time since last friend hit
            float target_hit;           ///< [s] time since last target hit
            float target_kill;          ///< [s] time since last target kill
            float bombs_drop;           ///< [s] time since last bomb dropped
            float rocket_launch;        ///< [s] time since last rocket launched

            bool   target;              ///< target valid
            bool   target_box;          ///< target box visible
            bool   target_cue;          ///< target cue visible
            UInt32 target_id;           ///< target ID
            UInt16 target_hp;           ///< target hit points
            float  target_dir_phi;      ///< [rad] target box verical direction
            float  target_box_tht;      ///< [rad] target box elevation
            float  target_box_psi;      ///< [rad] target box bearing
            float  target_cue_tht;      ///< [rad] target cue elevation
            float  target_cue_psi;      ///< [rad] target cue bearing

            UInt32 wreckage_id;         ///< target wreckage ID
        };

        Camera     camera;              ///< camera data
        Controls   controls;            ///< controls data
        Message    message;             ///< message data
        Mission    mission;             ///< mission data
        Ownship    ownship;             ///< ownship data

        bool paused;                    ///< specifies if simulation is paused
    };

private:

    /** Using this constructor is forbidden. */
    Data() {}

    /** Using this constructor is forbidden. */
    Data( const Data & ) {}

public:

    /** @brief Returns pointer to the common data object. */
    static inline SimulationData* get() { return &_data; }

    /** @brief Zeroes common data. */
    static inline void reset()
    {
        memset( &_data, 0, sizeof(SimulationData) );
    }

    /** @brief Destructor. */
    virtual ~Data() {}

private:

    static SimulationData _data;    ///< simulation data
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_DATA_H
