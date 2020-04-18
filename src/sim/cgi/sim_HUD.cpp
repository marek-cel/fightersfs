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

const UInt16 HUD::m_maxRadarMarks = 16;

const float HUD::m_sizeCaptions  = 17.50f;
const float HUD::m_sizePlayerBar =  8.75f;
const float HUD::m_sizeMessage   =  8.75f;

const float HUD::m_deg2px = 600.0f / 90.0f;
const float HUD::m_rad2px = Convert::rad2deg( HUD::m_deg2px );

const float HUD::m_rad11deg = Convert::deg2rad( 11.0f );

////////////////////////////////////////////////////////////////////////////////

HUD::HUD( float linesWidth, int width, int height ) :
    m_linesWidth ( linesWidth ),

    m_width  ( width ),
    m_height ( height ),

    m_maxX ( floor( 100.0f * (float)( m_width ) / (float)( m_height ) + 0.5f ) ),

    m_tutorial ( false ),

    m_timerTutorial ( 0.0f )
{
    m_root = new osg::Group();

    osg::ref_ptr<osg::StateSet> stateSet = m_root->getOrCreateStateSet();

    stateSet->setMode( GL_RESCALE_NORMAL , osg::StateAttribute::ON  );
    stateSet->setMode( GL_LIGHTING       , osg::StateAttribute::OFF );
    stateSet->setMode( GL_LIGHT0         , osg::StateAttribute::ON  );
    stateSet->setMode( GL_BLEND          , osg::StateAttribute::ON  );
    stateSet->setMode( GL_ALPHA_TEST     , osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST     , osg::StateAttribute::ON  );
    stateSet->setMode( GL_DITHER         , osg::StateAttribute::OFF );
    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );

    m_root->addChild( SplashScreen::create( width, height ) );
}

////////////////////////////////////////////////////////////////////////////////

HUD::~HUD() {}

////////////////////////////////////////////////////////////////////////////////

void HUD::init( bool tutorial )
{
    m_tutorial = tutorial;

    if ( m_root->getNumChildren() > 0 )
    {
        m_root->removeChildren( 0, m_root->getNumChildren() );
    }

    m_switch = new osg::Switch();
    m_root->addChild( m_switch.get() );

    Fonts::reset();

    m_font = Fonts::get( getPath( "fonts/fsg_stencil.ttf" ) );

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

    if ( m_tutorial )
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
            m_switch->setAllChildrenOn();

            updateControls();
            updateCrosshair();
            updateHitIndicator();
            updateIndicators();
            updatePlayerBar();
            updateTargetIndicators();
            updateWaypointIndicators();

            if ( m_tutorial )
            {
                updateTutorialSymbols();
            }
        }
        else
        {
            m_switch->setAllChildrenOff();
        }
    }
    else
    {
        m_switch->setAllChildrenOff();
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
    m_patCaption = new osg::PositionAttitudeTransform();
    m_root->addChild( m_patCaption.get() );

    m_patCaption->setPosition( osg::Vec3( 0.0, 25.0f, -1.0 ) );

    m_switchCaption = new osg::Switch();
    m_patCaption->addChild( m_switchCaption.get() );

    m_textCaption = new osgText::Text();

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    m_switchCaption->addChild( geode.get() );

    if ( m_font.valid() ) m_textCaption->setFont( m_font );
    m_textCaption->setColor( osg::Vec4( Color::red, 0.8f ) );
    m_textCaption->setCharacterSize( m_sizeCaptions );
    m_textCaption->setAxisAlignment( osgText::TextBase::XY_PLANE );
    m_textCaption->setPosition( osg::Vec3( 0.0f, 0.0f, 0.0f ) );
    m_textCaption->setLayout( osgText::Text::LEFT_TO_RIGHT );
    m_textCaption->setAlignment( osgText::Text::CENTER_CENTER );
    m_textCaption->setText( "CAPTION" );

    geode->addDrawable( m_textCaption );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_MSG, "RenderBin" );

    geode->addDrawable( m_textMessage );

    m_switchCaption->setAllChildrenOff();
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SIM_DESKTOP
void HUD::createControls()
{
    // material
    m_materialControls = new osg::Material();
    m_materialControls->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4( Color::white, 0.4f ) );
    m_materialControls->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( Color::white, 0.4f ) );

    createControlsThrottle();
    createControlsTrigger();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createControlsThrottle()
{
    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    m_switch->addChild( pat.get() );

    m_patControlsThrottle = new osg::PositionAttitudeTransform();
    pat->addChild( m_patControlsThrottle.get() );

    pat->setPosition( osg::Vec3( -m_maxX, 10.0, 0.0 ) );

    createControlsThrottleGrip( m_patControlsThrottle.get(), 0.4f );

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

        stateSet->setAttribute( m_materialControls.get() );
        stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
        stateSet->setTextureAttributeAndModes( 0, Textures::get( getPath( "textures/hud_throttle_base.rgb" ) ), osg::StateAttribute::ON );

        geode->addDrawable( geometry.get() );
    }

    // pointer
    if ( m_tutorial )
    {
        m_switchPointerRpm = new osg::Switch();
        pat->addChild( m_switchPointerRpm.get() );

        m_patPointerRpm = new osg::PositionAttitudeTransform();
        m_switchPointerRpm->addChild( m_patPointerRpm.get() );

        createControlsThrottleGrip( m_patPointerRpm.get(), 0.2f, -1.0f );

        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
        m_patPointerRpm->addChild( pat.get() );

        createPointer( pat.get() );

        pat->setAttitude( osg::Quat( M_PI_2, osg::Z_AXIS ) );
        pat->setPosition( osg::Vec3( 52.0f, 0.0f, 0.0f ) );

        m_switchPointerRpm->setAllChildrenOff();
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

    stateSet->setAttribute( m_materialControls.get() );
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
    m_switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( m_maxX - dx - r, 0.0, 0.0 ) );

    m_switchTrigger = new osg::Switch();
    pat->addChild( m_switchTrigger.get() );

    createFace( m_switchTrigger.get(), r, m_materialControls.get(), "textures/hud_trigger_0.rgb", 0.4f );
    createFace( m_switchTrigger.get(), r, m_materialControls.get(), "textures/hud_trigger_1.rgb", 0.4f );

    m_switchTrigger->setSingleChildOn( 0 );

    if ( m_tutorial )
    {
        m_switchPointerTrigger = new osg::Switch();
        pat->addChild( m_switchPointerTrigger.get() );

        osg::ref_ptr<osg::PositionAttitudeTransform> patPointer = new osg::PositionAttitudeTransform();

        m_switchPointerTrigger->addChild( patPointer.get() );

        patPointer->setPosition( osg::Vec3( -33.0f, 0.0f, 0.0f ) );
        patPointer->setAttitude( osg::Quat( -M_PI_2, osg::Z_AXIS ) );

        createPointer( patPointer.get() );

        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        m_switchPointerTrigger->addChild( geode.get() );

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

        m_switchPointerTrigger->setAllChildrenOff();
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////

void HUD::createCrosshair()
{
    const float r = 10.0f;

    m_patCrosshair = new osg::PositionAttitudeTransform();
    m_switch->addChild( m_patCrosshair.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    m_patCrosshair->addChild( geode.get() );

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

    stateSet->setAttribute( m_materialIndicators.get() );
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

    m_patHitIndicator = new osg::PositionAttitudeTransform();
    m_switch->addChild( m_patHitIndicator.get() );

    m_patHitIndicator->setPosition( osg::Vec3( 0.0f, -40.0f, 0.0f ) );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    m_patHitIndicator->addChild( geode.get() );

    m_hitIndicator = new osg::Geometry();
    geode->addDrawable( m_hitIndicator.get() );

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

    m_hitIndicator->setVertexArray( v.get() );
    m_hitIndicator->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP, 0, v->size() ) );

    m_hitIndicator->setColorArray( c.get() );
    m_hitIndicator->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    m_hitIndicator->setNormalArray( n.get() );
    m_hitIndicator->setNormalBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createEnemyIndicators()
{
    m_switchEnemyIndicators = new osg::Switch();
    m_switch->addChild( m_switchEnemyIndicators.get() );

    osg::ref_ptr< osg::Geode > geodeBox = new osg::Geode();
    osg::ref_ptr< osg::Geode > geodeDir = new osg::Geode();

    createBox( geodeBox.get(), osg::Vec4( Color::red, 0.7f ), 0.5f );
    createDir( geodeDir.get(), osg::Vec4( Color::red, 0.7f ) );

    for ( UInt16 i = 0; i < m_maxRadarMarks; i++ )
    {
        osg::ref_ptr<osg::PositionAttitudeTransform> patEnemyBox = new osg::PositionAttitudeTransform();
        m_switchEnemyIndicators->addChild( patEnemyBox.get() );

        m_patEnemyBox.push_back( patEnemyBox.get() );

        osg::ref_ptr<osg::PositionAttitudeTransform> patEnemyDir = new osg::PositionAttitudeTransform();
        m_switchEnemyIndicators->addChild( patEnemyDir.get() );

        m_patEnemyDir.push_back( patEnemyDir.get() );

        patEnemyBox->addChild( geodeBox );
        patEnemyDir->addChild( geodeDir );
    }

    m_switchEnemyIndicators->setAllChildrenOff();
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
    m_geodeRadarMarkEnemy  = new osg::Geode();
    m_geodeRadarMarkFriend = new osg::Geode();

    createRadarMark( m_geodeRadarMarkEnemy.get()  , osg::Vec4( Color::red  , 1.0f ) );
    createRadarMark( m_geodeRadarMarkFriend.get() , osg::Vec4( Color::lime , 1.0f ) );

    // material
    m_materialIndicators = new osg::Material();
    m_materialIndicators->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4( Color::white, 0.7f ) );
    m_materialIndicators->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( Color::white, 0.7f ) );

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
    m_switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( x, y, -1.0f ) );

    createFace( pat.get(), r, m_materialIndicators.get(), "textures/hud_altitude.rgb", 0.7f );

    m_patIndicatorALT1 = new osg::PositionAttitudeTransform();
    m_patIndicatorALT2 = new osg::PositionAttitudeTransform();

    pat->addChild( m_patIndicatorALT1.get() );
    pat->addChild( m_patIndicatorALT2.get() );

    // arrow 1
    {
        const float l = 10.0f;
        const float w =  2.5f;

        createIndicatorHand( m_patIndicatorALT1.get(), l, w );
    }

    // arrow 2
    {
        const float l = 15.0f;
        const float w =  1.5f;

        createIndicatorHand( m_patIndicatorALT2.get(), l, w );
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createIndicatorASI()
{
    const float x = -47.0f;
    const float y = -80.0f;
    const float r =  15.0f;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    m_switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( x, y, -1.0f ) );

    createFace( pat.get(), r, m_materialIndicators.get(), "textures/hud_airspeed.rgb", 0.7f );

    m_patIndicatorASI = new osg::PositionAttitudeTransform();
    pat->addChild( m_patIndicatorASI.get() );

    const float l = 11.0f;
    const float w =  1.5f;

    createIndicatorHand( m_patIndicatorASI.get(), l, w );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createIndicatorRadar()
{
    const float x = -86.0f;
    const float y = -75.0f;
    const float r =  20.0f;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    m_switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( x, y, -1.0f ) );

    m_patIndicatorRadar = new osg::PositionAttitudeTransform();
    pat->addChild( m_patIndicatorRadar.get() );

    m_switchRadarMarksEnemy  = new osg::Switch();
    m_switchRadarMarksFriend = new osg::Switch();

    m_patIndicatorRadar->addChild( m_switchRadarMarksEnemy.get() );
    m_patIndicatorRadar->addChild( m_switchRadarMarksFriend.get() );

    for ( UInt16 i = 0; i < m_maxRadarMarks; i++ )
    {
        osg::ref_ptr<osg::PositionAttitudeTransform> patE = new osg::PositionAttitudeTransform();
        osg::ref_ptr<osg::PositionAttitudeTransform> patF = new osg::PositionAttitudeTransform();

        m_switchRadarMarksEnemy->addChild( patE.get() );
        m_switchRadarMarksFriend->addChild( patF.get() );

        patE->addChild( m_geodeRadarMarkEnemy.get() );
        patF->addChild( m_geodeRadarMarkFriend.get() );

        m_patRadarMarksEnemy.push_back( patE.get() );
        m_patRadarMarksFriend.push_back( patF.get() );
    }

    createFace( pat.get(), r, m_materialIndicators.get(), "textures/hud_radar.rgb", 0.7f );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createIndicatorVSI()
{
    const float x =  47.0f;
    const float y = -80.0f;
    const float r =  15.0f;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    m_switch->addChild( pat.get() );

    pat->setPosition( osg::Vec3( x, y, -1.0f ) );

    createFace( pat.get(), r, m_materialIndicators.get(), "textures/hud_climb_rate.rgb", 0.7f );

    m_patIndicatorVSI = new osg::PositionAttitudeTransform();
    pat->addChild( m_patIndicatorVSI.get() );

    const float l = 11.0f;
    const float w =  1.5f;

    createIndicatorHand( m_patIndicatorVSI.get(), l, w );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createMessage()
{
    m_switchMessage = new osg::Switch();
    m_root->addChild( m_switchMessage.get() );

    m_switchMessage->setAllChildrenOff();

    const float dy = 60.0f;

    const float r =   4.0f;
    const float w = 180.0f;
    const float h =  40.0f;

    const float w_2 = w / 2.0f;
    const float h_2 = h / 2.0f;

    // text
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        m_switchMessage->addChild( geode.get() );

        m_textMessage = new osgText::Text();

        if ( m_font.valid() ) m_textMessage->setFont( m_font );
        m_textMessage->setColor( osg::Vec4( Color::white, 1.0f ) );
        m_textMessage->setCharacterSize( m_sizeMessage );
        m_textMessage->setAxisAlignment( osgText::TextBase::XY_PLANE );
        m_textMessage->setPosition( osg::Vec3( 0.0, dy, -0.5f ) );
        m_textMessage->setLayout( osgText::Text::LEFT_TO_RIGHT );
        m_textMessage->setAlignment( osgText::Text::CENTER_CENTER );
        m_textMessage->setText( "" );

        osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

        stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
        stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_MSG, "RenderBin" );

        geode->addDrawable( m_textMessage );
    }

    // box
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        m_switchMessage->addChild( geode.get() );

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
        m_switchMessage->addChild( geode.get() );

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
    m_patPlayerBar = new osg::PositionAttitudeTransform();
    m_switch->addChild( m_patPlayerBar.get() );

    const float del = 1.0f;

    const float w = 120.0f;
    const float h =   4.0f;
    const float d =  90.0f;

    const float w_2 = w / 2.0f;
    const float h_2 = h / 2.0f;

    // box
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        m_switch->addChild( geode.get() );

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
        m_patPlayerBar->addChild( geode.get() );

        //osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
        m_playerLifeBar = new osg::Geometry();

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

        m_playerLifeBar->setVertexArray( v.get() );
        m_playerLifeBar->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, v->size() ) );

        m_playerLifeBar->setNormalArray( n.get() );
        m_playerLifeBar->setNormalBinding( osg::Geometry::BIND_OVERALL );

//        m_playerLifeBar->setColorArray( c.get() );
//        m_playerLifeBar->setColorBinding( osg::Geometry::BIND_OVERALL );

        m_playerLifeBar->setColorArray( c.get() );
        m_playerLifeBar->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

        geode->addDrawable( m_playerLifeBar.get() );
    }

    // heart
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        m_switch->addChild( geode.get() );

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

        stateSet->setAttribute( m_materialIndicators.get() );
        stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD + 1, "RenderBin" );
        stateSet->setTextureAttributeAndModes( 0, Textures::get( getPath( "textures/hud_heart.rgb" ) ), osg::StateAttribute::ON );

        geode->addDrawable( geometry.get() );
    }

    // text - right
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        m_switch->addChild( geode.get() );

        m_textPlayerHP = new osgText::Text();

        if ( m_font.valid() ) m_textPlayerHP->setFont( m_font );
        m_textPlayerHP->setColor( osg::Vec4( Color::black, 1.0f ) );
        m_textPlayerHP->setCharacterSize( m_sizePlayerBar );
        m_textPlayerHP->setAxisAlignment( osgText::TextBase::XY_PLANE );
        m_textPlayerHP->setPosition( osg::Vec3(  w_2 + 4.0f, d - 3.0f, -1.0f ) );
        m_textPlayerHP->setLayout( osgText::Text::LEFT_TO_RIGHT );
        m_textPlayerHP->setAlignment( osgText::Text::LEFT_BASE_LINE );
        m_textPlayerHP->setText( "100" );

        geode->addDrawable( m_textPlayerHP );
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

    geode->setInitialBound( osg::BoundingSphere( osg::Vec3(0.0f,0.0f,0.0f), m_linesWidth ) );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    osg::ref_ptr<osg::Point> point = new osg::Point();
    point->setSize( m_linesWidth );

    stateSet->setAttribute( point );
//    stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
//    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTargetIndicators()
{
    m_switchTargetIndicators = new osg::Switch();
    m_switch->addChild( m_switchTargetIndicators.get() );

    m_patTargetBox = new osg::PositionAttitudeTransform();
    m_switchTargetIndicators->addChild( m_patTargetBox.get() );

    m_patTargetBar = new osg::PositionAttitudeTransform();
    m_patTargetBox->addChild( m_patTargetBar.get() );

    m_patTargetCue = new osg::PositionAttitudeTransform();
    m_switchTargetIndicators->addChild( m_patTargetCue.get() );

    m_patTargetDir = new osg::PositionAttitudeTransform();
    m_switchTargetIndicators->addChild( m_patTargetDir.get() );

    createTargetIndicatorBar();
    createTargetIndicatorBox();
    createTargetIndicatorCue();
    createTargetIndicatorDir();

    m_switchTargetIndicators->setAllChildrenOff();
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
        m_patTargetBox->addChild( geode.get() );

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
        m_patTargetBar->addChild( geode.get() );

        //osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
        m_targetLifeBar = new osg::Geometry();

        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
        osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

        v->push_back( osg::Vec3( -w_2, -h_2 + d, -2.0f ) ); // 4
        v->push_back( osg::Vec3( -w_2,  h_2 + d, -2.0f ) ); // 1
        v->push_back( osg::Vec3(  w_2,  h_2 + d, -2.0f ) ); // 2
        v->push_back( osg::Vec3(  w_2, -h_2 + d, -2.0f ) ); // 3


        n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

        c->push_back( osg::Vec4( Color::lime, 1.0f ) );

        m_targetLifeBar->setVertexArray( v.get() );
        m_targetLifeBar->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, v->size() ) );

        m_targetLifeBar->setNormalArray( n.get() );
        m_targetLifeBar->setNormalBinding( osg::Geometry::BIND_OVERALL );

        m_targetLifeBar->setColorArray( c.get() );
        m_targetLifeBar->setColorBinding( osg::Geometry::BIND_OVERALL );

        geode->addDrawable( m_targetLifeBar.get() );

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
    m_patTargetBox->addChild( geode.get() );

    createBox( geode.get(), osg::Vec4( Color::red, 0.7f ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTargetIndicatorCue()
{
    const float ro = 3.0f;
    const float ri = 1.0f;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    m_patTargetCue->addChild( geode.get() );

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
    lineWidth->setWidth( m_linesWidth );

    stateSet->setAttributeAndModes( lineWidth, osg::StateAttribute::ON );

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );

    if ( m_tutorial )
    {
        m_switchPointerTarget = new osg::Switch();
        m_patTargetCue->addChild( m_switchPointerTarget.get() );

        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
        m_switchPointerTarget->addChild( pat.get() );

        pat->setPosition( osg::Vec3( 0.0f, -10.0f, 0.0f ) );

        createPointer( pat.get() );

        m_switchPointerTarget->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTargetIndicatorDir()
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    m_patTargetDir->addChild( geode.get() );

    createDir( geode.get(), osg::Vec4( Color::red, 0.7f ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createTutorialSymbols()
{
    m_switchPointerCustom = new osg::Switch();
    m_switch->addChild( m_switchPointerCustom.get() );

    m_patPointerCustom = new osg::PositionAttitudeTransform();
    m_switchPointerCustom->addChild( m_patPointerCustom.get() );

    createPointer( m_patPointerCustom.get() );

    m_switchPointerCustom->setAllChildrenOff();

#   ifndef SIM_DESKTOP
    createTutorialTips();
#   endif
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SIM_DESKTOP
void HUD::createTutorialTips()
{
    m_switchTutorialTips = new osg::Switch();
    m_switch->addChild( m_switchTutorialTips.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    m_switchTutorialTips->addChild( geode.get() );

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
        m_tutorialTip = new osg::Geometry();
        geode->addDrawable( m_tutorialTip.get() );

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

        m_tutorialTip->setVertexArray( v.get() );
        m_tutorialTip->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN, 0, v->size() ) );

        m_tutorialTip->setTexCoordArray( 0, t.get() );

        m_tutorialTip->setNormalArray( n.get() );
        m_tutorialTip->setNormalBinding( osg::Geometry::BIND_OVERALL );

        m_tutorialTip->setColorArray( c.get() );
        m_tutorialTip->setColorBinding( osg::Geometry::BIND_OVERALL );
    }

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setMode( GL_BLEND      , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_HUD, "RenderBin" );

    m_switchTutorialTips->setAllChildrenOff();
}
#endif

////////////////////////////////////////////////////////////////////////////////

void HUD::createWaypointIndicators()
{
    m_switchWaypointIndicators = new osg::Switch();
    m_switch->addChild( m_switchWaypointIndicators.get() );

    m_patWaypointBox = new osg::PositionAttitudeTransform();
    m_switchWaypointIndicators->addChild( m_patWaypointBox.get() );

    m_patWaypointDir = new osg::PositionAttitudeTransform();
    m_switchWaypointIndicators->addChild( m_patWaypointDir.get() );

    createWaypointIndicatorBox();
    createWaypointIndicatorDir();

    m_switchWaypointIndicators->setAllChildrenOff();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::createWaypointIndicatorBox()
{
    const float size = Convert::rad2deg( atan( Gates::m_size / ( 2.0f * Gates::m_distScale ) ) ) * 1200.0f / 90.0f;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    m_patWaypointBox->addChild( geode.get() );

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
    lineWidth->setWidth( m_linesWidth );

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
    m_patWaypointDir->addChild( geode.get() );

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
            m_switchCaption->setAllChildrenOn();
            m_textCaption->setText( osgText::String( Captions::instance()->getMissionAccomplished(), osgText::String::ENCODING_UTF8 ) );
        }
        else if ( Data::get()->mission.status == Failure )
        {
            m_switchCaption->setAllChildrenOn();
            m_textCaption->setText( osgText::String( Captions::instance()->getMissionFailed(), osgText::String::ENCODING_UTF8 ) );
        }

        coef = Data::get()->mission.time_end;

        if ( coef > 1.0f ) coef = 1.0f;
    }
    else if ( !Data::get()->paused )
    {
        float time = 0.0f;

        if ( Data::get()->ownship.friend_hit < 1.0f )
        {
            m_switchCaption->setAllChildrenOn();
            m_textCaption->setText( osgText::String( Captions::instance()->getFriendlyFire(), osgText::String::ENCODING_UTF8 ) );

            time = Data::get()->ownship.friend_hit;
        }
        else if ( Data::get()->ownship.target_kill < 1.0f )
        {
            m_switchCaption->setAllChildrenOn();
            m_textCaption->setText( osgText::String( Captions::instance()->getTargetKilled(), osgText::String::ENCODING_UTF8 ) );

            time = Data::get()->ownship.target_kill;
        }
        else if ( Data::get()->ownship.target_hit < 1.0f )
        {
            m_switchCaption->setAllChildrenOn();
            m_textCaption->setText( osgText::String( Captions::instance()->getTargetHit(), osgText::String::ENCODING_UTF8 ) );

            time = Data::get()->ownship.target_hit;
        }
        else if ( Data::get()->mission.time_left < 30.0f && Data::get()->mission.time_left > 0.0f )
        {
            m_switchCaption->setAllChildrenOn();

            int s = floor( Data::get()->mission.time_left );
            int m = std::max( 0, std::min( 999, (int)( 1000 * ( Data::get()->mission.time_left - (float)s ) ) ) );
            int c = m / 10;

            if ( m - 10 * c == 1 ) m += 1;

            char text[ 16 ];

            sprintf( text, "00:%02d.%03d", s, m );

            m_textCaption->setText( text );
        }
        else
        {
            m_switchCaption->setAllChildrenOff();
        }

        coef = 1.0f - time;
    }
    else
    {
        m_switchCaption->setAllChildrenOff();
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

    m_textCaption->setColor( osg::Vec4( Color::red, 0.8f * coef ) );
    m_patCaption->setScale( osg::Vec3( scale, scale, scale ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateControls()
{
#   ifndef SIM_DESKTOP
    if ( Data::get()->controls.trigger )
    {
        m_switchTrigger->setSingleChildOn( 1 );
    }
    else
    {
        m_switchTrigger->setSingleChildOn( 0 );
    }

    m_patControlsThrottle->setPosition( osg::Vec3( 0.0f, -50.0f + Data::get()->controls.throttle * 100.0f, 0.0f ) );
#   endif
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateCrosshair()
{
    osg::Vec3 r( m_rad2px * Data::get()->camera.d_psi,
                -m_rad2px * Data::get()->camera.d_tht,
                 0.0f );

    m_patCrosshair->setPosition( r );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateHitIndicator()
{
    float coef = 1.0f - Data::get()->ownship.ownship_hit;

    if ( coef < 0.0f ) coef = 0.0f;

    osg::ref_ptr<osg::Vec4Array> c = dynamic_cast< osg::Vec4Array* >( m_hitIndicator->getColorArray() );

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

        m_hitIndicator->setColorArray( c.get() );
    }

    float scale = 0.8f + 0.2f * coef;

    osg::Vec3 pos( 1.2 * m_rad2px * Data::get()->camera.d_psi, -40.0f, 0.0f );

    m_patHitIndicator->setPosition( pos );
    m_patHitIndicator->setAttitude( osg::Quat( Data::get()->ownship.hit_direction, osg::Z_AXIS ) );
    m_patHitIndicator->setScale( osg::Vec3( scale, scale, scale ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateIndicators()
{
    // ALT
    float altitude1 = Convert::m2ft( Data::get()->ownship.altitude_asl ) / 10000.0f;
    float altitude2 = 10.0f * ( altitude1 - floor( altitude1 ) );

    altitude1 *= 2.0f * M_PI;
    altitude2 *= 2.0f * M_PI;

    m_patIndicatorALT1->setAttitude( osg::Quat( -altitude1, osg::Z_AXIS ) );
    m_patIndicatorALT2->setAttitude( osg::Quat( -altitude2, osg::Z_AXIS ) );

    // ASI
    float airspeed = Convert::mps2kts( Data::get()->ownship.airspeed ) / 100.0;
    airspeed *= M_PI_2;

    m_patIndicatorASI->setAttitude( osg::Quat( -airspeed, osg::Z_AXIS ) );

    // VSI
    float climbRate = Convert::mps2fpm( Data::get()->ownship.climbRate ) / 1000.0;
    if ( climbRate >  4.3f ) climbRate =  4.3f;
    if ( climbRate < -4.3f ) climbRate = -4.3f;
    climbRate *= Convert::deg2rad( 40.0 );

    m_patIndicatorVSI->setAttitude( osg::Quat( M_PI_2 - climbRate, osg::Z_AXIS ) );

    // radar
    updateIndicatorRadar();
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateIndicatorRadar()
{
    // 1 nm = 1852 m
    // 3 nm = 5556 m
    const float dist_coef = 16.0f / 5556.0f;

    m_patIndicatorRadar->setAttitude( osg::Quat( Data::get()->ownship.heading, osg::Z_AXIS ) );

    m_switchRadarMarksEnemy->setAllChildrenOff();
    m_switchRadarMarksFriend->setAllChildrenOff();
    m_switchEnemyIndicators->setAllChildrenOff();

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

    while ( it != entities->end() && indexE < m_maxRadarMarks && indexF < m_maxRadarMarks )
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
                        m_switchRadarMarksEnemy->setValue( indexE, true );
                        m_patRadarMarksEnemy[ indexE ]->setPosition( osg::Vec3( x, y, 0.0f ) );

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

                            if ( box_tht * box_tht + box_psi * box_psi < Ownship::m_target_fov_max_2 )
                            {
                                m_switchEnemyIndicators->setValue( 2 * indexE     , true  );
                                m_switchEnemyIndicators->setValue( 2 * indexE + 1 , false );

                                // box
                                osg::Vec3 r_box( m_rad2px * -box_psi,
                                                -m_rad2px * -box_tht,
                                                 0.0f );

                                m_patEnemyBox.at( indexE )->setPosition( r_box );
                            }
                            else
                            {
                                m_switchEnemyIndicators->setValue( 2 * indexE     , false );
                                m_switchEnemyIndicators->setValue( 2 * indexE + 1 , true  );

                                float dir_phi = atan2( -n_box.y(),  n_box.z() );

                                m_patEnemyDir.at( indexE )->setAttitude( osg::Quat( dir_phi, osg::Z_AXIS ) );
                            }
                        }

                        indexE++;
                    }
                    else
                    {
                        m_switchRadarMarksFriend->setValue( indexF, true );
                        m_patRadarMarksFriend[ indexF ]->setPosition( osg::Vec3( x, y, 0.0f ) );

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
        m_switchMessage->setValue( 0, true );
        m_switchMessage->setValue( Data::get()->message.overlay ? 1 : 2, false );
        m_switchMessage->setValue( Data::get()->message.overlay ? 2 : 1, true  );

        //m_textMessage->setText( osgText::String( String::toUpper( Data::get()->message.text ).c_str() ) );
        m_textMessage->setText( osgText::String( Data::get()->message.text, osgText::String::ENCODING_UTF8 ) );
    }
    else
    {
        m_switchMessage->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updatePlayerBar()
{
    float sx = (float)Data::get()->ownship.hit_points / 100.0f;
    float dx = 60.0f * sx - 60.0;

    m_patPlayerBar->setPosition( osg::Vec3( dx, 0.0f, 0.0f ) );
    m_patPlayerBar->setScale( osg::Vec3( sx, 1.0f, 1.0f ) );

    osg::ref_ptr<osg::Vec4Array> c = dynamic_cast< osg::Vec4Array* >( m_playerLifeBar->getColorArray() );

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

        m_playerLifeBar->setColorArray( c.get() );
    }

    m_textPlayerHP->setText( String::toString( Data::get()->ownship.hit_points ) );
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateTargetIndicators()
{
    if ( Data::get()->ownship.target && Data::get()->mission.status == Pending )
    {
        if ( Data::get()->ownship.target_box || Data::get()->ownship.target_cue )
        {
            m_switchTargetIndicators->setValue( 0, Data::get()->ownship.target_box );
            m_switchTargetIndicators->setValue( 1, Data::get()->ownship.target_cue );

            // bar
            float sx = (float)Data::get()->ownship.target_hp / 100.0f;
            float dx = 5.0f * sx - 5.0;

            m_patTargetBar->setPosition( osg::Vec3( dx, 0.0f, 0.0f ) );
            m_patTargetBar->setScale( osg::Vec3( sx, 1.0f, 1.0f ) );

            osg::ref_ptr<osg::Vec4Array> c = dynamic_cast< osg::Vec4Array* >( m_targetLifeBar->getColorArray() );

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

                m_targetLifeBar->setColorArray( c.get() );
            }

            // box
            osg::Vec3 r_box( m_rad2px * -Data::get()->ownship.target_box_psi,
                            -m_rad2px * -Data::get()->ownship.target_box_tht,
                             0.0f );

            m_patTargetBox->setPosition( r_box );

            // cue
            osg::Vec3 r_cue( m_rad2px * -Data::get()->ownship.target_cue_psi,
                            -m_rad2px * -Data::get()->ownship.target_cue_tht,
                             0.0f );

            m_patTargetCue->setPosition( r_cue );
        }
        else
        {
            m_switchTargetIndicators->setValue( 0, false );
            m_switchTargetIndicators->setValue( 1, false );
        }

        if ( !Data::get()->ownship.target_cue )
        {
            m_switchTargetIndicators->setValue( 2, true );
            m_patTargetDir->setAttitude( osg::Quat( Data::get()->ownship.target_dir_phi, osg::Z_AXIS ) );
        }
        else
        {
            m_switchTargetIndicators->setValue( 2, false );
        }
    }
    else
    {
        m_switchTargetIndicators->setAllChildrenOff();
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
            m_switchPointerCustom->setAllChildrenOn();

            m_patPointerCustom->setPosition( osg::Vec3( Data::get()->message.pointer_x,
                                                        Data::get()->message.pointer_y,
                                                        0.0f ) );
            m_patPointerCustom->setAttitude( osg::Quat( Data::get()->message.pointer_phi,
                                                        osg::Z_AXIS ) );
        }
        else
        {
            m_switchPointerCustom->setAllChildrenOff();
        }

#       ifndef SIM_DESKTOP
        // RPM pointer
        if ( Data::get()->message.pointer_rpm_dec
          || Data::get()->message.pointer_rpm_inc )
        {
            m_switchPointerRpm->setAllChildrenOn();

            float time = m_timerTutorial;

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

            m_patPointerRpm->setPosition( pos );
        }
        else
        {
            m_switchPointerRpm->setAllChildrenOff();
        }
#       endif

        // target pointer
        if ( Data::get()->message.pointer_target )
        {
            m_switchPointerTarget->setAllChildrenOn();
        }
        else
        {
            m_switchPointerTarget->setAllChildrenOff();
        }

#       ifndef SIM_DESKTOP
        // trigger pointer
        if ( Data::get()->message.pointer_trigger )
        {
            m_switchPointerTrigger->setAllChildrenOn();
        }
        else
        {
            m_switchPointerTrigger->setAllChildrenOff();
        }

        // tutorial tip
        if ( Data::get()->message.tip != NoTip )
        {
            m_switchTutorialTips->setAllChildrenOn();

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
                m_switchTutorialTips->setAllChildrenOff();
                break;
            }

            if ( textureFile.length() > 0 )
            {
                osg::ref_ptr<osg::StateSet> stateSet = m_tutorialTip->getOrCreateStateSet();

                stateSet->setTextureAttributeAndModes( 0, Textures::get( textureFile ), osg::StateAttribute::ON );
            }
        }
        else
        {
            m_switchTutorialTips->setAllChildrenOff();
        }
#       endif

        m_timerTutorial += Data::get()->mission.time_step;
    }
    else
    {
        m_switchPointerCustom->setAllChildrenOff();
        m_switchPointerTarget->setAllChildrenOff();
#       ifndef SIM_DESKTOP
        m_switchPointerRpm->setAllChildrenOff();
        m_switchPointerTrigger->setAllChildrenOff();
        m_switchTutorialTips->setAllChildrenOff();
#       endif

        m_timerTutorial = 0.0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void HUD::updateWaypointIndicators()
{
    if ( Data::get()->ownship.waypoint && Data::get()->mission.status == Pending )
    {
        // waypoint box
        if ( Data::get()->ownship.waypoint_dist >= Gates::m_distMax )
        {
            m_switchWaypointIndicators->setValue( 0, true );

            osg::Vec3 r_box( m_rad2px * -Data::get()->ownship.waypoint_psi,
                            -m_rad2px * -Data::get()->ownship.waypoint_tht,
                             0.0f );

            m_patWaypointBox->setPosition( r_box );
            m_patWaypointBox->setAttitude( osg::Quat( Data::get()->ownship.rollAngle, osg::Z_AXIS ) );
        }
        else
        {
            m_switchWaypointIndicators->setValue( 0, false );
        }

        // waypoint dir
        if ( ( fabs( Data::get()->ownship.waypoint_tht ) < m_rad11deg && fabs( Data::get()->ownship.waypoint_psi ) < m_rad11deg )
          || Data::get()->ownship.waypoint_dist < 50.0f )
        {
            m_switchWaypointIndicators->setValue( 1, false );
        }
        else
        {
            m_switchWaypointIndicators->setValue( 1, true );

            m_patWaypointDir->setAttitude( osg::Quat( Data::get()->ownship.waypoint_phi, osg::Z_AXIS ) );
        }
    }
    else
    {
        m_switchWaypointIndicators->setAllChildrenOff();
    }
}
