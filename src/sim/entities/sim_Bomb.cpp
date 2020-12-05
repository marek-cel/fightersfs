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

#include <sim/entities/sim_Bomb.h>

#include <sim/entities/sim_Explosion.h>
#include <sim/entities/sim_Entities.h>
#include <sim/sim_Elevation.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const float Bomb::_r_limit = 30.0f;
const float Bomb::_r_limit_2 = Bomb::_r_limit * Bomb::_r_limit;

////////////////////////////////////////////////////////////////////////////////

Bomb::Bomb( UInt32 shooterId, Group *parent ) :
    Munition( 50000, shooterId, 60.0f, parent ),

    _elevation ( 0.0f )
{}

////////////////////////////////////////////////////////////////////////////////

Bomb::~Bomb() {}

////////////////////////////////////////////////////////////////////////////////

void Bomb::update( double timeStep )
{
    /////////////////////////////
    Munition::update( timeStep );
    /////////////////////////////

    if ( isActive() )
    {
        if ( isTopLevel() )
        {
            if ( _pos.z() < _elevation )
            {
                rangeDamage();
                setState( Inactive );
            }
        }
        else
        {
            resetLifeTime();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Bomb::hit( Unit* )
{
    rangeDamage();
}

////////////////////////////////////////////////////////////////////////////////

void Bomb::rangeDamage()
{
    List entities = *Entities::instance()->getEntities();
    List::iterator it = entities.begin();

    while ( it != entities.end() )
    {
        Unit *target = dynamic_cast< Unit* >(*it);

        if ( target )
        {
            if ( target->isActive() )
            {
                float r2 = ( target->getPos() - _pos ).length2() - target->getRadius2();

                if ( r2 < _r_limit_2 )
                {
                    UInt16 dp = _dp;

                    if ( r2 > 0.0f )
                    {
                        dp = (float)_dp * ( 1.0f - r2 / _r_limit_2 );
                    }

                    target->hit( dp, this );
                    reportTargetHit( target );
                }
            }
        }

        ++it;
    }

    Explosion *explosion = new Explosion( 15.0f );
    explosion->setPos( _pos );
}

////////////////////////////////////////////////////////////////////////////////

void Bomb::updateElevation()
{
    if ( isTopLevel() )
    {
        ////////////////////////////
        Munition::updateElevation();
        ////////////////////////////

        _elevation = Elevation::instance()->getElevation( _pos.x(), _pos.y() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Bomb::updateVelocity()
{
    if ( isTopLevel() )
    {
        float v = _vel.length();

        if ( v < 200.0f )
        {
            Vec3 acc = _att.inverse() * Vec3( 0.0, 0.0, -SIM_GRAVITY_ACC );
            _vel += ( acc - ( _omg ^ _vel ) ) * _timeStep;
        }

        float alpha = 0.0f;

        if ( v > 0.01 )
        {
            float vel_xy = sqrt( _vel.x()*_vel.x() + _vel.y()*_vel.y() );
            alpha = atan2( -_vel.z(), vel_xy );
        }

        _omg.x() = -_angles.phi() / _timeStep;
        _omg.y() = -alpha / _timeStep;
    }
}
