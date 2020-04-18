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

const float Bomb::m_r_limit = 30.0f;
const float Bomb::m_r_limit_2 = Bomb::m_r_limit * Bomb::m_r_limit;

////////////////////////////////////////////////////////////////////////////////

Bomb::Bomb( UInt32 shooterId, Group *parent ) :
    Munition( 50000, shooterId, 60.0f, parent ),

    m_elevation ( 0.0f )
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
            if ( m_pos.z() < m_elevation )
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
                float r2 = ( target->getPos() - m_pos ).length2() - target->getRadius2();

                if ( r2 < m_r_limit_2 )
                {
                    UInt16 dp = m_dp;

                    if ( r2 > 0.0f )
                    {
                        dp = (float)m_dp * ( 1.0f - r2 / m_r_limit_2 );
                    }

                    target->hit( dp, this );
                    reportTargetHit( target );
                }
            }
        }

        ++it;
    }

    Explosion *explosion = new Explosion( 15.0f );
    explosion->setPos( m_pos );
}

////////////////////////////////////////////////////////////////////////////////

void Bomb::updateElevation()
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

void Bomb::updateVelocity()
{
    if ( isTopLevel() )
    {
        float v = m_vel.length();

        if ( v < 200.0f )
        {
            Vec3 acc = m_att.inverse() * Vec3( 0.0, 0.0, -SIM_GRAVITY_ACC );
            m_vel += ( acc - ( m_omg ^ m_vel ) ) * m_timeStep;
        }

        float alpha = 0.0f;

        if ( v > 0.01 )
        {
            float vel_xy = sqrt( m_vel.x()*m_vel.x() + m_vel.y()*m_vel.y() );
            alpha = atan2( -m_vel.z(), vel_xy );
        }

        m_omg.x() = -m_angles.phi() / m_timeStep;
        m_omg.y() = -alpha / m_timeStep;
    }
}
