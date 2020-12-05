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

#include <sim/entities/sim_Aircraft.h>

#include <osg/Light>
#include <osg/LightSource>

#include <sim/cgi/sim_Effects.h>
#include <sim/cgi/sim_FindNode.h>
#include <sim/cgi/sim_Models.h>

#include <sim/entities/sim_Explosion.h>
#include <sim/entities/sim_Tracer.h>

#include <sim/sim_Elevation.h>
#include <sim/sim_Ownship.h>

#include <sim/utils/sim_Inertia.h>
#include <sim/utils/sim_Misc.h>
#include <sim/utils/sim_String.h>
#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const std::string Aircraft::_tagName = "aircraft";

const float Aircraft::_threshold_vel = 1.0f;
const float Aircraft::_waypoint_dist = 27.0f;

////////////////////////////////////////////////////////////////////////////////

Aircraft::Aircraft( Affiliation affiliation ) :
    UnitAerial( affiliation ),

    _wreckage ( 0 ),

    _time_p ( 0.0f ),
    _time_q ( 0.0f ),
    _time_r ( 0.0f ),
    _time_v ( 0.0f ),

    _speed_min ( 0.0f ),
    _speed_max ( 0.0f ),

    _roll_max ( 0.0f ),
    _turn_max ( 0.0f ),

    _pid_phi ( 0 ),

    _pid_p ( 0 ),
    _pid_q ( 0 ),
    _pid_r ( 0 ),

    _enroute ( true ),
    _wingman ( false ),

    _wingmenCount ( 0 ),
    _leaderId ( 0 ),
    _wingmanOffset ( 50.0, 50.0, 0.0 ),
    _leaderValid ( false ),
    _formation ( false ),

    _waypointIndex ( 0 ),

    _destDist  ( 0.0f ),
    _destBear  ( 0.0f ),
    _destElev  ( 0.0f ),
    _destValid ( false ),

    _ctrlRoll  ( 0.0f ),
    _ctrlPitch ( 0.0f ),
    _ctrlYaw   ( 0.0f ),
    _throttle  ( 0.0f ),

    _initThrottle ( 0.0f ),

    _prop_angle ( 0.0f ),
    _prop_speed ( 0.0f ),

    _maxAilerons ( 0.0f ),
    _maxElevator ( 0.0f ),
    _maxRudder   ( 0.0f ),

    _unique ( false ),

    _trigger ( false ),

    _flash_count   ( 0 ),
    _flash_devider ( 2 ),
    _flash_angle ( 0.0f ),

    _time_drop   ( 1000.0f ),
    _time_launch ( 1000.0f ),
    _time_shoot  ( 1000.0f ),

    _elevation ( 0.0f ),

    _altitude_asl  ( 0.0f ),
    _altitude_agl  ( 0.0f ),
    _airspeed      ( 0.0f ),
    _climbRate     ( 0.0f ),
    _machNumber    ( 0.0f ),
    _angleOfAttack ( 0.0f ),
    _sideslipAngle ( 0.0f ),
    _pathAngle     ( 0.0f ),
    _rollAngle     ( 0.0f ),
    _pitchAngle    ( 0.0f ),
    _heading       ( 0.0f ),
    _turnRate      ( 0.0f )
{
    _flashSwitch = new osg::Switch();
    _switch->addChild( _flashSwitch.get() );

    _pid_phi = new PID( 2.0f, 0.2f, 0.1f, -SIM_AIRCRAFT_MAX_PHI, SIM_AIRCRAFT_MAX_PHI, true, 0.8f );

    _pid_p = new PID( 1.0f, 0.1f, 0.05f, -1.0f, 1.0f, true );
    _pid_q = new PID( 1.0f, 0.1f, 0.05f, -1.0f, 1.0f, true );
    _pid_r = new PID( 1.0f, 0.1f, 0.05f, -1.0f, 1.0f, true );
}

////////////////////////////////////////////////////////////////////////////////

Aircraft::~Aircraft()
{
    DELPTR( _pid_phi );

    DELPTR( _pid_p );
    DELPTR( _pid_q );
    DELPTR( _pid_r );
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::destroy()
{
    if ( isActive() )
    {
        Explosion *explosion = new Explosion( 5.0f );
        explosion->setPos( _pos );
        explosion->setAtt( _att );

        _wreckage = new WreckageAircraft( _modelFile, _livery, _smokeTrail.get(), _ownship );
        _wreckage->init( _pos, _att, _vel, _omg );

        if ( _ownship )
        {
            Ownship::instance()->setWreckage( _wreckage );
        }

        if ( _wingman )
        {
            Aircraft *leader = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( _leaderId ) );

            if ( leader )
            {
                leader->wingmenDecrement();

                if ( _wingmenCount > 0 )
                {
                    List entities = *Entities::instance()->getEntities();
                    List::iterator it = entities.begin();

                    while ( it != entities.end() )
                    {
                        Aircraft *wingman = dynamic_cast< Aircraft* >(*it);

                        if ( wingman )
                        {
                            if ( wingman->isActive() && wingman->getWingman()
                              && wingman->getLeaderId() == _id )
                            {
                                wingman->setLeader( _leaderId );
                                wingman->setOffset( _wingmanOffset + wingman->_wingmanOffset );
                            }
                        }

                        ++it;
                    }
                }
            }
        }
    }

    //////////////////////
    UnitAerial::destroy();
    //////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::load()
{
    if ( _flashSwitch->getNumChildren() > 0 )
    {
        _flashSwitch->removeChildren( 0, _flashSwitch->getNumChildren() );
    }

    _flashPAT.clear();

    ///////////////
    Entity::load(); // sic!
    ///////////////

    if ( _model.valid() )
    {
        _switch->removeChild( _model.get() );
    }

    if ( _unique || _ownship )
    {
        _model = Models::readNodeFile( getPath( _modelFile ) );
    }
    else
    {
        _model = Models::get( getPath( _modelFile ) );
    }

    if ( _model.valid() )
    {
        _modelStateSet = _model->getOrCreateStateSet();
        _switch->addChild( _model.get() );

        _aileronL = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "AileronL" ) );
        _aileronR = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "AileronR" ) );

        _elevator = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "Elevator" ) );

        _rudderL = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "RudderL" ) );
        _rudderR = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "RudderR" ) );

        if ( !_rudderL.valid() )
        {
            _rudderL = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "Rudder" ) );
        }

        _propeller1 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "Propeller1" ) );
        _propeller2 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "Propeller2" ) );
        _propeller3 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "Propeller3" ) );
        _propeller4 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "Propeller4" ) );

        if ( !_propeller1.valid() || !_propeller2.valid() )
        {
            _propeller1 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "PropellerL" ) );
            _propeller2 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "PropellerR" ) );
        }

        if ( !_propeller1.valid() )
        {
            _propeller1 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( _model, "Propeller" ) );
        }
    }

    setLivery( _livery );

    if ( _ownship )
    {
        createMuzzleFlash( _flashes, Vec3( 0.5, 0.5, 0.5 ) );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::makeAutomatic()
{
    ////////////////////////////
    UnitAerial::makeAutomatic();
    ////////////////////////////

    _unique = false;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::makeOwnship()
{
    //////////////////////////
    UnitAerial::makeOwnship();
    //////////////////////////

    _unique = true;

    Ownship::instance()->setAircraft( this );

    initOwnship();
}

////////////////////////////////////////////////////////////////////////////////

int Aircraft::read( const XmlNode &node )
{
    //////////////////////////////////////
    int result = UnitAerial::read( node );
    //////////////////////////////////////

    if ( result == SIM_SUCCESS )
    {
        XmlNode nodeData = node.getFirstChildElement( "aircraft" );

        if ( nodeData.isValid() )
        {
            XmlNode nodeTimeP = nodeData.getFirstChildElement( "time_p" );
            XmlNode nodeTimeQ = nodeData.getFirstChildElement( "time_q" );
            XmlNode nodeTimeR = nodeData.getFirstChildElement( "time_r" );
            XmlNode nodeTimeV = nodeData.getFirstChildElement( "time_v" );
            XmlNode nodeSpeedMin = nodeData.getFirstChildElement( "speed_min" );
            XmlNode nodeSpeedMax = nodeData.getFirstChildElement( "speed_max" );
            XmlNode nodeRollMax = nodeData.getFirstChildElement( "roll_max" );

            if ( nodeTimeP.isValid() && nodeTimeQ.isValid() && nodeTimeR.isValid()
              && nodeTimeV.isValid() && nodeSpeedMin.isValid() && nodeSpeedMax.isValid() )
            {
                float time_p = 0.0f;
                float time_q = 0.0f;
                float time_r = 0.0f;
                float time_v = 0.0f;
                float speed_min = 0.0f;
                float speed_max = 0.0f;
                float roll_max = 0.0f;

                if ( SIM_SUCCESS == XmlUtils::read( nodeTimeP, time_p )
                  && SIM_SUCCESS == XmlUtils::read( nodeTimeQ, time_q )
                  && SIM_SUCCESS == XmlUtils::read( nodeTimeR, time_r )
                  && SIM_SUCCESS == XmlUtils::read( nodeTimeV, time_v )
                  && SIM_SUCCESS == XmlUtils::read( nodeSpeedMin, speed_min )
                  && SIM_SUCCESS == XmlUtils::read( nodeSpeedMax, speed_max )
                  && SIM_SUCCESS == XmlUtils::read( nodeRollMax, roll_max ) )
                {
                    _time_p = time_p;
                    _time_q = time_q;
                    _time_r = time_r;
                    _time_v = time_v;
                    _speed_min = speed_min;
                    _speed_max = speed_max;

                    roll_max = Convert::deg2rad( roll_max );

                    if ( roll_max > SIM_AIRCRAFT_MAX_PHI )
                    {
                        roll_max = SIM_AIRCRAFT_MAX_PHI;
                    }

                    _roll_max = roll_max;
                    _turn_max = SIM_GRAVITY_ACC * tan( _roll_max ) / _speed_min;

                    _pid_phi->setMin( -roll_max );
                    _pid_phi->setMax(  roll_max );

                    XmlNode nodeMaxAilerons = nodeData.getFirstChildElement( "max_ailerons" );
                    XmlNode nodeMaxElevator = nodeData.getFirstChildElement( "max_elevator" );
                    XmlNode nodeMaxRudder   = nodeData.getFirstChildElement( "max_rudder"   );

                    float maxAilerons = 0.0f;
                    float maxElevator = 0.0f;
                    float maxRudder   = 0.0f;

                    if ( SIM_SUCCESS == XmlUtils::read( nodeMaxAilerons , maxAilerons ) )
                    {
                        _maxAilerons = Convert::deg2rad( maxAilerons );
                    }

                    if ( SIM_SUCCESS == XmlUtils::read( nodeMaxElevator , maxElevator ) )
                    {
                        _maxElevator = Convert::deg2rad( maxElevator );
                    }

                    if ( SIM_SUCCESS == XmlUtils::read( nodeMaxRudder, maxRudder ) )
                    {
                        _maxRudder = Convert::deg2rad( maxRudder );
                    }

                    XmlNode nodeFlash  = nodeData.getFirstChildElement( "flash" );
                    XmlNode nodeMuzzle = nodeData.getFirstChildElement( "muzzle" );

                    Vec3 pos;

                    while ( nodeFlash.isValid() )
                    {
                        if ( SIM_SUCCESS == XmlUtils::read( nodeFlash, pos ) )
                        {
                            Flash flash;

                            flash.pos = pos;
                            flash.light = String::toBool( nodeFlash.getAttribute( "light" ) );

                            _flashes.push_back( flash );
                        }

                        nodeFlash = nodeFlash.getNextSiblingElement( "flash" );
                    }

                    while ( nodeMuzzle.isValid() )
                    {
                        if ( SIM_SUCCESS == XmlUtils::read( nodeMuzzle, pos ) )
                        {
                            Muzzle muzzle;

                            muzzle.pos = pos;
                            muzzle.trail = String::toBool( nodeMuzzle.getAttribute( "trail" ) );;

                            _muzzles.push_back( muzzle );
                        }

                        nodeMuzzle = nodeMuzzle.getNextSiblingElement( "muzzle" );
                    }

                    return SIM_SUCCESS;
                }
            }
        }
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::update( double timeStep )
{
    ///////////////////////////////
    UnitAerial::update( timeStep );
    ///////////////////////////////

    if ( isActive() )
    {
        if ( _ownship )
        {
            if ( timeStep > 0.03 )
            {
                _flash_devider = 2;
            }
            else
            {
                _flash_devider = 3;
            }

            float ailerons = _maxAilerons * _ctrlRoll;
            float elevator = _maxElevator * _ctrlPitch;
            float rudder   = _maxRudder   * _ctrlYaw;

            // ailerons
            if ( _aileronL.valid() && _aileronR.valid() )
            {
                _aileronL->setAttitude( Quat( -ailerons, osg::Y_AXIS ) );
                _aileronR->setAttitude( Quat(  ailerons, osg::Y_AXIS ) );
            }

            // elevator
            if ( _elevator.valid() )
            {
                _elevator->setAttitude( Quat( -elevator, osg::Y_AXIS ) );
            }

            // rudder
            if ( _rudderL.valid() && _rudderR.valid() )
            {
                _rudderL->setAttitude( Quat( -rudder, osg::Z_AXIS ) );
                _rudderR->setAttitude( Quat( -rudder, osg::Z_AXIS ) );
            }
        }

        // propellers
        if ( _propeller1.valid() ) _propeller1->setAttitude( Quat( -_prop_angle, osg::X_AXIS ) );
        if ( _propeller2.valid() ) _propeller2->setAttitude( Quat(  _prop_angle, osg::X_AXIS ) );
        if ( _propeller3.valid() ) _propeller3->setAttitude( Quat( -_prop_angle, osg::X_AXIS ) );
        if ( _propeller4.valid() ) _propeller4->setAttitude( Quat(  _prop_angle, osg::X_AXIS ) );

        if ( _ownship && !Data::get()->controls.autopilot )
        {
            _ctrlRoll  = Ownship::instance()->getCtrlRoll();
            _ctrlPitch = Ownship::instance()->getCtrlPitch();
            _ctrlYaw   = Ownship::instance()->getCtrlYaw();
            _throttle  = Ownship::instance()->getThrottle();
        }
        else
        {
            updateControls();
            updateWingman();
        }

        updateDestination();
        updatePropeller();

        if ( _altitude_agl < 1.0 || ( _ownship && _altitude_agl < fabs( sin( _rollAngle ) ) * _radius ) )
        {
            destroy();
        }

        if ( _smokeTrail.valid() )
        {
            _smokeTrail->setPosition( _pos );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::wingmenIncrement()
{
    _wingmenCount++;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::wingmenDecrement()
{
    if ( _wingmenCount > 0 )
    {
        _wingmenCount--;
    }
}

////////////////////////////////////////////////////////////////////////////////

Unit* Aircraft::getTarget() const
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

float Aircraft::getSpeed( float throttle )
{
    return _speed_min + ( _speed_max - _speed_min ) * throttle;
}

////////////////////////////////////////////////////////////////////////////////

float Aircraft::getThrottle( float speed )
{
    float throttle = ( speed - _speed_min ) / ( _speed_max - _speed_min );

    if ( throttle < 0.0f ) throttle = 0.0f;
    if ( throttle > 1.0f ) throttle = 1.0f;

    return throttle;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setLeader( UInt32 leaderId )
{
    if ( _id != leaderId )
    {
        if ( _leaderValid )
        {
            Aircraft *leader = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( _leaderId ) );

            if ( leader )
            {
                leader->wingmenDecrement();
            }
        }

        Aircraft *leader = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( leaderId ) );

        if ( leader )
        {
            _leaderId = leaderId;
            _leaderValid = true;

            setRoute( &leader->getRoute() );

            leader->wingmenIncrement();

            _wingman = true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setOffset( const Vec3 &offset )
{
    _wingmanOffset = offset;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setLivery( const std::string &livery )
{
    _livery = livery;

    osg::ref_ptr<osg::Texture2D> texture = Textures::get( getPath( _livery ), 8.0f );

    if ( texture.valid() && _modelStateSet.valid() )
    {
        _modelStateSet->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setRoute( const Route *route )
{
    _waypointIndex = 0;

    if ( route != 0 )
    {
        _route = *route;

        if ( _route.size() > 0 )
        {
            _destination = _route[ 0 ];

            _waypointIndex = 0;
            _destValid = true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setHP( UInt16 hp )
{
    ////////////////////////
    UnitAerial::setHP( hp );
    ////////////////////////

    if ( _hp < 25 )
    {
        if ( !_smokeTrail.valid() )
        {
            _smokeTrail = Effects::createSmokeTrail();

            _root->addChild( _smokeTrail.get() );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setUnique( bool unique )
{
    _unique = unique;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::createMuzzleFlash( const Flashes &flashes, osg::Vec3 scale )
{
    // flash model
    osg::ref_ptr<osg::Node> muzzleFlashNode = Models::get( getPath( "entities/flash.osgb" ) );

    if ( muzzleFlashNode.valid() )
    {
        int lightCount = 0;

        for ( unsigned int i = 0; i < flashes.size(); i++ )
        {
            osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
            _flashSwitch->addChild( pat.get() );

            pat->getOrCreateStateSet()->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_OTHER, "DepthSortedBin" );

            pat->addChild( muzzleFlashNode.get() );

            pat->setScale( scale );
            pat->setPosition( flashes.at( i ).pos );

            _flashPAT.push_back( pat.get() );

            if ( flashes.at( i ).light && lightCount < 2 )
            {
                osg::ref_ptr<osg::PositionAttitudeTransform> patLight = new osg::PositionAttitudeTransform();
                _switch->addChild( patLight.get() );

                osg::ref_ptr<osg::Light> light = new osg::Light();
                osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();

                light->setLightNum( lightCount + 1 );

                light->setPosition( osg::Vec4( 0.0f, 0.0f, 0.0f, 1.0f ) );
                light->setDiffuse( osg::Vec4( 1.0f, 1.0f, 0.8f, 1.0f ) );

                light->setConstantAttenuation( 0.0f );
                light->setLinearAttenuation( 0.05f );
                light->setQuadraticAttenuation( 0.2f );

                lightSource->setLight( light.get() );

                patLight->setPosition( flashes.at( i ).pos + osg::Vec3d( 0.2, 0.0, 0.3 ) );
                patLight->addChild( lightSource.get() );

                lightCount++;
            }
        }
    }

    _flashSwitch->setAllChildrenOff();
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::avoidCollisions( float &phi, float &tht )
{
    const float dist2_limit = 16.0f * _radius2;

    float dist2 = std::numeric_limits< float >::max();

    UnitAerial *closestUnit = 0;

    List entities = *Entities::instance()->getEntities();
    List::iterator it = entities.begin();

    while ( it != entities.end() )
    {
        UnitAerial *unit = dynamic_cast< UnitAerial* >(*it);

        if ( unit )
        {
            if ( unit->isActive() && unit->getId() != _id )
            {
                float dist2_new = ( unit->getPos() - _pos ).length2();

                if ( dist2_new < dist2 )
                {
                    dist2 = dist2_new;
                    closestUnit = unit;
                }
            }
        }

        ++it;
    }

    if ( closestUnit && dist2 < dist2_limit )
    {
        float coef = sqrt( 1.0f - dist2 / dist2_limit );

        Vec3 pos_bas = _att.inverse() * ( closestUnit->getPos() - _pos );

        if ( pos_bas.y() > 0.0 )
        {
            phi =  SIM_AIRCRAFT_MAX_PHI * coef;
        }
        else
        {
            phi = -SIM_AIRCRAFT_MAX_PHI * coef;
        }

        if ( pos_bas.z() > 0.0 )
        {
            tht = -SIM_AIRCRAFT_MAX_THT * coef;
        }
        else
        {
            tht =  SIM_AIRCRAFT_MAX_THT * coef;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::initOwnship()
{
    _initThrottle = getThrottle( _vel.length() );
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::limitTht( float &tht )
{
    if ( _altitude_agl < 100.0 )
    {
        tht += sqrt( 1.0 - _altitude_agl / 100.0 ) * SIM_AIRCRAFT_MAX_THT;
    }

    if      ( tht < -SIM_AIRCRAFT_MAX_THT ) tht = -SIM_AIRCRAFT_MAX_THT;
    else if ( tht >  SIM_AIRCRAFT_MAX_THT ) tht =  SIM_AIRCRAFT_MAX_THT;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::timeIntegration()
{
    //////////////////////////////
    UnitAerial::timeIntegration();
    //////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateControls()
{
    float setpnt_phi = 0.0f;
    float setpnt_tht = 0.0f;

    float sinPhi = sin( _angles.phi() );
    float cosPhi = cos( _angles.phi() );

    if ( _destValid )
    {
        float diff_bear = _destBear - _angles.psi();

        while ( diff_bear < -M_PI ) diff_bear += 2.0f * M_PI;
        while ( diff_bear >  M_PI ) diff_bear -= 2.0f * M_PI;

        _pid_phi->update( _timeStep, diff_bear );

        setpnt_phi = _pid_phi->getValue();
        setpnt_tht = _destElev;
    }

    limitTht( setpnt_tht );

    avoidCollisions( setpnt_phi, setpnt_tht );

    float turnRate = _angles.phi() * _turn_max / _roll_max;

    float diff_tht = setpnt_tht - _angles.tht();
    float diff_turn = 4.0f * ( turnRate - _turnRate );

    _pid_p->update( _timeStep, setpnt_phi - _angles.phi() );
    _pid_q->update( _timeStep, sinPhi * diff_turn + cosPhi * diff_tht );
    _pid_r->update( _timeStep, cosPhi * diff_turn - sinPhi * diff_tht );

    _ctrlRoll  = Inertia< float >::update( _timeStep, _ctrlRoll  , _pid_p->getValue(), 0.1 );
    _ctrlPitch = Inertia< float >::update( _timeStep, _ctrlPitch , _pid_q->getValue(), 0.1 );
    _ctrlYaw   = Inertia< float >::update( _timeStep, _ctrlYaw   , _pid_r->getValue(), 0.1 );
    _throttle  = getThrottle( _destination.second );
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateDestination()
{
    if ( _destValid )
    {
        Vec3 r_enu = _destination.first - getPos();

        double r_xy = sqrt( r_enu.x()*r_enu.x() + r_enu.y()*r_enu.y() );

        _destDist = r_enu.length();
        _destBear = atan2( r_enu.y(), r_enu.x() ) - M_PI;
        _destElev = atan2( r_enu.z(), r_xy );

        if ( _enroute && _route.size() > 0 )
        {
            float destDist = _destDist;

            if ( _wingman )
            {
                destDist = ( _route[ _waypointIndex ].first - getPos() ).length();
            }

            if ( destDist < _waypoint_dist )
            {
                _waypointIndex++;

                if ( _waypointIndex < _route.size() )
                {
                    _destination = _route[ _waypointIndex ];
                }
                else
                {
                    _destValid = false;
                }
            }
            else
            {
                if ( _waypointIndex + 1 < _route.size() )
                {
                    Vec3 v1 = _route[ _waypointIndex + 1 ].first - _route[ _waypointIndex ].first;
                    Vec3 v2 = _pos - _route[ _waypointIndex ].first;

                    float len1 = v1.length();
                    Vec3 v1_norm = v1 * ( 1.0f / len1 );
                    float proj2 = v2 * v1_norm;

                    if ( proj2 >= 0.0 && proj2 <= len1 )
                    {
                        _waypointIndex++;

                        if ( _waypointIndex < _route.size() )
                        {
                            _destination = _route[ _waypointIndex ];
                        }
                        else
                        {
                            _destValid = false;
                        }
                    }
                }
            }
        }
        else
        {
            if ( _enroute ) _destValid = false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateElevation()
{
    //////////////////////////////
    UnitAerial::updateElevation();
    //////////////////////////////

    _elevation = Elevation::instance()->getElevation( _pos.x(), _pos.y() );
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updatePropeller()
{
    _prop_angle = _prop_angle + _timeStep * _prop_speed;
    _prop_speed = 2.0f * ( 2.0f * M_PI ) * ( 0.5f + 0.5f * (float)_throttle );

    while ( _prop_angle > 2.0f * M_PI ) _prop_angle -= 2.0f * M_PI;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateTrigger()
{
    _trigger = false;

    if ( _ownship && !Data::get()->controls.autopilot )
    {
        _trigger = Ownship::instance()->getTrigger();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateVariables()
{
    //////////////////////////////
    UnitAerial::updateVariables();
    //////////////////////////////

    _altitude_asl = _pos.z();
    _altitude_agl = _altitude_asl - _elevation;

    _airspeed  = _vel.length();
    _climbRate = ( _att * _vel ).z();

    _rollAngle  = -_angles.phi();
    _pitchAngle =  _angles.tht();
    _heading    = -_angles.psi() + 1.5f * M_PI;

    if ( _heading <        0.0f ) _heading += 2.0f * M_PI;
    if ( _heading > 2.0f * M_PI ) _heading -= 2.0f * M_PI;

    double cosTht = cos( _angles.tht() );

    if ( fabs( cosTht ) > 1.0e-6 )
    {
        double sinPhi = sin( _angles.phi() );
        double cosPhi = cos( _angles.phi() );

        _turnRate = ( sinPhi / cosTht ) * _omg.y()
                  + ( cosPhi / cosTht ) * _omg.z();
    }
    else
    {
        _turnRate = 0.0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateVelocity()
{
    if ( !_wingman || !_formation )
    {
        const float gain_p = 1.0f;
        const float gain_q = 1.0f;
        const float gain_r = 1.0f;

        const float damp_p = 0.1f;
        const float damp_q = 0.1f;
        const float damp_r = 0.1f;

        float coef_u = _angles.tht() - 1.0;

        if      ( coef_u < -1.2f ) coef_u = -1.2f;
        else if ( coef_u > -0.8f ) coef_u = -0.8f;

        float setpnt_u = getSpeed( _throttle ) * coef_u;

        float setpnt_p = gain_p * _ctrlRoll  - damp_p * _omg.x();
        float setpnt_q = gain_q * _ctrlPitch - damp_q * _omg.y();
        float setpnt_r = gain_r * _ctrlYaw   - damp_r * _omg.z();

        _vel.x() = Inertia< double >::update( _timeStep, _vel.x(), setpnt_u, _time_v );
        _vel.y() = 0.0;
        _vel.z() = 0.0;

        _omg.x() = Inertia< double >::update( _timeStep, _omg.x(), setpnt_p, _time_p );
        _omg.y() = Inertia< double >::update( _timeStep, _omg.y(), setpnt_q, _time_q );
        _omg.z() = Inertia< double >::update( _timeStep, _omg.z(), setpnt_r, _time_r );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateWeapons()
{
    updateTrigger();

    osg::ref_ptr<osg::StateSet> stateSet = _switch->getOrCreateStateSet();

    if ( _trigger )
    {
        if ( _time_shoot > 0.12f )
        {
            _time_shoot = 0.0f;

            for ( UInt8 i = 0; i < _muzzles.size(); i++ )
            {
                Tracer *tracer = new Tracer( _id, _ownship && _muzzles[ i ].trail );
                tracer->setPos( _pos + _att * _muzzles[ i ].pos );
                tracer->setAtt( _att );
            }
        }

        if ( _ownship )
        {
            if ( _flash_count % _flash_devider == 0 )
            {
                _flashSwitch->setAllChildrenOn();

                stateSet->setMode( GL_LIGHT1, osg::StateAttribute::ON );
                stateSet->setMode( GL_LIGHT2, osg::StateAttribute::ON );

                osg::Quat q( _flash_angle, osg::X_AXIS );

                for ( unsigned int i = 0; i < _flashPAT.size(); i++ )
                {
                    _flashPAT[ i ]->setAttitude( q );
                }

                _flash_angle = _flash_angle + 1.0f;
                _flash_count = 0;

                if ( _flash_angle > 2.0f * M_PI ) _flash_angle = 0.0f;
            }
            else
            {
                _flashSwitch->setAllChildrenOff();

                stateSet->setMode( GL_LIGHT1, osg::StateAttribute::OFF );
                stateSet->setMode( GL_LIGHT2, osg::StateAttribute::OFF );
            }

            _flash_count++;
        }
        else
        {
            _flashSwitch->setAllChildrenOff();
        }
    }
    else
    {
        _flashSwitch->setAllChildrenOff();

        stateSet->setMode( GL_LIGHT1, osg::StateAttribute::OFF );
        stateSet->setMode( GL_LIGHT2, osg::StateAttribute::OFF );
    }

    _time_drop   += _timeStep;
    _time_launch += _timeStep;
    _time_shoot  += _timeStep;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateWingman()
{
    if ( _wingman && _leaderValid )
    {
        _destValid = true;

        Aircraft *leader = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( _leaderId ) );

        if ( leader )
        {
            _waypointIndex = leader->getWaypointIndex();

            Vec3 pos_enu = leader->getPos() + Quat( leader->getAngles().psi(), osg::Z_AXIS ) * _wingmanOffset;

            if ( _formation || _destDist < 100.0f )
            {
                _formation = true;

                float tc_pos = 0.5f * 75.0f / leader->getAirspeed();
                float tc_vel = 1.0f;
                float tc_att = 1.0f;

                pos_enu.x() = Inertia< double >::update( _timeStep, _pos.x(), pos_enu.x(), tc_pos );
                pos_enu.y() = Inertia< double >::update( _timeStep, _pos.y(), pos_enu.y(), tc_pos );
                pos_enu.z() = Inertia< double >::update( _timeStep, _pos.z(), pos_enu.z(), tc_pos );

                Vec3 vel_enu( ( pos_enu.x() - _pos.x() ) / _timeStep,
                              ( pos_enu.y() - _pos.y() ) / _timeStep,
                              ( pos_enu.z() - _pos.z() ) / _timeStep );
                Vec3 vel_bas = _att.inverse() * vel_enu;

                float vel = vel_bas.length();
                if ( vel > _speed_max )
                {
                    vel_bas *= _speed_max / vel;
                }

                _vel.x() = Inertia< double >::update( _timeStep, _vel.x(), vel_bas.x(), tc_vel );
                _vel.y() = Inertia< double >::update( _timeStep, _vel.y(), vel_bas.y(), tc_vel );
                _vel.z() = Inertia< double >::update( _timeStep, _vel.z(), vel_bas.z(), tc_vel );

                Angles angles = leader->getAngles();

                if ( angles.psi() > 1.5 * M_PI && _angles.psi() < M_PI_2 )
                    angles.psi() -= 2.0 * M_PI;
                else if ( angles.psi() < M_PI_2 && _angles.psi() > 1.5 * M_PI )
                    angles.psi() += 2.0 * M_PI;

                _angles.phi() = Inertia< double >::update( _timeStep, _angles.phi(), angles.phi(), tc_att );
                _angles.tht() = Inertia< double >::update( _timeStep, _angles.tht(), angles.tht(), tc_att );
                _angles.psi() = Inertia< double >::update( _timeStep, _angles.psi(), angles.psi(), tc_att );

                _att = _angles.getRotate();

                _omg.x() = 0.0;
                _omg.y() = 0.0;
                _omg.z() = 0.0;
            }
            else
            {
                _formation = false;

                Vec3 vel_bas = _att.inverse() * ( leader->getAtt() * leader->getVel() );

                _destination.first  = pos_enu;
                _destination.second = getSpeed( -vel_bas.x() );

                float bear_rel = _destBear - _angles.psi();

                while ( bear_rel < -M_PI ) bear_rel += 2.0f * M_PI;
                while ( bear_rel >  M_PI ) bear_rel -= 2.0f * M_PI;

                if ( fabs( bear_rel ) < M_PI_4 )
                {
                    if ( _destDist > 100.0f )
                    {
                        _destination.second = 1.2f * _destination.second;

                        if ( _destDist > 200.0f )
                        {
                            _destination.second = 1.2f * _destination.second;
                        }
                    }
                }
            }
        }
        else
        {
            _leaderValid = false;

            if ( _waypointIndex < _route.size() )
            {
                _destination = _route[ _waypointIndex ];
            }
        }
    }
    else
    {
        _wingman = false;
        _formation = false;
    }
}
