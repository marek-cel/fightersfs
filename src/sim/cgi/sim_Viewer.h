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
#ifndef SIM_VIEWER_H
#define SIM_VIEWER_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Group>

#include <sim/cgi/sim_ManipulatorOrbit.h>
#include <sim/cgi/sim_Module.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Unit viewer class.
 * <p>This class is used to display single unit object.</p>
 */
class Viewer : public Module
{
public:

    /** Constructor. */
    Viewer( int width, int height );

    /** Destructor. */
    virtual ~Viewer();

    /** Loads and reloads view (models, textures, etc.). */
    void load();

    /** Resets camera position and attitude. */
    void resetViewer();

    /** Updates view. */
    void update( double timeStep );

    /** */
    inline osgGA::CameraManipulator* getCameraManipulator()
    {
        return _manipulatorOrbit.get();
    }

    void setUnit( UInt32 index );

private:

    const float _distCoef;  ///<

    UInt32 _index;          ///< current unit index

    osg::ref_ptr<ManipulatorOrbit> _manipulatorOrbit;           ///<

    osg::ref_ptr<osg::PositionAttitudeTransform> _patUnit;      ///< orbit manipulator center
    osg::ref_ptr<osg::PositionAttitudeTransform> _patModel;     ///< model group node

    osg::ref_ptr<osg::PositionAttitudeTransform> _propeller1;   ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> _propeller2;   ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> _propeller3;   ///<
    osg::ref_ptr<osg::PositionAttitudeTransform> _propeller4;   ///<

    Vec3 _pos;              ///< model position
    Quat _att;              ///< model attitude

    double _prop_angle;     ///< [rad] propeller angle
    double _real_time;      ///< [s] real time

    void createSky();
    void createSun();

    void createDome( osg::Geode *dome, osg::Texture2D *texture,
                     float radius, bool blend = false );

    void createGround();
    void createOcean();
    void createPlane( const std::string &textureFile );

    void loadUnit();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_VIEWER_H
