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

/** @brief Sound effects class. */
class SFX
{
public:

    /** @brief Constructor. */
    SFX();

    /** @brief Destructor. */
    virtual ~SFX();

    /** @brief Initializes sound effects. */
    void init();

    /** @brief Pauses sounds replaying. */
    void pause();

    /** @brief Unpauses sounds replaying. */
    void unpause();

    /** @brief Stops sound effects. */
    void stop();

    /** @brief Updates sound effects. */
    void update();

private:

    bool _inited;               ///<
    bool _paused;               ///<

    Inertia< float > *_rpm;     ///< engine RPM inertia

    bool _destroyed;            ///< ownship destroy flag (to play explosion only once)

    UInt8 _ownship_hits;        ///< number of ownship hits

    float _hit_time;            ///< [s] until when hit sound should be played
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_SFX_H
