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

const float Ownship::_target_fov_max   = osg::DegreesToRadians( 11.0 );
const float Ownship::_target_fov_max_2 = Ownship::_target_fov_max * Ownship::_target_fov_max;

////////////////////////////////////////////////////////////////////////////////

Ownship::Ownship() :
    _aircraft ( 0 ),
    _aircraftTarget   ( 0 ),
    _aircraftWreckage ( 0 ),
    _targetWreckage   ( 0 ),

    _id ( 0 ),

    _destroyed ( false ),

    _trigger ( false ),

    _ctrlRoll ( 0.0f ),
    _ctrlPitch ( 0.0f ),
    _ctrlYaw ( 0.0f ),
    _throttle ( 0.0f ),

    _pid_p ( 0 ),
    _pid_q ( 0 ),
    _pid_r ( 0 ),

    _waypoint_index ( 0 ),

    _waypoint ( false ),

    _waypoint_tht ( 0.0f ),
    _waypoint_psi ( 0.0f ),
    _waypoint_phi ( 0.0f ),

    _waypoint_time ( std::numeric_limits< float >::max() ),

    _hit_direction ( 0.0f ),

    _ownship_hits ( 0 ),

    _ownship_hit ( std::numeric_limits< float >::max() ),
    _friend_hit  ( std::numeric_limits< float >::max() ),
    _target_hit  ( std::numeric_limits< float >::max() ),
    _target_kill ( std::numeric_limits< float >::max() ),

    _target ( false ),
    _target_box ( false ),
    _target_cue ( false ),

    _target_id ( 0 ),
    _target_hp ( 0 ),

    _target_dir_phi ( 0.0f ),
    _target_box_tht ( 0.0f ),
    _target_box_psi ( 0.0f ),
    _target_cue_tht ( 0.0f ),
    _target_cue_psi ( 0.0f )
{
    _pid_p = new PID( 2.0f, 0.05f, 0.01f, -1.0f, 1.0f, true );
    _pid_q = new PID( 1.6f, 0.01f, 0.01f, -1.0f, 1.0f, true );
    _pid_r = new PID( 1.0f, 0.05f, 0.01f, -1.0f, 1.0f, true );
}

////////////////////////////////////////////////////////////////////////////////

Ownship::~Ownship()
{
    DELPTR( _pid_p );
    DELPTR( _pid_q );
    DELPTR( _pid_r );

    DELPTR( _aircraftTarget );
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::init()
{
    reset();

    Data::get()->ownship.init_throttle = 0.0f;

    if ( _aircraft )
    {
        _aircraftTarget = new Target< UnitAerial >( _aircraft,
            ( _aircraft->getAffiliation() == Hostile ) ? Friend : Hostile,
            3000.0f, Convert::nmi2m( 3.0f ) );

        Data::get()->ownship.init_throttle = _aircraft->getInitThrottle();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::reportDestroyed()
{
    _aircraft = 0;
    _destroyed = true;
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::reportHit( const Munition *munition )
{
    _ownship_hit = 0.0f;
    _ownship_hits++;

    if ( munition && _aircraft )
    {
        Vec3 r_bas = _aircraft->getAtt().inverse()
                * ( munition->getAtt() * ( -munition->getVel() ) );

        _hit_direction = atan2( -r_bas.y(), -r_bas.x() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::reportTargetHit( Unit *target )
{
    if ( Hostile == target->getAffiliation() )
    {
        _target_hit = 0.0f;
    }
    else if ( Friend == target->getAffiliation() )
    {
        _friend_hit = 0.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::reportTargetKill( Unit *target )
{
    if ( Hostile == target->getAffiliation() )
    {
        _target_kill = 0.0f;

        Aircraft *targetAircraft = dynamic_cast< Aircraft* >( target );

        if ( targetAircraft )
        {
            _targetWreckage = targetAircraft->getWreckage();
        }

        Statistics::instance()->reportOwnshipDestroyed();
    }
    else if ( Friend == target->getAffiliation() )
    {
        _friend_hit = 0.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::update( double timeStep )
{
    _timeStep = timeStep;

    if ( _aircraft )
    {
        _trigger = Data::get()->controls.trigger;

        _att_own_inv = _aircraft->getAtt().inverse();

        if ( !Data::get()->paused )
        {
            updateCamera();
            updateControls();
            updateTarget();
            updateWaypoint();
        }

        Data::get()->ownship.ownship_id = _aircraft->getId();
        Data::get()->ownship.hit_points = _aircraft->getHP();

        Data::get()->ownship.destroyed = _destroyed;

        Data::get()->ownship.gunfire = _trigger;

        Data::get()->ownship.init_throttle = _aircraft->getInitThrottle();

        Data::get()->ownship.altitude_asl  = _aircraft->getAltitudeASL();
        Data::get()->ownship.altitude_agl  = _aircraft->getAltitudeAGL();
        Data::get()->ownship.airspeed      = _aircraft->getAirspeed();
        Data::get()->ownship.climbRate     = _aircraft->getClimbRate();
        Data::get()->ownship.angleOfAttack = _aircraft->getAngleOfAttack();
        Data::get()->ownship.sideslipAngle = _aircraft->getSideslipAngle();
        Data::get()->ownship.rollAngle     = _aircraft->getRollAngle();
        Data::get()->ownship.pitchAngle    = _aircraft->getPitchAngle();
        Data::get()->ownship.heading       = _aircraft->getHeading();
        Data::get()->ownship.rollRate      = _aircraft->getOmg().x();
        Data::get()->ownship.pitchRate     = _aircraft->getOmg().y();
        Data::get()->ownship.yawRate       = _aircraft->getOmg().z();
        Data::get()->ownship.turnRate      = _aircraft->getTurnRate();

        Data::get()->ownship.pos_x = _aircraft->getPos().x();
        Data::get()->ownship.pos_y = _aircraft->getPos().y();
        Data::get()->ownship.pos_z = _aircraft->getPos().z();

        Data::get()->ownship.att_w = _aircraft->getAtt().w();
        Data::get()->ownship.att_x = _aircraft->getAtt().x();
        Data::get()->ownship.att_y = _aircraft->getAtt().y();
        Data::get()->ownship.att_z = _aircraft->getAtt().z();

        Data::get()->ownship.waypoint = _waypoint;

        Data::get()->ownship.waypoint_x = _waypoint_pos.x();
        Data::get()->ownship.waypoint_y = _waypoint_pos.y();
        Data::get()->ownship.waypoint_z = _waypoint_pos.z();

        Data::get()->ownship.waypoint_tht = _waypoint_tht;
        Data::get()->ownship.waypoint_psi = _waypoint_psi;
        Data::get()->ownship.waypoint_phi = _waypoint_phi;

        Data::get()->ownship.waypoint_dist = _aircraft->getDestinationDistance();
        Data::get()->ownship.waypoint_time = _waypoint_time;

        Data::get()->ownship.hit_direction = _hit_direction;

        Data::get()->ownship.ownship_hits = _ownship_hits;

        Data::get()->ownship.ownship_hit   = _ownship_hit;
        Data::get()->ownship.friend_hit    = _friend_hit;
        Data::get()->ownship.target_hit    = _target_hit;
        Data::get()->ownship.target_kill   = _target_kill;
        Data::get()->ownship.bombs_drop    = _aircraft->getTimeDrop();
        Data::get()->ownship.rocket_launch = _aircraft->getTimeLaunch();

        Data::get()->ownship.target     = _target;
        Data::get()->ownship.target_box = _target_box;
        Data::get()->ownship.target_cue = _target_cue;
        Data::get()->ownship.target_id  = _target_id;
        Data::get()->ownship.target_hp  = _target_hp;
        Data::get()->ownship.target_dir_phi = _target_dir_phi;
        Data::get()->ownship.target_box_tht = _target_box_tht;
        Data::get()->ownship.target_box_psi = _target_box_psi;
        Data::get()->ownship.target_cue_tht = _target_cue_tht;
        Data::get()->ownship.target_cue_psi = _target_cue_psi;

        if ( _targetWreckage )
        {
            Data::get()->ownship.wreckage_id = _targetWreckage->getId();
        }
        else
        {
            Data::get()->ownship.wreckage_id = _id;
        }
    }
    else
    {
        idleOutput();
    }

    _ownship_hit += _timeStep;
    _friend_hit  += _timeStep;
    _target_hit  += _timeStep;
    _target_kill += _timeStep;
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::setAircraft( Aircraft *aircraft )
{
    if ( aircraft )
    {
        _aircraft = aircraft;

        DELPTR( _aircraftTarget );

        _aircraftWreckage = 0;

        _destroyed = false;

        _ownship_hits = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::setWreckage( WreckageAircraft *wreckage )
{
    if ( wreckage )
    {
        _aircraftWreckage = wreckage;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::idleOutput()
{
    Data::get()->ownship.ownship_id = 0;
    Data::get()->ownship.hit_points = 0;

    Data::get()->ownship.destroyed = _destroyed;
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
    DELPTR( _aircraftTarget );

    _aircraftWreckage = 0;
    _targetWreckage   = 0;

    _trigger = false;

    _ctrlRoll = 0.0f;
    _ctrlPitch = 0.0f;
    _ctrlYaw = 0.0f;
    _throttle = 0.0f;

    _pid_p->setValue( 0.0f );
    _pid_q->setValue( 0.0f );
    _pid_r->setValue( 0.0f );

    _waypoint_index = 0;

    _waypoint_time = std::numeric_limits< float >::max();

    _hit_direction = 0.0f;

    _ownship_hits = 0;

    _ownship_hit = std::numeric_limits< float >::max();
    _friend_hit  = std::numeric_limits< float >::max();
    _target_hit  = std::numeric_limits< float >::max();
    _target_kill = std::numeric_limits< float >::max();

    _target = false;
    _target_box = false;
    _target_cue = false;

    _target_id = 0;
    _target_hp = 0;

    _target_dir_phi = 0.0f;
    _target_box_tht = 0.0f;
    _target_box_psi = 0.0f;
    _target_cue_tht = 0.0f;
    _target_cue_psi = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::updateCamera()
{
    _pos_cam.set( Data::get()->camera.pos_x,
                  Data::get()->camera.pos_y,
                  Data::get()->camera.pos_z );

    _att_cam.makeRotate( Data::get()->camera.d_phi, osg::X_AXIS,
                         Data::get()->camera.d_tht, osg::Y_AXIS,
                         Data::get()->camera.d_psi, osg::Z_AXIS );

    _att_cam_inv = _att_cam.inverse();
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::updateControls()
{
    if ( !Data::get()->controls.autopilot )
    {
        float ctrl_p = -Data::get()->controls.ctrlRoll  - 0.2f * Misc::sineWave( -Data::get()->controls.ctrlRoll  );
        float ctrl_q =  Data::get()->controls.ctrlPitch - 0.2f * Misc::sineWave(  Data::get()->controls.ctrlPitch );

        float sinPhi = sin( _aircraft->getAngles().phi() );
        float cosPhi = cos( _aircraft->getAngles().phi() );

        float max_phi = SIM_AIRCRAFT_MAX_PHI;
        float max_tht = SIM_AIRCRAFT_MAX_THT;

        float coef_tht = 1.0f;

        if ( _aircraft->getAltitudeASL() > 2500.0f && ctrl_q > 0.0f )
        {
            coef_tht = ctrl_q * ( 5000.0f - _aircraft->getAltitudeASL() ) / 2500.0f;
        }

        max_tht *= cosPhi * coef_tht;

        float turnRate = _aircraft->getAngles().phi() * _aircraft->getTurnMax()
                       / _aircraft->getRollMax();

        float diff_tht = max_tht * ctrl_q - _aircraft->getAngles().tht();
        float diff_turn = 4.0f * ( turnRate - _aircraft->getTurnRate() );

        _pid_p->update( _timeStep, max_phi * ctrl_p - _aircraft->getAngles().phi() );
        _pid_q->update( _timeStep, sinPhi * diff_turn + cosPhi * diff_tht );
        _pid_r->update( _timeStep, cosPhi * diff_turn - sinPhi * diff_tht );

        _ctrlRoll  = Inertia< float >::update( _timeStep, _ctrlRoll  , _pid_p->getValue(), 0.1 );
        _ctrlPitch = Inertia< float >::update( _timeStep, _ctrlPitch , _pid_q->getValue(), 0.1 );
        _ctrlYaw   = Inertia< float >::update( _timeStep, _ctrlYaw   , _pid_r->getValue(), 0.1 );
        _throttle  = Data::get()->controls.throttle;

        _ctrlRoll  = Misc::satur( -1.0f, 1.0f, _ctrlRoll  );
        _ctrlPitch = Misc::satur( -1.0f, 1.0f, _ctrlPitch );
        _ctrlYaw   = Misc::satur( -1.0f, 1.0f, _ctrlYaw   );
        _throttle  = Misc::satur(  0.0f, 1.0f, _throttle  );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::updateTarget()
{
    _target = false;
    _target_box = false;
    _target_cue = false;

    Unit *target = _aircraft->getTarget();

    if ( target )
    {
        if ( target->isActive() )
        {
            _target = true;
            _target_id = target->getId();
            _target_hp = target->getHP();

            float dist = ( target->getPos() - _aircraft->getPos() ).length();
            float time = dist / Bullet::m_vel_m;

            Vec3 r_enu = target->getPos() - _pos_cam;
            Vec3 e_enu = target->getPos() + ( target->getAtt() * target->getVel() ) * time - _pos_cam;

            Vec3 n_box = ( _att_cam_inv * ( _att_own_inv * r_enu ) );
            Vec3 n_cue = ( _att_cam_inv * ( _att_own_inv * e_enu ) );

            n_box *= 1.0/n_box.length();
            n_cue *= 1.0/n_cue.length();

            _target_dir_phi = atan2( -n_cue.y(),  n_cue.z() );
            _target_box_psi = atan2( -n_box.y(), -n_box.x() );
            _target_box_tht = atan2(  n_box.z(), -n_box.x() );
            _target_cue_psi = atan2( -n_cue.y(), -n_cue.x() );
            _target_cue_tht = atan2(  n_cue.z(), -n_cue.x() );

            float target_box_2 = _target_box_tht * _target_box_tht
                               + _target_box_psi * _target_box_psi;

            float target_cue_2 = _target_cue_tht * _target_cue_tht
                               + _target_cue_psi * _target_cue_psi;

            _target_box = target_box_2 < _target_fov_max_2;
            _target_cue = target_cue_2 < _target_fov_max_2;

            // looking for better target shooting and current target out of sight
            if ( _trigger && !_target_cue && !_target_box )
            {
                _aircraftTarget->setTarget( 0 );
                _aircraftTarget->findForward( _target_fov_max );

                UnitAerial *target = _aircraftTarget->getTarget();

                if ( target )
                {
                    Fighter *fighter = dynamic_cast< Fighter* >( _aircraft );
                    if ( fighter ) fighter->setTarget( target );
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Ownship::updateWaypoint()
{
    _waypoint_time += Data::get()->mission.time_step;

    if ( _aircraft->getDestValid() && _aircraft->getEnroute() )
    {
        if ( _waypoint_index != _aircraft->getWaypointIndex() )
        {
            _waypoint_time = 0.0f;

            _waypoint_index = _aircraft->getWaypointIndex();
        }

        _waypoint = true;

        _waypoint_pos = _aircraft->getDestination().first;

        Vec3 r_enu = _waypoint_pos - _pos_cam;
        Vec3 n_box = ( _att_cam_inv * ( _att_own_inv * r_enu ) );

        n_box *= 1.0/n_box.length();

        _waypoint_phi = atan2( -n_box.y(),  n_box.z() );
        _waypoint_psi = atan2( -n_box.y(), -n_box.x() );
        _waypoint_tht = atan2(  n_box.z(), -n_box.x() );
    }
    else if ( _waypoint && !_aircraft->getDestValid() )
    {
        if ( _waypoint_index != _aircraft->getWaypointIndex() )
        {
            _waypoint_time = 0.0f;

            _waypoint_index = _aircraft->getWaypointIndex();
        }

        _waypoint = false;
    }
    else
    {
        _waypoint = false;
    }
}
