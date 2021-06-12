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
