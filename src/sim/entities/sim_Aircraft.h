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
#ifndef SIM_AIRCRAFT_H
#define SIM_AIRCRAFT_H

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_TEST
#   include <osgParticle/SmokeTrailEffect>
#endif

#include <sim/cgi/sim_Effects.h>
#include <sim/cgi/sim_Textures.h>

#include <sim/sim_Route.h>

#include <sim/entities/sim_UnitAerial.h>
#include <sim/entities/sim_WreckageAircraft.h>

#include <sim/utils/sim_PID.h>
#include <sim/utils/sim_XmlNode.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Aircraft class. */
class Aircraft : public UnitAerial
{
public:

    /** */
    struct Flash
    {
        osg::Vec3 pos;
        bool light;
    };

    /** */
    struct Muzzle
    {
        osg::Vec3 pos;
        bool trail;
    };

    typedef std::vector< Flash  > Flashes;
    typedef std::vector< Muzzle > Muzzles;

    static const std::string m_tagName; ///<

    static const float m_threshold_vel; ///< [m/s] threshold velocity
    static const float m_waypoint_dist; ///< [m] maximum distance of reaching waypoint

    /** Constructor. */
    Aircraft( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~Aircraft();

    /** Destroys aircraft. */
    virtual void destroy();

    /** Loads aircraft (models, textures, etc.). */
    virtual void load();

    /** Makes entity automatic (not controlled by player). */
    virtual void makeAutomatic();

    /** Makes entity ownship (controlled by player). */
    virtual void makeOwnship();

    /**
     * Reads aircraft data.
     * @return SIM_SUCCESS on success or SIM_FAILURE on failure.
     */
    virtual int read( const XmlNode &node );

    /**
     * Updates aircraft.
     * @param timeStep [s] simulation time step
     */
    virtual void update( double timeStep );

    /** Increments the number of following aircrafts. */
    virtual void wingmenIncrement();

    /** Decrements the number of following aircrafts. */
    virtual void wingmenDecrement();

    inline WreckageAircraft* getWreckage() const { return m_wreckage; }

    inline float getSpeedMin() const { return m_speed_min; }
    inline float getSpeedMax() const { return m_speed_max; }
    inline float getRollMax() const { return m_roll_max; }
    inline float getTurnMax() const { return m_turn_max; }

    /** Returns aircraft target unit pointer. */
    virtual Unit* getTarget() const;

    /**
     * Returns aircraft current destination (waypoint, target,
     * wingman position, etc.).
     */
    inline WaypointData getDestination() const { return m_destination; }

    /**
     * @brief Returns distance to the current destination.
     * @return [m] destination distance
     */
    inline float getDestinationDistance() const { return m_destDist; }

    /** Returns aircraft route. */
    inline const Route& getRoute() const { return m_route; }

    /** Returns current waypoint index. */
    inline UInt32 getWaypointIndex() const { return m_waypointIndex; }

    /**
     * Returns true if aircraft has valid destination (waypoint, target,
     * wingman position, etc.).
     */
    inline bool getDestValid() const { return m_destValid; }

    /** Returns true if aircraft is enroute. */
    inline bool getEnroute() const { return m_enroute; }

    /** Returns true if aircraft is wingman. */
    inline bool getWingman() const { return m_wingman; }

    /**
     * @brief Returns aircraft leader ID.
     * @return unit ID
     */
    inline UInt32 getLeaderId() const { return m_leaderId; }

    inline float getInitThrottle() const { return m_initThrottle; }

    inline float getTimeDrop()   const { return m_time_drop;   }
    inline float getTimeLaunch() const { return m_time_launch; }
    inline float getTimeShoot()  const { return m_time_shoot;  }

    inline float getAltitudeASL()   const { return m_altitude_asl;  }
    inline float getAltitudeAGL()   const { return m_altitude_agl;  }
    inline float getAirspeed()      const { return m_airspeed;      }
    inline float getClimbRate()     const { return m_climbRate;     }
    inline float getMachNumber()    const { return m_machNumber;    }
    inline float getAngleOfAttack() const { return m_angleOfAttack; }
    inline float getSideslipAngle() const { return m_sideslipAngle; }
    inline float getPathAngle()     const { return m_pathAngle;     }
    inline float getRollAngle()     const { return m_rollAngle;     }
    inline float getPitchAngle()    const { return m_pitchAngle;    }
    inline float getHeading()       const { return m_heading;       }
    inline float getTurnRate()      const { return m_turnRate;      }

    /** Returns aircraft speed corresponding to the given throttle position. */
    virtual float getSpeed( float throttle );

    /** Returns throttle position corresponding to the given aircraft speed. */
    virtual float getThrottle( float speed );

    /** Sets leader ID. */
    virtual void setLeader( UInt32 leaderId );

    /** Sets offset to leader. */
    virtual void setOffset( const Vec3 &offset );

    /** Sets aircraft livery. */
    virtual void setLivery( const std::string &livery );

    /** Sets aircraft route. */
    virtual void setRoute( const Route *route );

    /** Sets aircraft hit points. */
    virtual void setHP( UInt16 hp );

    /** Sets unique flag. */
    virtual void setUnique( bool unique );

protected:

    WreckageAircraft *m_wreckage;   ///< aircraft wreckage entity

    osg::ref_ptr<osg::Node> m_model;                ///< aircraft model node
    osg::ref_ptr<osg::StateSet> m_modelStateSet;    ///< aircraft model node state set

    osg::ref_ptr<Effects::Smoke> m_smokeTrail;      ///< smoke trail

    osg::ref_ptr<osg::PositionAttitudeTransform> m_aileronL;    ///< left aileron deflection
    osg::ref_ptr<osg::PositionAttitudeTransform> m_aileronR;    ///< right aileron deflection
    osg::ref_ptr<osg::PositionAttitudeTransform> m_elevator;    ///< elevator deflection
    osg::ref_ptr<osg::PositionAttitudeTransform> m_rudderL;     ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> m_rudderR;     ///<

    osg::ref_ptr<osg::PositionAttitudeTransform> m_propeller1;  ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> m_propeller2;  ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> m_propeller3;  ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> m_propeller4;  ///<

    osg::ref_ptr<osg::Switch> m_flashSwitch;                                ///< fighter muzzle flash switch node
    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > m_flashPAT; ///< fighter muzzle flash PAT

    Flashes m_flashes;              ///< gun flashes data
    Muzzles m_muzzles;              ///< gun muzzles data (projectile start point)

    float m_time_p;                 ///< [s] roll inertia time constant
    float m_time_q;                 ///< [s] pitch inertia time constant
    float m_time_r;                 ///< [s] yaw inertia time constant
    float m_time_v;                 ///< [s] speed inertia time constant

    float m_speed_min;              ///< [m/s] min speed
    float m_speed_max;              ///< [m/s] max speed

    float m_roll_max;               ///< [rad] max roll angle
    float m_turn_max;               ///< [rad/s] max turn rate

    PID *m_pid_phi;                 ///< roll PID controller

    PID *m_pid_p;                   ///< roll rate PID controller
    PID *m_pid_q;                   ///< pitch rate PID controller
    PID *m_pid_r;                   ///< yaw rate PID controller

    bool m_enroute;                 ///< specifies if aircraft is enroute
    bool m_wingman;                 ///< specifies if aircraft is a wingman

    UInt32 m_wingmenCount;          ///< number of aircraft following wingmen

    UInt32 m_leaderId;              ///< leader ID

    Vec3 m_wingmanOffset;           ///< [m] offset to leader

    bool m_leaderValid;             ///< specifies if formation leader is valid
    bool m_formation;               ///< specifief if aircraft is in formation

    WaypointData m_destination;     ///< current destination (e.i.: true waypoint, target position, etc.)

    UInt32 m_waypointIndex;         ///< current waypoint index

    float m_destDist;               ///< [m] distance to the current destination
    float m_destBear;               ///< [rad] bearing of the current destination
    float m_destElev;               ///< [rad] elevation of the current destination

    bool m_destValid;               ///< specifies if destination is valid

    Route m_route;                  ///< route (waypoints list)

    float m_ctrlRoll;               ///< [-] roll control
    float m_ctrlPitch;              ///< [-] pitch control
    float m_ctrlYaw;                ///< [-] yaw control
    float m_throttle;               ///< [-] throttle

    float m_initThrottle;           ///< [-] initial throttle position

    float m_prop_angle;             ///< [rad]   propeller angle
    float m_prop_speed;             ///< [rad/s] propeller speed

    float m_maxAilerons;            ///< [rad] maximum ailerons deflection
    float m_maxElevator;            ///< [rad] maximum elevator deflection
    float m_maxRudder;              ///< [rad] maximum rudder deflection

    std::string m_livery;           ///< livery path

    bool m_unique;                  ///< specifies if aircraft is unique (e.i. has unique painting)

    bool m_trigger;                 ///< trigger

    short m_flash_count;            ///< muzzle flash counter
    short m_flash_devider;          ///< muzzle flash devider
    float m_flash_angle;            ///< [rad] muzzle flash "roll" angle

    float m_time_drop;              ///< [s] time since last bomb dropped
    float m_time_launch;            ///< [s] time since last rocket launched
    float m_time_shoot;             ///< [s] time since last shot

    float m_elevation;              ///< [m] terrain elevation

    float m_altitude_asl;           ///< [m] altitude above sea level
    float m_altitude_agl;           ///< [m] altitude above ground level
    float m_airspeed;               ///< [m/s] true airspeed
    float m_climbRate;              ///< [m/s] climb rate
    float m_machNumber;             ///< [-] Mach number (not computed in simplified flight model)
    float m_angleOfAttack;          ///< [rad] angle of attack (not computed in simplified flight model)
    float m_sideslipAngle;          ///< [rad] sideslip angle (not computed in simplified flight model)
    float m_pathAngle;              ///< [rad] flight path angle (not computed in simplified flight model)
    float m_rollAngle;              ///< [rad]
    float m_pitchAngle;             ///< [rad]
    float m_heading;                ///< [rad] true heading
    float m_turnRate;               ///< [rad/s] turn rate

    /**
     * @brief Creates muzzle flashes.
     * @param positions [m] muzzle flashes position relative to aircraft model origin
     * @param scale scale factor
     */
    virtual void createMuzzleFlash( const Flashes &flashes, osg::Vec3 scale );

    /**
     * @brief Modifies aircraft required attitude to avoid collisions.
     * @param phi [rad] roll angle
     * @param tht [rad] pitch angle
     */
    virtual void avoidCollisions( float &phi, float &tht );

    /** */
    virtual void initOwnship();

    /**
     * Limits aircraft required pitch angle due to given limitations and
     * to avoid flight into terrain.
     */
    virtual void limitTht( float &tht );

    /**
     * @brief Equations of motion time integration.
     * <p>Position and attitude are computed by this function.</p>
     */
    virtual void timeIntegration();

    /**
     * Updates aircraft controls when aircraft is not controlled
     * by player.
     */
    virtual void updateControls();

    /** Updates aircraft destination due to active route. */
    virtual void updateDestination();

    /** Updates elevation data. */
    virtual void updateElevation();

    /** Updates propeller speed and position. */
    virtual void updatePropeller();

    /** Updates trigger. */
    virtual void updateTrigger();

    /** Updates additional variables. */
    virtual void updateVariables();

    /** Updates aircraft velocity. */
    virtual void updateVelocity();

    /** Updates aircraft weapons. */
    virtual void updateWeapons();

    /** Updates aircraft destination due to formation leader position. */
    virtual void updateWingman();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_AIRCRAFT_H
