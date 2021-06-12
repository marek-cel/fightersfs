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
#ifndef SIM_ENTITIES_H
#define SIM_ENTITIES_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Unit.h>

#include <sim/utils/sim_Singleton.h>

#include <sim/entities/sim_Group.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Top level entities class.
 *
 * This singleton class should be used to access top level entities.
 */
class Entities : public Group, public Singleton< Entities >
{
    friend class Singleton< Entities >;

private:

    /** Constructor. */
    Entities();

    /** Constructor. */
    Entities( const Entities & ) : Group( 0 ), Singleton< Entities >() {}

public:

    /** Destructor. */
    virtual ~Entities();

    /**
     * Returns first unit of a given name if exists, otherwise returns 0.
     * @param name entity name
     */
    Unit* getUnitByName( const std::string &name );

    /** Returns ownship entity if exists, otherwise returns 0. */
    Unit* getOwnship();

    /** */
    void listAll();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_ENTITIES_H
