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

const std::string BomberTorpedo::_tagName = "bomber_torpedo";

const float BomberTorpedo::_dropAltMin  = 40.0f;
const float BomberTorpedo::_dropAltMax  = 60.0f;
const float BomberTorpedo::_dropDistMax = 600.0f;
const float BomberTorpedo::_tanMaxTht   = tan( SIM_AIRCRAFT_MAX_THT );

////////////////////////////////////////////////////////////////////////////////

BomberTorpedo::BomberTorpedo( Affiliation affiliation ) :
    Bomber( affiliation ),

    _attack ( false )
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
    if ( _attack )
    {
        if ( _altitude_agl < _dropAltMin )
        {
            tht += ( 1.0 - _altitude_agl / _dropAltMin ) * SIM_AIRCRAFT_MAX_THT;
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

    _attack = false;

    if ( m_engaged )
    {
        float dist_attack = m_target_dist - _dropDistMax;
        float dist_descent = m_target_alt / _tanMaxTht;

        if ( dist_attack < 2.0f * dist_descent )
        {
            UnitMarine *target = dynamic_cast< UnitMarine* >( m_target->getTarget() );

            if ( target )
            {
                _attack = true;

                Vec3 target_dir = m_target_pos - _pos;
                target_dir.z() = 0.0f;
                target_dir *= 1.0/target_dir.length();

                Vec3 drop_pos = _pos + target_dir * dist_attack;

                _destination.first  = Vec3( drop_pos.x(), drop_pos.y(), _dropAltMin );
                _destination.second = _speed_max;
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
        if ( m_target_alt < _dropAltMax
         && m_target_dist < _dropDistMax
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
        if ( _time_drop > 1.0f )
        {
            releaseWeapon();
        }
    }
}
