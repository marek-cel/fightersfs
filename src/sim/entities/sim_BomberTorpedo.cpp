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

#include <sim/entities/sim_BomberTorpedo.h>

#include <sim/entities/sim_UnitMarine.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const std::string BomberTorpedo::m_tagName = "bomber_torpedo";

const float BomberTorpedo::m_dropAltMin  = 40.0f;
const float BomberTorpedo::m_dropAltMax  = 60.0f;
const float BomberTorpedo::m_dropDistMax = 600.0f;
const float BomberTorpedo::m_tanMaxTht   = tan( SIM_AIRCRAFT_MAX_THT );

////////////////////////////////////////////////////////////////////////////////

BomberTorpedo::BomberTorpedo( Affiliation affiliation ) :
    Bomber( affiliation ),

    m_attack ( false )
{}

////////////////////////////////////////////////////////////////////////////////

BomberTorpedo::~BomberTorpedo() {}

////////////////////////////////////////////////////////////////////////////////

int BomberTorpedo::read( const XmlNode &node )
{
    //////////////////////////////////
    int result = Bomber::read( node );
    //////////////////////////////////

    if ( result == SIM_SUCCESS )
    {
        XmlNode nodeData = node.getFirstChildElement( "bomber_torpedo" );
        readOrdnance( nodeData );

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void BomberTorpedo::limitTht( float &tht )
{
    if ( m_attack )
    {
        if ( m_altitude_agl < m_dropAltMin )
        {
            tht += ( 1.0 - m_altitude_agl / m_dropAltMin ) * SIM_AIRCRAFT_MAX_THT;
        }

        if ( tht < -SIM_AIRCRAFT_MAX_THT ) tht = -SIM_AIRCRAFT_MAX_THT;
        if ( tht >  SIM_AIRCRAFT_MAX_THT ) tht =  SIM_AIRCRAFT_MAX_THT;
    }
    else
    {
        ////////////////////////
        Bomber::limitTht( tht );
        ////////////////////////
    }
}

////////////////////////////////////////////////////////////////////////////////

void BomberTorpedo::updateTarget()
{
    ///////////////////////
    Bomber::updateTarget();
    ///////////////////////

    m_attack = false;

    if ( m_engaged )
    {
        float dist_attack = m_target_dist - m_dropDistMax;
        float dist_descent = m_target_alt / m_tanMaxTht;

        if ( dist_attack < 2.0f * dist_descent )
        {
            UnitMarine *target = dynamic_cast< UnitMarine* >( m_target->getTarget() );

            if ( target )
            {
                m_attack = true;

                Vec3 target_dir = m_target_pos - m_pos;
                target_dir.z() = 0.0f;
                target_dir *= 1.0/target_dir.length();

                Vec3 drop_pos = m_pos + target_dir * dist_attack;

                m_destination.first  = Vec3( drop_pos.x(), drop_pos.y(), m_dropAltMin );
                m_destination.second = m_speed_max;
            }
            else
            {
                m_target->setTarget( 0 );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void BomberTorpedo::updateWeapons()
{
    ////////////////////////
    Bomber::updateWeapons();
    ////////////////////////

    if ( m_engaged )
    {
        if ( m_target_alt < m_dropAltMax
         && m_target_dist < m_dropDistMax
         && fabs( m_target_psi ) < 0.05f )
        {
            m_trigger = true;
        }
    }
    else
    {
        m_trigger = false;
    }

    if ( m_trigger && m_ordnanceIndex < m_ordnance.size() )
    {
        if ( m_time_drop > 1.0f )
        {
            releaseWeapon();
        }
    }
}
