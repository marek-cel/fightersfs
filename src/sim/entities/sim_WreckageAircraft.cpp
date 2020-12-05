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

#include <sim/entities/sim_WreckageAircraft.h>

#include <sim/sim_Elevation.h>
#include <sim/sim_Log.h>
#include <sim/cgi/sim_Models.h>
#include <sim/cgi/sim_Textures.h>
#include <sim/entities/sim_Explosion.h>
#include <sim/utils/sim_Inertia.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

WreckageAircraft::WreckageAircraft( const std::string &modelFile, const std::string &livery,
                                    osgParticle::SmokeEffect *smokeTrail, bool ownship ) :
    Wreckage( 0, 10.0f ),

    _ownship ( ownship ),

    _smokeTrail ( smokeTrail ),

    _livery ( livery ),
    _modelFile ( modelFile ),

    _rollRate ( 0.5f ),

    _altitude_agl ( std::numeric_limits< float >::max() ),
    _elevation    ( 0.0f )
{
    loadModel();
    loadLivery();

    if ( !_smokeTrail.valid() )
    {
        _smokeTrail = Effects::createSmokeTrail();
    }

    _root->addChild( _smokeTrail.get() );

    _switchFire = new osg::Switch();
    _pat->addChild( _switchFire.get() );

    createFire();

    _omg.x() = 0.2 * M_PI_2;

//    osg::ref_ptr<osg::Node> modelNode = model;

//    if ( modelNode.valid() )
//    {
//        _switch->addChild( modelNode.get() );

//        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
//        _switch->addChild( pat.get() );

//        pat->setAttitude( osg::Quat( M_PI_2, osg::Y_AXIS ) );
//    }
}

////////////////////////////////////////////////////////////////////////////////

WreckageAircraft::~WreckageAircraft() {}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::init( const Vec3 &pos,
                             const Quat &att,
                             const Vec3 &vel,
                             const Vec3 &omg )
{
    setPos( pos );
    setAtt( att );
    setVel( vel );
    setOmg( omg );

    _rollRate = ( _angles.phi() > 0.0 ) ? 0.5f : -0.5f;
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::load()
{
    /////////////////
    Wreckage::load();
    /////////////////

    if ( _model.valid() )
    {
        _switch->removeChild( _model.get() );
    }

    loadModel();
    loadLivery();
    createFire();
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::update( double timeStep )
{
    ///////////////////////////
    Entity::update( timeStep );
    ///////////////////////////

    if ( isActive() )
    {
        float altitude_agl = _pos.z() - _elevation;

        if ( altitude_agl <= 0.0f && _altitude_agl > 0.0f )
        {
            _switch->setValue( 0, false );
            _switchFire->setAllChildrenOff();

            Explosion *explosion = new Explosion( 20.0f );
            explosion->setPos( _pos );
            explosion->setAtt( _att );

            if ( _smokeTrail.valid() )
            {
                _smokeTrail->setEmitterDuration( 0.0 );
            }
        }

        _altitude_agl = altitude_agl;

        if ( _altitude_agl > 0.0 )
        {
            resetLifeTime();
        }

        if ( _smokeTrail.valid() )
        {
            _smokeTrail->setPosition( _pos );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::setState( State state )
{
    //////////////////////////
    Entity::setState( state );
    //////////////////////////

    _state = state;

    if ( _state == Active )
    {
        _switchFire->setAllChildrenOn();
    }
    else
    {
        _switchFire->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::updateElevation()
{
    _elevation = Elevation::instance()->getElevation( _pos.x(), _pos.y() );
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::updateVelocity()
{
    if ( _altitude_agl > 0.0f )
    {
        float v = _vel.length();

        Vec3 acc = _att.inverse() * Vec3( 0.0, 0.0, -SIM_GRAVITY_ACC )
                 - ( _vel * ( 1.0f/v ) ) * 0.01f * ( v*v );

        _vel += ( acc - ( _omg ^ _vel ) ) * _timeStep;

        _omg.x() = Inertia< float >::update( _timeStep, _omg.x(), _rollRate, 0.5f );

        // tangent to velocity vector
        _omg.y() = -atan2( -_vel.z(), -_vel.x() ) / _timeStep;
        _omg.z() = -atan2(  _vel.y(), -_vel.x() ) / _timeStep;
    }
    else
    {
        _vel.x() = 0.0;
        _vel.y() = 0.0;
        _vel.z() = 0.0;

        _omg.x() = 0.0;
        _omg.y() = 0.0;
        _omg.z() = 0.0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::createFire()
{
    if ( _switchFire->getNumChildren() > 0 )
    {
        _switchFire->removeChildren( 0, _switchFire->getNumChildren() );
    }

    osg::ref_ptr<osg::PositionAttitudeTransform> patFire = new osg::PositionAttitudeTransform();
    _switchFire->addChild( patFire.get() );

    patFire->setAttitude( osg::Quat( M_PI_2, osg::Y_AXIS ) );
    patFire->addChild( Effects::createSmoke( 3.0f, 1.2f, 0.4f, 0.1f ) );

#   ifdef SIM_TEST
    patFire->addChild( Effects::createFlames( getPath( "textures/fire_1.rgb" ).c_str() ) );
    patFire->addChild( Effects::createFlames( getPath( "textures/fire_2.rgb" ).c_str() ) );
#   else
    if ( _ownship )
    {
        patFire->addChild( Effects::createFlames( getPath( "textures/fire_1.rgb" ).c_str() ) );
        patFire->addChild( Effects::createFlames( getPath( "textures/fire_2.rgb" ).c_str() ) );
    }
#   endif
}

////////////////////////////////////////////////////////////////////////////////

osg::Node* WreckageAircraft::getModel( const std::string &modelFile )
{
    return Models::readNodeFile( getPath( modelFile ) );
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::loadLivery()
{
    if ( _model.valid() )
    {
        osg::ref_ptr<osg::Texture2D> texture = Textures::get( getPath( _livery ), 8.0f );

        if ( texture.valid() )
        {
            osg::ref_ptr<osg::StateSet> stateSet = _model->getOrCreateStateSet();
            stateSet->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
        }
    }
    else
    {
        Log::e() << "Model node does not exist." << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::loadModel()
{
    std::string aircraftFile = getPath( _modelFile );

    _model = Models::readNodeFile( aircraftFile );

    if ( _model.valid() )
    {
        _switch->addChild( _model.get() );
    }
    else
    {
        Log::e() << "Cannot open aircraft file: " << aircraftFile << std::endl;
    }
}
