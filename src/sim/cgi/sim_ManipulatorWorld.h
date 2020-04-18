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
#ifndef SIM_MANIPULATORWORLD_H
#define SIM_MANIPULATORWORLD_H

////////////////////////////////////////////////////////////////////////////////

#include <osgGA/TerrainManipulator>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** World view camera manipulator class. */
class ManipulatorWorld : public osgGA::TerrainManipulator
{
public:

    /** Constructor. */
    ManipulatorWorld();

protected:

    /** */
    bool handleFrame( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us );

    /** */
    bool performMovementRightMouseButton( const double /*eventTimeDelta*/,
                                          const double /*dx*/,
                                          const double /*dy*/ );

private:

    void boundDistance();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_MANIPULATORWORLD_H
