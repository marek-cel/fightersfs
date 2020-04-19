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
#ifndef SIM_DEFINES_H
#define SIM_DEFINES_H

////////////////////////////////////////////////////////////////////////////////

#define SIM_FAILURE 0
#define SIM_SUCCESS 1

////////////////////////////////////////////////////////////////////////////////

#define SIM_TIME_STEP 0.0166667
#define SIM_TIME_STEP_MIN 0.001
#define SIM_TIME_STEP_MAX 0.200

#define SIM_END_DELAY 5.0f

////////////////////////////////////////////////////////////////////////////////

#define SIM_ENTITIES_MAX 2048

////////////////////////////////////////////////////////////////////////////////

#define SIM_GRAVITY_ACC 9.81f

////////////////////////////////////////////////////////////////////////////////

#define SIM_AIRCRAFT_MAX_PHI ( 72.0 * M_PI / 180.0 )
#define SIM_AIRCRAFT_MAX_THT ( 20.0 * M_PI / 180.0 )

////////////////////////////////////////////////////////////////////////////////

#define SIM_DEPTH_SORTED_BIN_WORLD  0
#define SIM_DEPTH_SORTED_BIN_SKY    1
#define SIM_DEPTH_SORTED_BIN_OTHER  2
#define SIM_DEPTH_SORTED_BIN_TRAIL  2
#define SIM_DEPTH_SORTED_BIN_HUD    3
#define SIM_DEPTH_SORTED_BIN_MSG    5 /* This is intentional! */

////////////////////////////////////////////////////////////////////////////////

#define SIM_SKYDOME_RAD 8000.0f

////////////////////////////////////////////////////////////////////////////////

#define SIM_LIGHT_SUN_NUM 0

////////////////////////////////////////////////////////////////////////////////

#define SIM_MSG_LEN 2048

////////////////////////////////////////////////////////////////////////////////

#ifndef NULLPTR
#   if __cplusplus >= 201103L
#       define NULLPTR nullptr
#   else
#       define NULLPTR 0
#   endif
#endif

////////////////////////////////////////////////////////////////////////////////

#ifndef DELPTR
#define DELPTR( ptr ) \
{ \
    if ( ptr ) delete ptr; \
    ptr = NULLPTR; \
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifndef DELTAB
#define DELTAB( ptr ) \
{ \
    if ( ptr ) delete [] ptr; \
    ptr = NULLPTR; \
}
#endif

////////////////////////////////////////////////////////////////////////////////

#define THROW( e ) \
{ \
    e.setFile( __FILE__ ); \
    e.setLine( __LINE__ ); \
    throw e; \
}

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_DEFINES_H
