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

const std::string Kamikaze::_tagName = "kamikaze";

////////////////////////////////////////////////////////////////////////////////

Kamikaze::Kamikaze( Affiliation affiliation ) :
    Aircraft( affiliation ),

    _target ( 0 ),
    _engaged ( false )
{
    _target = new Target< UnitMarine >( this, ( _affiliation == Hostile ) ? Friend : Hostile );
}

////////////////////////////////////////////////////////////////////////////////

Kamikaze::~Kamikaze()
{
    DELPTR( _target );
}

////////////////////////////////////////////////////////////////////////////////

void Kamikaze::collide( Unit *unit )
{
    unit->damage( 65535 );
}

////////////////////////////////////////////////////////////////////////////////

void Kamikaze::update( double timeStep )
{
    if ( isActive() ) _target->update();

    /////////////////////////////
    Aircraft::update( timeStep );
    /////////////////////////////

    if ( isActive() )
    {
        if ( !_target->getTarget() )
        {
            _target->findForward( M_PI_2 );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

UnitMarine *Kamikaze::getTarget() const
{
    return _target->getTarget();
}

////////////////////////////////////////////////////////////////////////////////

void Kamikaze::limitTht( float &tht )
{
    if ( _engaged )
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

    if ( !_engaged )
    {
        if ( _route.size() > 0 )
        {
            if ( !_enroute || _engaged )
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
        _engaged = false;
    }

    //////////////////////////////
    Aircraft::updateDestination();
    //////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void Kamikaze::updateTarget()
{
    UnitMarine *target = _target->getTarget();

    if ( target )
    {
        _enroute = false;
        _wingman = false;
        _engaged = true;

        _destValid = true;

        Vec3 pos_enu = target->getPos();
        pos_enu.z() = 5.0f;

        _destination.first  = pos_enu;
        _destination.second = _speed_max;
    }
    else
    {
        _engaged = false;
    }
}

