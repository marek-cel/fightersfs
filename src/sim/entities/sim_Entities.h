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
 * <p>This singleton class should be used to access top level entities.</p>
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
