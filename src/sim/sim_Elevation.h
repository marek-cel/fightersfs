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

/** Ground elevation class. */
class Elevation : public Singleton< Elevation >
{
    friend class Singleton< Elevation >;

private:

    /** Constructor. */
    Elevation();

public:

    /** Destructor. */
    virtual ~Elevation();

    /** Returns terrain elevation at given coordinates. */
    float getElevation( float x, float y );

    /** Reads ground elevation data file. */
    void readFile( const std::string &fileName );

    /** Resets ground elevation data. */
    void reset();

private:

    bool m_valid;   ///< specifies if elevation data is valid

    int m_num;      ///< number of nodes along side

    float m_half;   ///< [m] half size (maximum valid range)
    float m_side;   ///< [m] side length
    float m_step;   ///< [m] nodes step

    float *m_elev;  ///< [m] ground elevation data array
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_ELEVATION_H
