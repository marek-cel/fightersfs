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

#include <sim/sfx/sim_Sample.h>

#include <cstring>

#include <osg/Notify>

#include <AL/alut.h>

#include <sim/sim_Log.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Sample::Sample( const std::string &file, bool looping )
{
    bool error = false;

    // source generation
    if ( !error )
    {
        alGenSources( (ALuint)1, &_source );
        error = checkForErrors();
    }

    if ( !error )
    {
        alSourcef( _source, AL_PITCH, 1);
        error = checkForErrors();
    }

    if ( !error )
    {
        alSourcef( _source, AL_GAIN, 1);
        error = checkForErrors();
    }

    if ( !error )
    {
        alSource3f( _source, AL_POSITION, 0, 0, 0);
        error = checkForErrors();
    }

    if ( !error )
    {
        alSource3f( _source, AL_VELOCITY, 0, 0, 0);
        error = checkForErrors();
    }

    if ( !error )
    {
        alSourcei( _source, AL_LOOPING, AL_FALSE );
        error = checkForErrors();
    }

    // buffer generation
    if ( !error )
    {
        alGenBuffers( (ALuint)1, &_buffer );
        error = checkForErrors();
    }

    if ( !error )
    {
        error = loadFile( file );
    }

    // binding source to buffer
    if ( !error )
    {
        alSourcei( _source, AL_BUFFER, _buffer );
        error = checkForErrors();
    }

    setLooping( looping );
}

////////////////////////////////////////////////////////////////////////////////

Sample::~Sample()
{
    alDeleteSources( 1, &_source );
    alDeleteBuffers( 1, &_buffer );
}

////////////////////////////////////////////////////////////////////////////////

void Sample::play()
{
    ALint state;

    alGetSourcei( _source, AL_SOURCE_STATE, &state );

    if ( state != AL_PLAYING )
    {
        alSourcePlay( _source );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Sample::stop()
{
    alSourceStop( _source );
}

////////////////////////////////////////////////////////////////////////////////

void Sample::setLooping( bool looping )
{
    alSourcei( _source, AL_LOOPING, ( looping ? AL_TRUE : AL_FALSE ) );
}

////////////////////////////////////////////////////////////////////////////////

void Sample::setPitch( double pitch )
{
    alSourcef( _source, AL_PITCH, std::max( 0.0, std::min( 1.0, pitch ) ) );
}

////////////////////////////////////////////////////////////////////////////////

void Sample::setVolume( double vol )
{
    alSourcef( _source, AL_GAIN, std::max( 0.0, std::min( 1.0, vol ) ) );
}


////////////////////////////////////////////////////////////////////////////////

bool Sample::checkForErrors()
{
    ALCenum error = alGetError();

    if ( error != AL_NO_ERROR )
    {
        std::string error_str = "";

        switch ( error )
        {
        case AL_INVALID_NAME:
            error_str = "AL_INVALID_NAME";
            break;

        case AL_INVALID_ENUM:
            error_str = "AL_INVALID_ENUM";
            break;

        case AL_INVALID_VALUE:
            error_str = "AL_INVALID_VALUE";
            break;

        case AL_INVALID_OPERATION:
            error_str = "AL_INVALID_OPERATION";
            break;

        case AL_OUT_OF_MEMORY:
            error_str = "AL_OUT_OF_MEMORY";
            break;

        default:
            error_str = "Unknown Error";
            break;
        }

        Log::e() << "SFX sample error: " << error_str << std::endl;

        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////

bool Sample::loadFile( const std::string &file )
{
    bool error = false;


    // loading audio stream to buffer
    ALenum format = 0;
    ALsizei size = 0;
    ALsizei freq = 0;

    ALvoid *data = nullptr;
    ALboolean loop = AL_FALSE;

    const size_t size_max = 4096;
    ALbyte file_temp[ size_max ];
    size_t size_str = ( file.size() < size_max ) ? ( file.size() + 1 ) : size_max;
    memcpy( file_temp, file.c_str(), size_str );

    alutLoadWAVFile( file_temp, &format, &data, &size, &freq, &loop );

    error = checkForErrors();

    // loading the raw audio stream into our buffer:
    if ( !error )
    {
        alBufferData( _buffer, format, data, size, freq );
        error = checkForErrors();
    }

    return error;
}
