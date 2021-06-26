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
#ifndef SIM_VIEWER_H
#define SIM_VIEWER_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Group>

#ifdef SIM_DESKTOP
#   include <osgSim/OverlayNode>
#endif

#include <sim/cgi/sim_ManipulatorOrbit.h>
#include <sim/cgi/sim_Module.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Unit viewer class.
 *
 * This class is used to display single unit object.
 */
class Viewer : public Module
{
public:

    /** @brief Constructor. */
    Viewer( int width, int height );

    /** @brief Destructor. */
    virtual ~Viewer();

    /** @brief Loads and reloads view (models, textures, etc.). */
    void load();

    /** @brief Resets camera position and attitude. */
    void resetViewer();

    /** @brief Updates view. */
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

#   ifdef SIM_DESKTOP
    osg::ref_ptr<osgSim::OverlayNode> _overlayNode;         ///< aircraft shadow
#   endif

    osg::ref_ptr<ManipulatorOrbit> _manipulatorOrbit;           ///<

    osg::ref_ptr<osg::Group> _groupGround;
    osg::ref_ptr<osg::Group> _groupShadow;

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
#   ifdef SIM_DESKTOP
    void createShadow();
#   endif

    void loadUnit();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_VIEWER_H
