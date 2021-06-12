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
