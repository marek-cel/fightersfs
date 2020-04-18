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

/** Waypoint gates module class. */
class Gates : public Module
{
public:

    static const float m_distMax;   ///< [m] maximum visibility distance
    static const float m_distScale; ///< [m] minimum scaling distance
    static const float m_size;      ///< [m] waypoint box size

    /** Constructor. */
    Gates( float linesWidth, Module *parent = 0 );

    /** Destructor. */
    virtual ~Gates();

    /** Updates gates module. */
    void update();

private:

    const float m_linesWidth;               ///< [px] lines width

    osg::ref_ptr<osg::Switch> m_switch;     ///< gates visibility switch

    osg::ref_ptr<osg::PositionAttitudeTransform> m_pat0;    ///< current gate PAT
    osg::ref_ptr<osg::PositionAttitudeTransform> m_pat1;    ///< next gate PAT
    osg::ref_ptr<osg::PositionAttitudeTransform> m_pat2;    ///< next next gate PAT

    osg::ref_ptr<osg::Geometry> m_geom0;    ///< current waypoint geometry
    osg::ref_ptr<osg::Geometry> m_geom1;    ///< next waypoint geometry
    osg::ref_ptr<osg::Geometry> m_geom2;    ///< next next waypoint geometry

    osg::Geometry* createGate( osg::Group *parent );

    float updateGate( osg::Geometry *geom, osg::PositionAttitudeTransform *pat,
                      Aircraft *ownship, int number, float coef_prev );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_GATES_H
