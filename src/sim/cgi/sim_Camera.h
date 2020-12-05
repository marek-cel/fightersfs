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

/** @brief Camera control class. */
class Camera : public Base
{
public:

    static const double _downAngle;     ///< [rad]

    /** @brief Constructor. */
    Camera();

    /** @brief Destructor. */
    virtual ~Camera();

    /** @brief Updates camera position and attitude. */
    void update();

    /** */
    inline osgGA::CameraManipulator* getManipulator()
    {
        return _manipulator.get();
    }

    /** @brief Returns true if orbit manipulator track node is valid. */
    bool isTrackNodeValid();

    /** @brief Returns true if world manipulator node is valid. */
    bool isWorldNodeValid();

    /** @brief Sets orbit manipulator track node. */
    void setTrackNode( osg::Node *node );

    /** @brief Sets world manipulator node. */
    void setWorldNode( osg::Node *node );

    /** @brief Sets chase view. */
    inline void setViewChase()
    {
        _type = ViewChase;
        _manipulator = _manipulatorShift.get();
    }

    /** @brief Sets flyby view. */
    inline void setViewFlyby()
    {
        _type = ViewFlyby;
        _manipulator = _manipulatorShift.get();

        setFlybyCameraPosition();
    }

    /** @brief Sets front view. */
    inline void setViewFront()
    {
        _type = ViewFront;
        _manipulator = _manipulatorShift.get();
    }

    /** @brief Sets orbit view. */
    inline void setViewOrbit()
    {
        _type = ViewOrbit;
        _manipulator = _manipulatorOrbit.get();
    }

    /** @brief Sets pilot view. */
    inline void setViewPilot()
    {
        _type = ViewPilot;
        _manipulator = _manipulatorShift.get();
    }

    /** @brief Sets shift view. */
    inline void setViewShift()
    {
        _type = ViewShift;
        _manipulator = _manipulatorShift.get();
    }

    /** @brief Sets world view. */
    inline void setViewWorld()
    {
        _type = ViewWorld;
        _manipulator = _manipulatorWorld.get();
    }

private:

    ViewType _type;     ///< view (manipulator) type
    osg::ref_ptr<osgGA::CameraManipulator> _manipulator;    ///< current manipulator

    osg::ref_ptr<ManipulatorOrbit> _manipulatorOrbit;       ///< orbit view manipulator
    osg::ref_ptr<ManipulatorShift> _manipulatorShift;       ///< shift view manipulator
    osg::ref_ptr<ManipulatorWorld> _manipulatorWorld;       ///< world view manipulator

    osg::ref_ptr<osg::Node> _trackNode;     ///< orbit manipulator track node
    osg::ref_ptr<osg::Node> _worldNode;     ///< world manipulator node

    osg::Vec3 _flyby;   ///< [m] flyby camera position

    float _d_x;         ///< [m] camera x offset
    float _d_y;         ///< [m] camera y offset
    float _d_z;         ///< [m] camera z offset

    float _d_phi;       ///< [rad] camera roll offset
    float _d_tht;       ///< [rad] camera pitch offset
    float _d_psi;       ///< [rad] camera yaw offset

    void setFlybyCameraPosition();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_CAMERA_H
