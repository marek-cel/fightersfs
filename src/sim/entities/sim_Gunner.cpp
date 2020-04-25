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

#include <sim/entities/sim_Gunner.h>

#include <sim/entities/sim_Flak.h>
#include <sim/entities/sim_Tracer.h>

#include <sim/utils/sim_Inertia.h>
#include <sim/utils/sim_Convert.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const float Gunner::m_10deg_rad = Convert::deg2rad( 10.0f );
const float Gunner::m_20deg_rad = Convert::deg2rad( 20.0f );
const float Gunner::m_40deg_rad = Convert::deg2rad( 40.0f );
const float Gunner::m_45deg_rad = M_PI_4;
const float Gunner::m_60deg_rad = Convert::deg2rad( 60.0f );
const float Gunner::m_80deg_rad = Convert::deg2rad( 80.0f );

////////////////////////////////////////////////////////////////////////////////

Gunner::Gunner( Affiliation affiliation, Group *parent, float range ) :
    Entity ( parent ),

    m_affiliation ( affiliation ),

    m_parent_valid ( false ),
    m_parent_id ( 0 ),

    m_range ( range ),

    m_inertia  ( 0.5f ),
    m_advanced ( true ),

    m_target ( 0 ),

    m_target_tht ( 0.0f ),
    m_target_psi ( 0.0f ),

    m_trigger ( false ),

    m_target_valid ( false ),

    m_shoot_time ( 0.0f )
{
    m_target = new Target< UnitAerial >( this, ( m_affiliation == Hostile ) ? Friend : Hostile,
                                         m_range, m_range );

    Unit *parentUnit = dynamic_cast< Unit* >( m_parent );

    if ( parentUnit )
    {
        m_parent_valid = true;
        m_parent_id = parentUnit->getId();
    }
}

////////////////////////////////////////////////////////////////////////////////

Gunner::~Gunner()
{
    DELPTR( m_target );
}

////////////////////////////////////////////////////////////////////////////////

void Gunner::update( double timeStep )
{
    ///////////////////////////
    Entity::update( timeStep );
    ///////////////////////////

    m_trigger = false;

    if ( isActive() )
    {
        ///////////////////
        m_target->update();
        ///////////////////

        m_pos_abs = getAbsPos();
        m_att_abs = getAbsAtt();

        m_target_valid = false;

        Unit *target = m_target->getTarget();

        if ( target )
        {
            Vec3 pos_est = target->getPos();

            // estimated future position
            if ( m_advanced )
            {
                float time = m_target_dist / Bullet::m_vel_m;
                pos_est += ( target->getAtt() * target->getVel() ) * time;
            }

            m_target_dist = ( pos_est - getAbsPos() ).length();

            if ( m_target_dist < m_range )
            {
                Vec3 dir_enu = pos_est - m_pos_abs;
                dir_enu *= 1.0/dir_enu.length();

                Vec3 dir_bas = m_att_abs.inverse() * dir_enu;
                dir_bas *= 1.0/dir_bas.length();

                double psi = atan2( -dir_bas.y(), -dir_bas.x() );
                double tht = atan2( dir_bas.z(), sqrt( dir_bas.x()*dir_bas.x() + dir_bas.y()*dir_bas.y() ) );

                m_target_psi = Inertia< float >::update( m_timeStep, m_target_psi, psi, m_inertia );
                m_target_tht = Inertia< float >::update( m_timeStep, m_target_tht, tht, m_inertia );

                Quat q_att( 0.0          , osg::X_AXIS,
                            m_target_tht , osg::Y_AXIS,
                            m_target_psi , osg::Z_AXIS );

                m_target_dir = q_att * m_att_abs;

                float delta_psi = m_target_psi - psi;
                float delta_tht = m_target_tht - tht;

                m_trigger = sqrt( delta_psi*delta_psi + delta_tht*delta_tht ) < 0.05;

                m_target_valid = true;
            }
            else
            {
                m_target->findForward( M_PI );
            }
        }
        else
        {
            m_target->findForward( M_PI );
        }

        updateWeapons();

        m_shoot_time += m_timeStep;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Gunner::updateWeapons()
{
    if ( m_target_valid )
    {
        if ( m_trigger && m_target_dist < m_range )
        {
            if ( m_shoot_time > 0.2f )
            {
                m_shoot_time = 0.0f;

                Tracer *bullet = new Tracer( m_parent_valid ? m_parent_id : m_id );

                bullet->setPos( m_pos_abs + m_target_dir * Vec3( -1.0, 0.0, 0.0 ) * 20.0f );
                bullet->setAtt( m_target_dir );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

GunnerFlak::GunnerFlak( Affiliation affiliation, Group *parent ) :
    Gunner( affiliation, parent, 2000.0f ),

    m_burst_count ( 0 ),
    m_burst_time ( 0.0f )
{}

////////////////////////////////////////////////////////////////////////////////

GunnerFlak::~GunnerFlak() {}

////////////////////////////////////////////////////////////////////////////////

void GunnerFlak::updateWeapons()
{
    m_burst_time += m_timeStep;

    if ( m_target_valid )
    {
        if ( m_target_dist < m_range && m_target_dist > 300.0 )
        {
            if ( m_burst_time > 5.0f )
            {
                if ( m_burst_count < 4 )
                {
                    if ( m_shoot_time > 0.1f )
                    {
                        m_shoot_time = 0.0f;

                        float offset = 20.0;
                        float fuse_time = ( m_target_dist - offset ) / Bullet::m_vel_m;

                        Quat dir = m_target_dir;

                        float ang = Convert::deg2rad( 1.0 );

                        switch ( m_burst_count )
                        {
                        case 1:
                            dir += derivAtt( dir, Vec3( 0.0f,  ang, 0.0f ) );
                            fuse_time += 0.05;
                            break;

                        case 2:
                            dir += derivAtt( dir, Vec3( 0.0f, -ang, 0.0f ) );
                            fuse_time -= 0.10;
                            break;

                        case 3:
                            dir += derivAtt( dir, Vec3( 0.0f, 0.0f,  ang ) );
                            fuse_time -= 0.05;
                            break;

                        case 4:
                            dir += derivAtt( dir, Vec3( 0.0f, 0.0f, -ang ) );
                            fuse_time += 0.10;
                            break;
                        }

                        Flak *bullet = new Flak( m_parent_valid ? m_parent_id : m_id, fuse_time );

                        bullet->setPos( m_pos_abs + dir * Vec3( -1.0, 0.0, 0.0 ) * offset );
                        bullet->setAtt( dir );

                        m_burst_count++;
                    }
                }
                else
                {
                    m_burst_time = 0.0f;
                    m_burst_count = 0;
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

GunnerRear::GunnerRear( Affiliation affiliation, Group *parent ) :
    Gunner ( affiliation, parent )
{
    m_att.makeRotate( M_PI, osg::Z_AXIS );
}

////////////////////////////////////////////////////////////////////////////////

GunnerRear::~GunnerRear() {}

////////////////////////////////////////////////////////////////////////////////

void GunnerRear::updateWeapons()
{
    float f_psi = fabs( m_target_psi );

    if ( ( f_psi < m_45deg_rad && ( f_psi > m_10deg_rad || m_target_tht >  m_20deg_rad ) )
      && ( m_target_tht > 0.0f || ( f_psi > m_40deg_rad && m_target_tht > -m_20deg_rad ) )
       )
    {
        ////////////////////////
        Gunner::updateWeapons();
        ////////////////////////
    }
}

////////////////////////////////////////////////////////////////////////////////

GunnerZone::GunnerZone( Affiliation affiliation, float psi_min, float psi_max,
                        Group *parent ) :
    Gunner ( affiliation, parent ),

    m_psi_min ( psi_min ),
    m_psi_max ( psi_max )
{}

////////////////////////////////////////////////////////////////////////////////

GunnerZone::~GunnerZone() {}

////////////////////////////////////////////////////////////////////////////////

void GunnerZone::updateWeapons()
{
    if ( m_target_psi > m_psi_min && m_target_psi < m_psi_max
      && m_target_tht > -m_80deg_rad && m_target_tht < m_80deg_rad )
    {
        ////////////////////////
        Gunner::updateWeapons();
        ////////////////////////
    }
}

////////////////////////////////////////////////////////////////////////////////

GunnerBack::GunnerBack( Affiliation affiliation, Group *parent ) :
    GunnerZone( affiliation, -m_60deg_rad, m_60deg_rad, parent )
{
    m_att.makeRotate( M_PI, osg::Z_AXIS );
}

////////////////////////////////////////////////////////////////////////////////

GunnerBack::~GunnerBack() {}

////////////////////////////////////////////////////////////////////////////////

GunnerFront::GunnerFront( Affiliation affiliation, Group *parent ) :
    GunnerZone( affiliation, -m_60deg_rad, m_60deg_rad, parent )
{}

////////////////////////////////////////////////////////////////////////////////

GunnerFront::~GunnerFront() {}

////////////////////////////////////////////////////////////////////////////////

GunnerLeft::GunnerLeft( Affiliation affiliation, Group *parent ) :
    GunnerZone( affiliation, -m_60deg_rad, m_60deg_rad, parent )
{
    m_att.makeRotate( M_PI_2, osg::Z_AXIS );
}

////////////////////////////////////////////////////////////////////////////////

GunnerLeft::~GunnerLeft() {}

////////////////////////////////////////////////////////////////////////////////

GunnerRight::GunnerRight( Affiliation affiliation, Group *parent ) :
    GunnerZone( affiliation, -m_60deg_rad, m_60deg_rad, parent )
{
    m_att.makeRotate( -M_PI_2, osg::Z_AXIS );
}

////////////////////////////////////////////////////////////////////////////////

GunnerRight::~GunnerRight() {}
