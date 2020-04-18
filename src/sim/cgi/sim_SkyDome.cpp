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

#include <sim/cgi/sim_SkyDome.h>

#include <math.h>
#include <stdio.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Material>
//#include <osg/ShapeDrawable>

#include <sim/cgi/sim_Color.h>
#include <sim/cgi/sim_Geometry.h>
#include <sim/cgi/sim_Textures.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

std::string SkyDome::m_skyDomeFile = "textures/sky_0.rgb";
float SkyDome::m_sunCoef = 1.0f;

////////////////////////////////////////////////////////////////////////////////

SkyDome::SkyDome( Module *parent ) :
    Module( new osg::Group(), parent )
{
    m_patSky = new osg::PositionAttitudeTransform();
    m_root->addChild( m_patSky.get() );

    m_patSun = new osg::PositionAttitudeTransform();
    m_patSky->addChild( m_patSun.get() );

#   ifdef SIM_TEST
    m_switch = new osg::Switch();
    m_patSky->addChild( m_switch.get() );
#   endif

    createSky();
    createSun();
}

////////////////////////////////////////////////////////////////////////////////

SkyDome::~SkyDome() {}

////////////////////////////////////////////////////////////////////////////////

void SkyDome::update()
{
    /////////////////
    Module::update();
    /////////////////

#   ifdef SIM_TEST
    if ( Data::get()->camera.type == ViewWorld )
    {
        m_switch->setAllChildrenOff();
    }
    else
    {
        m_switch->setAllChildrenOn();
    }
#   endif

    m_patSky->setPosition( osg::Vec3d( Data::get()->camera.pos_x,
                                       Data::get()->camera.pos_y,
                                       0.0 ) );
}

////////////////////////////////////////////////////////////////////////////////

void SkyDome::createSky()
{
    osg::ref_ptr<osg::Geode> geodeSky = new osg::Geode();

#   ifdef SIM_TEST
    m_switch->addChild( geodeSky.get() );
#   else
    m_patSky->addChild( geodeSky.get() );
#   endif

    std::string textureFile = getPath( m_skyDomeFile );

    createDome( geodeSky, Textures::get( textureFile ), SIM_SKYDOME_RAD );

    geodeSky->getOrCreateStateSet()->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_SKY, "DepthSortedBin" );
}

////////////////////////////////////////////////////////////////////////////////

void SkyDome::createSun()
{
    m_patSun->setAttitude( osg::Quat( M_PI_2, osg::X_AXIS,
                                     -M_PI_4, osg::Y_AXIS,
                                     -M_PI_4, osg::Z_AXIS ) );

    osg::ref_ptr<osg::LightSource> lightSourceSun = new osg::LightSource();
    m_patSun->addChild( lightSourceSun.get() );

    osg::ref_ptr<osg::Light> lightSun = new osg::Light();

    lightSun->setLightNum( SIM_LIGHT_SUN_NUM );

    lightSun->setPosition( osg::Vec4d( SIM_SKYDOME_RAD, 0.0f, 0.0f, 0.0f ) );

    float sunCoef_2 = m_sunCoef * m_sunCoef;

    osg::Vec4 colorAmbient  = osg::Vec4( Color::sun * m_sunCoef, 1.0f );
    osg::Vec4 colorDiffuse  = osg::Vec4( Color::sun * sunCoef_2, 1.0f );
    osg::Vec4 colorSpecular = osg::Vec4( Color::sun * sunCoef_2, 1.0f );

    lightSun->setAmbient( colorAmbient );
    lightSun->setDiffuse( colorDiffuse );
    lightSun->setSpecular( colorSpecular );

    lightSun->setConstantAttenuation( 1.0 );

    lightSourceSun->setLight( lightSun.get() );

    lightSourceSun->setLocalStateSetModes( osg::StateAttribute::ON );
    lightSourceSun->setStateSetModes( *m_patSun->getOrCreateStateSet(), osg::StateAttribute::ON );

//    if ( 0 )
//    {
//        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
//        m_patSun->addChild( geode.get() );

//        osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable();
//        shape->setShape( new osg::Sphere(osg::Vec3( SIM_SKYDOME_RAD, 0, 0 ), 100.0 ) );
//        shape->setColor( osg::Vec4( 1.0, 0.0, 0.0, 1.0 ) );

//        geode->addDrawable( shape.get() );
//    }
}

////////////////////////////////////////////////////////////////////////////////

void SkyDome::createDome( osg::Geode *dome, osg::Texture2D *texture,
                          float radius, bool blend )
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    dome->addDrawable( geom.get() );

    Geometry::createDome( geom, radius, true, Geometry::Azimuthal, 18, 36 );

    // state set
    osg::ref_ptr<osg::StateSet> stateSet = dome->getOrCreateStateSet();
    stateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    // texture
    if ( texture != 0 )
    {
        if ( blend )
        {
            stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
            stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        }

        stateSet->setTextureAttributeAndModes( 0, texture, osg::StateAttribute::ON );
    }

    // material
    osg::ref_ptr<osg::Material> material = new osg::Material();

    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
    material->setAmbient( osg::Material::FRONT, osg::Vec4( Color::white, 1.0f ) );
    material->setDiffuse( osg::Material::FRONT, osg::Vec4( Color::white, 1.0f ) );

    stateSet->setAttribute( material.get() );
}
