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
