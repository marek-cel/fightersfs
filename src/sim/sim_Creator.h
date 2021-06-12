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
#ifndef SIM_CREATOR_H
#define SIM_CREATOR_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Aircraft.h>
#include <sim/missions/sim_Mission.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** */
class Creator
{
public:

    /**
     * @brief Creates aircraft.
     * @param index
     * @param livery
     * @param affiliation
     * @return aircraft object
     */
    static Aircraft* createAircraft( int index, int livery, Affiliation affiliation );

    /**
     * @brief Creates unit due to unit type and file.
     * @param type unit type
     * @param file unit file
     * @return unit object
     */
    static Unit* createUnit( const std::string &type, const std::string &file,
                             Affiliation affiliation );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_CREATOR_H
