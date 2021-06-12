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
#ifndef SIM_GATES_H
#define SIM_GATES_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osg/Switch>

#include <sim/cgi/sim_Module.h>

#include <sim/entities/sim_Aircraft.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Waypoint gates module class. */
class Gates : public Module
{
public:

    static const float _distMax;    ///< [m] maximum visibility distance
    static const float _distScale;  ///< [m] minimum scaling distance
    static const float _size;       ///< [m] waypoint box size

    /** @brief Constructor. */
    Gates( float linesWidth, Module *parent = NULLPTR );

    /** @brief Destructor. */
    virtual ~Gates();

    /** @brief Updates gates module. */
    void update();

private:

    const float _linesWidth;                ///< [px] lines width

    osg::ref_ptr<osg::Switch> _switch;      ///< gates visibility switch

    osg::ref_ptr<osg::PositionAttitudeTransform> _pat0;     ///< current gate PAT
    osg::ref_ptr<osg::PositionAttitudeTransform> _pat1;     ///< next gate PAT
    osg::ref_ptr<osg::PositionAttitudeTransform> _pat2;     ///< next next gate PAT

    osg::ref_ptr<osg::Geometry> _geom0;     ///< current waypoint geometry
    osg::ref_ptr<osg::Geometry> _geom1;     ///< next waypoint geometry
    osg::ref_ptr<osg::Geometry> _geom2;     ///< next next waypoint geometry

    osg::Geometry* createGate( osg::Group *parent );

    float updateGate( osg::Geometry *geom, osg::PositionAttitudeTransform *pat,
                      Aircraft *ownship, int number, float coef_prev );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_GATES_H
