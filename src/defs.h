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
#ifndef DEFS_H
#define DEFS_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Defines.h>
#include <sim/sim_Log.h>
#include <sim/sim_Path.h>

#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_Singleton.h>
#include <sim/utils/sim_String.h>
#include <sim/utils/sim_Text.h>
#include <sim/utils/sim_XmlDoc.h>
#include <sim/utils/sim_XmlNode.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

#define SIM_APP_NAME   "FightersFS"
#define SIM_APP_VER    "1.0"
#define SIM_ORG_NAME   "Marek_Cel"
#define SIM_ORG_DOMAIN "marekcel.pl"

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

typedef sim::Convert Convert;
typedef sim::Log     Log;
typedef sim::Path    Path;
typedef sim::String  String;
typedef sim::Text    Text;

typedef sim::XmlDoc   XmlDoc;
typedef sim::XmlNode  XmlNode;
typedef sim::XmlUtils XmlUtils;

template < class TYPE >
class Singleton : public sim::Singleton< TYPE > {};

////////////////////////////////////////////////////////////////////////////////

#endif // DEFS_H
