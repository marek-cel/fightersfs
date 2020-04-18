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
#ifndef SIM_FOGSCENE_H
#define SIM_FOGSCENE_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Fog>
#include <osg/Group>

#include <sim/cgi/sim_Module.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Fog scene module class.
 * <p>This is parent module for all fogged sub-modules.</p>
 */
class FogScene : public Module
{
public:

    static int m_visibility;    ///< [m]

    /** Constructor. */
    FogScene( Module *parent = 0 );

    /** Destructor. */
    virtual ~FogScene();

    /** Updates fog scene module. */
    void update();

private:

    osg::ref_ptr<osg::Fog> m_fog;   ///< fog
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_FOGSCENE_H
