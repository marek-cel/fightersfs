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

#include <sim/entities/sim_Torpedo.h>

#include <sim/entities/sim_Explosion.h>
#include <sim/entities/sim_Entities.h>

#include <sim/sim_Elevation.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Torpedo::Torpedo( UInt32 shooterId, Group *parent ) :
    Munition( 50000, shooterId, 60.0f, parent ),

    _elevation ( 0.0f )
{}

////////////////////////////////////////////////////////////////////////////////

Torpedo::~Torpedo() {}

////////////////////////////////////////////////////////////////////////////////

void Torpedo::update( double timeStep )
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
                if ( fabs( _angles.phi() ) < 1.0e-3 )
                {
                    _omg.y() = -_angles.tht();
                }
            }
        }
        else
        {
            resetLifeTime();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Torpedo::hit( Unit *target )
{
    ////////////////////////
    Munition::hit( target );
    ////////////////////////

    Explosion *explosion = new Explosion( 15.0f );
    explosion->setPos( _pos );
}

////////////////////////////////////////////////////////////////////////////////

void Torpedo::updateElevation()
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

void Torpedo::updateVelocity()
{
    if ( isTopLevel() )
    {
        Vec3 acc;

        if ( _pos.z() > 0.0f )
        {
            if ( _vel.length() < 100.0f )
            {
                acc = _att.inverse() * Vec3( 0.0, 0.0, -SIM_GRAVITY_ACC );
            }
        }
        else
        {
            float acc_z = -2.0f * ( _pos.z() + 2.0f ) - 2.0f * _vel.z();

            acc = _att.inverse() * Vec3( 0.0, 0.0, acc_z )
                + Vec3( -_vel.x() - 25.0f, 0.0, 0.0 );
        }

        _vel += ( acc - ( _omg ^ _vel ) ) * _timeStep;

        _omg.x() = -_angles.phi();
    }
}
