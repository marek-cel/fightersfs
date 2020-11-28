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
#ifndef SIM_KAMIKAZE_H
#define SIM_KAMIKAZE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Target.h>

#include <sim/entities/sim_Aircraft.h>
#include <sim/entities/sim_UnitMarine.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Kamikaze class. */
class Kamikaze : public Aircraft
{
public:

    static const std::string _tagName;  ///< unit tag name

    /** Constructor. */
    Kamikaze( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~Kamikaze();

    /** Overloaded due to extra demage from kamikaze when collide. */
    virtual void collide( Unit *unit );

    /** Updates entity. */
    virtual void update( double timeStep );

    /** Returns aircraft target unit pointer. */
    virtual UnitMarine* getTarget() const;

protected:

    Target< UnitMarine > *m_target;     ///< target

    bool m_engaged;                     ///< specifies if aircraft is engaged into kamikaze attack

    virtual void limitTht( float &tht );

    virtual void updateDestination();
    virtual void updateTarget();

};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_KAMIKAZE_H
