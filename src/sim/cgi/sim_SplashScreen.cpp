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

#include <sim/cgi/sim_SplashScreen.h>

#include <osgText/Text>

#include <sim/sim_Base.h>
#include <sim/sim_Captions.h>

#include <sim/cgi/sim_Color.h>
#include <sim/cgi/sim_Fonts.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

osg::Group* SplashScreen::create( int width, int height )
{
    float maxX = floor( 100.0f * (float)( width ) / (float)( height ) + 0.5f );

    osg::ref_ptr<osg::Group> root = new osg::Group();

    osg::ref_ptr<osg::StateSet> stateSet = root->getOrCreateStateSet();

    stateSet->setMode( GL_RESCALE_NORMAL , osg::StateAttribute::ON  );
    stateSet->setMode( GL_LIGHTING       , osg::StateAttribute::OFF );
    stateSet->setMode( GL_LIGHT0         , osg::StateAttribute::OFF );
    stateSet->setMode( GL_BLEND          , osg::StateAttribute::ON  );
    stateSet->setMode( GL_ALPHA_TEST     , osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST     , osg::StateAttribute::OFF );
    stateSet->setMode( GL_DITHER         , osg::StateAttribute::OFF );
    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );

    createBack( root.get(), maxX );
    createText( root.get() );

    return root.release();
}

////////////////////////////////////////////////////////////////////////////////

void SplashScreen::createBack( osg::Group *parent, float maxX )
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    parent->addChild( geode.get() );

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    geode->addDrawable( geom.get() );

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();

    v->push_back( osg::Vec3( -maxX, -100.0f, -1.0f ) );
    v->push_back( osg::Vec3(  maxX, -100.0f, -1.0f ) );
    v->push_back( osg::Vec3(  maxX,  100.0f, -1.0f ) );
    v->push_back( osg::Vec3( -maxX,  100.0f, -1.0f ) );

    n->push_back( osg::Vec3( 0.0f, 0.0f, -1.0f ) );

//    // sky gradient
//    osg::Vec4 colorUpper( 111.0f/255.0f, 193.0f/255.0f, 255.0f/255.0f, 1.0f );
//    osg::Vec4 colorLower( 144.0f/255.0f, 186.0f/255.0f, 224.0f/255.0f, 1.0f );

    // grey gradient
    float byteUpper = 51.0f / 255.0f;
    float byteLower =  0.0f / 255.0f;
    osg::Vec4 colorUpper( byteUpper, byteUpper, byteUpper, 1.0f );
    osg::Vec4 colorLower( byteLower, byteLower, byteLower, 1.0f );


    c->push_back( colorLower );
    c->push_back( colorLower );
    c->push_back( colorUpper );
    c->push_back( colorUpper );

    geom->setVertexArray( v );
    geom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

    geom->setNormalArray( n );
    geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    geom->setColorArray( c.get() );
    geom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
}

////////////////////////////////////////////////////////////////////////////////

void SplashScreen::createText( osg::Group *parent )
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    parent->addChild( geode.get() );

    osg::ref_ptr<osgText::Text> text = new osgText::Text();
    geode->addDrawable( text.get() );

    text->setFont( Fonts::get( Base::getPath( "fonts/fsg_stencil.ttf" ) ) );
    text->setColor( osg::Vec4( Color::white, 1.0f ) );
    text->setCharacterSize( 12.0f );
    text->setAxisAlignment( osgText::TextBase::XY_PLANE );
    text->setPosition( osg::Vec3( 0.0f, 0.0f, 0.0f ) );
    text->setLayout( osgText::Text::LEFT_TO_RIGHT );
    text->setAlignment( osgText::Text::CENTER_CENTER );
    text->setText( osgText::String( Captions::instance()->getLoading(), osgText::String::ENCODING_UTF8 ) );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_MSG, "RenderBin" );
}
