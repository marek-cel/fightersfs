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

#include <assert.h>

#include <sim/sim_Base.h>

#include <sim/sfx/sim_EngineOpenSLES.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

void EngineOpenSLES::bufferQueuePlayerlayerCallback( SLBufferQueueItf buff, void *context )
{
    BufferQueuePlayer *player = 0;

    if ( buff == instance()->m_playerEngine->getItfBufferQueue() )
    {
        player = instance()->m_playerEngine;
    }
    else if ( buff == instance()->m_playerGunfire->getItfBufferQueue() )
    {
        player = instance()->m_playerGunfire;
    }
    else if ( buff == instance()->m_playerHeartbeat->getItfBufferQueue() )
    {
        player = instance()->m_playerHeartbeat;
    }
    else if ( buff == instance()->m_playerHit->getItfBufferQueue() )
    {
        player = instance()->m_playerHit;
    }

    if ( player )
    {
        player->onFinished();
    }
}

////////////////////////////////////////////////////////////////////////////////

EngineOpenSLES::EngineOpenSLES() :
    m_soundObject ( 0 ),
    m_soundEngine ( 0 ),
    m_outputMixer ( 0 ),

    m_playerBombs     ( 0 ),
    m_playerCrash     ( 0 ),
    m_playerEngine    ( 0 ),
    m_playerGunfire   ( 0 ),
    m_playerHeartbeat ( 0 ),
    m_playerHit       ( 0 ),
    m_playerWaypoint  ( 0 )
{}

////////////////////////////////////////////////////////////////////////////////

EngineOpenSLES::~EngineOpenSLES()
{
    stop();
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenSLES::init()
{
    SLresult result;

    // create engine
    result = slCreateEngine( &m_soundObject, 0, NULL, 0, NULL, NULL );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // realize the engine
    result = (*m_soundObject)->Realize( m_soundObject, SL_BOOLEAN_FALSE );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // get the engine interface, which is needed in order to create other objects
    result = (*m_soundObject)->GetInterface( m_soundObject, SL_IID_ENGINE, &m_soundEngine );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[ 1 ] = { SL_IID_ENVIRONMENTALREVERB };
    const SLboolean     req[ 1 ] = { SL_BOOLEAN_FALSE };
    result = (*m_soundEngine)->CreateOutputMix( m_soundEngine, &m_outputMixer, 1, ids, req );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // realize the output mix
    result = (*m_outputMixer)->Realize( m_outputMixer, SL_BOOLEAN_FALSE );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // create players
    if ( m_playerBombs     == 0 ) m_playerBombs     = new BufferQueuePlayer( Base::getPath( "sfx/bombs.raw"     ) );
    if ( m_playerCrash     == 0 ) m_playerCrash     = new BufferQueuePlayer( Base::getPath( "sfx/crash.raw"     ) );
    if ( m_playerEngine    == 0 ) m_playerEngine    = new BufferQueuePlayer( Base::getPath( "sfx/engine.raw"    ), true );
    if ( m_playerGunfire   == 0 ) m_playerGunfire   = new BufferQueuePlayer( Base::getPath( "sfx/gunfire.raw"   ), true );
    if ( m_playerHeartbeat == 0 ) m_playerHeartbeat = new BufferQueuePlayer( Base::getPath( "sfx/heartbeat.raw" ), true );
    if ( m_playerHit       == 0 ) m_playerHit       = new BufferQueuePlayer( Base::getPath( "sfx/hit.raw"       ), true );
    if ( m_playerWaypoint  == 0 ) m_playerWaypoint  = new BufferQueuePlayer( Base::getPath( "sfx/waypoint.raw"  ) );
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenSLES::stop()
{
    // delete players
    if ( m_playerBombs     ) delete m_playerBombs;     m_playerBombs     = 0;
    if ( m_playerCrash     ) delete m_playerCrash;     m_playerCrash     = 0;
    if ( m_playerEngine    ) delete m_playerEngine;    m_playerEngine    = 0;
    if ( m_playerGunfire   ) delete m_playerGunfire;   m_playerGunfire   = 0;
    if ( m_playerHeartbeat ) delete m_playerHeartbeat; m_playerHeartbeat = 0;
    if ( m_playerHit       ) delete m_playerHit;       m_playerHit       = 0;
    if ( m_playerWaypoint  ) delete m_playerWaypoint;  m_playerWaypoint  = 0;

    // destroy output mix object, and invalidate all associated interfaces
    if ( m_outputMixer != NULL )
    {
        (*m_outputMixer)->Destroy( m_outputMixer );
        m_outputMixer = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if ( m_soundObject != NULL )
    {
        (*m_soundObject)->Destroy( m_soundObject );
        m_soundObject = NULL;
        m_soundEngine = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////

void EngineOpenSLES::setPlayer( bool play, BufferQueuePlayer *player )
{
    if ( player )
    {
        if ( play )
            player->play();
        else
            player->stop();
    }
}
