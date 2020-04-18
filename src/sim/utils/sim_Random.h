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
#ifndef SIM_RANDOM_H
#define SIM_RANDOM_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Random number generator class. */
class Random : public Singleton< Random >
{
    friend class Singleton< Random >;

private:

    /**
     * You should use static function getInstance() due to get refernce
     * to Random class instance.
     */
    Random();

    /** Using this constructor is forbidden. */
    Random( const Random & ) : Singleton< Random >() {}

public:

    /**
     * @brief Returns random integer number within the given range.
     * @param min minimum value
     * @param max maximum value
     * @return random number
     */
    inline static int get( int min, int max )
    {
        return Random::instance()->getRandom( min, max );
    }

    /**
     * @brief Returns random float number within the given range.
     * @param min minimum value
     * @param max maximum value
     * @return random number
     */
    inline static float get( float min, float max )
    {
        return Random::instance()->getRandom( min, max );
    }

    /** Destructor. */
    virtual ~Random();

    /**
     * @brief Returns random integer number within the given range.
     * @param min minimum value
     * @param max maximum value
     * @return random number
     */
    int getRandom( int min, int max );

    /**
     * @brief Returns random float number within the given range.
     * @param min minimum value
     * @param max maximum value
     * @return random number
     */
    float getRandom( float min, float max );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_RANDOM_H
