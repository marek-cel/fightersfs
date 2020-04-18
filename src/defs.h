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
#ifndef DEFS_H
#define DEFS_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Defs.h>
#include <sim/sim_Log.h>

////////////////////////////////////////////////////////////////////////////////

#define SIM_APP_NAME    "Fighters"
#define SIM_APP_VER     "1.0"
#define SIM_ORG_NAME    "Marek_Cel"
#define SIM_ORG_DOMAIN  "marekcel.pl"

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

typedef sim::Log Log;

////////////////////////////////////////////////////////////////////////////////

#endif // DEFS_H
