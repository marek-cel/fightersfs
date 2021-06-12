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
