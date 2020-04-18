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

#include <sim/cgi/sim_ManipulatorOrbit.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ManipulatorOrbit::ManipulatorOrbit() :
    osgGA::NodeTrackerManipulator()
{
    setTrackerMode( NODE_CENTER );

    setWheelZoomFactor( -getWheelZoomFactor() );
}

////////////////////////////////////////////////////////////////////////////////

bool ManipulatorOrbit::handleFrame( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us )
{
    ///////////////////////////////////////////////////////////////////
    bool result = osgGA::NodeTrackerManipulator::handleFrame( ea, us );
    ///////////////////////////////////////////////////////////////////

    boundDistance();

    return result;
}

////////////////////////////////////////////////////////////////////////////////

bool ManipulatorOrbit::performMovementRightMouseButton( const double /*eventTimeDelta*/,
                                                        const double /*dx*/,
                                                        const double /*dy*/ )
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////

void ManipulatorOrbit::boundDistance()
{
#   ifdef SIM_TEST
    if ( getDistance() > 5000.0 )
    {
        setDistance( 5000.0 );
    }
    else if ( getDistance() < 10.0 )
    {
        setDistance( 10.0 );
    }
#   else
    if ( getDistance() > 300.0 )
    {
        setDistance( 300.0 );
    }
    else if ( getDistance() < 5.0 )
    {
        setDistance( 5.0 );
    }
#   endif
}
