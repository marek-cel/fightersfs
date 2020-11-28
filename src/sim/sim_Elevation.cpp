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

#include <sim/sim_Elevation.h>

#include <math.h>
#include <stdio.h>

#include <sim/sim_Defines.h>
#include <sim/sim_Log.h>

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

    FILE *file = fopen( fileName.c_str(), "r" );

    if ( file )
    {
        int num = 0;

        double coef = 0.0;
        double step = 0.0;

        if ( 3 == fscanf( file, "%d,%lf,%lf", &num, &coef, &step ) )
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
                    int elev = 0;

                    if ( ic == 0 )
                    {
                        if ( 1 != fscanf( file, "\n%d" , &elev ) )
                        {
                            error = true;
                        }
                    }
                    else
                    {
                        if ( 1 != fscanf( file, ",%d"  , &elev ) )
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

        fclose( file );
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
