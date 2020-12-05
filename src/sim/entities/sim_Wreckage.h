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
#ifndef SIM_WRECKAGE_H
#define SIM_WRECKAGE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Entity.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Wreckage effect base class. */
class Wreckage : public Entity
{
public:

    /** Constructor. */
    Wreckage( osg::Node *model, float life_span = std::numeric_limits< float >::max() );

    /** Destructor. */
    virtual ~Wreckage();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_WRECKAGE_H
