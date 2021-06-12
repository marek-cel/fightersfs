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

std::string SkyDome::_skyDomeFile = "textures/sky_0.rgb";
float SkyDome::_sunCoef = 1.0f;

////////////////////////////////////////////////////////////////////////////////

SkyDome::SkyDome( Module *parent ) :
    Module( new osg::Group(), parent )
{
    _patSky = new osg::PositionAttitudeTransform();
    _root->addChild( _patSky.get() );

    _patSun = new osg::PositionAttitudeTransform();
    _patSky->addChild( _patSun.get() );

#   ifdef SIM_TEST
    _switch = new osg::Switch();
    _patSky->addChild( _switch.get() );
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
        _switch->setAllChildrenOff();
    }
    else
    {
        _switch->setAllChildrenOn();
    }
#   endif

    _patSky->setPosition( osg::Vec3d( Data::get()->camera.pos_x,
                                      Data::get()->camera.pos_y,
                                      0.0 ) );
}

////////////////////////////////////////////////////////////////////////////////

void SkyDome::createSky()
{
    osg::ref_ptr<osg::Geode> geodeSky = new osg::Geode();

#   ifdef SIM_TEST
    _switch->addChild( geodeSky.get() );
#   else
    _patSky->addChild( geodeSky.get() );
#   endif

    std::string textureFile = getPath( _skyDomeFile );

    createDome( geodeSky, Textures::get( textureFile ), SIM_SKYDOME_RAD );

    geodeSky->getOrCreateStateSet()->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_SKY, "DepthSortedBin" );
}

////////////////////////////////////////////////////////////////////////////////

void SkyDome::createSun()
{
    _patSun->setAttitude( osg::Quat( M_PI_2, osg::X_AXIS,
                                    -M_PI_4, osg::Y_AXIS,
                                    -M_PI_4, osg::Z_AXIS ) );

    osg::ref_ptr<osg::LightSource> lightSourceSun = new osg::LightSource();
    _patSun->addChild( lightSourceSun.get() );

    osg::ref_ptr<osg::Light> lightSun = new osg::Light();

    lightSun->setLightNum( SIM_LIGHT_SUN_NUM );

    lightSun->setPosition( osg::Vec4d( SIM_SKYDOME_RAD, 0.0f, 0.0f, 0.0f ) );

    float sunCoef_2 = _sunCoef * _sunCoef;

    osg::Vec4 colorAmbient  = osg::Vec4( Color::sun * _sunCoef   , 1.0f );
    osg::Vec4 colorDiffuse  = osg::Vec4( Color::sun *  sunCoef_2 , 1.0f );
    osg::Vec4 colorSpecular = osg::Vec4( Color::sun *  sunCoef_2 , 1.0f );

    lightSun->setAmbient( colorAmbient );
    lightSun->setDiffuse( colorDiffuse );
    lightSun->setSpecular( colorSpecular );

    lightSun->setConstantAttenuation( 1.0 );

    lightSourceSun->setLight( lightSun.get() );

    lightSourceSun->setLocalStateSetModes( osg::StateAttribute::ON );
    lightSourceSun->setStateSetModes( *_patSun->getOrCreateStateSet(), osg::StateAttribute::ON );

//    if ( 0 )
//    {
//        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
//        _patSun->addChild( geode.get() );

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
