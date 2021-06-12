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
#ifndef SIM_BUFFERQUEUEPLAYER_H
#define SIM_BUFFERQUEUEPLAYER_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string>

#include <sys/types.h>

#include <SLES/OpenSLES.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** OpenSLES buffer queue player class. */
class BufferQueuePlayer
{
public:

    /** Constructor. */
    BufferQueuePlayer( const std::string &bufferPath, bool looping = false );

    /** Destructor. */
    virtual ~BufferQueuePlayer();

    void onFinished();

    void play();
    void stop();

    SLBufferQueueItf getItfBufferQueue() { return m_itfBufferQueue; }

    void setPlaybackRate( float coef );

    void setVolume( float coef );

private:

    SLEngineItf m_soundEngine;              ///<
    SLObjectItf m_outputMixer;              ///<

    SLObjectItf m_player;                   ///<

    SLPlayItf         m_itfPlay;            ///<
    SLBufferQueueItf  m_itfBufferQueue;     ///<
    SLVolumeItf       m_itfVolume;          ///<
    SLPlaybackRateItf m_itfPlaybackRate;    ///<

    uint8_t *m_buffer;                      ///<
    off_t    m_bufferLength;                ///<

    bool m_playing;                         ///<
    bool m_looping;                         ///<

    void readBuffer( const std::string &bufferPath );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_BUFFERQUEUEPLAYER_H
