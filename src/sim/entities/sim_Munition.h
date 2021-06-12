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
