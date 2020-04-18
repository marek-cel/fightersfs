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

#include <sim/entities/sim_Torpedo.h>

#include <sim/entities/sim_Explosion.h>
#include <sim/entities/sim_Entities.h>

#include <sim/sim_Elevation.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Torpedo::Torpedo( UInt32 shooterId, Group *parent ) :
    Munition( 50000, shooterId, 60.0f, parent ),

    m_elevation ( 0.0f )
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
            if ( m_pos.z() < m_elevation )
            {
                if ( fabs( m_angles.phi() ) < 1.0e-3 )
                {
                    m_omg.y() = -m_angles.tht();
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
    explosion->setPos( m_pos );
}

////////////////////////////////////////////////////////////////////////////////

void Torpedo::updateElevation()
{
    if ( isTopLevel() )
    {
        ////////////////////////////
        Munition::updateElevation();
        ////////////////////////////

        m_elevation = Elevation::instance()->getElevation( m_pos.x(), m_pos.y() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Torpedo::updateVelocity()
{
    if ( isTopLevel() )
    {
        Vec3 acc;

        if ( m_pos.z() > 0.0f )
        {
            if ( m_vel.length() < 100.0f )
            {
                acc = m_att.inverse() * Vec3( 0.0, 0.0, -SIM_GRAVITY_ACC );
            }
        }
        else
        {
            float acc_z = -2.0f * ( m_pos.z() + 2.0f ) - 2.0f * m_vel.z();

            acc = m_att.inverse() * Vec3( 0.0, 0.0, acc_z )
                + Vec3( -m_vel.x() - 25.0f, 0.0, 0.0 );
        }

        m_vel += ( acc - ( m_omg ^ m_vel ) ) * m_timeStep;

        m_omg.x() = -m_angles.phi();
    }
}
