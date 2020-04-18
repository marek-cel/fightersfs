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

#include <sim/sim_Ownship.h>

#include <limits>

#include <sim/sim_Statistics.h>

#include <sim/entities/sim_Bullet.h>
#include <sim/entities/sim_Entities.h>

#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_Inertia.h>
#include <sim/utils/sim_Misc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const float Ownship::m_target_fov_max   = osg::DegreesToRadians( 11.0 );
const float Ownship::m_target_fov_max_2 = Ownship::m_target_fov_max * Ownship::m_target_fov_max;

////////////////////////////////////////////////////////////////////////////////

Ownship::Ownship() :
    m_aircraft ( 0 ),
    m_aircraftTarget   ( 0 ),
    m_aircraftWreckage ( 0 ),
    m_targetWreckage   ( 0 ),

    m_id ( 0 ),

    m_destroyed ( false ),

    m_trigger ( false ),

    m_ctrlRoll ( 0.0f ),
    m_ctrlPitch ( 0.0f ),
    m_ctrlYaw ( 0.0f ),
    m_throttle ( 0.0f ),

    m_pid_p ( 0 ),
    m_pid_q ( 0 ),
    m_pid_r ( 0 ),

    m_waypoint_index ( 0 ),

    m_waypoint ( false ),

    m_waypoint_tht ( 0.0f ),
    m_waypoint_psi ( 0.0f ),
    m_waypoint_phi ( 0.0f ),

    m_waypoint_time ( std::numeric_limits< float >::max() ),

    m_hit_direction ( 0.0f ),

    m_ownship_hits ( 0 ),

    m_ownship_hit ( std::numeric_limits< float >::max() ),
    m_friend_hit  ( std::numeric_limits< float >::max() ),
    m_target_hit  ( std::numeric_limits< float >::max() ),
    m_target_kill ( std::numeric_limits< float >::max() ),

    m_target ( false ),
    m_target_box ( false ),
    m_target_cue ( false ),

    m_target_id ( 0 ),
    m_target_hp ( 0 ),

    m_target_dir_phi ( 0.0f ),
    m_target_box_tht ( 0.0f ),
    m_target_box_psi ( 0.0f ),
    m_target_cue_tht ( 0.0f ),
    m_target_cue_psi ( 0.0f )
{
    m_pid_p = new PID( 2.0f, 0.05f, 0.01f, -1.0f, 1.0f, true );
    m_pid_q = new PID( 1.6f, 0.01f, 0.01f, -1.0f, 1.0f, true );
    m_pid_r = new PID( 1.0f, 0.05f, 0.01f, -1.0f, 1.0f, true );
}

////////////////////////////////////////////////////////////////////////////////

Ownship::~Ownship()
{
    if ( m_pid_p ) delete m_pid_p;
    m_pid_p = 0;

    if ( m_pid_q ) delete m_pid_q;
    m_pid_q = 0;

    if ( m_pid_r ) delete m_pid_r;
    m_pid_r = 0;

    if ( m_aircraftTarget ) delete m_aircraftTarget;
    m_aircraftTarget = 0;
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::init()
{
    reset();

    Data::get()->ownship.init_throttle = 0.0f;

    if ( m_aircraft )
    {
        m_aircraftTarget = new Target< UnitAerial >( m_aircraft,
            ( m_aircraft->getAffiliation() == Hostile ) ? Friend : Hostile,
            3000.0f, Convert::nm2m( 3.0f ) );

        Data::get()->ownship.init_throttle = m_aircraft->getInitThrottle();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::reportDestroyed()
{
    m_aircraft = 0;
    m_destroyed = true;
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::reportHit( const Munition *munition )
{
    m_ownship_hit = 0.0f;
    m_ownship_hits++;

    if ( munition && m_aircraft )
    {
        Vec3 r_bas = m_aircraft->getAtt().inverse()
                * ( munition->getAtt() * ( -munition->getVel() ) );

        m_hit_direction = atan2( -r_bas.y(), -r_bas.x() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::reportTargetHit( Unit *target )
{
    if ( Hostile == target->getAffiliation() )
    {
        m_target_hit = 0.0f;
    }
    else if ( Friend == target->getAffiliation() )
    {
        m_friend_hit = 0.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::reportTargetKill( Unit *target )
{
    if ( Hostile == target->getAffiliation() )
    {
        m_target_kill = 0.0f;

        Aircraft *targetAircraft = dynamic_cast< Aircraft* >( target );

        if ( targetAircraft )
        {
            m_targetWreckage = targetAircraft->getWreckage();
        }

        Statistics::instance()->reportOwnshipDestroyed();
    }
    else if ( Friend == target->getAffiliation() )
    {
        m_friend_hit = 0.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::update( double timeStep )
{
    m_timeStep = timeStep;

    if ( m_aircraft )
    {
        m_trigger = Data::get()->controls.trigger;

        m_att_own_inv = m_aircraft->getAtt().inverse();

        if ( !Data::get()->paused )
        {
            updateCamera();
            updateControls();
            updateTarget();
            updateWaypoint();
        }

        Data::get()->ownship.ownship_id = m_aircraft->getId();
        Data::get()->ownship.hit_points = m_aircraft->getHP();

        Data::get()->ownship.destroyed = m_destroyed;

        Data::get()->ownship.gunfire = m_trigger;

        Data::get()->ownship.init_throttle = m_aircraft->getInitThrottle();

        Data::get()->ownship.altitude_asl  = m_aircraft->getAltitudeASL();
        Data::get()->ownship.altitude_agl  = m_aircraft->getAltitudeAGL();
        Data::get()->ownship.airspeed      = m_aircraft->getAirspeed();
        Data::get()->ownship.climbRate     = m_aircraft->getClimbRate();
        Data::get()->ownship.angleOfAttack = m_aircraft->getAngleOfAttack();
        Data::get()->ownship.sideslipAngle = m_aircraft->getSideslipAngle();
        Data::get()->ownship.rollAngle     = m_aircraft->getRollAngle();
        Data::get()->ownship.pitchAngle    = m_aircraft->getPitchAngle();
        Data::get()->ownship.heading       = m_aircraft->getHeading();
        Data::get()->ownship.rollRate      = m_aircraft->getOmg().x();
        Data::get()->ownship.pitchRate     = m_aircraft->getOmg().y();
        Data::get()->ownship.yawRate       = m_aircraft->getOmg().z();
        Data::get()->ownship.turnRate      = m_aircraft->getTurnRate();

        Data::get()->ownship.pos_x = m_aircraft->getPos().x();
        Data::get()->ownship.pos_y = m_aircraft->getPos().y();
        Data::get()->ownship.pos_z = m_aircraft->getPos().z();

        Data::get()->ownship.att_w = m_aircraft->getAtt().w();
        Data::get()->ownship.att_x = m_aircraft->getAtt().x();
        Data::get()->ownship.att_y = m_aircraft->getAtt().y();
        Data::get()->ownship.att_z = m_aircraft->getAtt().z();

        Data::get()->ownship.waypoint = m_waypoint;

        Data::get()->ownship.waypoint_x = m_waypoint_pos.x();
        Data::get()->ownship.waypoint_y = m_waypoint_pos.y();
        Data::get()->ownship.waypoint_z = m_waypoint_pos.z();

        Data::get()->ownship.waypoint_tht = m_waypoint_tht;
        Data::get()->ownship.waypoint_psi = m_waypoint_psi;
        Data::get()->ownship.waypoint_phi = m_waypoint_phi;

        Data::get()->ownship.waypoint_dist = m_aircraft->getDestinationDistance();
        Data::get()->ownship.waypoint_time = m_waypoint_time;

        Data::get()->ownship.hit_direction = m_hit_direction;

        Data::get()->ownship.ownship_hits = m_ownship_hits;

        Data::get()->ownship.ownship_hit   = m_ownship_hit;
        Data::get()->ownship.friend_hit    = m_friend_hit;
        Data::get()->ownship.target_hit    = m_target_hit;
        Data::get()->ownship.target_kill   = m_target_kill;
        Data::get()->ownship.bombs_drop    = m_aircraft->getTimeDrop();
        Data::get()->ownship.rocket_launch = m_aircraft->getTimeLaunch();

        Data::get()->ownship.target     = m_target;
        Data::get()->ownship.target_box = m_target_box;
        Data::get()->ownship.target_cue = m_target_cue;
        Data::get()->ownship.target_id  = m_target_id;
        Data::get()->ownship.target_hp  = m_target_hp;
        Data::get()->ownship.target_dir_phi = m_target_dir_phi;
        Data::get()->ownship.target_box_tht = m_target_box_tht;
        Data::get()->ownship.target_box_psi = m_target_box_psi;
        Data::get()->ownship.target_cue_tht = m_target_cue_tht;
        Data::get()->ownship.target_cue_psi = m_target_cue_psi;

        if ( m_targetWreckage )
        {
            Data::get()->ownship.wreckage_id = m_targetWreckage->getId();
        }
        else
        {
            Data::get()->ownship.wreckage_id = m_id;
        }
    }
    else
    {
        idleOutput();
    }

    m_ownship_hit += m_timeStep;
    m_friend_hit  += m_timeStep;
    m_target_hit  += m_timeStep;
    m_target_kill += m_timeStep;
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::setAircraft( Aircraft *aircraft )
{
    if ( aircraft )
    {
        m_aircraft = aircraft;

        if ( m_aircraftTarget ) delete m_aircraftTarget;
        m_aircraftTarget = 0;
        m_aircraftWreckage = 0;

        m_destroyed = false;

        m_ownship_hits = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::setWreckage( WreckageAircraft *wreckage )
{
    if ( wreckage )
    {
        m_aircraftWreckage = wreckage;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::idleOutput()
{
    Data::get()->ownship.ownship_id = 0;
    Data::get()->ownship.hit_points = 0;

    Data::get()->ownship.destroyed = m_destroyed;
    Data::get()->ownship.gunfire   = false;

    Data::get()->ownship.init_throttle = 0.0f;

    Data::get()->ownship.altitude_asl  = 0.0f;
    Data::get()->ownship.altitude_agl  = 0.0f;
    Data::get()->ownship.airspeed      = 0.0f;
    Data::get()->ownship.climbRate     = 0.0f;
    Data::get()->ownship.angleOfAttack = 0.0f;
    Data::get()->ownship.sideslipAngle = 0.0f;
    Data::get()->ownship.rollAngle     = 0.0f;
    Data::get()->ownship.pitchAngle    = 0.0f;
    Data::get()->ownship.heading       = 0.0f;
    Data::get()->ownship.rollRate      = 0.0f;
    Data::get()->ownship.pitchRate     = 0.0f;
    Data::get()->ownship.yawRate       = 0.0f;
    Data::get()->ownship.turnRate      = 0.0f;

    Data::get()->ownship.pos_x = 0.0f;
    Data::get()->ownship.pos_y = 0.0f;
    Data::get()->ownship.pos_z = 0.0f;

    Data::get()->ownship.att_w = 1.0f;
    Data::get()->ownship.att_x = 0.0f;
    Data::get()->ownship.att_y = 0.0f;
    Data::get()->ownship.att_z = 0.0f;

    Data::get()->ownship.waypoint = false;

    Data::get()->ownship.waypoint_x = 0.0f;
    Data::get()->ownship.waypoint_y = 0.0f;
    Data::get()->ownship.waypoint_z = 0.0f;

    Data::get()->ownship.waypoint_tht = 0.0f;
    Data::get()->ownship.waypoint_psi = 0.0f;
    Data::get()->ownship.waypoint_phi = 0.0f;

    Data::get()->ownship.waypoint_dist = 0.0f;
    Data::get()->ownship.waypoint_time = std::numeric_limits< float >::max();

    Data::get()->ownship.hit_direction = 0.0f;

    Data::get()->ownship.ownship_hits = 0;

    Data::get()->ownship.ownship_hit = std::numeric_limits< float >::max();
    Data::get()->ownship.friend_hit  = std::numeric_limits< float >::max();

    Data::get()->ownship.target        = false;
    Data::get()->ownship.target_box    = false;
    Data::get()->ownship.target_cue    = false;
    Data::get()->ownship.target_id     = 0;
    Data::get()->ownship.target_hp     = 0;
    Data::get()->ownship.target_hit    = std::numeric_limits< float >::max();
    Data::get()->ownship.target_kill   = std::numeric_limits< float >::max();
    Data::get()->ownship.bombs_drop    = std::numeric_limits< float >::max();
    Data::get()->ownship.rocket_launch = std::numeric_limits< float >::max();

    Data::get()->ownship.target_dir_phi = 0.0f;
    Data::get()->ownship.target_box_tht = 0.0f;
    Data::get()->ownship.target_box_psi = 0.0f;
    Data::get()->ownship.target_cue_tht = 0.0f;
    Data::get()->ownship.target_cue_psi = 0.0f;

    Data::get()->ownship.wreckage_id = 0;
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::reset()
{
    if ( m_aircraftTarget ) delete m_aircraftTarget;
    m_aircraftTarget = 0;

    m_aircraftWreckage = 0;
    m_targetWreckage   = 0;

    m_trigger = false;

    m_ctrlRoll = 0.0f;
    m_ctrlPitch = 0.0f;
    m_ctrlYaw = 0.0f;
    m_throttle = 0.0f;

    m_pid_p->setValue( 0.0f );
    m_pid_q->setValue( 0.0f );
    m_pid_r->setValue( 0.0f );

    m_waypoint_index = 0;

    m_waypoint_time = std::numeric_limits< float >::max();

    m_hit_direction = 0.0f;

    m_ownship_hits = 0;

    m_ownship_hit = std::numeric_limits< float >::max();
    m_friend_hit  = std::numeric_limits< float >::max();
    m_target_hit  = std::numeric_limits< float >::max();
    m_target_kill = std::numeric_limits< float >::max();

    m_target = false;
    m_target_box = false;
    m_target_cue = false;

    m_target_id = 0;
    m_target_hp = 0;

    m_target_dir_phi = 0.0f;
    m_target_box_tht = 0.0f;
    m_target_box_psi = 0.0f;
    m_target_cue_tht = 0.0f;
    m_target_cue_psi = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::updateCamera()
{
    m_pos_cam.set( Data::get()->camera.pos_x,
                   Data::get()->camera.pos_y,
                   Data::get()->camera.pos_z );

    m_att_cam.makeRotate( Data::get()->camera.d_phi, osg::X_AXIS,
                          Data::get()->camera.d_tht, osg::Y_AXIS,
                          Data::get()->camera.d_psi, osg::Z_AXIS );

    m_att_cam_inv = m_att_cam.inverse();
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::updateControls()
{
    if ( !Data::get()->controls.autopilot )
    {
        float ctrl_p = -Data::get()->controls.ctrlRoll  - 0.2f * Misc::sineWave( -Data::get()->controls.ctrlRoll  );
        float ctrl_q =  Data::get()->controls.ctrlPitch - 0.2f * Misc::sineWave(  Data::get()->controls.ctrlPitch );

        float sinPhi = sin( m_aircraft->getAngles().phi() );
        float cosPhi = cos( m_aircraft->getAngles().phi() );

        float max_phi = SIM_AIRCRAFT_MAX_PHI;
        float max_tht = SIM_AIRCRAFT_MAX_THT;

        float coef_tht = 1.0f;

        if ( m_aircraft->getAltitudeASL() > 2500.0f && ctrl_q > 0.0f )
        {
            coef_tht = ctrl_q * ( 5000.0f - m_aircraft->getAltitudeASL() ) / 2500.0f;
        }

        max_tht *= cosPhi * coef_tht;

        float turnRate = m_aircraft->getAngles().phi() * m_aircraft->getTurnMax()
                       / m_aircraft->getRollMax();

        float diff_tht = max_tht * ctrl_q - m_aircraft->getAngles().tht();
        float diff_turn = 4.0f * ( turnRate - m_aircraft->getTurnRate() );

        m_pid_p->update( m_timeStep, max_phi * ctrl_p - m_aircraft->getAngles().phi() );
        m_pid_q->update( m_timeStep, sinPhi * diff_turn + cosPhi * diff_tht );
        m_pid_r->update( m_timeStep, cosPhi * diff_turn - sinPhi * diff_tht );

        m_ctrlRoll  = Inertia< float >::update( m_timeStep, m_ctrlRoll  , m_pid_p->getValue(), 0.1 );
        m_ctrlPitch = Inertia< float >::update( m_timeStep, m_ctrlPitch , m_pid_q->getValue(), 0.1 );
        m_ctrlYaw   = Inertia< float >::update( m_timeStep, m_ctrlYaw   , m_pid_r->getValue(), 0.1 );
        m_throttle  = Data::get()->controls.throttle;

        m_ctrlRoll  = Misc::satur( -1.0f, 1.0f, m_ctrlRoll  );
        m_ctrlPitch = Misc::satur( -1.0f, 1.0f, m_ctrlPitch );
        m_ctrlYaw   = Misc::satur( -1.0f, 1.0f, m_ctrlYaw   );
        m_throttle  = Misc::satur(  0.0f, 1.0f, m_throttle  );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::updateTarget()
{
    m_target = false;
    m_target_box = false;
    m_target_cue = false;

    Unit *target = m_aircraft->getTarget();

    if ( target )
    {
        if ( target->isActive() )
        {
            m_target = true;
            m_target_id = target->getId();
            m_target_hp = target->getHP();

            float dist = ( target->getPos() - m_aircraft->getPos() ).length();
            float time = dist / Bullet::m_vel_m;

            Vec3 r_enu = target->getPos() - m_pos_cam;
            Vec3 e_enu = target->getPos() + ( target->getAtt() * target->getVel() ) * time - m_pos_cam;

            Vec3 n_box = ( m_att_cam_inv * ( m_att_own_inv * r_enu ) );
            Vec3 n_cue = ( m_att_cam_inv * ( m_att_own_inv * e_enu ) );

            n_box *= 1.0/n_box.length();
            n_cue *= 1.0/n_cue.length();

            m_target_dir_phi = atan2( -n_cue.y(),  n_cue.z() );
            m_target_box_psi = atan2( -n_box.y(), -n_box.x() );
            m_target_box_tht = atan2(  n_box.z(), -n_box.x() );
            m_target_cue_psi = atan2( -n_cue.y(), -n_cue.x() );
            m_target_cue_tht = atan2(  n_cue.z(), -n_cue.x() );

            float target_box_2 = m_target_box_tht * m_target_box_tht
                               + m_target_box_psi * m_target_box_psi;

            float target_cue_2 = m_target_cue_tht * m_target_cue_tht
                               + m_target_cue_psi * m_target_cue_psi;

            m_target_box = target_box_2 < m_target_fov_max_2;
            m_target_cue = target_cue_2 < m_target_fov_max_2;

            // looking for better target shooting and current target out of sight
            if ( m_trigger && !m_target_cue && !m_target_box )
            {
                m_aircraftTarget->setTarget( 0 );
                m_aircraftTarget->findForward( m_target_fov_max );

                UnitAerial *target = m_aircraftTarget->getTarget();

                if ( target )
                {
                    Fighter *fighter = dynamic_cast< Fighter* >( m_aircraft );
                    if ( fighter ) fighter->setTarget( target );
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::updateWaypoint()
{
    m_waypoint_time += Data::get()->mission.time_step;

    if ( m_aircraft->getDestValid() && m_aircraft->getEnroute() )
    {
        if ( m_waypoint_index != m_aircraft->getWaypointIndex() )
        {
            m_waypoint_time = 0.0f;

            m_waypoint_index = m_aircraft->getWaypointIndex();
        }

        m_waypoint = true;

        m_waypoint_pos = m_aircraft->getDestination().first;

        Vec3 r_enu = m_waypoint_pos - m_pos_cam;
        Vec3 n_box = ( m_att_cam_inv * ( m_att_own_inv * r_enu ) );

        n_box *= 1.0/n_box.length();

        m_waypoint_phi = atan2( -n_box.y(),  n_box.z() );
        m_waypoint_psi = atan2( -n_box.y(), -n_box.x() );
        m_waypoint_tht = atan2(  n_box.z(), -n_box.x() );
    }
    else if ( m_waypoint && !m_aircraft->getDestValid() )
    {
        if ( m_waypoint_index != m_aircraft->getWaypointIndex() )
        {
            m_waypoint_time = 0.0f;

            m_waypoint_index = m_aircraft->getWaypointIndex();
        }

        m_waypoint = false;
    }
    else
    {
        m_waypoint = false;
    }
}
