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

    m_target ( 0 ),

    m_target_dist ( 0.0f ),
    m_target_bear ( 0.0f ),
    m_target_tht  ( 0.0f ),
    m_target_psi  ( 0.0f ),
    m_target_rad  ( 0.0f ),

    m_engaged ( false )
{
    m_target = new Target< UnitAerial >( this, ( m_affiliation == Hostile ) ? Friend : Hostile,
                                         3000.0f, Convert::nmi2m( 3.0f ) );
}

////////////////////////////////////////////////////////////////////////////////

Fighter::~Fighter()
{
    DELPTR( m_target );
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::hit( UInt16 dp, const Munition *munition )
{
    //////////////////////////////
    Aircraft::hit( dp, munition );
    //////////////////////////////

    if ( !m_ownship && ( m_target_dist > 5000.0f || fabs( m_target_bear ) > M_PI_2 ) )
    {
        Aircraft *shooter = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( munition->getShooterId() ) );

        if ( shooter )
        {
            if ( m_affiliation != shooter->getAffiliation() )
            {
                m_target->setTarget( shooter );
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

    if ( m_affiliation != target->getAffiliation() )
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
    return m_target->getTarget();
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::setTarget( UnitAerial *target )
{
    m_target->setTarget( target );
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::update( double timeStep )
{
    if ( isActive() )
    {
        m_target->update();
    }

    /////////////////////////////
    Aircraft::update( timeStep );
    /////////////////////////////

    if ( isActive() )
    {
        if ( !m_target->getTarget() )
        {
            m_target->findNearest( M_PI_4 );

            if ( !m_target->getTarget() )
            {
                m_target->findNearest();
            }

            UnitAerial *target = m_target->getTarget();

            if ( target && !m_ownship )
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
                                    m_target->findNearest( target );

                                    if ( !m_target->isValid() )
                                    {
                                        m_target->setTarget( target );
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

    if ( m_engaged )
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

    if ( !m_engaged )
    {
        if ( _route.size() > 0 && _waypointIndex < _route.size() )
        {
            if ( !_enroute || m_engaged )
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
        m_engaged = false;
    }

    //////////////////////////////
    Aircraft::updateDestination();
    //////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::updateTarget()
{
    UnitAerial *target = m_target->getTarget();

    if ( target )
    {
        _enroute = false;
        _wingman = false;
        m_engaged = true;

        _destValid = true;

        if ( !m_ownship || Data::get()->controls.autopilot )
        {
            m_target_dist = ( target->getPos() - _pos ).length();

            float time = m_target_dist / Bullet::m_vel_m;

            Vec3 pos_est = target->getPos() + ( target->getAtt() * target->getVel() ) * time;
            Vec3 vel_bas = _att.inverse() *   ( target->getAtt() * target->getVel() );

            Vec3 dir_enu = pos_est - _pos;
            dir_enu *= 1.0/dir_enu.length();

            m_target_bear = _angles.psi() - atan2( -dir_enu.y(), -dir_enu.x() );
            if      ( m_target_bear < -M_PI ) m_target_bear += 2.0f * M_PI;
            else if ( m_target_bear >  M_PI ) m_target_bear -= 2.0f * M_PI;

            Vec3 dir_bas = _att.inverse() * dir_enu;
            dir_bas *= 1.0/dir_bas.length();

            m_target_psi = atan2( -dir_bas.y(), -dir_bas.x() );
            m_target_tht = atan2( dir_bas.z(), sqrt( dir_bas.x()*dir_bas.x() + dir_bas.y()*dir_bas.y() ) );

            m_target_rad = target->getRadius();

            _destination.first  = pos_est;
            _destination.second = -vel_bas.x();

            if ( m_target_dist < 500.0f )
            {
                _destination.second = 0.8f * _destination.second;

                if ( m_target_dist < 250.0f )
                {
                    _destination.second = _speed_min;
                }
            }

            if ( m_target_dist > 750.0f )
            {
                _destination.second = 1.2f * _destination.second;

                if ( m_target_dist > 1000.0f )
                {
                    _destination.second = _speed_max;
                }
            }
        }
    }
    else
    {
        m_engaged = false;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Fighter::updateTrigger()
{
    _trigger = false;

    if ( m_ownship && !Data::get()->controls.autopilot )
    {
        _trigger = Ownship::instance()->getTrigger();
    }
    else
    {
        _trigger = false;

        if ( m_engaged && m_target_dist < 1000.0f )
        {
            float delta = sqrt( m_target_tht*m_target_tht + m_target_psi*m_target_psi );

            if ( delta < Misc::max( atan( m_target_rad / m_target_dist ), 0.05 ) )
            {
                _trigger = true;

                // avoid shooting to allies
                List::iterator it = Entities::instance()->getEntities()->begin();

                while ( it != Entities::instance()->getEntities()->end() )
                {
                    Unit *unit = dynamic_cast< Unit* >(*it);

                    if ( unit )
                    {
                        if ( unit->getAffiliation() == m_affiliation )
                        {
                            Vec3 dir_enu = unit->getPos() - _pos;
                            float dist = dir_enu.length();

                            if ( dist < m_target_dist )
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
