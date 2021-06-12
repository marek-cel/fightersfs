/****************************************************************************//*
 * Copyright (C) 2021 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
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

int FogScene::_visibility = 7200;

////////////////////////////////////////////////////////////////////////////////

FogScene::FogScene( Module *parent ) :
    Module( new osg::Group(), parent )
{
    if ( _visibility > 0.9f * SIM_SKYDOME_RAD )
    {
        _visibility = 0.9f * SIM_SKYDOME_RAD;
    }
    else if ( _visibility < 0.6f * SIM_SKYDOME_RAD )
    {
        _visibility = 0.6f * SIM_SKYDOME_RAD;
    }

    _fog = new osg::Fog();

    osg::ref_ptr<osg::StateSet> stateSet = _root->getOrCreateStateSet();

    osg::Vec4 color( Color::fog_light, 0.0f );

    if ( _visibility < 7200 )
    {
        color = osg::Vec4( Color::fog_heavy, 0.0f );
    }

    _fog->setMode( osg::Fog::LINEAR );
    _fog->setDensity( 0.5f );
    _fog->setColor( color );
    _fog->setStart( 0.0f );
    _fog->setEnd( _visibility );

    stateSet->setAttributeAndModes( _fog.get(), osg::StateAttribute::ON );
    stateSet->setMode( GL_FOG, osg::StateAttribute::ON );

    addChild( new Scenery( this ) );

    _root->addChild( Entities::instance()->getNode() );
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
    osg::ref_ptr<osg::StateSet> stateSet = _root->getOrCreateStateSet();

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
