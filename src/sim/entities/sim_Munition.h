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
#ifndef SIM_MUNITION_H
#define SIM_MUNITION_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Unit.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Munition base class. */
class Munition : public Entity
{
public:

    /** Constructor. */
    Munition( UInt16 dp, UInt32 shooterId, float life_span, Group *parent = 0 );

    /** Destructor. */
    virtual ~Munition();

    /** Loads munition. */
    virtual void load();

    /**
     * @brief Updates munition.
     *
     * Itarates throught top level unit type entities checking intersections
     * of projectile pathway with unit bounding sphere defined as unit radius.
     * If intersection is detected hits unit with specified damage points.
     *
     * @param timeStep
     */
    virtual void update( double timeStep );

    inline UInt32 getShooterId() const { return _shooterId; }

    /** */
    virtual void setModelFile( const std::string &modelFile );

protected:

    const UInt16 _dp;   ///< damage points

    UInt32 _shooterId;  ///< shooter ID

    osg::ref_ptr<osg::Node> _model;     ///< ordnance model node
    std::string _modelFile;             ///< ordnance model file

    virtual void hit( Unit *target );

    virtual void reportTargetHit( Unit *target );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_MUNITION_H
