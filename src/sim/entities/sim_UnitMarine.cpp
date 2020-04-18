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

#include <sim/entities/sim_UnitMarine.h>

#ifdef SIM_TEST
#   include <osg/ClipNode>
#   include <osg/Geode>
#   include <osg/Geometry>
#   include <osg/LOD>
#   include <osg/MatrixTransform>
#   include <osg/Program>

#   include <sim/cgi/sim_Textures.h>
#endif

#include <sim/cgi/sim_Effects.h>

#include <sim/entities/sim_Explosion.h>
#include <sim/entities/sim_WreckageSurface.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_TEST
void UnitMarine::createReflection( osg::Node *model, osg::Group *parent )
{
    // TODO
}
#endif

////////////////////////////////////////////////////////////////////////////////

UnitMarine::UnitMarine( Affiliation affiliation ) : UnitSurface( affiliation ) {}

////////////////////////////////////////////////////////////////////////////////

UnitMarine::~UnitMarine() {}

////////////////////////////////////////////////////////////////////////////////

void UnitMarine::destroy()
{
    if ( isActive() )
    {
        Explosion *explosion = new Explosion( 25.0f );
        explosion->setPos( m_pos );
        explosion->setAtt( m_att );

        WreckageSurface *wreckage = new WreckageSurface( m_model.get(), m_smoke.get() );
        wreckage->setPos( m_pos );
        wreckage->setAtt( m_att );

        if ( m_ownship )
        {
            wreckage->setName( "ownship" );
        }
    }

    ///////////////////////
    UnitSurface::destroy();
    ///////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void UnitMarine::load()
{
    ////////////////////
    UnitSurface::load();
    ////////////////////

#   ifdef SIM_TEST
    if ( m_model.valid() )
    {
        createReflection( m_model.get(), m_switch.get() );
    }
#   endif
}

////////////////////////////////////////////////////////////////////////////////

void UnitMarine::setHP( UInt16 hp )
{
    /////////////////////////
    UnitSurface::setHP( hp );
    /////////////////////////

    if ( m_hp < 50 )
    {
        if ( !m_smoke.valid() )
        {
            m_smoke = Effects::createSmoke( 60.0f, 10.0f, 150.0f, 1.0f, 0.04f, 0.5f );
            m_switch->addChild( m_smoke.get() );
        }
    }
}
