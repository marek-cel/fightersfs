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

    static const std::string _tagName;  ///< unit tag name

    static const float _threshold_vel;  ///< [m/s] threshold velocity
    static const float _waypoint_dist;  ///< [m] maximum distance of reaching waypoint

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

    inline WreckageAircraft* getWreckage() const { return _wreckage; }

    inline float getSpeedMin() const { return _speed_min; }
    inline float getSpeedMax() const { return _speed_max; }
    inline float getRollMax() const { return _roll_max; }
    inline float getTurnMax() const { return _turn_max; }

    /** Returns aircraft target unit pointer. */
    virtual Unit* getTarget() const;

    /**
     * Returns aircraft current destination (waypoint, target,
     * wingman position, etc.).
     */
    inline WaypointData getDestination() const { return _destination; }

    /**
     * @brief Returns distance to the current destination.
     * @return [m] destination distance
     */
    inline float getDestinationDistance() const { return _destDist; }

    /** Returns aircraft route. */
    inline const Route& getRoute() const { return _route; }

    /** Returns current waypoint index. */
    inline UInt32 getWaypointIndex() const { return _waypointIndex; }

    /**
     * Returns true if aircraft has valid destination (waypoint, target,
     * wingman position, etc.).
     */
    inline bool getDestValid() const { return _destValid; }

    /** Returns true if aircraft is enroute. */
    inline bool getEnroute() const { return _enroute; }

    /** Returns true if aircraft is wingman. */
    inline bool getWingman() const { return _wingman; }

    /**
     * @brief Returns aircraft leader ID.
     * @return unit ID
     */
    inline UInt32 getLeaderId() const { return _leaderId; }

    inline float getInitThrottle() const { return _initThrottle; }

    inline float getTimeDrop()   const { return _time_drop;   }
    inline float getTimeLaunch() const { return _time_launch; }
    inline float getTimeShoot()  const { return _time_shoot;  }

    inline float getAltitudeASL()   const { return _altitude_asl;  }
    inline float getAltitudeAGL()   const { return _altitude_agl;  }
    inline float getAirspeed()      const { return _airspeed;      }
    inline float getClimbRate()     const { return _climbRate;     }
    inline float getMachNumber()    const { return _machNumber;    }
    inline float getAngleOfAttack() const { return _angleOfAttack; }
    inline float getSideslipAngle() const { return _sideslipAngle; }
    inline float getPathAngle()     const { return _pathAngle;     }
    inline float getRollAngle()     const { return _rollAngle;     }
    inline float getPitchAngle()    const { return _pitchAngle;    }
    inline float getHeading()       const { return _heading;       }
    inline float getTurnRate()      const { return _turnRate;      }

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

    WreckageAircraft *_wreckage;    ///< aircraft wreckage entity

    osg::ref_ptr<osg::Node> _model;                 ///< aircraft model node
    osg::ref_ptr<osg::StateSet> _modelStateSet;     ///< aircraft model node state set

    osg::ref_ptr<Effects::Smoke> _smokeTrail;       ///< smoke trail

    osg::ref_ptr<osg::PositionAttitudeTransform> _aileronL;     ///< left aileron deflection
    osg::ref_ptr<osg::PositionAttitudeTransform> _aileronR;     ///< right aileron deflection
    osg::ref_ptr<osg::PositionAttitudeTransform> _elevator;     ///< elevator deflection
    osg::ref_ptr<osg::PositionAttitudeTransform> _rudderL;      ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> _rudderR;      ///<

    osg::ref_ptr<osg::PositionAttitudeTransform> _propeller1;   ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> _propeller2;   ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> _propeller3;   ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> _propeller4;   ///<

    osg::ref_ptr<osg::Switch> _flashSwitch;                                 ///< fighter muzzle flash switch node
    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > _flashPAT;  ///< fighter muzzle flash PAT

    Flashes _flashes;               ///< gun flashes data
    Muzzles _muzzles;               ///< gun muzzles data (projectile start point)

    float _time_p;                  ///< [s] roll inertia time constant
    float _time_q;                  ///< [s] pitch inertia time constant
    float _time_r;                  ///< [s] yaw inertia time constant
    float _time_v;                  ///< [s] speed inertia time constant

    float _speed_min;               ///< [m/s] min speed
    float _speed_max;               ///< [m/s] max speed

    float _roll_max;                ///< [rad] max roll angle
    float _turn_max;                ///< [rad/s] max turn rate

    PID *_pid_phi;                  ///< roll PID controller

    PID *_pid_p;                    ///< roll rate PID controller
    PID *_pid_q;                    ///< pitch rate PID controller
    PID *_pid_r;                    ///< yaw rate PID controller

    bool _enroute;                  ///< specifies if aircraft is enroute
    bool _wingman;                  ///< specifies if aircraft is a wingman

    UInt32 _wingmenCount;           ///< number of aircraft following wingmen

    UInt32 _leaderId;               ///< leader ID

    Vec3 _wingmanOffset;            ///< [m] offset to leader

    bool _leaderValid;              ///< specifies if formation leader is valid
    bool _formation;                ///< specifief if aircraft is in formation

    WaypointData _destination;      ///< current destination (e.i.: true waypoint, target position, etc.)

    UInt32 _waypointIndex;          ///< current waypoint index

    float _destDist;                ///< [m] distance to the current destination
    float _destBear;                ///< [rad] bearing of the current destination
    float _destElev;                ///< [rad] elevation of the current destination

    bool _destValid;                ///< specifies if destination is valid

    Route _route;                   ///< route (waypoints list)

    float _ctrlRoll;                ///< [-] roll control
    float _ctrlPitch;               ///< [-] pitch control
    float _ctrlYaw;                 ///< [-] yaw control
    float _throttle;                ///< [-] throttle

    float _initThrottle;            ///< [-] initial throttle position

    float _prop_angle;              ///< [rad]   propeller angle
    float _prop_speed;              ///< [rad/s] propeller speed

    float _maxAilerons;             ///< [rad] maximum ailerons deflection
    float _maxElevator;             ///< [rad] maximum elevator deflection
    float _maxRudder;               ///< [rad] maximum rudder deflection

    std::string _livery;            ///< livery path

    bool _unique;                   ///< specifies if aircraft is unique (e.i. has unique painting)

    bool _trigger;                  ///< trigger

    short _flash_count;             ///< muzzle flash counter
    short _flash_devider;           ///< muzzle flash devider
    float _flash_angle;             ///< [rad] muzzle flash "roll" angle

    float _time_drop;               ///< [s] time since last bomb dropped
    float _time_launch;             ///< [s] time since last rocket launched
    float _time_shoot;              ///< [s] time since last shot

    float _elevation;               ///< [m] terrain elevation

    float _altitude_asl;            ///< [m] altitude above sea level
    float _altitude_agl;            ///< [m] altitude above ground level
    float _airspeed;                ///< [m/s] true airspeed
    float _climbRate;               ///< [m/s] climb rate
    float _machNumber;              ///< [-] Mach number (not computed in simplified flight model)
    float _angleOfAttack;           ///< [rad] angle of attack (not computed in simplified flight model)
    float _sideslipAngle;           ///< [rad] sideslip angle (not computed in simplified flight model)
    float _pathAngle;               ///< [rad] flight path angle (not computed in simplified flight model)
    float _rollAngle;               ///< [rad]
    float _pitchAngle;              ///< [rad]
    float _heading;                 ///< [rad] true heading
    float _turnRate;                ///< [rad/s] turn rate

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
