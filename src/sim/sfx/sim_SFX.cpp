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

#include <sim/sfx/sim_SFX.h>

#ifdef SIM_SFX_OPENSLES
#   include <sim/sfx/sim_EngineOpenSLES.h>
#else
#   include <sim/sfx/sim_EngineOpenAL.h>
#endif

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

SFX::SFX() :
    _inited ( false ),
    _paused ( false ),
    _rpm ( 0 ),
    _destroyed ( false ),
    _ownship_hits ( 0 ),
    _hit_time ( 0.0f )
{
    _rpm = new Inertia< float >( 0.5f, 0.85f );
}

////////////////////////////////////////////////////////////////////////////////

SFX::~SFX()
{
    stop();

    DELPTR( _rpm );
}

////////////////////////////////////////////////////////////////////////////////

void SFX::init()
{
    if ( !_inited )
    {
#       ifdef SIM_SFX_OPENSLES
        EngineOpenSLES::instance()->init();
#       else
        EngineOpenAL::instance()->init();
#       endif

        _inited = true;
        _paused = true;
    }
}

////////////////////////////////////////////////////////////////////////////////

void SFX::pause()
{
    _paused = true;

#   ifdef SIM_SFX_OPENSLES
    EngineOpenSLES::instance()->setPlayerCrash     ( false );
    EngineOpenSLES::instance()->setPlayerEngine    ( false );
    EngineOpenSLES::instance()->setPlayerGunfire   ( false );
    EngineOpenSLES::instance()->setPlayerHeartbeat ( false );
    EngineOpenSLES::instance()->setPlayerHit       ( false );
    EngineOpenSLES::instance()->setPlayerWaypoint  ( false );
#   else
    EngineOpenAL::instance()->setCrash     ( false );
    EngineOpenAL::instance()->setEngine    ( false );
    EngineOpenAL::instance()->setGunfire   ( false );
    EngineOpenAL::instance()->setHeartbeat ( false );
    EngineOpenAL::instance()->setHit       ( false );
    EngineOpenAL::instance()->setWaypoint  ( false );
#   endif
}

////////////////////////////////////////////////////////////////////////////////

void SFX::unpause()
{
    _paused = false;
}

////////////////////////////////////////////////////////////////////////////////

void SFX::stop()
{
    if ( _inited )
    {
#       ifdef SIM_SFX_OPENSLES
        EngineOpenSLES::instance()->stop();
#       else
        EngineOpenAL::instance()->stop();
#       endif

        _inited = false;
    }
}

////////////////////////////////////////////////////////////////////////////////

void SFX::update()
{
    _rpm->update( 0.016f, 0.7f + 0.3f * Data::get()->controls.throttle );

#   ifdef SIM_SFX_OPENSLES
    if ( !_paused )
    {
        if ( !Data::get()->ownship.destroyed )
        {
            // engine
            EngineOpenSLES::instance()->setPlayerEngine( true );
            EngineOpenSLES::instance()->setPlayerEngineRPM( _rpm->getValue() );

            // gunfire
            EngineOpenSLES::instance()->setPlayerGunfire( Data::get()->ownship.gunfire );

            // hearbeat
            EngineOpenSLES::instance()->setPlayerHeartbeat( Data::get()->ownship.hit_points < 25 );

            // bombs
            EngineOpenSLES::instance()->setPlayerBombs( Data::get()->ownship.bombs_drop < 5.0f );

            // hit
            EngineOpenSLES::instance()->setPlayerHit( Data::get()->ownship.ownship_hit < 0.5f );

            // waypoint
            EngineOpenSLES::instance()->setPlayerWaypoint( Data::get()->ownship.waypoint_time < 1.0 );
        }
        else
        {
            // crash
            if ( !_destroyed )
            {
                EngineOpenSLES::instance()->setPlayerCrash( true );
            }

            EngineOpenSLES::instance()->setPlayerEngine    ( false );
            EngineOpenSLES::instance()->setPlayerGunfire   ( false );
            EngineOpenSLES::instance()->setPlayerHeartbeat ( false );
            EngineOpenSLES::instance()->setPlayerHit       ( false );
            EngineOpenSLES::instance()->setPlayerWaypoint  ( false );
        }
    }
#   else
    if ( !_paused )
    {
        if ( !Data::get()->ownship.destroyed )
        {
            // engine
            EngineOpenAL::instance()->setEngine( true );
            EngineOpenAL::instance()->setEngineRPM( _rpm->getValue() );

            // gunfire
            EngineOpenAL::instance()->setGunfire( Data::get()->ownship.gunfire );

            // hearbeat
            EngineOpenAL::instance()->setHeartbeat( Data::get()->ownship.hit_points < 25 );

            // bombs
            EngineOpenAL::instance()->setBombs( Data::get()->ownship.bombs_drop < 5.0f );

            // hit
            EngineOpenAL::instance()->setHit( Data::get()->ownship.ownship_hit < 0.5f );

            // waypoint
            EngineOpenAL::instance()->setWaypoint( Data::get()->ownship.waypoint_time < 1.0 );
        }
        else
        {
            // crash
            if ( !_destroyed )
            {
                EngineOpenAL::instance()->setCrash( true );
            }

            EngineOpenAL::instance()->setEngine( false );
            EngineOpenAL::instance()->setGunfire( false );
            EngineOpenAL::instance()->setHeartbeat( false );
            EngineOpenAL::instance()->setHit( false );
            EngineOpenAL::instance()->setWaypoint( false );
        }
    }
#   endif

    _destroyed = Data::get()->ownship.destroyed;
}
