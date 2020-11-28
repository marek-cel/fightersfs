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

#include <sim/entities/sim_BomberLevel.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const std::string BomberLevel::_tagName = "bomber_level";

////////////////////////////////////////////////////////////////////////////////

BomberLevel::BomberLevel( Affiliation affiliation ) : Bomber( affiliation ) {}

////////////////////////////////////////////////////////////////////////////////

BomberLevel::~BomberLevel() {}

////////////////////////////////////////////////////////////////////////////////

int BomberLevel::read( const XmlNode &node )
{
    //////////////////////////////////
    int result = Bomber::read( node );
    //////////////////////////////////

    if ( result == SIM_SUCCESS )
    {
        XmlNode nodeData = node.getFirstChildElement( "bomber_level" );
        readOrdnance( nodeData );

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void BomberLevel::updateWeapons()
{
    ////////////////////////
    Bomber::updateWeapons();
    ////////////////////////

    if ( m_engaged )
    {
        float drop_dist = ( _airspeed / SIM_GRAVITY_ACC ) * sqrt( 2.0f*SIM_GRAVITY_ACC*m_target_alt );

        if ( fabs( drop_dist - m_target_dist ) < 30.0f )
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
        if ( _time_drop > 0.2f )
        {
            releaseWeapon();
        }
    }
}
