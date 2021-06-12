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

#include <sim/cgi/sim_Camera.h>

#include <sim/utils/sim_Angles.h>
#include <sim/utils/sim_Misc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const double Camera::_downAngle = osg::DegreesToRadians( 6.9 );

////////////////////////////////////////////////////////////////////////////////

Camera::Camera() :
    _d_x ( 0.0f ),
    _d_y ( 0.0f ),
    _d_z ( 0.0f ),

    _d_phi ( 0.0f ),
    _d_tht ( 0.0f ),
    _d_psi ( 0.0f )
{
    _manipulatorOrbit = new ManipulatorOrbit();
    _manipulatorShift = new ManipulatorShift();
    _manipulatorWorld = new ManipulatorWorld();

    _type = ViewChase;
    _manipulator = _manipulatorShift.get();
}

////////////////////////////////////////////////////////////////////////////////

Camera::~Camera() {}

////////////////////////////////////////////////////////////////////////////////

void Camera::update()
{
    _d_x = 0.0f;
    _d_y = 0.0f;
    _d_z = 0.0f;

    _d_phi = 0.0f;
    _d_tht = 0.0f;
    _d_psi = 0.0f;

    if ( _type == ViewChase
      || _type == ViewFlyby
      || _type == ViewFront
      || _type == ViewPilot
      || _type == ViewShift )
    {
        Quat q_tmp( M_PI / 2.0, osg::X_AXIS,
                           0.0, osg::Y_AXIS,
                    M_PI / 2.0, osg::Z_AXIS );

        Quat q_enu( Data::get()->ownship.att_x,
                    Data::get()->ownship.att_y,
                    Data::get()->ownship.att_z,
                    Data::get()->ownship.att_w );

        Vec3 r_enu( Data::get()->ownship.pos_x,
                    Data::get()->ownship.pos_y,
                    Data::get()->ownship.pos_z );

        if ( _type == ViewChase )
        {
            _d_tht = -0.5f * Data::get()->ownship.pitchRate;
            _d_psi =  0.1f * Data::get()->ownship.yawRate;

            _d_x = 25.0f;
            _d_y = _d_x * tan( _d_psi ) * ( -1.0f );
            _d_z = _d_x * tan( _downAngle - _d_tht );

            Vec3 r_camera_bas( _d_x, _d_y, _d_z );
            Quat q_camera_bas( _d_phi, osg::X_AXIS,
                               _d_tht, osg::Y_AXIS,
                               _d_psi, osg::Z_AXIS );

            Vec3 r_camera_enu = r_enu + q_enu * r_camera_bas;

            osg::Matrixd matrix( osg::Matrixd::rotate( q_tmp )
                               * osg::Matrixd::rotate( q_camera_bas )
                               * osg::Matrixd::rotate( q_enu )
                               * osg::Matrixd::translate( r_camera_enu ) );

            _manipulatorShift->setByMatrix( matrix );
        }
        else if ( _type == ViewPilot )
        {
            osg::Matrixd matrix( osg::Matrixd::rotate( q_tmp )
                               * osg::Matrixd::rotate( q_enu )
                               * osg::Matrixd::translate( r_enu ) );

            _manipulatorShift->setByMatrix( matrix );
        }
        else if ( _type == ViewFlyby )
        {
            Vec3 r_cam = _flyby;

            Vec3 dir = r_enu - r_cam;
            dir *= 1.0/dir.length();

            double psi = atan2( -dir.y(), -dir.x() );
            double tht = atan2(  dir.z(), sqrt( dir.x()*dir.x() + dir.y()*dir.y() ) );

            Quat q_att( 0.0, osg::X_AXIS,
                        tht, osg::Y_AXIS,
                        psi, osg::Z_AXIS );

            osg::Matrixd matrix( osg::Matrixd::rotate( q_tmp )
                               * osg::Matrixd::rotate( q_att )
                               * osg::Matrixd::translate( r_cam ) );

            _manipulatorShift->setByMatrix( matrix );
        }
        else
        {
            if ( _type == ViewFront )
            {
                _d_x = -18.0f;
                _d_y = 0.0f;
                _d_z = 6.0f;

                _d_tht = osg::DegreesToRadians( -16.0 );
                _d_psi = M_PI;
            }
            else
            {
                _d_x = 18.0f;
                _d_y = 5.0f;
                _d_z = 4.0f;

                _d_tht = -atan2( _d_z, _d_x ) * 0.5f;
                _d_psi =  atan2( _d_y, _d_x ) * 0.5f;
            }

            if ( 0 )
            {
                q_enu.makeRotate( 0.0, osg::X_AXIS,
                                  Angles( q_enu ).tht(), osg::Y_AXIS,
                                  Angles( q_enu ).psi(), osg::Z_AXIS );
            }
            else
            {
                q_enu.makeRotate( 0.0, osg::X_AXIS,
                                  0.0, osg::Y_AXIS,
                                  Angles( q_enu ).psi(), osg::Z_AXIS );
            }

            Vec3 r_camera_bas( _d_x, _d_y, _d_z );
            Quat q_camera_bas( _d_phi, osg::X_AXIS,
                               _d_tht, osg::Y_AXIS,
                               _d_psi, osg::Z_AXIS );

            Vec3 r_camera_enu = r_enu + q_enu * r_camera_bas;

            osg::Matrixd matrix( osg::Matrixd::rotate( q_tmp )
                               * osg::Matrixd::rotate( q_camera_bas )
                               * osg::Matrixd::rotate( q_enu )
                               * osg::Matrixd::translate( r_camera_enu ) );

            _manipulatorShift->setByMatrix( matrix );
        }
    }

    Quat attitude = _manipulator->getMatrix().getRotate();
    Vec3 position = _manipulator->getMatrix().getTrans();

    Angles angles( attitude );

    Data::get()->camera.type = _type;

    Data::get()->camera.altitude_asl = position.z();

    Data::get()->camera.pos_x = position.x();
    Data::get()->camera.pos_y = position.y();
    Data::get()->camera.pos_z = position.z();

    Data::get()->camera.att_w = attitude.w();
    Data::get()->camera.att_x = attitude.x();
    Data::get()->camera.att_y = attitude.y();
    Data::get()->camera.att_z = attitude.z();

    Data::get()->camera.att_phi = angles.phi();
    Data::get()->camera.att_tht = angles.tht();
    Data::get()->camera.att_psi = angles.psi();

    Data::get()->camera.d_x = _d_x;
    Data::get()->camera.d_y = _d_y;
    Data::get()->camera.d_z = _d_z;

    Data::get()->camera.d_phi = _d_phi;
    Data::get()->camera.d_tht = _d_tht;
    Data::get()->camera.d_psi = _d_psi;
}

////////////////////////////////////////////////////////////////////////////////

bool Camera::isTrackNodeValid()
{
    if ( !_trackNode.valid() )
    {
        return false;
    }
    else if ( _trackNode->getParents().size() == 0 )
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Camera::isWorldNodeValid()
{
    if ( !_worldNode.valid() )
    {
        return false;
    }
    else if ( _worldNode->getParents().size() == 0 )
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

void Camera::setTrackNode( osg::Node *node )
{
    _trackNode = node;

    _manipulatorOrbit->setTrackNode( _trackNode.get() );
}

////////////////////////////////////////////////////////////////////////////////

void Camera::setWorldNode( osg::Node *node )
{
    _worldNode = node;

    _manipulatorWorld->setNode( _worldNode.get() );
    _manipulatorWorld->setElevation( M_PI_2 );
    _manipulatorWorld->setHeading( 0 );
}

////////////////////////////////////////////////////////////////////////////////

void Camera::setFlybyCameraPosition()
{
    Quat att( Data::get()->ownship.att_x,
              Data::get()->ownship.att_y,
              Data::get()->ownship.att_z,
              Data::get()->ownship.att_w );

    Vec3 pos( Data::get()->ownship.pos_x,
              Data::get()->ownship.pos_y,
              Data::get()->ownship.pos_z );

    double dy = ( Data::get()->ownship.rollAngle > 0.0 ) ? 20.0 : -20.0;

    Vec3 vel( -5.0 * Data::get()->ownship.airspeed, dy, 10.0 );

    _flyby = pos + att * vel;
}
