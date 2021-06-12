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
#ifndef SIM_UNIT_H
#define SIM_UNIT_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Entity.h>

#include <sim/utils/sim_XmlNode.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

class Munition;

/**
 * @brief Unit base class.
 *
 * Unlike other entities, units initial state is Standby.
 */
class Unit : public Entity
{
public:

    /** Constructor. */
    Unit( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~Unit();

    /** Activates unit. */
    virtual void activate();

    /** Demages unit which collided with this unit. */
    virtual void collide( Unit *unit );

    /**
     * Demages unit.
     * @param dp demage points
     */
    virtual void damage( UInt16 dp );

    /** Destroys unit. */
    virtual void destroy();

    /**
     * Demages unit.
     * @param dp demage points
     * @param munition munition which hit unit
     */
    virtual void hit( UInt16 dp, const Munition *munition );

    /** Loads unit (models, textures, etc.). */
    virtual void load();

    /** Makes entity automatic (not controlled by player). */
    virtual void makeAutomatic();

    /** Makes entity an ownship (controlled by player) and activates it. */
    virtual void makeOwnship();

    /**
     * Reads unit data.
     * @return SIM_SUCCESS on success or SIM_FAILURE on failure.
     */
    virtual int read( const XmlNode &node );

    /**
     * Reports target hit by unit's munition.
     * @param target target unit
     */
    virtual void reportTargetHit( Unit *target );

    /** Updates unit. */
    virtual void update( double timeStep );

    /** Returns unit affiliation. */
    inline Affiliation getAffiliation() const { return _affiliation; }

    /** Returns unit hit points. */
    inline UInt16 getHP() const { return _hp; }

    /** Returns unit armor points. */
    inline UInt16 getAP() const { return _ap; }

    /**
     * @brief Returns unit bounding sphere radius.
     * @return [m] bounding sphere radius
     */
    inline float getRadius()  const { return _radius; }

    /**
     * @brief Returns unit bounding sphere squared radius.
     * @return [m^2] bounding sphere squared radius
     */
    inline float getRadius2() const { return _radius2; }

    /** Returns true if unit is ownship (controlled by player). */
    inline bool isOwnship() const { return _ownship; }

    /** Returns true if unit has not been destroyed. */
    inline bool isAlive() const { return _hp > 0; }

    /** Sets unit armor points. */
    virtual void setAP( UInt16 ap );

    /** Sets unit hit points. */
    virtual void setHP( UInt16 hp );

    /**
     * Sets unit radius.
     * @param radius [m] radius (for the purpose of collision detections)
     */
    virtual void setRadius( float radius );

protected:

    Affiliation _affiliation;           ///< unit affiliation

    osg::ref_ptr<osg::Node> _model;     ///< unit model node
    std::string _modelFile;             ///< unit model file

    UInt16 _ap;                         ///< armor points
    UInt16 _hp;                         ///< hit points (max. 100)

    float _radius;                      ///< [m] entity radius for the purpose of collisions detections
    float _radius2;                     ///< [m^2] entity radius squared

    bool _ownship;                      ///< specifies if unit is ownship

    /** Reads gunners. */
    virtual void readGunners( const XmlNode &node );

    /** Determines if unit collided with another one. */
    virtual void updateCollisions();

    /** Updates weapons. */
    virtual void updateWeapons();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_UNIT_H
