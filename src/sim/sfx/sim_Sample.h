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
#ifndef SFX_SAMPLE_H
#define SFX_SAMPLE_H

////////////////////////////////////////////////////////////////////////////////

#include <string>

#include <AL/al.h>
#include <AL/alc.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** */
class Sample
{
public:

    /** Constructor. */
    Sample( const std::string &file, bool looping = false );

    /** Destructor. */
    virtual ~Sample();

    void play();

    void stop();

    void setLooping( bool looping );
    void setPitch( double pitch );
    void setVolume( double vol );

private:

    ALuint _source;         ///<
    ALuint _buffer;         ///<

    bool checkForErrors();

    bool loadFile( const std::string &file );
};

} // end of sim namepsace

////////////////////////////////////////////////////////////////////////////////

#endif // SFX_SAMPLE_H
