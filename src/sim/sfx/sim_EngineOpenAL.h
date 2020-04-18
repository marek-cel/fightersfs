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
#ifndef SIM_ENGINEOPENAL_H
#define SIM_ENGINEOPENAL_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/utils/sim_Singleton.h>

#include <sim/sfx/sim_Sample.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** OpenAL sound engine singleton class. */
class EngineOpenAL : public Singleton< EngineOpenAL >
{
    friend class Singleton< EngineOpenAL >;

private:

    /** Constructor. */
    EngineOpenAL();

public:

    /** Destructor. */
    virtual ~EngineOpenAL();

    /** Initializes sound engine. */
    void init();

    /** Stops sound engine. */
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
