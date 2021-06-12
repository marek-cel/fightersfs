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
