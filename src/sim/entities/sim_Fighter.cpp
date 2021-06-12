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

#include <sim/entities/sim_Fighter.h>
#include <sim/entities/sim_Bullet.h>

#include <sim/cgi/sim_Models.h>

#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_Misc.h>
#include <sim/utils/sim_XmlUtils.h>

#include <sim/sim_Ownship.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const std::string Fighter::_tagName = "fighter";

////////////////////////////////////////////////////////////////////////////////

Fighter::Fighter( Affiliation affiliation ) :
    Aircraft( affiliation ),

    _target ( 0 ),

    _target_dist ( 0.0f ),
    _target_bear ( 0.0f ),
    _target_tht  ( 0.0f ),
    _target_psi  ( 0.0f ),
    _target_rad  ( 0.0f ),

    _engaged ( false )
{
    _target = new Target< UnitAerial >( this, ( _affiliation == Hostile ) ? Friend : Hostile,
                                        3000.0f, Convert::nmi2m( 3.0f ) );
}

////////////////////////////////////////////////////////////////////////////////

Fighter::~Fighter()
{
    DELPTR( _target );
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::hit( UInt16 dp, const Munition *munition )
{
    //////////////////////////////
    Aircraft::hit( dp, munition );
    //////////////////////////////

    if ( !_ownship && ( _target_dist > 5000.0f || fabs( _target_bear ) > M_PI_2 ) )
    {
        Aircraft *shooter = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( munition->getShooterId() ) );

        if ( shooter )
        {
            if ( _affiliation != shooter->getAffiliation() )
            {
                _target->setTarget( shooter );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::reportTargetHit( Unit *target )
{
    ////////////////////////////////////
    Aircraft::reportTargetHit( target );
    ////////////////////////////////////

    if ( _affiliation != target->getAffiliation() )
    {
        UnitAerial *unitAerial = dynamic_cast< UnitAerial* >( target );

        if ( unitAerial )
        {
            setTarget( unitAerial );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

UnitAerial* Fighter::getTarget() const
{
    return _target->getTarget();
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::setTarget( UnitAerial *target )
{
    _target->setTarget( target );
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::update( double timeStep )
{
    if ( isActive() )
    {
        _target->update();
    }

    /////////////////////////////
    Aircraft::update( timeStep );
    /////////////////////////////

    if ( isActive() )
    {
        if ( !_target->getTarget() )
        {
            _target->findNearest( M_PI_4 );

            if ( !_target->getTarget() )
            {
                _target->findNearest();
            }

            UnitAerial *target = _target->getTarget();

            if ( target && !_ownship )
            {
                List::iterator it = Entities::instance()->getEntities()->begin();

                while ( it != Entities::instance()->getEntities()->end() )
                {
                    Fighter *fighter = dynamic_cast< Fighter* >(*it);

                    if ( fighter )
                    {
                        if ( fighter->getId() != _id )
                        {
                            Unit *fighterTarget = fighter->getTarget();

                            if ( fighterTarget )
                            {
                                if ( fighterTarget->getId() == target->getId() )
                                {
                                    _target->findNearest( target );

                                    if ( !_target->isValid() )
                                    {
                                        _target->setTarget( target );
                                    }

                                    break;
                                }
                            }
                        }
                    }

                    ++it;
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::updateControls()
{
    ///////////////////////////
    Aircraft::updateControls();
    ///////////////////////////

    if ( _engaged )
    {
        _pid_phi->setKi( 0.4f );
    }
    else
    {
        _pid_phi->setKi( 0.2f );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::updateDestination()
{
    updateTarget();

    if ( !_engaged )
    {
        if ( _route.size() > 0 && _waypointIndex < _route.size() )
        {
            if ( !_enroute || _engaged )
            {
                _destination = _route[ _waypointIndex ];
                _destValid = true;
            }
        }
        else
        {
            _destValid = false;
        }

        _enroute = true;
        _wingman = _leaderValid;
        _engaged = false;
    }

    //////////////////////////////
    Aircraft::updateDestination();
    //////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::updateTarget()
{
    UnitAerial *target = _target->getTarget();

    if ( target )
    {
        _enroute = false;
        _wingman = false;
        _engaged = true;

        _destValid = true;

        if ( !_ownship || Data::get()->controls.autopilot )
        {
            _target_dist = ( target->getPos() - _pos ).length();

            float time = _target_dist / Bullet::_vel_m;

            Vec3 pos_est = target->getPos() + ( target->getAtt() * target->getVel() ) * time;
            Vec3 vel_bas = _att.inverse() *   ( target->getAtt() * target->getVel() );

            Vec3 dir_enu = pos_est - _pos;
            dir_enu *= 1.0/dir_enu.length();

            _target_bear = _angles.psi() - atan2( -dir_enu.y(), -dir_enu.x() );
            if      ( _target_bear < -M_PI ) _target_bear += 2.0f * M_PI;
            else if ( _target_bear >  M_PI ) _target_bear -= 2.0f * M_PI;

            Vec3 dir_bas = _att.inverse() * dir_enu;
            dir_bas *= 1.0/dir_bas.length();

            _target_psi = atan2( -dir_bas.y(), -dir_bas.x() );
            _target_tht = atan2( dir_bas.z(), sqrt( dir_bas.x()*dir_bas.x() + dir_bas.y()*dir_bas.y() ) );

            _target_rad = target->getRadius();

            _destination.first  = pos_est;
            _destination.second = -vel_bas.x();

            if ( _target_dist < 500.0f )
            {
                _destination.second = 0.8f * _destination.second;

                if ( _target_dist < 250.0f )
                {
                    _destination.second = _speed_min;
                }
            }

            if ( _target_dist > 750.0f )
            {
                _destination.second = 1.2f * _destination.second;

                if ( _target_dist > 1000.0f )
                {
                    _destination.second = _speed_max;
                }
            }
        }
    }
    else
    {
        _engaged = false;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::updateTrigger()
{
    _trigger = false;

    if ( _ownship && !Data::get()->controls.autopilot )
    {
        _trigger = Ownship::instance()->getTrigger();
    }
    else
    {
        _trigger = false;

        if ( _engaged && _target_dist < 1000.0f )
        {
            float delta = sqrt( _target_tht*_target_tht + _target_psi*_target_psi );

            if ( delta < Misc::max( atan( _target_rad / _target_dist ), 0.05 ) )
            {
                _trigger = true;

                // avoid shooting to allies
                List::iterator it = Entities::instance()->getEntities()->begin();

                while ( it != Entities::instance()->getEntities()->end() )
                {
                    Unit *unit = dynamic_cast< Unit* >(*it);

                    if ( unit )
                    {
                        if ( unit->getAffiliation() == _affiliation )
                        {
                            Vec3 dir_enu = unit->getPos() - _pos;
                            float dist = dir_enu.length();

                            if ( dist < _target_dist )
                            {
                                Vec3 dir_bas = _att.inverse() * dir_enu;

                                float yz = sqrt( dir_bas.y()*dir_bas.y() + dir_bas.z()*dir_bas.z() );

                                if ( dir_bas.x() < -10.0 && yz < 4.0f * unit->getRadius() )
                                {
                                    _trigger = false;
                                    break;
                                }
                            }
                        }
                    }

                    ++it;
                }
            }
        }
    }
}
