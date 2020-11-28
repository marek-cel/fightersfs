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

#include <sim/entities/sim_UnitGround.h>
#include <sim/entities/sim_WreckageSurface.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

UnitGround::UnitGround( Affiliation affiliation ) : UnitSurface( affiliation ) {}

////////////////////////////////////////////////////////////////////////////////

UnitGround::~UnitGround() {}

////////////////////////////////////////////////////////////////////////////////

void UnitGround::destroy()
{
    if ( isActive() )
    {
        WreckageSurface *wreckage = new WreckageSurface( m_model.get() );
        wreckage->setPos( _pos );
        wreckage->setAtt( _att );

        if ( m_ownship )
        {
            wreckage->setName( "ownship" );
        }
    }

    ///////////////////////
    UnitSurface::destroy();
    ///////////////////////
}
