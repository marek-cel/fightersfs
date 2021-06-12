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
#ifndef SIM_RANDOM_H
#define SIM_RANDOM_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Random number generator class. */
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
