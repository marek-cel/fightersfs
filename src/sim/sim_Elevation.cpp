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

#include <sim/sim_Elevation.h>

#include <cmath>
#include <fstream>
#include <limits>

#include <sim/sim_Defines.h>
#include <sim/sim_Log.h>

#include <sim/utils/sim_Misc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Elevation::Elevation() :
    _valid ( false ),

    _num ( 0 ),

    _half ( 0.0f ),
    _side ( 0.0f ),
    _step ( 0.0f ),

    _elev ( 0 )
{
    reset();
}

////////////////////////////////////////////////////////////////////////////////

Elevation::~Elevation()
{
    reset();
}

////////////////////////////////////////////////////////////////////////////////

float Elevation::getElevation( float x, float y )
{
    if ( _valid && fabs( x ) < _half && fabs( y ) < _half )
    {
        float xn = ( x + _half ) / _step;
        float yn = ( y + _half ) / _step;

        int ix_0 = floor( xn );
        int ix_1 =  ceil( xn );
        int iy_0 = floor( yn );
        int iy_1 =  ceil( yn );

        float dx = xn - ix_0;
        float dy = yn - iy_0;

        float z_x0_y0 = _elev[ ix_0 * _num + iy_0 ];
        float z_x0_y1 = _elev[ ix_0 * _num + iy_1 ];
        float z_x1_y0 = _elev[ ix_1 * _num + iy_0 ];
        float z_x1_y1 = _elev[ ix_1 * _num + iy_1 ];

        float z_y0 = z_x0_y0 + dx * ( z_x1_y0 - z_x0_y0 );
        float z_y1 = z_x0_y1 + dx * ( z_x1_y1 - z_x0_y1 );

        return dy * ( z_y1 - z_y0 ) + z_y0;
    }

    return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

void Elevation::readFile( const std::string &fileName )
{
    bool error = false;

    reset();

    std::fstream fs( fileName.c_str(), std::ios_base::in );

    if ( fs.is_open() )
    {
        int num = std::numeric_limits< int >::quiet_NaN();

        double coef = std::numeric_limits< double >::quiet_NaN();
        double step = std::numeric_limits< double >::quiet_NaN();

        fs >> num;
        fs >> coef;
        fs >> step;

        if ( Misc::isValid( num ) && Misc::isValid( coef ) && Misc::isValid( step ) )
        {
            if ( num > 0 && coef > 0.0 && step > 0.0 )
            {
                _num = num;

                _step = step;
                _side = (float)( _num - 1 ) * _step;
                _half = _side / 2.0f;

                _elev = new float [ _num * _num ];
            }
            else
            {
                error = true;
            }

            for ( int ir = 0; ir < _num && !error; ir++ )
            {
                for ( int ic = 0; ic < _num && !error; ic++ )
                {
                    int elev = std::numeric_limits< int >::quiet_NaN();
                    char separator;

                    if ( ic == 0 )
                    {
                        fs >> elev;

                        if ( Misc::isValid( elev ) )
                        {
                            error = true;
                        }
                    }
                    else
                    {
                        fs >> separator;
                        fs >> elev;

                        if ( Misc::isValid( elev ) )
                        {
                            error = true;
                        }
                    }

                    _elev[ ir * _num + ic ] = (float)elev * coef;
                }
            }
        }
        else
        {
            error = true;
        }

        fs.close();
    }
    else
    {
        Log::e() << "Cannot open elevation file: " << fileName << std::endl;

        error = true;
    }

    _valid = !error;

    if ( error )
    {
        reset();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Elevation::reset()
{
    _valid = false;

    _num = 0;

    _half = 0.0f;
    _side = 0.0f;
    _step = 0.0f;

    DELTAB( _elev );
}
