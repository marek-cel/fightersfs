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
 *
 * All individual objects within simulation (units, aircrafts, ships,
 * projectiles, wrecks, some kind of effects, etc.) are entities.
 *
 * Entity can be top level which means it is independent object within
 * simulation, or it can be a child of a parent entity (e.i. bomb carrier
 * by a plane).
 *
 * Any entity can be make top level in any time.
 *
 * When an entity is created without specifies parent it is assumed that
 * this is top level entity.
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
     *
     * Updates entity state, velocity, position, attitude, etc.
     *
     * Motion equations time integration function is called here.
     *
     * @param timeStep [s] time step
     */
    virtual void update( double timeStep );

    /** Returns entity absolute position. */
    Vec3 getAbsPos() const;

    /** Returns entity absolute attitude. */
    Quat getAbsAtt() const;

    /**
     * @brief Returns entity scene node.
     *
     * This node position corresponds to the entity position.
     *
     * @return entity node
     */
    inline osg::Node* getEntityNode() const { return _switch.get(); }

    inline UInt32 getId() const { return _id; }

    inline Vec3 getPos() const { return _pos; }
    inline Quat getAtt() const { return _att; }
    inline Vec3 getVel() const { return _vel; }
    inline Vec3 getOmg() const { return _omg; }

    inline Angles getAngles() const { return _angles; }

    inline std::string getName() const { return _name; }

    inline State getState() const { return _state; }

    inline bool isActive() const { return _active; }

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

    static std::vector< UInt32 > _ids;  ///< list of entities ID's being in use

    osg::ref_ptr<osg::PositionAttitudeTransform> _pat;  ///<
    osg::ref_ptr<osg::Switch> _switch;                  ///< root for children
    osg::ref_ptr<osg::Group> _parentGroup;              ///< parent group

    const UInt32 _id;           ///< entity ID

    double _timeStep;           ///< [s] time step

    Vec3 _pos;                  ///< [m] position expressed in ENU
    Quat _att;                  ///< attitude expressed as quaternion
    Vec3 _vel;                  ///< [m/s] velocity expressed in BAS
    Vec3 _omg;                  ///< [rad/s] angular velocity expressed in BAS

    Angles _angles;             ///< [rad] attitude expressed as rotation angles from ENU to BAS in z-y-x convention

    State _state;               ///< entity state

    bool _active;               ///< specify if entity is active

    float _life_time;           ///< [s] life time
    float _life_span;           ///< [s] life span

    std::string _name;          ///< entity name

    /** Computes position derivative. */
    Vec3 derivPos( const Quat &att, const Vec3 &vel );

    /** Computes attitude derivative. */
    Quat derivAtt( const Quat &att, const Vec3 &omg );

    /**
     * @brief Equations of motion time integration.
     *
     * Position and attitude are computed by this function.
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
