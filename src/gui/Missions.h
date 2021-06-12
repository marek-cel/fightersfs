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
#ifndef MISSIONS_H
#define MISSIONS_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/utils/sim_Singleton.h>

#include <vector>

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Missions class.
 */
class Missions : public sim::Singleton< Missions >
{
    friend class Singleton< Missions >;

public:

    struct Mission
    {

    };

    typedef std::vector< std::vector< Mission > > Data;

    /** @brief Destructor. */
    virtual ~Missions();

    Mission getMission( int campaign, int mission );

private:

    Data _data;

    /**
     * You should use static function instance() due to get refernce
     * to Airports class instance.
     */
    Missions();

    /** Using this constructor is forbidden. */
    Missions( const Missions & ) : sim::Singleton< Missions > () {}
};

////////////////////////////////////////////////////////////////////////////////

#endif // MISSIONS_H
