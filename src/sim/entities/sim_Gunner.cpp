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

#include <sim/entities/sim_Gunner.h>

#include <sim/entities/sim_Flak.h>
#include <sim/entities/sim_Tracer.h>

#include <sim/utils/sim_Inertia.h>
#include <sim/utils/sim_Convert.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const float Gunner::_10deg_rad = Convert::deg2rad( 10.0f );
const float Gunner::_20deg_rad = Convert::deg2rad( 20.0f );
const float Gunner::_40deg_rad = Convert::deg2rad( 40.0f );
const float Gunner::_45deg_rad = M_PI_4;
const float Gunner::_60deg_rad = Convert::deg2rad( 60.0f );
const float Gunner::_80deg_rad = Convert::deg2rad( 80.0f );

////////////////////////////////////////////////////////////////////////////////

Gunner::Gunner( Affiliation affiliation, Group *parent, float range ) :
    Entity ( parent ),

    _affiliation ( affiliation ),

    _parent_valid ( false ),
    _parent_id ( 0 ),

    _range ( range ),

    _inertia  ( 0.5f ),
    _advanced ( true ),

    _target ( 0 ),

    _target_tht ( 0.0f ),
    _target_psi ( 0.0f ),

    _trigger ( false ),

    _target_valid ( false ),

    _shoot_time ( 0.0f )
{
    _target = new Target< UnitAerial >( this, ( _affiliation == Hostile ) ? Friend : Hostile,
                                        _range, _range );

    Unit *parentUnit = dynamic_cast< Unit* >( _parent );

    if ( parentUnit )
    {
        _parent_valid = true;
        _parent_id = parentUnit->getId();
    }
}

////////////////////////////////////////////////////////////////////////////////

Gunner::~Gunner()
{
    DELPTR( _target );
}

////////////////////////////////////////////////////////////////////////////////

void Gunner::update( double timeStep )
{
    ///////////////////////////
    Entity::update( timeStep );
    ///////////////////////////

    _trigger = false;

    if ( isActive() )
    {
        //////////////////
        _target->update();
        //////////////////

        _pos_abs = getAbsPos();
        _att_abs = getAbsAtt();

        _target_valid = false;

        Unit *target = _target->getTarget();

        if ( target )
        {
            Vec3 pos_est = target->getPos();

            // estimated future position
            if ( _advanced )
            {
                float time = _target_dist / Bullet::_vel_m;
                pos_est += ( target->getAtt() * target->getVel() ) * time;
            }

            _target_dist = ( pos_est - getAbsPos() ).length();

            if ( _target_dist < _range )
            {
                Vec3 dir_enu = pos_est - _pos_abs;
                dir_enu *= 1.0/dir_enu.length();

                Vec3 dir_bas = _att_abs.inverse() * dir_enu;
                dir_bas *= 1.0/dir_bas.length();

                double psi = atan2( -dir_bas.y(), -dir_bas.x() );
                double tht = atan2( dir_bas.z(), sqrt( dir_bas.x()*dir_bas.x() + dir_bas.y()*dir_bas.y() ) );

                _target_psi = Inertia< float >::update( _timeStep, _target_psi, psi, _inertia );
                _target_tht = Inertia< float >::update( _timeStep, _target_tht, tht, _inertia );

                Quat q_att( 0.0         , osg::X_AXIS,
                            _target_tht , osg::Y_AXIS,
                            _target_psi , osg::Z_AXIS );

                _target_dir = q_att * _att_abs;

                float delta_psi = _target_psi - psi;
                float delta_tht = _target_tht - tht;

                _trigger = sqrt( delta_psi*delta_psi + delta_tht*delta_tht ) < 0.05;

                _target_valid = true;
            }
            else
            {
                _target->findForward( M_PI );
            }
        }
        else
        {
            _target->findForward( M_PI );
        }

        updateWeapons();

        _shoot_time += _timeStep;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Gunner::updateWeapons()
{
    if ( _target_valid )
    {
        if ( _trigger && _target_dist < _range )
        {
            if ( _shoot_time > 0.2f )
            {
                _shoot_time = 0.0f;

                Tracer *bullet = new Tracer( _parent_valid ? _parent_id : _id );

                bullet->setPos( _pos_abs + _target_dir * Vec3( -1.0, 0.0, 0.0 ) * 20.0f );
                bullet->setAtt( _target_dir );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

GunnerFlak::GunnerFlak( Affiliation affiliation, Group *parent ) :
    Gunner( affiliation, parent, 2000.0f ),

    _burst_count ( 0 ),
    _burst_time ( 0.0f )
{}

////////////////////////////////////////////////////////////////////////////////

GunnerFlak::~GunnerFlak() {}

////////////////////////////////////////////////////////////////////////////////

void GunnerFlak::updateWeapons()
{
    _burst_time += _timeStep;

    if ( _target_valid )
    {
        if ( _target_dist < _range && _target_dist > 300.0 )
        {
            if ( _burst_time > 5.0f )
            {
                if ( _burst_count < 4 )
                {
                    if ( _shoot_time > 0.1f )
                    {
                        _shoot_time = 0.0f;

                        float offset = 20.0;
                        float fuse_time = ( _target_dist - offset ) / Bullet::_vel_m;

                        Quat dir = _target_dir;

                        float ang = Convert::deg2rad( 1.0 );

                        switch ( _burst_count )
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

                        Flak *bullet = new Flak( _parent_valid ? _parent_id : _id, fuse_time );

                        bullet->setPos( _pos_abs + dir * Vec3( -1.0, 0.0, 0.0 ) * offset );
                        bullet->setAtt( dir );

                        _burst_count++;
                    }
                }
                else
                {
                    _burst_time = 0.0f;
                    _burst_count = 0;
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

GunnerRear::GunnerRear( Affiliation affiliation, Group *parent ) :
    Gunner ( affiliation, parent )
{
    _att.makeRotate( M_PI, osg::Z_AXIS );
}

////////////////////////////////////////////////////////////////////////////////

GunnerRear::~GunnerRear() {}

////////////////////////////////////////////////////////////////////////////////

void GunnerRear::updateWeapons()
{
    float f_psi = fabs( _target_psi );

    if ( ( f_psi < _45deg_rad && ( f_psi > _10deg_rad || _target_tht >  _20deg_rad ) )
      && ( _target_tht > 0.0f || ( f_psi > _40deg_rad && _target_tht > -_20deg_rad ) )
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

    _psi_min ( psi_min ),
    _psi_max ( psi_max )
{}

////////////////////////////////////////////////////////////////////////////////

GunnerZone::~GunnerZone() {}

////////////////////////////////////////////////////////////////////////////////

void GunnerZone::updateWeapons()
{
    if ( _target_psi > _psi_min && _target_psi < _psi_max
      && _target_tht > -_80deg_rad && _target_tht < _80deg_rad )
    {
        ////////////////////////
        Gunner::updateWeapons();
        ////////////////////////
    }
}

////////////////////////////////////////////////////////////////////////////////

GunnerBack::GunnerBack( Affiliation affiliation, Group *parent ) :
    GunnerZone( affiliation, -_60deg_rad, _60deg_rad, parent )
{
    _att.makeRotate( M_PI, osg::Z_AXIS );
}

////////////////////////////////////////////////////////////////////////////////

GunnerBack::~GunnerBack() {}

////////////////////////////////////////////////////////////////////////////////

GunnerFront::GunnerFront( Affiliation affiliation, Group *parent ) :
    GunnerZone( affiliation, -_60deg_rad, _60deg_rad, parent )
{}

////////////////////////////////////////////////////////////////////////////////

GunnerFront::~GunnerFront() {}

////////////////////////////////////////////////////////////////////////////////

GunnerLeft::GunnerLeft( Affiliation affiliation, Group *parent ) :
    GunnerZone( affiliation, -_60deg_rad, _60deg_rad, parent )
{
    _att.makeRotate( M_PI_2, osg::Z_AXIS );
}

////////////////////////////////////////////////////////////////////////////////

GunnerLeft::~GunnerLeft() {}

////////////////////////////////////////////////////////////////////////////////

GunnerRight::GunnerRight( Affiliation affiliation, Group *parent ) :
    GunnerZone( affiliation, -_60deg_rad, _60deg_rad, parent )
{
    _att.makeRotate( -M_PI_2, osg::Z_AXIS );
}

////////////////////////////////////////////////////////////////////////////////

GunnerRight::~GunnerRight() {}
