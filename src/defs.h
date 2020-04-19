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

#include <sim/sim_Defines.h>
#include <sim/sim_Log.h>
#include <sim/sim_Path.h>

#include <sim/utils/sim_Singleton.h>
#include <sim/utils/sim_Text.h>
#include <sim/utils/sim_XmlDoc.h>
#include <sim/utils/sim_XmlNode.h>
#include <sim/utils/sim_XmlUtils.h>

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

#ifndef THROW
#define THROW( e ) \
{ \
    e.setFile( __FILE__ ); \
    e.setLine( __LINE__ ); \
    throw e; \
}
#endif

////////////////////////////////////////////////////////////////////////////////

typedef unsigned char  UInt8;   ///< 8-bits unsigned integer type
typedef unsigned short UInt16;  ///< 16-bits unsigned integer type
typedef unsigned int   UInt32;  ///< 32-bits unsigned integer type

////////////////////////////////////////////////////////////////////////////////

typedef sim::Log  Log;
typedef sim::Path Path;
typedef sim::Text Text;

typedef sim::XmlDoc   XmlDoc;
typedef sim::XmlNode  XmlNode;
typedef sim::XmlUtils XmlUtils;

template < class TYPE >
class Singleton : public sim::Singleton< TYPE > {};

////////////////////////////////////////////////////////////////////////////////

#endif // DEFS_H
