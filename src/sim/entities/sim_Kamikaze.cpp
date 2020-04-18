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

#include <sim/entities/sim_Kamikaze.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const std::string Kamikaze::m_tagName = "kamikaze";

////////////////////////////////////////////////////////////////////////////////

Kamikaze::Kamikaze( Affiliation affiliation ) :
    Aircraft( affiliation ),

    m_target ( 0 ),
    m_engaged ( false )
{
    m_target = new Target< UnitMarine >( this, ( m_affiliation == Hostile ) ? Friend : Hostile );
}

////////////////////////////////////////////////////////////////////////////////

Kamikaze::~Kamikaze()
{
    if ( m_target ) delete m_target;
    m_target = 0;
}

////////////////////////////////////////////////////////////////////////////////

void Kamikaze::collide( Unit *unit )
{
    unit->damage( 65535 );
}

////////////////////////////////////////////////////////////////////////////////

void Kamikaze::update( double timeStep )
{
    if ( isActive() ) m_target->update();

    /////////////////////////////
    Aircraft::update( timeStep );
    /////////////////////////////

    if ( isActive() )
    {
        if ( !m_target->getTarget() )
        {
            m_target->findForward( M_PI_2 );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

UnitMarine *Kamikaze::getTarget() const
{
    return m_target->getTarget();
}

////////////////////////////////////////////////////////////////////////////////

void Kamikaze::limitTht( float &tht )
{
    if ( m_engaged )
    {
        if ( tht < -SIM_AIRCRAFT_MAX_THT ) tht = -SIM_AIRCRAFT_MAX_THT;
        if ( tht >  SIM_AIRCRAFT_MAX_THT ) tht =  SIM_AIRCRAFT_MAX_THT;
    }
    else
    {
        //////////////////////////
        Aircraft::limitTht( tht );
        //////////////////////////
    }
}

////////////////////////////////////////////////////////////////////////////////

void Kamikaze::updateDestination()
{
    updateTarget();

    if ( !m_engaged )
    {
        if ( m_route.size() > 0 )
        {
            if ( !m_enroute || m_engaged )
            {
                m_destination = m_route[ m_waypointIndex ];
                m_destValid = true;
            }
        }
        else
        {
            m_destValid = false;
        }

        m_enroute = true;
        m_wingman = m_leaderValid;
        m_engaged = false;
    }

    //////////////////////////////
    Aircraft::updateDestination();
    //////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void Kamikaze::updateTarget()
{
    UnitMarine *target = m_target->getTarget();

    if ( target )
    {
        m_enroute = false;
        m_wingman = false;
        m_engaged = true;

        m_destValid = true;

        Vec3 pos_enu = target->getPos();
        pos_enu.z() = 5.0f;

        m_destination.first  = pos_enu;
        m_destination.second = m_speed_max;
    }
    else
    {
        m_engaged = false;
    }
}

