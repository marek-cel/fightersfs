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

#include <sim/entities/sim_Entities.h>

#include <limits>

#include <sim/sim_Log.h>
#include <sim/entities/sim_Unit.h>
#include <sim/utils/sim_String.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Entities::Entities() :
    Group( 0 )
{}

////////////////////////////////////////////////////////////////////////////////

Entities::~Entities() {}

////////////////////////////////////////////////////////////////////////////////

Unit* Entities::getUnitByName( const std::string &name )
{
    List::iterator it = _children.begin();

    while ( it != _children.end() )
    {
        Unit *unit = dynamic_cast< Unit* >( *it );

        if ( unit )
        {
            if ( 0 == String::icompare( unit->getName(), name ) )
            {
                return unit;
            }
        }

        ++it;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

Unit* Entities::getOwnship()
{
    List::iterator it = _children.begin();

    while ( it != _children.end() )
    {
        Unit *unit = dynamic_cast< Unit* >( *it );

        if ( unit )
        {
            if ( unit->isOwnship() )
            {
                return unit;
            }
        }

        ++it;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void Entities::listAll()
{
    List::iterator it = _children.begin();

    while ( it != _children.end() )
    {
        Log::i() << (*it)->getName() << std::endl;
        ++it;
    }
}
