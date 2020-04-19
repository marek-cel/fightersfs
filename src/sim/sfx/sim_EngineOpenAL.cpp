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

#include <sim/sfx/sim_EngineOpenAL.h>

#include <osg/Notify>

#include <sim/sim_Base.h>
#include <sim/sim_Log.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

EngineOpenAL::EngineOpenAL() :
    _device ( nullptr ),
    _context ( nullptr ),

    _bombs     ( nullptr ),
    _crash     ( nullptr ),
    _engine    ( nullptr ),
    _gunfire   ( nullptr ),
    _heartbeat ( nullptr ),
    _hit       ( nullptr ),
    _waypoint  ( nullptr )
{
    _device = alcOpenDevice( NULL );

    if ( _device )
    {
        _context = alcCreateContext( _device, NULL );

        if ( !alcMakeContextCurrent( _context ) )
        {
            Log::e() << "Cannot create context." << std::endl;
        }
    }
    else
    {
        Log::e() << "Cannot open audio device." << std::endl;
    }

    _bombs     = new Sample( Base::getPath( "sfx/bombs.wav" ) );
    _crash     = new Sample( Base::getPath( "sfx/crash.wav" ) );
    _engine    = new Sample( Base::getPath( "sfx/engine.wav"    ), true );
    _gunfire   = new Sample( Base::getPath( "sfx/gunfire.wav"   ), true );
    _heartbeat = new Sample( Base::getPath( "sfx/heartbeat.wav" ), true );
    _hit       = new Sample( Base::getPath( "sfx/hit.wav" ) );
    _waypoint  = new Sample( Base::getPath( "sfx/waypoint.wav" ) );
}

////////////////////////////////////////////////////////////////////////////////

EngineOpenAL::~EngineOpenAL()
{
    DELPTR( _bombs );
    DELPTR( _crash );
    DELPTR( _engine );
    DELPTR( _gunfire );
    DELPTR( _heartbeat );
    DELPTR( _hit );
    DELPTR( _waypoint );

    alcMakeContextCurrent( NULL );
    alcDestroyContext( _context );
    alcCloseDevice( _device );
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::init() {}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::stop()
{
    _bombs     ->stop();
    _crash     ->stop();
    _engine    ->stop();
    _gunfire   ->stop();
    _heartbeat ->stop();
    _hit       ->stop();
    _waypoint  ->stop();
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::setBombs( bool play )
{
    if ( play )
        _bombs->play();
    else
        _bombs->stop();
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::setCrash( bool play )
{
    if ( play )
        _crash->play();
    else
        _crash->stop();
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::setEngine( bool play )
{
    if ( play )
        _engine->play();
    else
        _engine->stop();
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::setGunfire( bool play )
{
    if ( play )
        _gunfire->play();
    else
        _gunfire->stop();
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::setHeartbeat( bool play )
{
    if ( play )
        _heartbeat->play();
    else
        _heartbeat->stop();
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::setHit( bool play )
{
    if ( play )
        _hit->play();
    else
        _hit->stop();
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::setWaypoint( bool play )
{
    if ( play )
        _waypoint->play();
    else
        _waypoint->stop();
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenAL::setEngineRPM( float rpm )
{
    _engine->setPitch( rpm );
}
