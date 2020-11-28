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

    m_ownship ( ownship ),

    m_smokeTrail ( smokeTrail ),

    m_livery ( livery ),
    m_modelFile ( modelFile ),

    m_rollRate ( 0.5f ),

    m_altitude_agl ( std::numeric_limits< float >::max() ),
    m_elevation    ( 0.0f )
{
    loadModel();
    loadLivery();

    if ( !m_smokeTrail.valid() )
    {
        m_smokeTrail = Effects::createSmokeTrail();
    }

    m_root->addChild( m_smokeTrail.get() );

    m_switchFire = new osg::Switch();
    _pat->addChild( m_switchFire.get() );

    createFire();

    _omg.x() = 0.2 * M_PI_2;

//    osg::ref_ptr<osg::Node> modelNode = model;

//    if ( modelNode.valid() )
//    {
//        m_switch->addChild( modelNode.get() );

//        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
//        m_switch->addChild( pat.get() );

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

    m_rollRate = ( _angles.phi() > 0.0 ) ? 0.5f : -0.5f;
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::load()
{
    /////////////////
    Wreckage::load();
    /////////////////

    if ( m_model.valid() )
    {
        _switch->removeChild( m_model.get() );
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
        float altitude_agl = _pos.z() - m_elevation;

        if ( altitude_agl <= 0.0f && m_altitude_agl > 0.0f )
        {
            _switch->setValue( 0, false );
            m_switchFire->setAllChildrenOff();

            Explosion *explosion = new Explosion( 20.0f );
            explosion->setPos( _pos );
            explosion->setAtt( _att );

            if ( m_smokeTrail.valid() )
            {
                m_smokeTrail->setEmitterDuration( 0.0 );
            }
        }

        m_altitude_agl = altitude_agl;

        if ( m_altitude_agl > 0.0 )
        {
            resetLifeTime();
        }

        if ( m_smokeTrail.valid() )
        {
            m_smokeTrail->setPosition( _pos );
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
        m_switchFire->setAllChildrenOn();
    }
    else
    {
        m_switchFire->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::updateElevation()
{
    m_elevation = Elevation::instance()->getElevation( _pos.x(), _pos.y() );
}

////////////////////////////////////////////////////////////////////////////////

void WreckageAircraft::updateVelocity()
{
    if ( m_altitude_agl > 0.0f )
    {
        float v = _vel.length();

        Vec3 acc = _att.inverse() * Vec3( 0.0, 0.0, -SIM_GRAVITY_ACC )
                 - ( _vel * ( 1.0f/v ) ) * 0.01f * ( v*v );

        _vel += ( acc - ( _omg ^ _vel ) ) * _timeStep;

        _omg.x() = Inertia< float >::update( _timeStep, _omg.x(), m_rollRate, 0.5f );

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
    if ( m_switchFire->getNumChildren() > 0 )
    {
        m_switchFire->removeChildren( 0, m_switchFire->getNumChildren() );
    }

    osg::ref_ptr<osg::PositionAttitudeTransform> patFire = new osg::PositionAttitudeTransform();
    m_switchFire->addChild( patFire.get() );

    patFire->setAttitude( osg::Quat( M_PI_2, osg::Y_AXIS ) );
    patFire->addChild( Effects::createSmoke( 3.0f, 1.2f, 0.4f, 0.1f ) );

#   ifdef SIM_TEST
    patFire->addChild( Effects::createFlames( getPath( "textures/fire_1.rgb" ).c_str() ) );
    patFire->addChild( Effects::createFlames( getPath( "textures/fire_2.rgb" ).c_str() ) );
#   else
    if ( m_ownship )
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
    if ( m_model.valid() )
    {
        osg::ref_ptr<osg::Texture2D> texture = Textures::get( getPath( m_livery ), 8.0f );

        if ( texture.valid() )
        {
            osg::ref_ptr<osg::StateSet> stateSet = m_model->getOrCreateStateSet();
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
    std::string aircraftFile = getPath( m_modelFile );

    m_model = Models::readNodeFile( aircraftFile );

    if ( m_model.valid() )
    {
        _switch->addChild( m_model.get() );
    }
    else
    {
        Log::e() << "Cannot open aircraft file: " << aircraftFile << std::endl;
    }
}
