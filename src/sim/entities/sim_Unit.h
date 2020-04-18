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
 * Unit base class.
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
    inline Affiliation getAffiliation() const { return m_affiliation; }

    /** Returns unit hit points. */
    inline UInt16 getHP() const { return m_hp; }

    /** Returns unit armor points. */
    inline UInt16 getAP() const { return m_ap; }

    /**
     * @brief Returns unit bounding sphere radius.
     * @return [m] bounding sphere radius
     */
    inline float getRadius()  const { return m_radius; }

    /**
     * @brief Returns unit bounding sphere squared radius.
     * @return [m^2] bounding sphere squared radius
     */
    inline float getRadius2() const { return m_radius2; }

    /** Returns true if unit is ownship (controlled by player). */
    inline bool isOwnship() const { return m_ownship; }

    /** Returns true if unit has not been destroyed. */
    inline bool isAlive() const { return m_hp > 0; }

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

    Affiliation m_affiliation;          ///< unit affiliation

    osg::ref_ptr<osg::Node> m_model;    ///< unit model node
    std::string m_modelFile;            ///< unit model file

    UInt16 m_ap;                        ///< armor points
    UInt16 m_hp;                        ///< hit points (max. 100)

    float m_radius;                     ///< [m] entity radius for the purpose of collisions detections
    float m_radius2;                    ///< [m^2] entity radius squared

    bool m_ownship;                     ///< specifies if unit is ownship

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
