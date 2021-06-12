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
#ifndef SIM_ENGINEOPENAL_H
#define SIM_ENGINEOPENAL_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/utils/sim_Singleton.h>

#include <sim/sfx/sim_Sample.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief OpenAL sound engine singleton class. */
class EngineOpenAL : public Singleton< EngineOpenAL >
{
    friend class Singleton< EngineOpenAL >;

private:

    /** @brief Constructor. */
    EngineOpenAL();

public:

    /** @brief Destructor. */
    virtual ~EngineOpenAL();

    /** @brief Initializes sound engine. */
    void init();

    /** @brief Stops sound engine. */
    void stop();

    void setBombs     ( bool play );
    void setCrash     ( bool play );
    void setEngine    ( bool play );
    void setGunfire   ( bool play );
    void setHeartbeat ( bool play );
    void setHit       ( bool play );
    void setWaypoint  ( bool play );

    void setEngineRPM( float rpm );

private:

    ALCdevice  *_device;        ///<
    ALCcontext *_context;       ///<

    Sample *_bombs;             ///<
    Sample *_crash;             ///<
    Sample *_engine;            ///<
    Sample *_gunfire;           ///<
    Sample *_heartbeat;         ///<
    Sample *_hit;               ///<
    Sample *_waypoint;          ///<
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_ENGINEOPENAL_H
