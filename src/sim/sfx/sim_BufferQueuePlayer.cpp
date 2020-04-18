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
#include <fstream>
#include <math.h>
#include <vector>

#include <sim/sfx/sim_BufferQueuePlayer.h>
#include <sim/sfx/sim_EngineOpenSLES.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

BufferQueuePlayer::BufferQueuePlayer( const std::string &bufferPath, bool looping ) :
    m_soundEngine ( EngineOpenSLES::instance()->getSoundEngine() ),
    m_outputMixer ( EngineOpenSLES::instance()->getOutputMixer() ),

    m_player ( 0 ),

    m_itfPlay         ( 0 ),
    m_itfBufferQueue  ( 0 ),
    m_itfVolume       ( 0 ),
    m_itfPlaybackRate ( 0 ),

    m_buffer       ( 0 ),
    m_bufferLength ( 0 ),

    m_playing ( false ),
    m_looping ( looping )
{
    SLresult result;

    // configure audio source
    SLDataLocator_BufferQueue loc_bufq = { SL_DATALOCATOR_BUFFERQUEUE, 2 };
    SLDataFormat_PCM format_pcm = { SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_48,
                                    SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                    SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN };
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, m_outputMixer };
    SLDataSink audioSnk = { &loc_outmix, NULL };

    // create audio player
    const SLInterfaceID ids[ 3 ] = { SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_PLAYBACKRATE };
    const SLboolean     req[ 3 ] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    result = (*m_soundEngine)->CreateAudioPlayer( m_soundEngine, &m_player,
                                                  &audioSrc, &audioSnk,
                                                  3, ids, req );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // realize the player
    result = (*m_player)->Realize( m_player, SL_BOOLEAN_FALSE );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // get the play interface
    result = (*m_player)->GetInterface( m_player, SL_IID_PLAY, &m_itfPlay );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // get the volume interface
    result = (*m_player)->GetInterface( m_player, SL_IID_VOLUME, &m_itfVolume );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // get the playback rate interface
    result = (*m_player)->GetInterface( m_player, SL_IID_PLAYBACKRATE, &m_itfPlaybackRate );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // get the buffer queue interface
    result = (*m_player)->GetInterface( m_player, SL_IID_BUFFERQUEUE, &m_itfBufferQueue );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    // register callback on the buffer queue
    result = (*m_itfBufferQueue)->RegisterCallback( m_itfBufferQueue, EngineOpenSLES::bufferQueuePlayerlayerCallback, NULL );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;

    readBuffer( bufferPath );

    // enqueue buffer
    if ( m_bufferLength > 0 )
    {
        result = (*m_itfBufferQueue)->Enqueue( m_itfBufferQueue, m_buffer, m_bufferLength );
        assert( SL_RESULT_SUCCESS == result );
        (void)result;
    }
}

////////////////////////////////////////////////////////////////////////////////

BufferQueuePlayer::~BufferQueuePlayer()
{
    if ( m_player != NULL )
    {
        SLuint32 playerState;

        (*m_player)->GetState( m_player, &playerState );

        if ( playerState == SL_OBJECT_STATE_REALIZED )
        {
            (*m_itfPlay)->SetPlayState( m_itfPlay, SL_PLAYSTATE_STOPPED );
            (*m_player)->Destroy( m_player );

            m_player = 0;

            m_itfPlay        = 0;
            m_itfBufferQueue = 0;
            m_itfVolume      = 0;
        }
    }

    if ( m_buffer ) delete [] m_buffer; m_buffer = 0;
}

////////////////////////////////////////////////////////////////////////////////

void BufferQueuePlayer::onFinished()
{
    if ( m_looping )
    {
        if ( m_bufferLength > 0 )
        {
            SLresult result;

            result = (*m_itfBufferQueue)->Enqueue( m_itfBufferQueue, m_buffer, m_bufferLength );
            assert( SL_RESULT_SUCCESS == result );
            (void)result;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void BufferQueuePlayer::play()
{
    if ( !m_playing )
    {
        SLresult result;

        if ( m_player != NULL )
        {
            SLuint32 playerState;

            (*m_player)->GetState( m_player, &playerState );

            if ( playerState == SL_OBJECT_STATE_REALIZED )
            {
                SLuint32 playerPlayState;

                (*m_itfPlay)->GetPlayState( m_itfPlay, &playerPlayState );

                if ( playerPlayState != SL_PLAYSTATE_PLAYING )
                {
                    (*m_itfBufferQueue)->Clear( m_itfBufferQueue );

                    result = (*m_itfBufferQueue)->Enqueue( m_itfBufferQueue, m_buffer, m_bufferLength );
                    assert( SL_RESULT_SUCCESS == result );
                    (void)result;


                    result = (*m_itfPlay)->SetPlayState( m_itfPlay, SL_PLAYSTATE_PLAYING );
                    assert( SL_RESULT_SUCCESS == result );
                    (void)result;
                }

                m_playing = true;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void BufferQueuePlayer::stop()
{
    if ( m_playing )
    {
        SLresult result;

        if ( m_player != NULL )
        {
            SLuint32 playerState;

            (*m_player)->GetState( m_player, &playerState );

            if ( playerState == SL_OBJECT_STATE_REALIZED )
            {
                SLuint32 playerPlayState;

                (*m_itfPlay)->GetPlayState( m_itfPlay, &playerPlayState );

                if ( playerPlayState != SL_PLAYSTATE_STOPPED )
                {
                    (*m_itfPlay)->SetPlayState( m_itfPlay, SL_PLAYSTATE_STOPPED );
                }

                m_playing = false;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void BufferQueuePlayer::setPlaybackRate( float coef )
{
    SLresult result;

    SLpermille rate = 1000.0f * coef;

    result = (*m_itfPlaybackRate)->SetRate( m_itfPlaybackRate, rate );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;
}

////////////////////////////////////////////////////////////////////////////////

void BufferQueuePlayer::setVolume( float coef )
{
    SLresult result;

    SLmillibel vol = ( coef < 0.01f ) ? SL_MILLIBEL_MIN : 10000.0f * log10( coef );

    result = (*m_itfVolume)->SetVolumeLevel( m_itfVolume, vol );
    assert( SL_RESULT_SUCCESS == result );
    (void)result;
}

////////////////////////////////////////////////////////////////////////////////

void BufferQueuePlayer::readBuffer( const std::string &bufferPath )
{
    std::vector< uint8_t > data;
    std::fstream file;

    file.open( bufferPath.c_str(), std::ios::in | std::ios::binary );

    if ( file.is_open() )
    {
        while ( !file.eof() )
        {
            uint8_t temp = 0;
            file.read( (char*)&temp, 1 );
            data.push_back( temp );
        }
    }

    m_bufferLength = data.size();

    if ( data.size() > 0 )
    {
        m_buffer = new uint8_t[ data.size() ];

        for ( int i = 0; i < data.size(); i++ )
        {
            m_buffer[ i ] = data[ i ];
        }
    }
}
