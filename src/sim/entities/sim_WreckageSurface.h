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
#ifndef SIM_WRECKAGESURFACE_H
#define SIM_WRECKAGESURFACE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Wreckage.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Surface unit wreckage effect class. */
class WreckageSurface : public Wreckage
{
public:

    /** Constructor. */
    WreckageSurface( osg::Node *model, osg::Group* smoke = 0 );

    /** Destructor. */
    virtual ~WreckageSurface();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_WRECKAGESURFACE_H
