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
#ifndef SIM_SFX_H
#define SIM_SFX_H

////////////////////////////////////////////////////////////////////////////////

#ifdef __ANDROID__
#   define SIM_SFX_OPENSLES
#else
#endif

#include <sim/sim_Data.h>

#include <sim/utils/sim_Inertia.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Sound effects class. */
class SFX
{
public:

    /** Constructor. */
    SFX();

    /** Destructor. */
    virtual ~SFX();

    /** Initializes sound effects. */
    void init();

    /** Pauses sounds replaying. */
    void pause();

    /** Unpauses sounds replaying. */
    void unpause();

    /** Stops sound effects. */
    void stop();

    /** Updates sound effects. */
    void update();

private:

    bool m_inited;              ///<
    bool m_paused;              ///<

    Inertia< float > *m_rpm;    ///< engine RPM inertia

    bool m_destroyed;           ///< ownship destroy flag (to play explosion only once)

    UInt8 m_ownship_hits;       ///< number of ownship hits

    float m_hit_time;           ///< [s] until when hit sound should be played
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_SFX_H
