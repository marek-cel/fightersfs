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
#ifndef SIM_ELEVATION_H
#define SIM_ELEVATION_H

////////////////////////////////////////////////////////////////////////////////

#include <string>

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Ground elevation class. */
class Elevation : public Singleton< Elevation >
{
    friend class Singleton< Elevation >;

private:

    /** @brief Constructor. */
    Elevation();

public:

    /** @brief Destructor. */
    virtual ~Elevation();

    /** @brief Returns terrain elevation at given coordinates. */
    float getElevation( float x, float y );

    /** @brief Reads ground elevation data file. */
    void readFile( const std::string &fileName );

    /** @brief Resets ground elevation data. */
    void reset();

private:

    bool _valid;    ///< specifies if elevation data is valid

    int _num;       ///< number of nodes along side

    float _half;    ///< [m] half size (maximum valid range)
    float _side;    ///< [m] side length
    float _step;    ///< [m] nodes step

    float *_elev;   ///< [m] ground elevation data array
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_ELEVATION_H
