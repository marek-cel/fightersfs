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
#ifndef SIM_ENTITY_H
#define SIM_ENTITY_H

////////////////////////////////////////////////////////////////////////////////

#include <limits>

#include <osg/PositionAttitudeTransform>
#include <osg/Switch>

#include <sim/sim_Data.h>

#include <sim/entities/sim_Group.h>

#include <sim/utils/sim_Angles.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Entity base class.
 * <p>All individual objects within simulation (units, aircrafts, ships,
 * projectiles, wrecks, some kind of effects, etc.) are entities.</p>
 *
 * <p>Entity can be top level which means it is independent object within
 * simulation, or it can be a child of a parent entity (e.i. bomb carrier
 * by a plane).</p>
 *
 * <p>Any entity can be make top level in any time.</p>
 *
 * <p>When an entity is created without specifies parent it is assumed that
 * this is top level entity.</p>
 *
 * @see Entities
 * @see Group
 */
class Entity : public Group
{
public:

    /** Returns lowest available ID on success and NaN on failure. */
    static UInt32 createId();

    /** Removes entity ID from used IDs set. */
    static void removeId( UInt32 id );

    /** Constructor. */
    Entity( Group *parent = 0, State state = Active,
            float life_span = std::numeric_limits< float >::max() );

    /** Destructor. */
    virtual ~Entity();

    /** Detaches entity from current parent and makes it a top level entity. */
    virtual void makeTopLevel();

    /** Resets entity life time. */
    virtual void resetLifeTime();

    /** Loads entity. */
    virtual void load();

    /**
     * @brief Updates entity.
     * <p>Updates entity state, velocity, position, attitude, etc.</p>
     * <p>Motion equations time integration function is called here.</p>
     * @param timeStep [s] time step
     */
    virtual void update( double timeStep );

    /** Returns entity absolute position. */
    Vec3 getAbsPos() const;

    /** Returns entity absolute attitude. */
    Quat getAbsAtt() const;

    /**
     * @brief Returns entity scene node.
     * <p>This node position corresponds to the entity position.</p>
     * @return entity node
     */
    inline osg::Node* getEntityNode() const { return m_switch.get(); }

    inline UInt32 getId() const { return m_id; }

    inline Vec3 getPos() const { return m_pos; }
    inline Quat getAtt() const { return m_att; }
    inline Vec3 getVel() const { return m_vel; }
    inline Vec3 getOmg() const { return m_omg; }

    inline Angles getAngles() const { return m_angles; }

    inline std::string getName() const { return m_name; }

    inline State getState() const { return m_state; }

    inline bool isActive() const { return m_active; }

    /** Returns true if entity is top level. */
    bool isTopLevel() const;

    virtual void setPos( const Vec3 &pos );
    virtual void setAtt( const Quat &att );
    virtual void setVel( const Vec3 &vel );
    virtual void setOmg( const Vec3 &omg );

    virtual void setHeading( float heading );

    virtual void setName( const std::string &name );

    virtual void setParent( Group *parent );

    virtual void setState( State state );

    virtual void setVelocity( float velocity );

protected:

    static std::vector< UInt32 > m_ids; ///< list of entities ID's being in use

    osg::ref_ptr<osg::PositionAttitudeTransform> m_pat; ///<
    osg::ref_ptr<osg::Switch> m_switch;                 ///< root for children
    osg::ref_ptr<osg::Group> m_parentGroup;             ///< parent group

    const UInt32 m_id;          ///< entity ID

    double m_timeStep;          ///< [s] time step

    Vec3 m_pos;                 ///< [m] position expressed in ENU
    Quat m_att;                 ///< attitude expressed as quaternion
    Vec3 m_vel;                 ///< [m/s] velocity expressed in BAS
    Vec3 m_omg;                 ///< [rad/s] angular velocity expressed in BAS

    Angles m_angles;            ///< [rad] attitude expressed as rotation angles from ENU to BAS in z-y-x convention

    State m_state;              ///< entity state

    bool m_active;              ///< specify if entity is active

    float m_life_time;          ///< [s] life time
    float m_life_span;          ///< [s] life span

    std::string m_name;         ///< entity name

    /** Computes position derivative. */
    Vec3 derivPos( const Quat &att, const Vec3 &vel );

    /** Computes attitude derivative. */
    Quat derivAtt( const Quat &att, const Vec3 &omg );

    /**
     * @brief Equations of motion time integration.
     * <p>Position and attitude are computed by this function.</p>
     */
    virtual void timeIntegration();

    /** Updates elevation data. */
    virtual void updateElevation();

    /** Updates additional variables. */
    virtual void updateVariables();

    /** Updates linear and angular velocity. */
    virtual void updateVelocity();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_ENTITY_H
