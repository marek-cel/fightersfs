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
#ifndef SIM_OBJECTIVE_H
#define SIM_OBJECTIVE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Base.h>

#include <sim/utils/sim_XmlNode.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Mission objective base class. */
class Objective : public Base
{
public:

    /** Reads objective from XML file. */
    static Objective* read( const XmlNode &node );

    /** Constructor. */
    Objective() : m_status( Pending ) {}

    /** Destructor. */
    virtual ~Objective() {}

    /** Updates objective state. */
    virtual void update() = 0;

    inline Status getStatus() const { return m_status; }

protected:

    Status m_status;    ///< objective status
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OBJECTIVE_H
