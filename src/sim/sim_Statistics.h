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
