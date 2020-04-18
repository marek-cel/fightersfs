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

#include <sim/cgi/sim_FogScene.h>

#include <osg/Fog>

#include <sim/cgi/sim_Color.h>
#include <sim/cgi/sim_Scenery.h>
#include <sim/cgi/sim_SkyDome.h>

#include <sim/entities/sim_Entities.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

int FogScene::m_visibility = 7200;

////////////////////////////////////////////////////////////////////////////////

FogScene::FogScene( Module *parent ) :
    Module( new osg::Group(), parent )
{
    if ( m_visibility > 0.9f * SIM_SKYDOME_RAD )
    {
        m_visibility = 0.9f * SIM_SKYDOME_RAD;
    }
    else if ( m_visibility < 0.6f * SIM_SKYDOME_RAD )
    {
        m_visibility = 0.6f * SIM_SKYDOME_RAD;
    }

    m_fog = new osg::Fog();

    osg::ref_ptr<osg::StateSet> stateSet = m_root->getOrCreateStateSet();

    osg::Vec4 color( Color::fog_light, 0.0f );

    if ( m_visibility < 7200 )
    {
        color = osg::Vec4( Color::fog_heavy, 0.0f );
    }

    m_fog->setMode( osg::Fog::LINEAR );
    m_fog->setDensity( 0.5f );
    m_fog->setColor( color );
    m_fog->setStart( 0.0f );
    m_fog->setEnd( m_visibility );

    stateSet->setAttributeAndModes( m_fog.get(), osg::StateAttribute::ON );
    stateSet->setMode( GL_FOG, osg::StateAttribute::ON );

    addChild( new Scenery( this ) );

    m_root->addChild( Entities::instance()->getNode() );
}

////////////////////////////////////////////////////////////////////////////////

FogScene::~FogScene() {}

////////////////////////////////////////////////////////////////////////////////

void FogScene::update()
{
    /////////////////
    Module::update();
    /////////////////

#   ifdef SIM_TEST
    osg::ref_ptr<osg::StateSet> stateSet = m_root->getOrCreateStateSet();

    if ( Data::get()->camera.type == ViewWorld )
    {
        stateSet->setMode( GL_FOG, osg::StateAttribute::OFF );
    }
    else
    {
        stateSet->setMode( GL_FOG, osg::StateAttribute::ON );
    }
#   endif
}
