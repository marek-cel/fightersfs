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

#include <sim/entities/sim_WreckageSurface.h>

#ifdef SIM_TEST
#   include <sim/entities/sim_UnitMarine.h>
#endif

#include <sim/cgi/sim_Effects.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

WreckageSurface::WreckageSurface( osg::Node *model , osg::Group *smoke ) :
    Wreckage( model )
{
    osg::ref_ptr<osg::Group> smokeTmp = smoke;

    if ( !smokeTmp.valid() )
    {
        smokeTmp = Effects::createSmoke( 60.0f, 10.0f, 150.0f, 1.0f, 0.04f, 0.5f );
    }

    _switch->addChild( smokeTmp.get() );

#   ifdef SIM_TEST
    UnitMarine::createReflection( model, _switch.get() );
#   endif
}

////////////////////////////////////////////////////////////////////////////////

WreckageSurface::~WreckageSurface() {}
