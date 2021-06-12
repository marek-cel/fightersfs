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
#ifndef SIM_STATISTICS_H
#define SIM_STATISTICS_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Types.h>

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Mission statistics class. */
class Statistics : public Singleton< Statistics >
{
    friend class Singleton< Statistics >;

private:

    /** Constructor. */
    Statistics();

public:

    /** @brief Destructor. */
    virtual ~Statistics();

    inline void update( float realTime )
    {
        _flightTime += realTime;
    }

    /** @brief Resets statistics data. */
    void reset();

    /** @brief This function should be called whenever unit is activated. */
    void reportActivated( Affiliation affiliation );

    /** @brief This function should be called whenever unit is destroyed. */
    void reportDestroyed( Affiliation affiliation );

    /** */
    inline void reportOwnshipDestroyed()
    {
        _ownshipDestroyed++;
    }

    inline UInt16 getActivated() const { return _activated; }
    inline UInt16 getDestroyed() const { return _destroyed; }

    inline UInt16 getFriendsActivated() const { return _friendsActivated; }
    inline UInt16 getFriendsDestroyed() const { return _friendsDestroyed; }
    inline UInt16 getEnemiesActivated() const { return _enemiesActivated; }
    inline UInt16 getEnemiesDestroyed() const { return _enemiesDestroyed; }
    inline UInt16 getOwnshipDestroyed() const { return _ownshipDestroyed; }

    inline UInt16 getFlightTime() const { return (UInt16)_flightTime; }

private:

    UInt16 _activated;          ///< total number of activated units
    UInt16 _destroyed;          ///< total number of destroyed units

    UInt16 _friendsActivated;   ///< number of activated friend units
    UInt16 _friendsDestroyed;   ///< number of destroyed friend units
    UInt16 _enemiesActivated;   ///< number of activated enemy units
    UInt16 _enemiesDestroyed;   ///< number of destroyed enemy units
    UInt16 _ownshipDestroyed;   ///< number of units destroyed by ownship

    float _flightTime;          ///< [s] total flight time
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_STATISTICS_H
