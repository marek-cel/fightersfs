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

#include <sim/cgi/sim_Camera.h>

#include <sim/utils/sim_Angles.h>
#include <sim/utils/sim_Misc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const double Camera::m_downAngle = osg::DegreesToRadians( 6.9 );

////////////////////////////////////////////////////////////////////////////////

Camera::Camera() :
    m_d_x ( 0.0f ),
    m_d_y ( 0.0f ),
    m_d_z ( 0.0f ),

    m_d_phi ( 0.0f ),
    m_d_tht ( 0.0f ),
    m_d_psi ( 0.0f )
{
    m_manipulatorOrbit = new ManipulatorOrbit();
    m_manipulatorShift = new ManipulatorShift();
    m_manipulatorWorld = new ManipulatorWorld();

    m_type = ViewChase;
    m_manipulator = m_manipulatorShift.get();
}

////////////////////////////////////////////////////////////////////////////////

Camera::~Camera() {}

////////////////////////////////////////////////////////////////////////////////

void Camera::update()
{
    m_d_x = 0.0f;
    m_d_y = 0.0f;
    m_d_z = 0.0f;

    m_d_phi = 0.0f;
    m_d_tht = 0.0f;
    m_d_psi = 0.0f;

    if ( m_type == ViewChase
      || m_type == ViewFlyby
      || m_type == ViewFront
      || m_type == ViewPilot
      || m_type == ViewShift )
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

        if ( m_type == ViewChase )
        {
            m_d_tht = -0.5f * Data::get()->ownship.pitchRate;
            m_d_psi =  0.1f * Data::get()->ownship.yawRate;

            m_d_x = 25.0f;
            m_d_y = m_d_x * tan( m_d_psi ) * ( -1.0f );
            m_d_z = m_d_x * tan( m_downAngle - m_d_tht );

            Vec3 r_camera_bas( m_d_x, m_d_y, m_d_z );
            Quat q_camera_bas( m_d_phi, osg::X_AXIS,
                               m_d_tht, osg::Y_AXIS,
                               m_d_psi, osg::Z_AXIS );

            Vec3 r_camera_enu = r_enu + q_enu * r_camera_bas;

            osg::Matrixd matrix( osg::Matrixd::rotate( q_tmp )
                               * osg::Matrixd::rotate( q_camera_bas )
                               * osg::Matrixd::rotate( q_enu )
                               * osg::Matrixd::translate( r_camera_enu ) );

            m_manipulatorShift->setByMatrix( matrix );
        }
        else if ( m_type == ViewPilot )
        {
            osg::Matrixd matrix( osg::Matrixd::rotate( q_tmp )
                               * osg::Matrixd::rotate( q_enu )
                               * osg::Matrixd::translate( r_enu ) );

            m_manipulatorShift->setByMatrix( matrix );
        }
        else if ( m_type == ViewFlyby )
        {
            Vec3 r_cam = m_flyby;

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

            m_manipulatorShift->setByMatrix( matrix );
        }
        else
        {
            if ( m_type == ViewFront )
            {
                m_d_x = -18.0f;
                m_d_y = 0.0f;
                m_d_z = 6.0f;

                m_d_tht = osg::DegreesToRadians( -16.0 );
                m_d_psi = M_PI;
            }
            else
            {
                m_d_x = 18.0f;
                m_d_y = 5.0f;
                m_d_z = 4.0f;

                m_d_tht = -atan2( m_d_z, m_d_x ) * 0.5f;
                m_d_psi =  atan2( m_d_y, m_d_x ) * 0.5f;
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

            Vec3 r_camera_bas( m_d_x, m_d_y, m_d_z );
            Quat q_camera_bas( m_d_phi, osg::X_AXIS,
                               m_d_tht, osg::Y_AXIS,
                               m_d_psi, osg::Z_AXIS );

            Vec3 r_camera_enu = r_enu + q_enu * r_camera_bas;

            osg::Matrixd matrix( osg::Matrixd::rotate( q_tmp )
                               * osg::Matrixd::rotate( q_camera_bas )
                               * osg::Matrixd::rotate( q_enu )
                               * osg::Matrixd::translate( r_camera_enu ) );

            m_manipulatorShift->setByMatrix( matrix );
        }
    }

    Quat attitude = m_manipulator->getMatrix().getRotate();
    Vec3 position = m_manipulator->getMatrix().getTrans();

    Angles angles( attitude );

    Data::get()->camera.type = m_type;

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

    Data::get()->camera.d_x = m_d_x;
    Data::get()->camera.d_y = m_d_y;
    Data::get()->camera.d_z = m_d_z;

    Data::get()->camera.d_phi = m_d_phi;
    Data::get()->camera.d_tht = m_d_tht;
    Data::get()->camera.d_psi = m_d_psi;
}

////////////////////////////////////////////////////////////////////////////////

bool Camera::isTrackNodeValid()
{
    if ( !m_trackNode.valid() )
    {
        return false;
    }
    else if ( m_trackNode->getParents().size() == 0 )
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Camera::isWorldNodeValid()
{
    if ( !m_worldNode.valid() )
    {
        return false;
    }
    else if ( m_worldNode->getParents().size() == 0 )
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

void Camera::setTrackNode( osg::Node *node )
{
    m_trackNode = node;

    m_manipulatorOrbit->setTrackNode( m_trackNode.get() );
}

////////////////////////////////////////////////////////////////////////////////

void Camera::setWorldNode( osg::Node *node )
{
    m_worldNode = node;

    m_manipulatorWorld->setNode( m_worldNode.get() );
    m_manipulatorWorld->setElevation( M_PI_2 );
    m_manipulatorWorld->setHeading( 0 );
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

    m_flyby = pos + att * vel;
}
