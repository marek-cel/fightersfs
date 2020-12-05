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
#ifndef SIM_UNITMARINE_H
#define SIM_UNITMARINE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_UnitSurface.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Marine unit base class. */
class UnitMarine : public UnitSurface
{
public:

#   ifdef SIM_DESKTOP
    static const char _frag[];      ///<
    static const char _vert[];      ///<

    static void createReflection( osg::Node *model, osg::Group *parent );
#   endif

    /** Constructor. */
    UnitMarine( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~UnitMarine();

    /** Destroys unit. */
    virtual void destroy();

    /** Loads unit (models, textures, etc.). */
    virtual void load();

    /** Sets unit hit points. */
    virtual void setHP( UInt16 hp );

protected:

    osg::ref_ptr<osg::Group> _smoke;    ///< damaged unit smoke group
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_UNITMARINE_H
