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

#include <sim/cgi/sim_Viewer.h>

#include <osg/Fog>
#include <osg/LightSource>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>

#include <sim/sim_ListUnits.h>
#include <sim/sim_Log.h>

#include <sim/cgi/sim_Color.h>
#include <sim/cgi/sim_FindNode.h>
#include <sim/cgi/sim_Geometry.h>
#include <sim/cgi/sim_Models.h>
#include <sim/cgi/sim_Textures.h>

#ifdef SIM_DESKTOP
#   include <sim/entities/sim_UnitMarine.h>
#endif

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Viewer::Viewer( int width, int height ):
    Module( new osg::Group(), NULLPTR ),

    _distCoef ( std::max( ( 4.0f * (float)height ) / ( 3.0f * (float)width ), 1.0f ) ),

    _index ( 0 ),

    _prop_angle ( 0.0 ),
    _real_time  ( 0.0 )
{
    _patUnit  = new osg::PositionAttitudeTransform();
    _patModel = new osg::PositionAttitudeTransform();

    osg::ref_ptr<osg::StateSet> stateSet = _root->getOrCreateStateSet();

    stateSet->setMode( GL_RESCALE_NORMAL , osg::StateAttribute::ON  );
    stateSet->setMode( GL_LIGHTING       , osg::StateAttribute::ON  );
    stateSet->setMode( GL_LIGHT0         , osg::StateAttribute::ON  );
    stateSet->setMode( GL_BLEND          , osg::StateAttribute::ON  );
    stateSet->setMode( GL_ALPHA_TEST     , osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST     , osg::StateAttribute::ON  );
    stateSet->setMode( GL_DITHER         , osg::StateAttribute::OFF );

    _manipulatorOrbit = new ManipulatorOrbit();

    load();
}

////////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer() {}

////////////////////////////////////////////////////////////////////////////////

void Viewer::load()
{
    if ( _root->getNumChildren() > 0 )
    {
        _root->removeChildren( 0, _root->getNumChildren() );
    }

    _root->addChild( _patUnit.get() );
    _root->addChild( _patModel.get() );

    _propeller1 = 0;
    _propeller2 = 0;
    _propeller3 = 0;
    _propeller4 = 0;

    createSky();
    createSun();

    if ( _index < ListUnits::instance()->getCount() )
    {
        if ( ListUnits::instance()->getData( _index ).type == ListUnits::Marine )
        {
            createOcean();
        }
        else
        {
            createGround();
        }
    }
    else
    {
        createGround();
    }

    loadUnit();
}

////////////////////////////////////////////////////////////////////////////////

void Viewer::resetViewer()
{
    double distance = 35.0;
    double elevation = osg::DegreesToRadians( 10.0 );
    double heading = -M_PI_2;

    if ( _index < ListUnits::instance()->getCount() )
    {
        distance = ListUnits::instance()->getData( _index ).distance;
        heading  = -M_PI_2 + osg::DegreesToRadians( ListUnits::instance()->getData( _index ).heading );
    }

    _manipulatorOrbit->setDistance( _distCoef * distance );
    _manipulatorOrbit->setElevation( elevation );
    _manipulatorOrbit->setHeading( heading );
}

////////////////////////////////////////////////////////////////////////////////

void Viewer::update( double timeStep )
{
    /////////////////
    Module::update();
    /////////////////

    _prop_angle = _prop_angle + timeStep * 2.0 * ( 2.0 * M_PI ) * ( 0.5 + 0.5 * 0.25 );
    _real_time += timeStep;

    if ( _propeller1.valid() ) _propeller1->setAttitude( Quat( -_prop_angle, osg::X_AXIS ) );
    if ( _propeller2.valid() ) _propeller2->setAttitude( Quat(  _prop_angle, osg::X_AXIS ) );
    if ( _propeller3.valid() ) _propeller3->setAttitude( Quat( -_prop_angle, osg::X_AXIS ) );
    if ( _propeller4.valid() ) _propeller4->setAttitude( Quat(  _prop_angle, osg::X_AXIS ) );

    if ( _index < ListUnits::instance()->getCount() )
    {
        ListUnits::UnitData data = ListUnits::instance()->getData( _index );

        if ( data.type == ListUnits::Aerial )
        {
            double phi = osg::DegreesToRadians( 2.0 ) * sin( 2.0 * M_PI * _real_time / 10.0 );
            double tht = osg::DegreesToRadians( 1.0 ) * sin( 2.0 * M_PI * _real_time / 12.0 );
            double psi = osg::DegreesToRadians( 1.0 ) * sin( 2.0 * M_PI * _real_time / 12.0 );

            _att.makeRotate( phi, osg::X_AXIS,
                             tht, osg::Y_AXIS,
                             psi, osg::Z_AXIS );

            _pos += ( _att * Vec3( -10.0, 0.0, 0.0 ) ) * timeStep;
            _pos.x() = 0.0;
        }
        else
        {
            _att.makeRotate( 0.0, osg::X_AXIS );
            _pos = Vec3( 0.0, 0.0, 0.0 );
        }

        _patModel->setPosition( _pos );
        _patModel->setAttitude( _att );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Viewer::setUnit( UInt32 index )
{
    if ( index < ListUnits::instance()->getCount() )
    {
        _index = index;
        load();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Viewer::createSky()
{
    osg::ref_ptr<osg::Geode> geodeSky = new osg::Geode();
    _root->addChild( geodeSky.get() );

    createDome( geodeSky, Textures::get( getPath( "textures/sky_1.rgb" ) ), 2048.0f );

    geodeSky->getOrCreateStateSet()->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_SKY, "DepthSortedBin" );
}

////////////////////////////////////////////////////////////////////////////////

void Viewer::createSun()
{
    osg::ref_ptr<osg::PositionAttitudeTransform> patSun = new osg::PositionAttitudeTransform();
    _root->addChild( patSun.get() );

    patSun->setAttitude( osg::Quat( -M_PI_2 - M_PI_4, osg::Z_AXIS, M_PI_4, osg::Y_AXIS, 0.0, osg::X_AXIS ) );

    osg::ref_ptr<osg::LightSource> lightSourceSun = new osg::LightSource();
    patSun->addChild( lightSourceSun.get() );

    osg::ref_ptr<osg::Light> lightSun = new osg::Light();

    lightSun->setLightNum( SIM_LIGHT_SUN_NUM );

    lightSun->setPosition( osg::Vec4d( 2048.0f, 0.0f, 0.0f, 0.0f ) );

    lightSun->setAmbient( osg::Vec4( Color::sun, 1.0f ) );
    lightSun->setDiffuse( osg::Vec4( Color::sun, 1.0f ) );
    lightSun->setSpecular( osg::Vec4( Color::sun, 1.0f ) );

    lightSun->setConstantAttenuation( 1.0 );

    lightSourceSun->setLight( lightSun.get() );

    lightSourceSun->setLocalStateSetModes( osg::StateAttribute::ON );
    lightSourceSun->setStateSetModes( *_root->getOrCreateStateSet(), osg::StateAttribute::ON );
}

////////////////////////////////////////////////////////////////////////////////

void Viewer::createDome( osg::Geode *dome, osg::Texture2D *texture,
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

////////////////////////////////////////////////////////////////////////////////

void Viewer::createGround()
{
    createPlane( "textures/grass.rgb" );
}

////////////////////////////////////////////////////////////////////////////////

void Viewer::createOcean()
{
    createPlane( "textures/water.rgb" );
}

////////////////////////////////////////////////////////////////////////////////

void Viewer::createPlane( const std::string &textureFile )
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _root->addChild( geode.get() );

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    geode->addDrawable( geom.get() );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    Geometry::createPlane( geom.get(), 4096.0, 16, -2048.0, -2048.0, true, 4 );

    osg::ref_ptr<osg::Texture2D> texture = Textures::get( getPath( textureFile ), 1.0f, osg::Texture::REPEAT );

    if ( texture != 0 )
    {
        stateSet->setTextureAttributeAndModes( 0, texture, osg::StateAttribute::ON );
    }

    osg::ref_ptr<osg::Fog> fog = new osg::Fog();
    fog->setMode( osg::Fog::LINEAR );
    fog->setDensity( 0.5f );
    fog->setColor( osg::Vec4( Color::fog_light, 0.0f ) );
    fog->setStart( 0.0f );
    fog->setEnd( 1920.0f );

    stateSet->setAttributeAndModes( fog.get(), osg::StateAttribute::ON );
    stateSet->setMode( GL_FOG, osg::StateAttribute::ON );
}

////////////////////////////////////////////////////////////////////////////////

void Viewer::loadUnit()
{
    if ( _patModel->getNumChildren() > 0 )
    {
        _patModel->removeChildren( 0, _patModel->getNumChildren() );
    }

    if ( _index < ListUnits::instance()->getCount() )
    {
        ListUnits::UnitData data = ListUnits::instance()->getData( _index );

        std::string unitFile = getPath( data.model );

        osg::ref_ptr<osg::Node> model = Models::readNodeFile( unitFile );

        if ( model.valid() )
        {
            _patModel->addChild( model.get() );

            _manipulatorOrbit->setTrackNode( _patUnit.get() );

            Vec3 pos( data.offset_x, 0.0, 0.0 );

            if ( data.type == ListUnits::Aerial )
            {
                pos.z() = 100.0;

                _pos = Vec3( 0.0, 0.0, pos.z() );

                _patModel->setPosition( _pos );

                if ( data.texture.length() > 0 )
                {
                    osg::ref_ptr<osg::Texture2D> texture = Textures::get( getPath( data.texture ), 8.0f );

                    if ( texture.valid() )
                    {
                        model->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
                    }
                }
            }
            else if ( data.type == ListUnits::Marine )
            {
#               ifdef SIM_DESKTOP
                UnitMarine::createReflection( model.get(), _root.get() );
#               endif
            }
            else if ( data.type == ListUnits::Ground )
            {
                if ( data.texture.length() > 0 )
                {
                    osg::ref_ptr<osg::Texture2D> texture = Textures::get( getPath( data.texture ), 8.0f );

                    if ( texture.valid() )
                    {
                        model->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
                    }
                }
            }

            _patUnit->setPosition( pos );

            _propeller1 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( model, "Propeller1" ) );
            _propeller2 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( model, "Propeller2" ) );
            _propeller3 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( model, "Propeller3" ) );
            _propeller4 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( model, "Propeller4" ) );

            if ( !_propeller1.valid() || !_propeller2.valid() )
            {
                _propeller1 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( model, "PropellerL" ) );
                _propeller2 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( model, "PropellerR" ) );
            }

            if ( !_propeller1.valid() )
            {
                _propeller1 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( model, "Propeller" ) );
            }
        }
        else
        {
            Log::e() << "Cannot open aircraft file: " << unitFile << std::endl;
        }
    }
    else
    {
        Log::e() << "Wrong unit index: " << _index << std::endl;
    }
}
