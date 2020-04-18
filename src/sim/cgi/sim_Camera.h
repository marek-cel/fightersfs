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
#ifndef SIM_CAMERA_H
#define SIM_CAMERA_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/cgi/sim_ManipulatorOrbit.h>
#include <sim/cgi/sim_ManipulatorShift.h>
#include <sim/cgi/sim_ManipulatorWorld.h>

#include <sim/sim_Base.h>
#include <sim/sim_Data.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Camera control class. */
class Camera : public Base
{
public:

    static const double m_downAngle;    ///< [rad]

    /** Constructor. */
    Camera();

    /** Destructor. */
    virtual ~Camera();

    /** Updates camera position and attitude. */
    void update();

    /** */
    inline osgGA::CameraManipulator* getManipulator()
    {
        return m_manipulator.get();
    }

    /** Returns true if orbit manipulator track node is valid. */
    bool isTrackNodeValid();

    /** Returns true if world manipulator node is valid. */
    bool isWorldNodeValid();

    /** Sets orbit manipulator track node. */
    void setTrackNode( osg::Node *node );

    /** Sets world manipulator node. */
    void setWorldNode( osg::Node *node );

    /** Sets chase view. */
    inline void setViewChase()
    {
        m_type = ViewChase;
        m_manipulator = m_manipulatorShift.get();
    }

    /** Sets flyby view. */
    inline void setViewFlyby()
    {
        m_type = ViewFlyby;
        m_manipulator = m_manipulatorShift.get();

        setFlybyCameraPosition();
    }

    /** Sets front view. */
    inline void setViewFront()
    {
        m_type = ViewFront;
        m_manipulator = m_manipulatorShift.get();
    }

    /** Sets orbit view. */
    inline void setViewOrbit()
    {
        m_type = ViewOrbit;
        m_manipulator = m_manipulatorOrbit.get();
    }

    /** Sets pilot view. */
    inline void setViewPilot()
    {
        m_type = ViewPilot;
        m_manipulator = m_manipulatorShift.get();
    }

    /** Sets shift view. */
    inline void setViewShift()
    {
        m_type = ViewShift;
        m_manipulator = m_manipulatorShift.get();
    }

    /** Sets world view. */
    inline void setViewWorld()
    {
        m_type = ViewWorld;
        m_manipulator = m_manipulatorWorld.get();
    }

private:

    ViewType m_type;    ///< view (manipulator) type
    osg::ref_ptr<osgGA::CameraManipulator> m_manipulator;   ///< current manipulator

    osg::ref_ptr<ManipulatorOrbit> m_manipulatorOrbit;      ///< orbit view manipulator
    osg::ref_ptr<ManipulatorShift> m_manipulatorShift;      ///< shift view manipulator
    osg::ref_ptr<ManipulatorWorld> m_manipulatorWorld;      ///< world view manipulator

    osg::ref_ptr<osg::Node> m_trackNode;    ///< orbit manipulator track node
    osg::ref_ptr<osg::Node> m_worldNode;    ///< world manipulator node

    osg::Vec3 m_flyby;  ///< [m] flyby camera position

    float m_d_x;        ///< [m] camera x offset
    float m_d_y;        ///< [m] camera y offset
    float m_d_z;        ///< [m] camera z offset

    float m_d_phi;      ///< [rad] camera roll offset
    float m_d_tht;      ///< [rad] camera pitch offset
    float m_d_psi;      ///< [rad] camera yaw offset

    void setFlybyCameraPosition();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_CAMERA_H
