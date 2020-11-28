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

#include <sim/cgi/sim_HUD.h>

#include <osg/LineWidth>
#include <osg/Material>
#include <osg/Point>

#include <sim/sim_Captions.h>
#include <sim/sim_Ownship.h>

#include <sim/cgi/sim_Color.h>
#include <sim/cgi/sim_Fonts.h>
#include <sim/cgi/sim_Gates.h>
#include <sim/cgi/sim_Geometry.h>
#include <sim/cgi/sim_SplashScreen.h>
#include <sim/cgi/sim_Textures.h>

#include <sim/entities/sim_Entities.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_Convert.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const UInt16 HUD::_maxRadarMarks = 16;

const float HUD::_sizeCaptions  = 17.50f;
const float HUD::_sizePlayerBar =  8.75f;
const float HUD::_sizeMessage   =  8.75f;

const float HUD::_deg2px = 600.0f / 90.0f;
const float HUD::_rad2px = Convert::rad2deg( HUD::_deg2px );

const float HUD::_rad11deg = Convert::deg2rad( 11.0f );

////////////////////////////////////////////////////////////////////////////////

HUD::HUD( float linesWidth, int width, int height ) :
    _linesWidth ( linesWidth ),

    _width  ( width ),
    _height ( height ),

    _maxX ( floor( 100.0f * (float)( _width ) / (float)( _height ) + 0.5f ) ),

    _tutorial ( false ),

    _timerTutorial ( 0.0f )
{
    _root = new osg::Group();

    osg::ref_ptr<osg::StateSet> stateSet = _root->getOrCreateStateSet();

    stateSet->setMode( GL_RESCALE_NORMAL , osg::StateAttribute::ON  );
    stateSet->setMode( GL_LIGHTING       , osg::StateAttribute::OFF );
    stateSet->setMode( GL_LIGHT0         , osg::StateAttribute::ON  );
    stateSet->setMode( GL_BLEND          , osg::StateAttribute::ON  );
    stateSet->setMode( GL_ALPHA_TEST     , osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST     , osg::StateAttribute::ON  );
    stateSet->setMode( GL_DITHER         , osg::StateAttribute::OFF );
    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );

    _root->addChild( SplashScreen::create( width, height ) );
}

////////////////////////////////////////////////////////////////////////////////

HUD::~HUD() {}

////////////////////////////////////////////////////////////////////////////////

void HUD::init( bool tutorial )
{
    _tutorial = tutorial;

    if ( _root->getNumChildren() > 0 )
    {
        _root->removeChildren( 0, _root->getNumChildren() );
    }

    _switch = new osg::Switch();
    _root->addChild( _switch.get() );

    Fonts::reset();

    _font = Fonts::get( getPath( "fonts/fsg_stencil.ttf" ) );

#   ifndef SIM_DESKTOP
    createControls();
#   endif
    createCrosshair();
    createEnemyIndicators();
    createHitIndicator();
    createIndicators();
    createPlayerBar();
    createTargetIndicators();
    createWaypointIndicators();

    createCaption();
    createMessage();

    if ( _tutorial )
    {
        createTutorialSymbols();
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::load() {}

////////////////////////////////////////////////////////////////////////////////

void HUD::update()
{
    if ( Data::get()->mission.status == Pending
      && ( Data::get()->camera.type == ViewChase || Data::get()->camera.type == ViewPilot ) )
    {
        if ( !Data::get()->message.visible || !Data::get()->message.overlay )
        {
            _switch->setAllChildrenOn();

            updateControls();
            updateCrosshair();
            updateHitIndicator();
            updateIndicators();
            updatePlayerBar();
            updateTargetIndicators();
            updateWaypointIndicators();

            if ( _tutorial )
            {
                updateTutorialSymbols();
            }
        }
        else
        {
            _switch->setAllChildrenOff();
        }
    }
    else
    {
        _switch->setAllChildrenOff();
    }

    updateCaption();
    updateMessage();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createBox( osg::Geode *geode, osg::Vec4 color, float width )
{
    const float del = width / 2.0f;

    const float w = 10.0f;
    const float h = 10.0f;

    const float w_2 = w / 2.0f;
    const float h_2 = h / 2.0f;

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    v->push_back( osg::Vec3( -w_2 + del, -h_2 + del, -1.0f ) );
    v->push_back( osg::Vec3( -w_2 - del, -h_2 - del, -1.0f ) );

    v->push_back( osg::Vec3(  w_2 - del, -h_2 + del, -1.0f ) );
    v->push_back( osg::Vec3(  w_2 + del, -h_2 - del, -1.0f ) );

    v->push_back( osg::Vec3(  w_2 - del,  h_2 - del, -1.0f ) );
    v->push_back( osg::Vec3(  w_2 + del,  h_2 + del, -1.0f ) );

    v->push_back( osg::Vec3( -w_2 + del,  h_2 - del, -1.0f ) );
    v->push_back( osg::Vec3( -w_2 - del,  h_2 + del, -1.0f ) );

    v->push_back( osg::Vec3( -w_2 + del, -h_2 + del, -1.0f ) );
    v->push_back( osg::Vec3( -w_2 - del, -h_2 - del, -1.0f ) );

    n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

    c->push_back( color );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createCaption()
{
    _patCaption = new osg::PositionAttitudeTransform();
    _root->addChild( _patCaption.get() );

    _patCaption->setPosition( osg::Vec3( 0.0, 25.0f, -1.0 ) );

    _switchCaption = new osg::Switch();
    _patCaption->addChild( _switchCaption.get() );

    _textCaption = new osgText::Text();

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _switchCaption->addChild( geode.get() );

    if ( _font.valid() ) _textCaption->setFont( _font );
    _textCaption->setColor( osg::Vec4( Color::red, 0.8f ) );
    _textCaption->setCharacterSize( _sizeCaptions );
    _textCaption->setAxisAlignment( osgText::TextBase::XY_PLANE );
    _textCaption->setPosition( osg::Vec3( 0.0f, 0.0f, 0.0f ) );
    _textCaption->setLayout( osgText::Text::LEFT_TO_RIGHT );
    _textCaption->setAlignment( osgText::Text::CENTER_CENTER );
    _textCaption->setText( "CAPTION" );

    geode->addDrawable( _textCaption );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_MSG, "RenderBin" );

    geode->addDrawable( _textMessage );

    _switchCaption->setAllChildrenOff();
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SIM_DESKTOP
void HUD::createControls()
{
    // material
    _materialControls = new osg::Material();
    _materialControls->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4( Color::white, 0.4f ) );
    _materialControls->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( Color::white, 0.4f ) );

    createControlsThrottle();
    createControlsTrigger();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createControlsThrottle()
{
    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    _switch->addChild( pat.get() );

    _patControlsThrottle = new osg::PositionAttitudeTransform();
    pat->addChild( _patControlsThrottle.get() );

    pat->setPosition( osg::Vec3( -_maxX, 10.0, 0.0 ) );

    createControlsThrottleGrip( _patControlsThrottle.get(), 0.4f );

    // base
    {
        const float w  =  10.0f;
        const float h  = 120.0f;

        const float h_2 = h / 2.0f;

        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        pat->addChild( geode.get() );

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors
        osg::ref_ptr<osg::Vec2Array> t = new osg::Vec2Array();  // colors

        v->push_back( osg::Vec3( 0.0f, -h_2, -1.0f ) );
        v->push_back( osg::Vec3(    w, -h_2, -1.0f ) );
        v->push_back( osg::Vec3(    w,  h_2, -1.0f ) );
        v->push_back( osg::Vec3( 0.0f,  h_2, -1.0f ) );

        t->push_back( osg::Vec2( 0.0, 0.0 ) );
        t->push_back( osg::Vec2( 1.0, 0.0 ) );
        t->push_back( osg::Vec2( 1.0, 1.0 ) );
        t->push_back( osg::Vec2( 0.0, 1.0 ) );

        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::white, 0.4f ) );

        geometry->setVertexArray( v.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

        geometry->setNormalArray( n.get() );
        geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

        geometry->setColorArray( c.get() );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        geometry->setTexCoordArray( 0, t.get() );

        osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

        stateSet->setAttribute( _materialControls.get() );
        stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
        stateSet->setTextureAttributeAndModes( 0, Textures::get( getPath( "textures/hud_throttle_base.rgb" ) ), osg::StateAttribute::ON );

        geode->addDrawable( geometry.get() );
    }

    // pointer
    if ( _tutorial )
    {
        _switchPointerRpm = new osg::Switch();
        pat->addChild( _switchPointerRpm.get() );

        _patPointerRpm = new osg::PositionAttitudeTransform();
        _switchPointerRpm->addChild( _patPointerRpm.get() );

        createControlsThrottleGrip( _patPointerRpm.get(), 0.2f, -1.0f );

        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
        _patPointerRpm->addChild( pat.get() );

        createPointer( pat.get() );

        pat->setAttitude( osg::Quat( M_PI_2, osg::Z_AXIS ) );
        pat->setPosition( osg::Vec3( 52.0f, 0.0f, 0.0f ) );

        _switchPointerRpm->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createControlsThrottleGrip( osg::Group *parent, float alpha, float z_offset )
{
    const float w  = 40.0f;
    const float h  = 20.0f;
    const float dx = 10.0f;

    const float h_2 = h / 2.0f;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    parent->addChild( geode.get() );

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors
    osg::ref_ptr<osg::Vec2Array> t = new osg::Vec2Array();  // colors

    v->push_back( osg::Vec3( dx     , -h_2, -1.0f + z_offset ) ); // 1
    v->push_back( osg::Vec3( dx     ,  h_2, -1.0f + z_offset ) ); // 2
    v->push_back( osg::Vec3( dx + w ,  h_2, -1.0f + z_offset ) ); // 3
    v->push_back( osg::Vec3( dx + w , -h_2, -1.0f + z_offset ) ); // 3

    t->push_back( osg::Vec2( 0, 0 ) ); // 1
    t->push_back( osg::Vec2( 0, 1 ) ); // 2
    t->push_back( osg::Vec2( 1, 1 ) ); // 3
    t->push_back( osg::Vec2( 1, 0 ) ); // 3

    n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

    c->push_back( osg::Vec4( Color::white, alpha ) );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geometry->setTexCoordArray( 0, t.get() );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setAttribute( _materialControls.get() );
    stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD + (int)z_offset, "RenderBin" );
    stateSet->setTextureAttributeAndModes( 0, Textures::get( getPath( "textures/hud_throttle_grip.rgb" ) ), osg::StateAttribute::ON );

    geode->addDrawable( geometry.get() );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createControlsTrigger()
{
    const float r  = 25.0f;
    const float dx =  5.0f;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    _switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( _maxX - dx - r, 0.0, 0.0 ) );

    _switchTrigger = new osg::Switch();
    pat->addChild( _switchTrigger.get() );

    createFace( _switchTrigger.get(), r, _materialControls.get(), "textures/hud_trigger_0.rgb", 0.4f );
    createFace( _switchTrigger.get(), r, _materialControls.get(), "textures/hud_trigger_1.rgb", 0.4f );

    _switchTrigger->setSingleChildOn( 0 );

    if ( _tutorial )
    {
        _switchPointerTrigger = new osg::Switch();
        pat->addChild( _switchPointerTrigger.get() );

        osg::ref_ptr<osg::PositionAttitudeTransform> patPointer = new osg::PositionAttitudeTransform();

        _switchPointerTrigger->addChild( patPointer.get() );

        patPointer->setPosition( osg::Vec3( -33.0f, 0.0f, 0.0f ) );
        patPointer->setAttitude( osg::Quat( -M_PI_2, osg::Z_AXIS ) );

        createPointer( patPointer.get() );

        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _switchPointerTrigger->addChild( geode.get() );

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
        geode->addDrawable( geometry.get() );

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        const float w = 4.0f;

        for ( int i = 0; i < 37; i++ )
        {
            float a_rad = osg::DegreesToRadians( i * 10.0f );

            double sinA = sin( a_rad );
            double cosA = cos( a_rad );

            float xi = r * sinA;
            float yi = r * cosA;

            float xo = ( r + w ) * sinA;
            float yo = ( r + w ) * cosA;

            v->push_back( osg::Vec3( xi, yi, -1.0f ) );
            v->push_back( osg::Vec3( xo, yo, -1.0f ) );
        }

        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::lime, 1.0f ) );

        geometry->setVertexArray( v.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP, 0, v->size() ) );

        geometry->setNormalArray( n.get() );
        geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

        geometry->setColorArray( c.get() );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        _switchPointerTrigger->setAllChildrenOff();
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////

void HUD::createCrosshair()
{
    const float r = 10.0f;

    _patCrosshair = new osg::PositionAttitudeTransform();
    _switch->addChild( _patCrosshair.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _patCrosshair->addChild( geode.get() );

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors
    osg::ref_ptr<osg::Vec2Array> t = new osg::Vec2Array();  // colors

    v->push_back( osg::Vec3( -r, -r, -1.0f ) );
    v->push_back( osg::Vec3(  r, -r, -1.0f ) );
    v->push_back( osg::Vec3(  r,  r, -1.0f ) );
    v->push_back( osg::Vec3( -r,  r, -1.0f ) );

    t->push_back( osg::Vec2( 0.0, 0.0 ) );
    t->push_back( osg::Vec2( 1.0, 0.0 ) );
    t->push_back( osg::Vec2( 1.0, 1.0 ) );
    t->push_back( osg::Vec2( 0.0, 1.0 ) );

    n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

    c->push_back( osg::Vec4( Color::white, 1.0f ) );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geometry->setTexCoordArray( 0, t.get() );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setAttribute( _materialIndicators.get() );
    stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD + 1, "RenderBin" );
    stateSet->setTextureAttributeAndModes( 0, Textures::get( getPath( "textures/hud_crosshair.rgb" ) ), osg::StateAttribute::ON );

    geode->addDrawable( geometry.get() );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createDir( osg::Geode *geode, osg::Vec4 color )
{
    const float y = 70.0f;
    const float w =  6.0f;
    const float h = 10.0f;
    const float d =  8.0f;

    const float w_2 = w / 2.0f;

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    v->push_back( osg::Vec3( 0.0f, y - d, -1.0f ) ); // 1
    v->push_back( osg::Vec3(  w_2, y - h, -1.0f ) ); // 2
    v->push_back( osg::Vec3( 0.0f, y    , -1.0f ) ); // 3
    v->push_back( osg::Vec3( -w_2, y - h, -1.0f ) ); // 4

    n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

    c->push_back( color );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createHitIndicator()
{
    const float ri = 35.0f;
    const float ro = 40.0f;

    const float a_min = -osg::DegreesToRadians( 30.0f );
    const float a_max =  osg::DegreesToRadians( 30.0f );

//    const float a_min = -osg::DegreesToRadians( 180.0f );
//    const float a_max =  osg::DegreesToRadians( 180.0f );

    const int steps = 8;

    const float step = ( a_max - a_min ) / ( steps - 1 );

    _patHitIndicator = new osg::PositionAttitudeTransform();
    _switch->addChild( _patHitIndicator.get() );

    _patHitIndicator->setPosition( osg::Vec3( 0.0f, -40.0f, 0.0f ) );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _patHitIndicator->addChild( geode.get() );

    _hitIndicator = new osg::Geometry();
    geode->addDrawable( _hitIndicator.get() );

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    for ( int i = 0 ; i < steps; i++ )
    {
        float a = a_min + i * step;

        float xi = ri * sin( a );
        float yi = ri * cos( a );
        float xo = ro * sin( a );
        float yo = ro * cos( a );

        v->push_back( osg::Vec3( xi, yi, -3.0f ) );
        v->push_back( osg::Vec3( xo, yo, -3.0f ) );

        c->push_back( osg::Vec4( Color::red, 0.0f ) );
        c->push_back( osg::Vec4( Color::red, 0.0f ) );
    }

    n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

    _hitIndicator->setVertexArray( v.get() );
    _hitIndicator->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP, 0, v->size() ) );

    _hitIndicator->setColorArray( c.get() );
    _hitIndicator->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    _hitIndicator->setNormalArray( n.get() );
    _hitIndicator->setNormalBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createEnemyIndicators()
{
    _switchEnemyIndicators = new osg::Switch();
    _switch->addChild( _switchEnemyIndicators.get() );

    osg::ref_ptr< osg::Geode > geodeBox = new osg::Geode();
    osg::ref_ptr< osg::Geode > geodeDir = new osg::Geode();

    createBox( geodeBox.get(), osg::Vec4( Color::red, 0.7f ), 0.5f );
    createDir( geodeDir.get(), osg::Vec4( Color::red, 0.7f ) );

    for ( UInt16 i = 0; i < _maxRadarMarks; i++ )
    {
        osg::ref_ptr<osg::PositionAttitudeTransform> patEnemyBox = new osg::PositionAttitudeTransform();
        _switchEnemyIndicators->addChild( patEnemyBox.get() );

        _patEnemyBox.push_back( patEnemyBox.get() );

        osg::ref_ptr<osg::PositionAttitudeTransform> patEnemyDir = new osg::PositionAttitudeTransform();
        _switchEnemyIndicators->addChild( patEnemyDir.get() );

        _patEnemyDir.push_back( patEnemyDir.get() );

        patEnemyBox->addChild( geodeBox );
        patEnemyDir->addChild( geodeDir );
    }

    _switchEnemyIndicators->setAllChildrenOff();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createFace( osg::Group *parent, float radius,
                      osg::Material *material,
                      const std::string &textureFile,
                      float alpha )
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    parent->addChild( geode.get() );

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec2Array> t = new osg::Vec2Array();  // texture

    float a_deg = 0.0f;
    float a_rad = osg::DegreesToRadians( a_deg );

    while ( a_deg < 360.0 )
    {
        float x_n = sin( a_rad );
        float y_n = cos( a_rad );

        float x = radius * x_n;
        float y = radius * y_n;

        v->push_back( osg::Vec3( x, y, -2.0f ) );
        t->push_back( osg::Vec2( ( 1.0f + x_n ) / 2.0f, ( 1.0f + y_n ) / 2.0f ) );

        a_deg += 10.0f;
        a_rad = osg::DegreesToRadians( a_deg );
    }

    n->push_back( osg::Vec3( 0.0f, 0.0f,  1.0f ) );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    if ( 1 )
    {
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();

        c->push_back( osg::Vec4( Color::white, alpha ) );

        geometry->setColorArray( c.get() );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    }

    geometry->setTexCoordArray( 0, t.get() );

    osg::ref_ptr<osg::StateSet> stateSet = geometry->getOrCreateStateSet();

    stateSet->setAttribute( material );
    stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
    stateSet->setTextureAttributeAndModes( 0, Textures::get( getPath( textureFile ) ), osg::StateAttribute::ON );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createIndicators()
{
    _geodeRadarMarkEnemy  = new osg::Geode();
    _geodeRadarMarkFriend = new osg::Geode();

    createRadarMark( _geodeRadarMarkEnemy.get()  , osg::Vec4( Color::red  , 1.0f ) );
    createRadarMark( _geodeRadarMarkFriend.get() , osg::Vec4( Color::lime , 1.0f ) );

    // material
    _materialIndicators = new osg::Material();
    _materialIndicators->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4( Color::white, 0.7f ) );
    _materialIndicators->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( Color::white, 0.7f ) );

    createIndicatorALT();
    createIndicatorASI();
    createIndicatorRadar();
    createIndicatorVSI();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createIndicatorHand( osg::Group *parent, float l, float w )
{
    const float w_2 = w / 2.0f;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    parent->addChild( geode.get() );

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    v->push_back( osg::Vec3( 0.0f,     0.0f, -1.0f ) );
    v->push_back( osg::Vec3(  w_2, l * 0.6f, -1.0f ) );
    v->push_back( osg::Vec3( -w_2, l * 0.6f, -1.0f ) );
    v->push_back( osg::Vec3( -w_2, l * 0.6f, -1.0f ) );
    v->push_back( osg::Vec3(  w_2, l * 0.6f, -1.0f ) );
    v->push_back( osg::Vec3( 0.0f,        l, -1.0f ) );

    n->push_back( osg::Vec3( 0.0f, 0.0f,  1.0f ) );
    c->push_back( osg::Vec4( Color::white, 1.0f ) );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createIndicatorALT()
{
    const float x =  86.0f;
    const float y = -75.0f;
    const float r =  20.0f;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    _switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( x, y, -1.0f ) );

    createFace( pat.get(), r, _materialIndicators.get(), "textures/hud_altitude.rgb", 0.7f );

    _patIndicatorALT1 = new osg::PositionAttitudeTransform();
    _patIndicatorALT2 = new osg::PositionAttitudeTransform();

    pat->addChild( _patIndicatorALT1.get() );
    pat->addChild( _patIndicatorALT2.get() );

    // arrow 1
    {
        const float l = 10.0f;
        const float w =  2.5f;

        createIndicatorHand( _patIndicatorALT1.get(), l, w );
    }

    // arrow 2
    {
        const float l = 15.0f;
        const float w =  1.5f;

        createIndicatorHand( _patIndicatorALT2.get(), l, w );
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createIndicatorASI()
{
    const float x = -47.0f;
    const float y = -80.0f;
    const float r =  15.0f;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    _switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( x, y, -1.0f ) );

    createFace( pat.get(), r, _materialIndicators.get(), "textures/hud_airspeed.rgb", 0.7f );

    _patIndicatorASI = new osg::PositionAttitudeTransform();
    pat->addChild( _patIndicatorASI.get() );

    const float l = 11.0f;
    const float w =  1.5f;

    createIndicatorHand( _patIndicatorASI.get(), l, w );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createIndicatorRadar()
{
    const float x = -86.0f;
    const float y = -75.0f;
    const float r =  20.0f;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    _switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( x, y, -1.0f ) );

    _patIndicatorRadar = new osg::PositionAttitudeTransform();
    pat->addChild( _patIndicatorRadar.get() );

    _switchRadarMarksEnemy  = new osg::Switch();
    _switchRadarMarksFriend = new osg::Switch();

    _patIndicatorRadar->addChild( _switchRadarMarksEnemy.get() );
    _patIndicatorRadar->addChild( _switchRadarMarksFriend.get() );

    for ( UInt16 i = 0; i < _maxRadarMarks; i++ )
    {
        osg::ref_ptr<osg::PositionAttitudeTransform> patE = new osg::PositionAttitudeTransform();
        osg::ref_ptr<osg::PositionAttitudeTransform> patF = new osg::PositionAttitudeTransform();

        _switchRadarMarksEnemy  ->addChild( patE.get() );
        _switchRadarMarksFriend ->addChild( patF.get() );

        patE->addChild( _geodeRadarMarkEnemy.get() );
        patF->addChild( _geodeRadarMarkFriend.get() );

        _patRadarMarksEnemy.push_back( patE.get() );
        _patRadarMarksFriend.push_back( patF.get() );
    }

    createFace( pat.get(), r, _materialIndicators.get(), "textures/hud_radar.rgb", 0.7f );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createIndicatorVSI()
{
    const float x =  47.0f;
    const float y = -80.0f;
    const float r =  15.0f;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    _switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( x, y, -1.0f ) );

    createFace( pat.get(), r, _materialIndicators.get(), "textures/hud_climb_rate.rgb", 0.7f );

    _patIndicatorVSI = new osg::PositionAttitudeTransform();
    pat->addChild( _patIndicatorVSI.get() );

    const float l = 11.0f;
    const float w =  1.5f;

    createIndicatorHand( _patIndicatorVSI.get(), l, w );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createMessage()
{
    _switchMessage = new osg::Switch();
    _root->addChild( _switchMessage.get() );

    _switchMessage->setAllChildrenOff();

    const float dy = 60.0f;

    const float r =   4.0f;
    const float w = 180.0f;
    const float h =  40.0f;

    const float w_2 = w / 2.0f;
    const float h_2 = h / 2.0f;

    // text
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _switchMessage->addChild( geode.get() );

        _textMessage = new osgText::Text();

        if ( _font.valid() ) _textMessage->setFont( _font );
        _textMessage->setColor( osg::Vec4( Color::white, 1.0f ) );
        _textMessage->setCharacterSize( _sizeMessage );
        _textMessage->setAxisAlignment( osgText::TextBase::XY_PLANE );
        _textMessage->setPosition( osg::Vec3( 0.0, dy, -0.5f ) );
        _textMessage->setLayout( osgText::Text::LEFT_TO_RIGHT );
        _textMessage->setAlignment( osgText::Text::CENTER_CENTER );
        _textMessage->setText( "" );

        osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

        stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
        stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_MSG, "RenderBin" );

        geode->addDrawable( _textMessage );
    }

    // box
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _switchMessage->addChild( geode.get() );

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
        geode->addDrawable( geometry.get() );

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        v->push_back( osg::Vec3( 0.0f, dy, -1.0f ) );

        v->push_back( osg::Vec3( -w_2, -h_2 + r + dy, -1.0f ) );
        v->push_back( osg::Vec3( -w_2 + r, -h_2 + dy, -1.0f ) );

        v->push_back( osg::Vec3(  w_2 - r, -h_2 + dy, -1.0f ) );
        v->push_back( osg::Vec3(  w_2, -h_2 + r + dy, -1.0f ) );

        v->push_back( osg::Vec3(  w_2,  h_2 - r + dy, -1.0f ) );
        v->push_back( osg::Vec3(  w_2 - r,  h_2 + dy, -1.0f ) );

        v->push_back( osg::Vec3( -w_2 + r,  h_2 + dy, -1.0f ) );
        v->push_back( osg::Vec3( -w_2,  h_2 - r + dy, -1.0f ) );

        v->push_back( osg::Vec3( -w_2, -h_2 + r + dy, -1.0f ) );

        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::black, 0.5f ) );

        geometry->setVertexArray( v.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

        geometry->setNormalArray( n.get() );
        geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

        geometry->setColorArray( c.get() );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

        stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
        stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_MSG - 1, "RenderBin" );
    }

    // overlay
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _switchMessage->addChild( geode.get() );

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
        geode->addDrawable( geometry.get() );

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        v->push_back( osg::Vec3( -250, -100, -1.0f ) );
        v->push_back( osg::Vec3(  250, -100, -1.0f ) );
        v->push_back( osg::Vec3(  250,  100, -1.0f ) );
        v->push_back( osg::Vec3( -250,  100, -1.0f ) );

        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::black, 0.4f ) );

        geometry->setVertexArray( v.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

        geometry->setNormalArray( n.get() );
        geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

        geometry->setColorArray( c.get() );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

        stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
        stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_MSG - 1, "RenderBin" );
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createPlayerBar()
{
    _patPlayerBar = new osg::PositionAttitudeTransform();
    _switch->addChild( _patPlayerBar.get() );

    const float del = 1.0f;

    const float w = 120.0f;
    const float h =   4.0f;
    const float d =  90.0f;

    const float w_2 = w / 2.0f;
    const float h_2 = h / 2.0f;

    // box
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _switch->addChild( geode.get() );

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        v->push_back( osg::Vec3( -w_2, -h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3( -w_2 - del, -h_2 + d - del, -1.0f ) );

        v->push_back( osg::Vec3( -w_2,  h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3( -w_2 - del,  h_2 + d + del, -1.0f ) );

        v->push_back( osg::Vec3(  w_2,  h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3(  w_2 + del,  h_2 + d + del, -1.0f ) );

        v->push_back( osg::Vec3(  w_2, -h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3(  w_2 + del, -h_2 + d - del, -1.0f ) );

        v->push_back( osg::Vec3( -w_2, -h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3( -w_2 - del, -h_2 + d - del, -1.0f ) );

        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::black, 1.0f ) );

        geometry->setVertexArray( v.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP, 0, v->size() ) );

        geometry->setNormalArray( n.get() );
        geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

        geometry->setColorArray( c.get() );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        geode->addDrawable( geometry.get() );
    }

    // bar
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _patPlayerBar->addChild( geode.get() );

        //osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
        _playerLifeBar = new osg::Geometry();

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        v->push_back( osg::Vec3( -w_2, -h_2 + d, -2.0f ) ); // 4
        v->push_back( osg::Vec3( -w_2,  h_2 + d, -2.0f ) ); // 1
        v->push_back( osg::Vec3(  w_2,  h_2 + d, -2.0f ) ); // 2
        v->push_back( osg::Vec3(  w_2, -h_2 + d, -2.0f ) ); // 3


        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        osg::Vec3f upper = Color::lime;
        osg::Vec3f lower = Color::lime * 0.5f;

        c->push_back( osg::Vec4( lower, 1.0f ) );
        c->push_back( osg::Vec4( upper, 1.0f ) );
        c->push_back( osg::Vec4( upper, 1.0f ) );
        c->push_back( osg::Vec4( lower, 1.0f ) );

        _playerLifeBar->setVertexArray( v.get() );
        _playerLifeBar->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, v->size() ) );

        _playerLifeBar->setNormalArray( n.get() );
        _playerLifeBar->setNormalBinding( osg::Geometry::BIND_OVERALL );

//        _playerLifeBar->setColorArray( c.get() );
//        _playerLifeBar->setColorBinding( osg::Geometry::BIND_OVERALL );

        _playerLifeBar->setColorArray( c.get() );
        _playerLifeBar->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

        geode->addDrawable( _playerLifeBar.get() );
    }

    // heart
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _switch->addChild( geode.get() );

        const float hh = 10.0f;
        const float ww = 10.0f;

        const float hh_2 = hh / 2.0f;
        const float ww_2 = ww / 2.0f;

        const float dy = -w_2 - 4.0f - ww_2;

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors
        osg::ref_ptr<osg::Vec2Array> t = new osg::Vec2Array();  // tex coords

        v->push_back( osg::Vec3( -ww_2 + dy, -hh_2 + d, -1.0f ) );
        v->push_back( osg::Vec3(  ww_2 + dy, -hh_2 + d, -1.0f ) );
        v->push_back( osg::Vec3(  ww_2 + dy,  hh_2 + d, -1.0f ) );
        v->push_back( osg::Vec3( -ww_2 + dy,  hh_2 + d, -1.0f ) );

        t->push_back( osg::Vec2( 0, 0 ) );
        t->push_back( osg::Vec2( 1, 0 ) );
        t->push_back( osg::Vec2( 1, 1 ) );
        t->push_back( osg::Vec2( 0, 1 ) );

        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::white, 1.0f ) );

        geometry->setVertexArray( v.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

        geometry->setNormalArray( n.get() );
        geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

        geometry->setColorArray( c.get() );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        geometry->setTexCoordArray( 0, t.get() );

        osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

        stateSet->setAttribute( _materialIndicators.get() );
        stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD + 1, "RenderBin" );
        stateSet->setTextureAttributeAndModes( 0, Textures::get( getPath( "textures/hud_heart.rgb" ) ), osg::StateAttribute::ON );

        geode->addDrawable( geometry.get() );
    }

    // text - right
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _switch->addChild( geode.get() );

        _textPlayerHP = new osgText::Text();

        if ( _font.valid() ) _textPlayerHP->setFont( _font );
        _textPlayerHP->setColor( osg::Vec4( Color::black, 1.0f ) );
        _textPlayerHP->setCharacterSize( _sizePlayerBar );
        _textPlayerHP->setAxisAlignment( osgText::TextBase::XY_PLANE );
        _textPlayerHP->setPosition( osg::Vec3(  w_2 + 4.0f, d - 3.0f, -1.0f ) );
        _textPlayerHP->setLayout( osgText::Text::LEFT_TO_RIGHT );
        _textPlayerHP->setAlignment( osgText::Text::LEFT_BASE_LINE );
        _textPlayerHP->setText( "100" );

        geode->addDrawable( _textPlayerHP );
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createPointer( osg::Group *parent )
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    parent->addChild( geode.get() );

    const float h  = 24.0f;
    const float wh = 12.0f;
    const float ws =  6.0f;

    const float h_2  = h  / 2.0f;
    const float wh_2 = wh / 2.0f;
    const float ws_2 = ws / 2.0f;

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    v->push_back( osg::Vec3(  0.0f, 0.0f, 0.0f ) ); // 1
    v->push_back( osg::Vec3( -wh_2, -h_2, 0.0f ) ); // 2
    v->push_back( osg::Vec3( -ws_2, -h_2, 0.0f ) ); // 3
    v->push_back( osg::Vec3( -ws_2,   -h, 0.0f ) ); // 4
    v->push_back( osg::Vec3(  ws_2,   -h, 0.0f ) ); // 5
    v->push_back( osg::Vec3(  ws_2, -h_2, 0.0f ) ); // 6
    v->push_back( osg::Vec3(  wh_2, -h_2, 0.0f ) ); // 7

    n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

    //c->push_back( osg::Vec4( Color::lime, 0.8f ) );
    c->push_back( osg::Vec4( Color::lime, 1.0f ) );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createRadarMark( osg::Geode *geode, osg::Vec4 color )
{
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    v->push_back( osg::Vec3( 0.0f, 0.0f, -1.0f ) );
    n->push_back( osg::Vec3( 0.0f, 0.0f,  1.0f ) );
    c->push_back( color );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geode->setInitialBound( osg::BoundingSphere( osg::Vec3(0.0f,0.0f,0.0f), _linesWidth ) );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    osg::ref_ptr<osg::Point> point = new osg::Point();
    point->setSize( _linesWidth );

    stateSet->setAttribute( point );
//    stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
//    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTargetIndicators()
{
    _switchTargetIndicators = new osg::Switch();
    _switch->addChild( _switchTargetIndicators.get() );

    _patTargetBox = new osg::PositionAttitudeTransform();
    _switchTargetIndicators->addChild( _patTargetBox.get() );

    _patTargetBar = new osg::PositionAttitudeTransform();
    _patTargetBox->addChild( _patTargetBar.get() );

    _patTargetCue = new osg::PositionAttitudeTransform();
    _switchTargetIndicators->addChild( _patTargetCue.get() );

    _patTargetDir = new osg::PositionAttitudeTransform();
    _switchTargetIndicators->addChild( _patTargetDir.get() );

    createTargetIndicatorBar();
    createTargetIndicatorBox();
    createTargetIndicatorCue();
    createTargetIndicatorDir();

    _switchTargetIndicators->setAllChildrenOff();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTargetIndicatorBar()
{
    const float del = 1.0f / 2.0f;

    const float w = 10.0f;
    const float h =  2.0f;
    const float d =  8.0f;

    const float w_2 = w / 2.0f;
    const float h_2 = h / 2.0f;

    // box
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _patTargetBox->addChild( geode.get() );

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        v->push_back( osg::Vec3( -w_2, -h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3( -w_2 - del, -h_2 + d - del, -1.0f ) );

        v->push_back( osg::Vec3( -w_2,  h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3( -w_2 - del,  h_2 + d + del, -1.0f ) );

        v->push_back( osg::Vec3(  w_2,  h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3(  w_2 + del,  h_2 + d + del, -1.0f ) );

        v->push_back( osg::Vec3(  w_2, -h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3(  w_2 + del, -h_2 + d - del, -1.0f ) );

        v->push_back( osg::Vec3( -w_2, -h_2 + d, -1.0f ) );
        v->push_back( osg::Vec3( -w_2 - del, -h_2 + d - del, -1.0f ) );

        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::black, 1.0f ) );

        geometry->setVertexArray( v.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP, 0, v->size() ) );

        geometry->setNormalArray( n.get() );
        geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

        geometry->setColorArray( c.get() );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        geode->addDrawable( geometry.get() );

        osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

        //stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
        stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
    }

    // bar
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        _patTargetBar->addChild( geode.get() );

        //osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
        _targetLifeBar = new osg::Geometry();

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        v->push_back( osg::Vec3( -w_2, -h_2 + d, -2.0f ) ); // 4
        v->push_back( osg::Vec3( -w_2,  h_2 + d, -2.0f ) ); // 1
        v->push_back( osg::Vec3(  w_2,  h_2 + d, -2.0f ) ); // 2
        v->push_back( osg::Vec3(  w_2, -h_2 + d, -2.0f ) ); // 3


        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::lime, 1.0f ) );

        _targetLifeBar->setVertexArray( v.get() );
        _targetLifeBar->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, v->size() ) );

        _targetLifeBar->setNormalArray( n.get() );
        _targetLifeBar->setNormalBinding( osg::Geometry::BIND_OVERALL );

        _targetLifeBar->setColorArray( c.get() );
        _targetLifeBar->setColorBinding( osg::Geometry::BIND_OVERALL );

        geode->addDrawable( _targetLifeBar.get() );

        osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

        //stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
        stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTargetIndicatorBox()
{
    osg::ref_ptr< osg::Geode > geode = new osg::Geode();
    _patTargetBox->addChild( geode.get() );

    createBox( geode.get(), osg::Vec4( Color::red, 0.7f ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTargetIndicatorCue()
{
    const float ro = 3.0f;
    const float ri = 1.0f;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _patTargetCue->addChild( geode.get() );

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    v->push_back( osg::Vec3(  ro, 0.0f, -1.0f ) );
    v->push_back( osg::Vec3(  ri, 0.0f, -1.0f ) );

    v->push_back( osg::Vec3( -ro, 0.0f, -1.0f ) );
    v->push_back( osg::Vec3( -ri, 0.0f, -1.0f ) );

    v->push_back( osg::Vec3( 0.0f,  ro, -1.0f ) );
    v->push_back( osg::Vec3( 0.0f,  ri, -1.0f ) );

    v->push_back( osg::Vec3( 0.0f, -ro, -1.0f ) );
    v->push_back( osg::Vec3( 0.0f, -ri, -1.0f ) );

    n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

    c->push_back( osg::Vec4( Color::red, 0.7f ) );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth();
    lineWidth->setWidth( _linesWidth );

    stateSet->setAttributeAndModes( lineWidth, osg::StateAttribute::ON );

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );

    if ( _tutorial )
    {
        _switchPointerTarget = new osg::Switch();
        _patTargetCue->addChild( _switchPointerTarget.get() );

        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
        _switchPointerTarget->addChild( pat.get() );

        pat->setPosition( osg::Vec3( 0.0f, -10.0f, 0.0f ) );

        createPointer( pat.get() );

        _switchPointerTarget->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTargetIndicatorDir()
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _patTargetDir->addChild( geode.get() );

    createDir( geode.get(), osg::Vec4( Color::red, 0.7f ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTutorialSymbols()
{
    _switchPointerCustom = new osg::Switch();
    _switch->addChild( _switchPointerCustom.get() );

    _patPointerCustom = new osg::PositionAttitudeTransform();
    _switchPointerCustom->addChild( _patPointerCustom.get() );

    createPointer( _patPointerCustom.get() );

    _switchPointerCustom->setAllChildrenOff();

#   ifndef SIM_DESKTOP
    createTutorialTips();
#   endif
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SIM_DESKTOP
void HUD::createTutorialTips()
{
    _switchTutorialTips = new osg::Switch();
    _switch->addChild( _switchTutorialTips.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _switchTutorialTips->addChild( geode.get() );

    const float d_y = -36.0f;
    const float w_2 =  36.0f;
    const float h_2 =  18.0f;
    const float r   =   4.0f;

    // box
    {
        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
        geode->addDrawable( geometry.get() );

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        v->push_back( osg::Vec3( 0.0f, d_y, -1.0f ) );

        v->push_back( osg::Vec3( -w_2, -h_2 + r + d_y, -1.0f ) );
        v->push_back( osg::Vec3( -w_2 + r, -h_2 + d_y, -1.0f ) );

        v->push_back( osg::Vec3(  w_2 - r, -h_2 + d_y, -1.0f ) );
        v->push_back( osg::Vec3(  w_2, -h_2 + r + d_y, -1.0f ) );

        v->push_back( osg::Vec3(  w_2,  h_2 - r + d_y, -1.0f ) );
        v->push_back( osg::Vec3(  w_2 - r,  h_2 + d_y, -1.0f ) );

        v->push_back( osg::Vec3( -w_2 + r,  h_2 + d_y, -1.0f ) );
        v->push_back( osg::Vec3( -w_2,  h_2 - r + d_y, -1.0f ) );

        v->push_back( osg::Vec3( -w_2, -h_2 + r + d_y, -1.0f ) );

        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::black, 0.5f ) );

        geometry->setVertexArray( v.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

        geometry->setNormalArray( n.get() );
        geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

        geometry->setColorArray( c.get() );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    }

    // tip
    {
        _tutorialTip = new osg::Geometry();
        geode->addDrawable( _tutorialTip.get() );

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec2Array> t = new osg::Vec2Array();  // texcoords
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        v->push_back( osg::Vec3( -w_2, -h_2 + d_y, 0.0f ) );
        v->push_back( osg::Vec3(  w_2, -h_2 + d_y, 0.0f ) );
        v->push_back( osg::Vec3(  w_2,  h_2 + d_y, 0.0f ) );
        v->push_back( osg::Vec3( -w_2,  h_2 + d_y, 0.0f ) );

        t->push_back( osg::Vec2( 0.0f, 0.0f ) );
        t->push_back( osg::Vec2( 1.0f, 0.0f ) );
        t->push_back( osg::Vec2( 1.0f, 1.0f ) );
        t->push_back( osg::Vec2( 0.0f, 1.0f ) );

        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::white, 1.0f ) );

        _tutorialTip->setVertexArray( v.get() );
        _tutorialTip->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

        _tutorialTip->setTexCoordArray( 0, t.get() );

        _tutorialTip->setNormalArray( n.get() );
        _tutorialTip->setNormalBinding( osg::Geometry::BIND_OVERALL );

        _tutorialTip->setColorArray( c.get() );
        _tutorialTip->setColorBinding( osg::Geometry::BIND_OVERALL );
    }

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );

    _switchTutorialTips->setAllChildrenOff();
}
#endif

////////////////////////////////////////////////////////////////////////////////

void HUD::createWaypointIndicators()
{
    _switchWaypointIndicators = new osg::Switch();
    _switch->addChild( _switchWaypointIndicators.get() );

    _patWaypointBox = new osg::PositionAttitudeTransform();
    _switchWaypointIndicators->addChild( _patWaypointBox.get() );

    _patWaypointDir = new osg::PositionAttitudeTransform();
    _switchWaypointIndicators->addChild( _patWaypointDir.get() );

    createWaypointIndicatorBox();
    createWaypointIndicatorDir();

    _switchWaypointIndicators->setAllChildrenOff();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createWaypointIndicatorBox()
{
    const float size = Convert::rad2deg( atan( Gates::_size / ( 2.0f * Gates::_distScale ) ) ) * 1200.0f / 90.0f;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _patWaypointBox->addChild( geode.get() );

    const float w_2 = size / 2.0f;
    const float h_2 = size / 2.0f;

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    v->push_back( Vec3( -w_2,  h_2, 0.0f ) );
    v->push_back( Vec3( -w_2, -h_2, 0.0f ) );
    v->push_back( Vec3(  w_2, -h_2, 0.0f ) );
    v->push_back( Vec3(  w_2,  h_2, 0.0f ) );

    n->push_back( osg::Vec3( 0.0f, 1.0f, 0.0f ) );

    c->push_back( osg::Vec4( Color::lime, 0.8f ) );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_LOOP, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth();
    lineWidth->setWidth( _linesWidth );

    stateSet->setAttributeAndModes( lineWidth, osg::StateAttribute::ON );

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createWaypointIndicatorDir()
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _patWaypointDir->addChild( geode.get() );

    createDir( geode.get(), osg::Vec4( Color::lime, 0.8f ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateCaption()
{
    float coef = 1.0f;

    if ( !Data::get()->paused && Data::get()->mission.status != Pending )
    {
        if ( Data::get()->mission.status == Success )
        {
            _switchCaption->setAllChildrenOn();
            _textCaption->setText( osgText::String( Captions::instance()->getMissionSuccess(), osgText::String::ENCODING_UTF8 ) );
        }
        else if ( Data::get()->mission.status == Failure )
        {
            _switchCaption->setAllChildrenOn();
            _textCaption->setText( osgText::String( Captions::instance()->getMissionFailure(), osgText::String::ENCODING_UTF8 ) );
        }

        coef = Data::get()->mission.time_end;

        if ( coef > 1.0f ) coef = 1.0f;
    }
    else if ( !Data::get()->paused )
    {
        float time = 0.0f;

        if ( Data::get()->ownship.friend_hit < 1.0f )
        {
            _switchCaption->setAllChildrenOn();
            _textCaption->setText( osgText::String( Captions::instance()->getFriendlyFire(), osgText::String::ENCODING_UTF8 ) );

            time = Data::get()->ownship.friend_hit;
        }
        else if ( Data::get()->ownship.target_kill < 1.0f )
        {
            _switchCaption->setAllChildrenOn();
            _textCaption->setText( osgText::String( Captions::instance()->getTargetKilled(), osgText::String::ENCODING_UTF8 ) );

            time = Data::get()->ownship.target_kill;
        }
        else if ( Data::get()->ownship.target_hit < 1.0f )
        {
            _switchCaption->setAllChildrenOn();
            _textCaption->setText( osgText::String( Captions::instance()->getTargetHit(), osgText::String::ENCODING_UTF8 ) );

            time = Data::get()->ownship.target_hit;
        }
        else if ( Data::get()->mission.time_left < 30.0f && Data::get()->mission.time_left > 0.0f )
        {
            _switchCaption->setAllChildrenOn();

            int s = floor( Data::get()->mission.time_left );
            int m = std::max( 0, std::min( 999, (int)( 1000 * ( Data::get()->mission.time_left - (float)s ) ) ) );
            int c = m / 10;

            if ( m - 10 * c == 1 ) m += 1;

            char text[ 16 ];

            sprintf( text, "00:%02d.%03d", s, m );

            _textCaption->setText( text );
        }
        else
        {
            _switchCaption->setAllChildrenOff();
        }

        coef = 1.0f - time;
    }
    else
    {
        _switchCaption->setAllChildrenOff();
    }

    if ( coef < 0.0f ) coef = 0.0f;

#   ifdef SIM_TEST
    if ( Data::get()->mission.status == Pending
         && ( Data::get()->camera.type == ViewFlyby
           || Data::get()->camera.type == ViewFront
           || Data::get()->camera.type == ViewOrbit
           || Data::get()->camera.type == ViewShift ) )
    {
        coef = 0.0f;
    }
#   endif

    float scale = 0.5f + 0.5f * coef;

    _textCaption->setColor( osg::Vec4( Color::red, 0.8f * coef ) );
    _patCaption->setScale( osg::Vec3( scale, scale, scale ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateControls()
{
#   ifndef SIM_DESKTOP
    if ( Data::get()->controls.trigger )
    {
        _switchTrigger->setSingleChildOn( 1 );
    }
    else
    {
        _switchTrigger->setSingleChildOn( 0 );
    }

    _patControlsThrottle->setPosition( osg::Vec3( 0.0f, -50.0f + Data::get()->controls.throttle * 100.0f, 0.0f ) );
#   endif
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateCrosshair()
{
    osg::Vec3 r( _rad2px * Data::get()->camera.d_psi,
                -_rad2px * Data::get()->camera.d_tht,
                 0.0f );

    _patCrosshair->setPosition( r );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateHitIndicator()
{
    float coef = 1.0f - Data::get()->ownship.ownship_hit;

    if ( coef < 0.0f ) coef = 0.0f;

    osg::ref_ptr<osg::Vec4Array> c = dynamic_cast< osg::Vec4Array* >( _hitIndicator->getColorArray() );

    if ( c.valid() )
    {
        c->clear();

        osg::Vec4 inner = osg::Vec4( Color::red, 0.0f );
        osg::Vec4 outer = osg::Vec4( Color::red, 0.8f * coef );

        for ( int i = 0 ; i < 8; i++ )
        {
            c->push_back( inner );
            c->push_back( outer );
        }

        _hitIndicator->setColorArray( c.get() );
    }

    float scale = 0.8f + 0.2f * coef;

    osg::Vec3 pos( 1.2 * _rad2px * Data::get()->camera.d_psi, -40.0f, 0.0f );

    _patHitIndicator->setPosition( pos );
    _patHitIndicator->setAttitude( osg::Quat( Data::get()->ownship.hit_direction, osg::Z_AXIS ) );
    _patHitIndicator->setScale( osg::Vec3( scale, scale, scale ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateIndicators()
{
    // ALT
    float altitude1 = Convert::m2ft( Data::get()->ownship.altitude_asl ) / 10000.0f;
    float altitude2 = 10.0f * ( altitude1 - floor( altitude1 ) );

    altitude1 *= 2.0f * M_PI;
    altitude2 *= 2.0f * M_PI;

    _patIndicatorALT1->setAttitude( osg::Quat( -altitude1, osg::Z_AXIS ) );
    _patIndicatorALT2->setAttitude( osg::Quat( -altitude2, osg::Z_AXIS ) );

    // ASI
    float airspeed = Convert::mps2kts( Data::get()->ownship.airspeed ) / 100.0;
    airspeed *= M_PI_2;

    _patIndicatorASI->setAttitude( osg::Quat( -airspeed, osg::Z_AXIS ) );

    // VSI
    float climbRate = Convert::mps2fpm( Data::get()->ownship.climbRate ) / 1000.0;
    if ( climbRate >  4.3f ) climbRate =  4.3f;
    if ( climbRate < -4.3f ) climbRate = -4.3f;
    climbRate *= Convert::deg2rad( 40.0 );

    _patIndicatorVSI->setAttitude( osg::Quat( M_PI_2 - climbRate, osg::Z_AXIS ) );

    // radar
    updateIndicatorRadar();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateIndicatorRadar()
{
    // 1 nm = 1852 m
    // 3 nm = 5556 m
    const float dist_coef = 16.0f / 5556.0f;

    _patIndicatorRadar->setAttitude( osg::Quat( Data::get()->ownship.heading, osg::Z_AXIS ) );

    _switchRadarMarksEnemy  ->setAllChildrenOff();
    _switchRadarMarksFriend ->setAllChildrenOff();
    _switchEnemyIndicators  ->setAllChildrenOff();

    Vec3 pos_own( Data::get()->ownship.pos_x,
                  Data::get()->ownship.pos_y,
                  Data::get()->ownship.pos_z );
    Quat att_own( Data::get()->ownship.att_x,
                  Data::get()->ownship.att_y,
                  Data::get()->ownship.att_z,
                  Data::get()->ownship.att_w );

    Vec3 pos_cam( Data::get()->camera.pos_x,
                  Data::get()->camera.pos_y,
                  Data::get()->camera.pos_z );
    Quat att_cam( Data::get()->camera.d_phi, osg::X_AXIS,
                  Data::get()->camera.d_tht, osg::Y_AXIS,
                  Data::get()->camera.d_psi, osg::Z_AXIS );

    Quat att_own_inv = att_own.inverse();
    Quat att_cam_inv = att_cam.inverse();

    UInt32 ownship_id = Data::get()->ownship.ownship_id;

    UInt16 indexE = 0;
    UInt16 indexF = 0;

    Group::List *entities = Entities::instance()->getEntities();
    Group::List::iterator it = entities->begin();

    while ( it != entities->end() && indexE < _maxRadarMarks && indexF < _maxRadarMarks )
    {
        Unit *unit = dynamic_cast< Unit* >(*it);

        if ( unit )
        {
            if ( unit->isActive() && ownship_id != unit->getId() )
            {
                osg::Vec3 r = unit->getPos() - pos_own;

                float dist2 = r.length2();

                // 3 nm = 5556 m
                // 5556^2 = 30869136
                if ( dist2 < 30869136.0f )
                {
                    float x = r.x() * dist_coef;
                    float y = r.y() * dist_coef;

                    if ( Hostile == unit->getAffiliation() )
                    {
                        _switchRadarMarksEnemy->setValue( indexE, true );
                        _patRadarMarksEnemy[ indexE ]->setPosition( osg::Vec3( x, y, 0.0f ) );

                        UnitAerial *aerialUnit = dynamic_cast< UnitAerial* >( unit );

                        if ( aerialUnit != 0
                             &&
                             ( Data::get()->ownship.target_id != unit->getId()
                           || !Data::get()->ownship.target ) )
                        {
                            Vec3 r_enu = unit->getPos() - pos_cam;
                            Vec3 n_box = ( att_cam_inv * ( att_own_inv * r_enu ) );

                            n_box *= 1.0/n_box.length();

                            float box_psi = atan2( -n_box.y(), -n_box.x() );
                            float box_tht = atan2(  n_box.z(), -n_box.x() );

                            if ( box_tht * box_tht + box_psi * box_psi < Ownship::_target_fov_max_2 )
                            {
                                _switchEnemyIndicators->setValue( 2 * indexE     , true  );
                                _switchEnemyIndicators->setValue( 2 * indexE + 1 , false );

                                // box
                                osg::Vec3 r_box( _rad2px * -box_psi,
                                                -_rad2px * -box_tht,
                                                 0.0f );

                                _patEnemyBox.at( indexE )->setPosition( r_box );
                            }
                            else
                            {
                                _switchEnemyIndicators->setValue( 2 * indexE     , false );
                                _switchEnemyIndicators->setValue( 2 * indexE + 1 , true  );

                                float dir_phi = atan2( -n_box.y(),  n_box.z() );

                                _patEnemyDir.at( indexE )->setAttitude( osg::Quat( dir_phi, osg::Z_AXIS ) );
                            }
                        }

                        indexE++;
                    }
                    else
                    {
                        _switchRadarMarksFriend->setValue( indexF, true );
                        _patRadarMarksFriend[ indexF ]->setPosition( osg::Vec3( x, y, 0.0f ) );

                        indexF++;
                    }
                }
            }
        }

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateMessage()
{
    if ( Data::get()->message.visible && ( Data::get()->mission.status == Pending || Data::get()->paused ) )
    {
        _switchMessage->setValue( 0, true );
        _switchMessage->setValue( Data::get()->message.overlay ? 1 : 2, false );
        _switchMessage->setValue( Data::get()->message.overlay ? 2 : 1, true  );

        //_textMessage->setText( osgText::String( String::toUpper( Data::get()->message.text ).c_str() ) );
        _textMessage->setText( osgText::String( Data::get()->message.text, osgText::String::ENCODING_UTF8 ) );
    }
    else
    {
        _switchMessage->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updatePlayerBar()
{
    float sx = (float)Data::get()->ownship.hit_points / 100.0f;
    float dx = 60.0f * sx - 60.0;

    _patPlayerBar->setPosition( osg::Vec3( dx, 0.0f, 0.0f ) );
    _patPlayerBar->setScale( osg::Vec3( sx, 1.0f, 1.0f ) );

    osg::ref_ptr<osg::Vec4Array> c = dynamic_cast< osg::Vec4Array* >( _playerLifeBar->getColorArray() );

    if ( c.valid() )
    {
        c->clear();

        osg::Vec3f upper = Color::lime;
        osg::Vec3f lower = Color::lime * 0.5f;

        if ( Data::get()->ownship.hit_points < 25 )
        {
            upper = Color::red;
            lower = Color::red * 0.5f;
        }
        else if ( Data::get()->ownship.hit_points < 50 )
        {
            upper = Color::yellow;
            lower = Color::yellow * 0.5f;
        }

        c->push_back( osg::Vec4( lower, 1.0f ) );
        c->push_back( osg::Vec4( upper, 1.0f ) );
        c->push_back( osg::Vec4( upper, 1.0f ) );
        c->push_back( osg::Vec4( lower, 1.0f ) );

        _playerLifeBar->setColorArray( c.get() );
    }

    _textPlayerHP->setText( String::toString( Data::get()->ownship.hit_points ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateTargetIndicators()
{
    if ( Data::get()->ownship.target && Data::get()->mission.status == Pending )
    {
        if ( Data::get()->ownship.target_box || Data::get()->ownship.target_cue )
        {
            _switchTargetIndicators->setValue( 0, Data::get()->ownship.target_box );
            _switchTargetIndicators->setValue( 1, Data::get()->ownship.target_cue );

            // bar
            float sx = (float)Data::get()->ownship.target_hp / 100.0f;
            float dx = 5.0f * sx - 5.0;

            _patTargetBar->setPosition( osg::Vec3( dx, 0.0f, 0.0f ) );
            _patTargetBar->setScale( osg::Vec3( sx, 1.0f, 1.0f ) );

            osg::ref_ptr<osg::Vec4Array> c = dynamic_cast< osg::Vec4Array* >( _targetLifeBar->getColorArray() );

            if ( c.valid() )
            {
                c->clear();

                if ( Data::get()->ownship.target_hp < 25 )
                {
                    c->push_back( osg::Vec4( Color::red, 1.0f ) );
                }
                else if ( Data::get()->ownship.target_hp < 50 )
                {
                    c->push_back( osg::Vec4( Color::yellow, 1.0f ) );
                }
                else
                {
                    c->push_back( osg::Vec4( Color::lime, 1.0f ) );
                }

                _targetLifeBar->setColorArray( c.get() );
            }

            // box
            osg::Vec3 r_box( _rad2px * -Data::get()->ownship.target_box_psi,
                            -_rad2px * -Data::get()->ownship.target_box_tht,
                             0.0f );

            _patTargetBox->setPosition( r_box );

            // cue
            osg::Vec3 r_cue( _rad2px * -Data::get()->ownship.target_cue_psi,
                            -_rad2px * -Data::get()->ownship.target_cue_tht,
                             0.0f );

            _patTargetCue->setPosition( r_cue );
        }
        else
        {
            _switchTargetIndicators->setValue( 0, false );
            _switchTargetIndicators->setValue( 1, false );
        }

        if ( !Data::get()->ownship.target_cue )
        {
            _switchTargetIndicators->setValue( 2, true );
            _patTargetDir->setAttitude( osg::Quat( Data::get()->ownship.target_dir_phi, osg::Z_AXIS ) );
        }
        else
        {
            _switchTargetIndicators->setValue( 2, false );
        }
    }
    else
    {
        _switchTargetIndicators->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateTutorialSymbols()
{
    if ( Data::get()->message.visible && ( Data::get()->mission.status == Pending || Data::get()->paused ) )
    {
        // custom pointer
        if ( Data::get()->message.pointer_custom )
        {
            _switchPointerCustom->setAllChildrenOn();

            _patPointerCustom->setPosition( osg::Vec3( Data::get()->message.pointer_x,
                                                       Data::get()->message.pointer_y,
                                                       0.0f ) );
            _patPointerCustom->setAttitude( osg::Quat( Data::get()->message.pointer_phi,
                                                       osg::Z_AXIS ) );
        }
        else
        {
            _switchPointerCustom->setAllChildrenOff();
        }

#       ifndef SIM_DESKTOP
        // RPM pointer
        if ( Data::get()->message.pointer_rpm_dec
          || Data::get()->message.pointer_rpm_inc )
        {
            _switchPointerRpm->setAllChildrenOn();

            float time = _timerTutorial;

            while ( time > 4.0f )
            {
                time -= 4.0f;
            }

            osg::Vec3 pos;
            osg::Vec3 pos0 = osg::Vec3( 0.0f, -50.0f + Data::get()->controls.throttle * 100.0f, 0.0f );
            osg::Vec3 pos1;

            if ( Data::get()->message.pointer_rpm_dec )
            {
                pos1 = osg::Vec3( 0.0f, -50.0f, 0.0 );
            }
            else
            {
                pos1 = osg::Vec3( 0.0f,  50.0f, 0.0 );
            }

            if ( time < 1.0f )
            {
                pos = pos0;
            }
            else if ( time < 3.0f )
            {
                float coef = 1.0f - ( 3.0f - time ) / 2.0f;

                pos = pos0 + ( pos1 - pos0 ) * coef;
            }
            else if ( time < 4.0f )
            {
                pos = pos1;
            }
            else
            {
                pos = pos0;
            }

            _patPointerRpm->setPosition( pos );
        }
        else
        {
            _switchPointerRpm->setAllChildrenOff();
        }
#       endif

        // target pointer
        if ( Data::get()->message.pointer_target )
        {
            _switchPointerTarget->setAllChildrenOn();
        }
        else
        {
            _switchPointerTarget->setAllChildrenOff();
        }

#       ifndef SIM_DESKTOP
        // trigger pointer
        if ( Data::get()->message.pointer_trigger )
        {
            _switchPointerTrigger->setAllChildrenOn();
        }
        else
        {
            _switchPointerTrigger->setAllChildrenOff();
        }

        // tutorial tip
        if ( Data::get()->message.tip != NoTip )
        {
            _switchTutorialTips->setAllChildrenOn();

            std::string textureFile = "";

            switch ( Data::get()->message.tip )
            {
            case BankLeft:
                textureFile = getPath( "textures/hud_tip_bank_l.rgb" );
                break;

            case BankRight:
                textureFile = getPath( "textures/hud_tip_bank_r.rgb" );
                break;

            case PitchUp:
                textureFile = getPath( "textures/hud_tip_pitch_u.rgb" );
                break;

            case PitchDown:
                textureFile = getPath( "textures/hud_tip_pitch_d.rgb" );
                break;

            default:
                textureFile = "";
                _switchTutorialTips->setAllChildrenOff();
                break;
            }

            if ( textureFile.length() > 0 )
            {
                osg::ref_ptr<osg::StateSet> stateSet = _tutorialTip->getOrCreateStateSet();

                stateSet->setTextureAttributeAndModes( 0, Textures::get( textureFile ), osg::StateAttribute::ON );
            }
        }
        else
        {
            _switchTutorialTips->setAllChildrenOff();
        }
#       endif

        _timerTutorial += Data::get()->mission.time_step;
    }
    else
    {
        _switchPointerCustom->setAllChildrenOff();
        _switchPointerTarget->setAllChildrenOff();
#       ifndef SIM_DESKTOP
        _switchPointerRpm->setAllChildrenOff();
        _switchPointerTrigger->setAllChildrenOff();
        _switchTutorialTips->setAllChildrenOff();
#       endif

        _timerTutorial = 0.0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateWaypointIndicators()
{
    if ( Data::get()->ownship.waypoint && Data::get()->mission.status == Pending )
    {
        // waypoint box
        if ( Data::get()->ownship.waypoint_dist >= Gates::_distMax )
        {
            _switchWaypointIndicators->setValue( 0, true );

            osg::Vec3 r_box( _rad2px * -Data::get()->ownship.waypoint_psi,
                            -_rad2px * -Data::get()->ownship.waypoint_tht,
                             0.0f );

            _patWaypointBox->setPosition( r_box );
            _patWaypointBox->setAttitude( osg::Quat( Data::get()->ownship.rollAngle, osg::Z_AXIS ) );
        }
        else
        {
            _switchWaypointIndicators->setValue( 0, false );
        }

        // waypoint dir
        if ( ( fabs( Data::get()->ownship.waypoint_tht ) < _rad11deg && fabs( Data::get()->ownship.waypoint_psi ) < _rad11deg )
          || Data::get()->ownship.waypoint_dist < 50.0f )
        {
            _switchWaypointIndicators->setValue( 1, false );
        }
        else
        {
            _switchWaypointIndicators->setValue( 1, true );

            _patWaypointDir->setAttitude( osg::Quat( Data::get()->ownship.waypoint_phi, osg::Z_AXIS ) );
        }
    }
    else
    {
        _switchWaypointIndicators->setAllChildrenOff();
    }
}
