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

const std::string Aircraft::m_tagName = "aircraft";

const float Aircraft::m_threshold_vel = 1.0f;
const float Aircraft::m_waypoint_dist = 27.0f;

////////////////////////////////////////////////////////////////////////////////

Aircraft::Aircraft( Affiliation affiliation ) :
    UnitAerial( affiliation ),

    m_wreckage ( 0 ),

    m_time_p ( 0.0f ),
    m_time_q ( 0.0f ),
    m_time_r ( 0.0f ),
    m_time_v ( 0.0f ),

    m_speed_min ( 0.0f ),
    m_speed_max ( 0.0f ),

    m_roll_max ( 0.0f ),
    m_turn_max ( 0.0f ),

    m_pid_phi ( 0 ),

    m_pid_p ( 0 ),
    m_pid_q ( 0 ),
    m_pid_r ( 0 ),

    m_enroute ( true ),
    m_wingman ( false ),

    m_wingmenCount ( 0 ),
    m_leaderId ( 0 ),
    m_wingmanOffset ( 50.0, 50.0, 0.0 ),
    m_leaderValid ( false ),
    m_formation ( false ),

    m_waypointIndex ( 0 ),

    m_destDist  ( 0.0f ),
    m_destBear  ( 0.0f ),
    m_destElev  ( 0.0f ),
    m_destValid ( false ),

    m_ctrlRoll  ( 0.0f ),
    m_ctrlPitch ( 0.0f ),
    m_ctrlYaw   ( 0.0f ),
    m_throttle  ( 0.0f ),

    m_initThrottle ( 0.0f ),

    m_prop_angle ( 0.0f ),
    m_prop_speed ( 0.0f ),

    m_maxAilerons ( 0.0f ),
    m_maxElevator ( 0.0f ),
    m_maxRudder   ( 0.0f ),

    m_unique ( false ),

    m_trigger ( false ),

    m_flash_count   ( 0 ),
    m_flash_devider ( 2 ),
    m_flash_angle ( 0.0f ),

    m_time_drop   ( 1000.0f ),
    m_time_launch ( 1000.0f ),
    m_time_shoot  ( 1000.0f ),

    m_elevation ( 0.0f ),

    m_altitude_asl  ( 0.0f ),
    m_altitude_agl  ( 0.0f ),
    m_airspeed      ( 0.0f ),
    m_climbRate     ( 0.0f ),
    m_machNumber    ( 0.0f ),
    m_angleOfAttack ( 0.0f ),
    m_sideslipAngle ( 0.0f ),
    m_pathAngle     ( 0.0f ),
    m_rollAngle     ( 0.0f ),
    m_pitchAngle    ( 0.0f ),
    m_heading       ( 0.0f ),
    m_turnRate      ( 0.0f )
{
    m_flashSwitch = new osg::Switch();
    m_switch->addChild( m_flashSwitch.get() );

    m_pid_phi = new PID( 2.0f, 0.2f, 0.1f, -SIM_AIRCRAFT_MAX_PHI, SIM_AIRCRAFT_MAX_PHI, true, 0.8f );

    m_pid_p = new PID( 1.0f, 0.1f, 0.05f, -1.0f, 1.0f, true );
    m_pid_q = new PID( 1.0f, 0.1f, 0.05f, -1.0f, 1.0f, true );
    m_pid_r = new PID( 1.0f, 0.1f, 0.05f, -1.0f, 1.0f, true );
}

////////////////////////////////////////////////////////////////////////////////

Aircraft::~Aircraft()
{
    if ( m_pid_phi ) delete m_pid_phi;
    m_pid_phi = 0;

    if ( m_pid_p ) delete m_pid_p;
    m_pid_p = 0;

    if ( m_pid_q ) delete m_pid_q;
    m_pid_q = 0;

    if ( m_pid_r ) delete m_pid_r;
    m_pid_r = 0;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::destroy()
{
    if ( isActive() )
    {
        Explosion *explosion = new Explosion( 5.0f );
        explosion->setPos( m_pos );
        explosion->setAtt( m_att );

        m_wreckage = new WreckageAircraft( m_modelFile, m_livery, m_smokeTrail.get(), m_ownship );
        m_wreckage->init( m_pos, m_att, m_vel, m_omg );

        if ( m_ownship )
        {
            Ownship::instance()->setWreckage( m_wreckage );
        }

        if ( m_wingman )
        {
            Aircraft *leader = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( m_leaderId ) );

            if ( leader )
            {
                leader->wingmenDecrement();

                if ( m_wingmenCount > 0 )
                {
                    List entities = *Entities::instance()->getEntities();
                    List::iterator it = entities.begin();

                    while ( it != entities.end() )
                    {
                        Aircraft *wingman = dynamic_cast< Aircraft* >(*it);

                        if ( wingman )
                        {
                            if ( wingman->isActive() && wingman->getWingman()
                              && wingman->getLeaderId() == m_id )
                            {
                                wingman->setLeader( m_leaderId );
                                wingman->setOffset( m_wingmanOffset + wingman->m_wingmanOffset );
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
    if ( m_flashSwitch->getNumChildren() > 0 )
    {
        m_flashSwitch->removeChildren( 0, m_flashSwitch->getNumChildren() );
    }

    m_flashPAT.clear();

    ///////////////
    Entity::load(); // sic!
    ///////////////

    if ( m_model.valid() )
    {
        m_switch->removeChild( m_model.get() );
    }

    if ( m_unique || m_ownship )
    {
        m_model = Models::readNodeFile( getPath( m_modelFile ) );
    }
    else
    {
        m_model = Models::get( getPath( m_modelFile ) );
    }

    if ( m_model.valid() )
    {
        m_modelStateSet = m_model->getOrCreateStateSet();
        m_switch->addChild( m_model.get() );

        m_aileronL = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "AileronL" ) );
        m_aileronR = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "AileronR" ) );

        m_elevator = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "Elevator" ) );

        m_rudderL = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "RudderL" ) );
        m_rudderR = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "RudderR" ) );

        if ( !m_rudderL.valid() )
        {
            m_rudderL = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "Rudder" ) );
        }

        m_propeller1 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "Propeller1" ) );
        m_propeller2 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "Propeller2" ) );
        m_propeller3 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "Propeller3" ) );
        m_propeller4 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "Propeller4" ) );

        if ( !m_propeller1.valid() || !m_propeller2.valid() )
        {
            m_propeller1 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "PropellerL" ) );
            m_propeller2 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "PropellerR" ) );
        }

        if ( !m_propeller1.valid() )
        {
            m_propeller1 = dynamic_cast<osg::PositionAttitudeTransform*>( FindNode::findFirst( m_model, "Propeller" ) );
        }
    }

    setLivery( m_livery );

    if ( m_ownship )
    {
        createMuzzleFlash( m_flashes, Vec3( 0.5, 0.5, 0.5 ) );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::makeAutomatic()
{
    ////////////////////////////
    UnitAerial::makeAutomatic();
    ////////////////////////////

    m_unique = false;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::makeOwnship()
{
    //////////////////////////
    UnitAerial::makeOwnship();
    //////////////////////////

    m_unique = true;

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
                    m_time_p = time_p;
                    m_time_q = time_q;
                    m_time_r = time_r;
                    m_time_v = time_v;
                    m_speed_min = speed_min;
                    m_speed_max = speed_max;

                    roll_max = Convert::deg2rad( roll_max );

                    if ( roll_max > SIM_AIRCRAFT_MAX_PHI )
                    {
                        roll_max = SIM_AIRCRAFT_MAX_PHI;
                    }

                    m_roll_max = roll_max;
                    m_turn_max = SIM_GRAVITY_ACC * tan( m_roll_max ) / m_speed_min;

                    m_pid_phi->setMin( -roll_max );
                    m_pid_phi->setMax(  roll_max );

                    XmlNode nodeMaxAilerons = nodeData.getFirstChildElement( "max_ailerons" );
                    XmlNode nodeMaxElevator = nodeData.getFirstChildElement( "max_elevator" );
                    XmlNode nodeMaxRudder   = nodeData.getFirstChildElement( "max_rudder"   );

                    float maxAilerons = 0.0f;
                    float maxElevator = 0.0f;
                    float maxRudder   = 0.0f;

                    if ( SIM_SUCCESS == XmlUtils::read( nodeMaxAilerons , maxAilerons ) )
                    {
                        m_maxAilerons = Convert::deg2rad( maxAilerons );
                    }

                    if ( SIM_SUCCESS == XmlUtils::read( nodeMaxElevator , maxElevator ) )
                    {
                        m_maxElevator = Convert::deg2rad( maxElevator );
                    }

                    if ( SIM_SUCCESS == XmlUtils::read( nodeMaxRudder, maxRudder ) )
                    {
                        m_maxRudder = Convert::deg2rad( maxRudder );
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

                            m_flashes.push_back( flash );
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

                            m_muzzles.push_back( muzzle );
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
        if ( m_ownship )
        {
            if ( timeStep > 0.03 )
            {
                m_flash_devider = 2;
            }
            else
            {
                m_flash_devider = 3;
            }

            float ailerons = m_maxAilerons * m_ctrlRoll;
            float elevator = m_maxElevator * m_ctrlPitch;
            float rudder   = m_maxRudder   * m_ctrlYaw;

            // ailerons
            if ( m_aileronL.valid() && m_aileronR.valid() )
            {
                m_aileronL->setAttitude( Quat( -ailerons, osg::Y_AXIS ) );
                m_aileronR->setAttitude( Quat(  ailerons, osg::Y_AXIS ) );
            }

            // elevator
            if ( m_elevator.valid() )
            {
                m_elevator->setAttitude( Quat( -elevator, osg::Y_AXIS ) );
            }

            // rudder
            if ( m_rudderL.valid() && m_rudderR.valid() )
            {
                m_rudderL->setAttitude( Quat( -rudder, osg::Z_AXIS ) );
                m_rudderR->setAttitude( Quat( -rudder, osg::Z_AXIS ) );
            }
        }

        // propellers
        if ( m_propeller1.valid() ) m_propeller1->setAttitude( Quat( -m_prop_angle, osg::X_AXIS ) );
        if ( m_propeller2.valid() ) m_propeller2->setAttitude( Quat(  m_prop_angle, osg::X_AXIS ) );
        if ( m_propeller3.valid() ) m_propeller3->setAttitude( Quat( -m_prop_angle, osg::X_AXIS ) );
        if ( m_propeller4.valid() ) m_propeller4->setAttitude( Quat(  m_prop_angle, osg::X_AXIS ) );

        if ( m_ownship && !Data::get()->controls.autopilot )
        {
            m_ctrlRoll  = Ownship::instance()->getCtrlRoll();
            m_ctrlPitch = Ownship::instance()->getCtrlPitch();
            m_ctrlYaw   = Ownship::instance()->getCtrlYaw();
            m_throttle  = Ownship::instance()->getThrottle();
        }
        else
        {
            updateControls();
            updateWingman();
        }

        updateDestination();
        updatePropeller();

        if ( m_altitude_agl < 1.0 || ( m_ownship && m_altitude_agl < fabs( sin( m_rollAngle ) ) * m_radius ) )
        {
            destroy();
        }

        if ( m_smokeTrail.valid() )
        {
            m_smokeTrail->setPosition( m_pos );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::wingmenIncrement()
{
    m_wingmenCount++;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::wingmenDecrement()
{
    if ( m_wingmenCount > 0 )
    {
        m_wingmenCount--;
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
    return m_speed_min + ( m_speed_max - m_speed_min ) * throttle;
}

////////////////////////////////////////////////////////////////////////////////

float Aircraft::getThrottle( float speed )
{
    float throttle = ( speed - m_speed_min ) / ( m_speed_max - m_speed_min );

    if ( throttle < 0.0f ) throttle = 0.0f;
    if ( throttle > 1.0f ) throttle = 1.0f;

    return throttle;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setLeader( UInt32 leaderId )
{
    if ( m_id != leaderId )
    {
        if ( m_leaderValid )
        {
            Aircraft *leader = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( m_leaderId ) );

            if ( leader )
            {
                leader->wingmenDecrement();
            }
        }

        Aircraft *leader = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( leaderId ) );

        if ( leader )
        {
            m_leaderId = leaderId;
            m_leaderValid = true;

            setRoute( &leader->getRoute() );

            leader->wingmenIncrement();

            m_wingman = true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setOffset( const Vec3 &offset )
{
    m_wingmanOffset = offset;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setLivery( const std::string &livery )
{
    m_livery = livery;

    osg::ref_ptr<osg::Texture2D> texture = Textures::get( getPath( m_livery ), 8.0f );

    if ( texture.valid() && m_modelStateSet.valid() )
    {
        m_modelStateSet->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setRoute( const Route *route )
{
    m_waypointIndex = 0;

    if ( route != 0 )
    {
        m_route = *route;

        if ( m_route.size() > 0 )
        {
            m_destination = m_route[ 0 ];

            m_waypointIndex = 0;
            m_destValid = true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setHP( UInt16 hp )
{
    ////////////////////////
    UnitAerial::setHP( hp );
    ////////////////////////

    if ( m_hp < 25 )
    {
        if ( !m_smokeTrail.valid() )
        {
            m_smokeTrail = Effects::createSmokeTrail();

            m_root->addChild( m_smokeTrail.get() );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::setUnique( bool unique )
{
    m_unique = unique;
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
            m_flashSwitch->addChild( pat.get() );

            pat->getOrCreateStateSet()->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_OTHER, "DepthSortedBin" );

            pat->addChild( muzzleFlashNode.get() );

            pat->setScale( scale );
            pat->setPosition( flashes.at( i ).pos );

            m_flashPAT.push_back( pat.get() );

            if ( flashes.at( i ).light && lightCount < 2 )
            {
                osg::ref_ptr<osg::PositionAttitudeTransform> patLight = new osg::PositionAttitudeTransform();
                m_switch->addChild( patLight.get() );

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

    m_flashSwitch->setAllChildrenOff();
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::avoidCollisions( float &phi, float &tht )
{
    const float dist2_limit = 16.0f * m_radius2;

    float dist2 = std::numeric_limits< float >::max();

    UnitAerial *closestUnit = 0;

    List entities = *Entities::instance()->getEntities();
    List::iterator it = entities.begin();

    while ( it != entities.end() )
    {
        UnitAerial *unit = dynamic_cast< UnitAerial* >(*it);

        if ( unit )
        {
            if ( unit->isActive() && unit->getId() != m_id )
            {
                float dist2_new = ( unit->getPos() - m_pos ).length2();

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

        Vec3 pos_bas = m_att.inverse() * ( closestUnit->getPos() - m_pos );

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
    m_initThrottle = getThrottle( m_vel.length() );
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::limitTht( float &tht )
{
    if ( m_altitude_agl < 100.0 )
    {
        tht += sqrt( 1.0 - m_altitude_agl / 100.0 ) * SIM_AIRCRAFT_MAX_THT;
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

    float sinPhi = sin( m_angles.phi() );
    float cosPhi = cos( m_angles.phi() );

    if ( m_destValid )
    {
        float diff_bear = m_destBear - m_angles.psi();

        while ( diff_bear < -M_PI ) diff_bear += 2.0f * M_PI;
        while ( diff_bear >  M_PI ) diff_bear -= 2.0f * M_PI;

        m_pid_phi->update( m_timeStep, diff_bear );

        setpnt_phi = m_pid_phi->getValue();
        setpnt_tht = m_destElev;
    }

    limitTht( setpnt_tht );

    avoidCollisions( setpnt_phi, setpnt_tht );

    float turnRate = m_angles.phi() * m_turn_max / m_roll_max;

    float diff_tht = setpnt_tht - m_angles.tht();
    float diff_turn = 4.0f * ( turnRate - m_turnRate );

    m_pid_p->update( m_timeStep, setpnt_phi - m_angles.phi() );
    m_pid_q->update( m_timeStep, sinPhi * diff_turn + cosPhi * diff_tht );
    m_pid_r->update( m_timeStep, cosPhi * diff_turn - sinPhi * diff_tht );

    m_ctrlRoll  = Inertia< float >::update( m_timeStep, m_ctrlRoll  , m_pid_p->getValue(), 0.1 );
    m_ctrlPitch = Inertia< float >::update( m_timeStep, m_ctrlPitch , m_pid_q->getValue(), 0.1 );
    m_ctrlYaw   = Inertia< float >::update( m_timeStep, m_ctrlYaw   , m_pid_r->getValue(), 0.1 );
    m_throttle  = getThrottle( m_destination.second );
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateDestination()
{
    if ( m_destValid )
    {
        Vec3 r_enu = m_destination.first - getPos();

        double r_xy = sqrt( r_enu.x()*r_enu.x() + r_enu.y()*r_enu.y() );

        m_destDist = r_enu.length();
        m_destBear = atan2( r_enu.y(), r_enu.x() ) - M_PI;
        m_destElev = atan2( r_enu.z(), r_xy );

        if ( m_enroute && m_route.size() > 0 )
        {
            float destDist = m_destDist;

            if ( m_wingman )
            {
                destDist = ( m_route[ m_waypointIndex ].first - getPos() ).length();
            }

            if ( destDist < m_waypoint_dist )
            {
                m_waypointIndex++;

                if ( m_waypointIndex < m_route.size() )
                {
                    m_destination = m_route[ m_waypointIndex ];
                }
                else
                {
                    m_destValid = false;
                }
            }
            else
            {
                if ( m_waypointIndex + 1 < m_route.size() )
                {
                    Vec3 v1 = m_route[ m_waypointIndex + 1 ].first - m_route[ m_waypointIndex ].first;
                    Vec3 v2 = m_pos - m_route[ m_waypointIndex ].first;

                    float len1 = v1.length();
                    Vec3 v1_norm = v1 * ( 1.0f / len1 );
                    float proj2 = v2 * v1_norm;

                    if ( proj2 >= 0.0 && proj2 <= len1 )
                    {
                        m_waypointIndex++;

                        if ( m_waypointIndex < m_route.size() )
                        {
                            m_destination = m_route[ m_waypointIndex ];
                        }
                        else
                        {
                            m_destValid = false;
                        }
                    }
                }
            }
        }
        else
        {
            if ( m_enroute ) m_destValid = false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateElevation()
{
    //////////////////////////////
    UnitAerial::updateElevation();
    //////////////////////////////

    m_elevation = Elevation::instance()->getElevation( m_pos.x(), m_pos.y() );
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updatePropeller()
{
    m_prop_angle = m_prop_angle + m_timeStep * m_prop_speed;
    m_prop_speed = 2.0f * ( 2.0f * M_PI ) * ( 0.5f + 0.5f * (float)m_throttle );

    while ( m_prop_angle > 2.0f * M_PI ) m_prop_angle -= 2.0f * M_PI;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateTrigger()
{
    m_trigger = false;

    if ( m_ownship && !Data::get()->controls.autopilot )
    {
        m_trigger = Ownship::instance()->getTrigger();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateVariables()
{
    //////////////////////////////
    UnitAerial::updateVariables();
    //////////////////////////////

    m_altitude_asl = m_pos.z();
    m_altitude_agl = m_altitude_asl - m_elevation;

    m_airspeed  = m_vel.length();
    m_climbRate = ( m_att * m_vel ).z();

    m_rollAngle  = -m_angles.phi();
    m_pitchAngle =  m_angles.tht();
    m_heading    = -m_angles.psi() + 1.5f * M_PI;

    if ( m_heading <        0.0f ) m_heading += 2.0f * M_PI;
    if ( m_heading > 2.0f * M_PI ) m_heading -= 2.0f * M_PI;

    double cosTht = cos( m_angles.tht() );

    if ( fabs( cosTht ) > 1.0e-6 )
    {
        double sinPhi = sin( m_angles.phi() );
        double cosPhi = cos( m_angles.phi() );

        m_turnRate = ( sinPhi / cosTht ) * m_omg.y()
                   + ( cosPhi / cosTht ) * m_omg.z();
    }
    else
    {
        m_turnRate = 0.0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateVelocity()
{
    if ( !m_wingman || !m_formation )
    {
        const float gain_p = 1.0f;
        const float gain_q = 1.0f;
        const float gain_r = 1.0f;

        const float damp_p = 0.1f;
        const float damp_q = 0.1f;
        const float damp_r = 0.1f;

        float coef_u = m_angles.tht() - 1.0;

        if      ( coef_u < -1.2f ) coef_u = -1.2f;
        else if ( coef_u > -0.8f ) coef_u = -0.8f;

        float setpnt_u = getSpeed( m_throttle ) * coef_u;

        float setpnt_p = gain_p * m_ctrlRoll  - damp_p * m_omg.x();
        float setpnt_q = gain_q * m_ctrlPitch - damp_q * m_omg.y();
        float setpnt_r = gain_r * m_ctrlYaw   - damp_r * m_omg.z();

        m_vel.x() = Inertia< double >::update( m_timeStep, m_vel.x(), setpnt_u, m_time_v );
        m_vel.y() = 0.0;
        m_vel.z() = 0.0;

        m_omg.x() = Inertia< double >::update( m_timeStep, m_omg.x(), setpnt_p, m_time_p );
        m_omg.y() = Inertia< double >::update( m_timeStep, m_omg.y(), setpnt_q, m_time_q );
        m_omg.z() = Inertia< double >::update( m_timeStep, m_omg.z(), setpnt_r, m_time_r );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateWeapons()
{
    updateTrigger();

    osg::ref_ptr<osg::StateSet> stateSet = m_switch->getOrCreateStateSet();

    if ( m_trigger )
    {
        if ( m_time_shoot > 0.12f )
        {
            m_time_shoot = 0.0f;

            for ( UInt8 i = 0; i < m_muzzles.size(); i++ )
            {
                Tracer *tracer = new Tracer( m_id, m_ownship && m_muzzles[ i ].trail );
                tracer->setPos( m_pos + m_att * m_muzzles[ i ].pos );
                tracer->setAtt( m_att );
            }
        }

        if ( m_ownship )
        {
            if ( m_flash_count % m_flash_devider == 0 )
            {
                m_flashSwitch->setAllChildrenOn();

                stateSet->setMode( GL_LIGHT1, osg::StateAttribute::ON );
                stateSet->setMode( GL_LIGHT2, osg::StateAttribute::ON );

                osg::Quat q( m_flash_angle, osg::X_AXIS );

                for ( unsigned int i = 0; i < m_flashPAT.size(); i++ )
                {
                    m_flashPAT[ i ]->setAttitude( q );
                }

                m_flash_angle = m_flash_angle + 1.0f;
                m_flash_count = 0;

                if ( m_flash_angle > 2.0f * M_PI ) m_flash_angle = 0.0f;
            }
            else
            {
                m_flashSwitch->setAllChildrenOff();

                stateSet->setMode( GL_LIGHT1, osg::StateAttribute::OFF );
                stateSet->setMode( GL_LIGHT2, osg::StateAttribute::OFF );
            }

            m_flash_count++;
        }
        else
        {
            m_flashSwitch->setAllChildrenOff();
        }
    }
    else
    {
        m_flashSwitch->setAllChildrenOff();

        stateSet->setMode( GL_LIGHT1, osg::StateAttribute::OFF );
        stateSet->setMode( GL_LIGHT2, osg::StateAttribute::OFF );
    }

    m_time_drop   += m_timeStep;
    m_time_launch += m_timeStep;
    m_time_shoot  += m_timeStep;
}

////////////////////////////////////////////////////////////////////////////////

void Aircraft::updateWingman()
{
    if ( m_wingman && m_leaderValid )
    {
        m_destValid = true;

        Aircraft *leader = dynamic_cast< Aircraft* >( Entities::instance()->getEntityById( m_leaderId ) );

        if ( leader )
        {
            m_waypointIndex = leader->getWaypointIndex();

            Vec3 pos_enu = leader->getPos() + Quat( leader->getAngles().psi(), osg::Z_AXIS ) * m_wingmanOffset;

            if ( m_formation || m_destDist < 100.0f )
            {
                m_formation = true;

                float tc_pos = 0.5f * 75.0f / leader->getAirspeed();
                float tc_vel = 1.0f;
                float tc_att = 1.0f;

                pos_enu.x() = Inertia< double >::update( m_timeStep, m_pos.x(), pos_enu.x(), tc_pos );
                pos_enu.y() = Inertia< double >::update( m_timeStep, m_pos.y(), pos_enu.y(), tc_pos );
                pos_enu.z() = Inertia< double >::update( m_timeStep, m_pos.z(), pos_enu.z(), tc_pos );

                Vec3 vel_enu( ( pos_enu.x() - m_pos.x() ) / m_timeStep,
                              ( pos_enu.y() - m_pos.y() ) / m_timeStep,
                              ( pos_enu.z() - m_pos.z() ) / m_timeStep );
                Vec3 vel_bas = m_att.inverse() * vel_enu;

                float vel = vel_bas.length();
                if ( vel > m_speed_max )
                {
                    vel_bas *= m_speed_max / vel;
                }

                m_vel.x() = Inertia< double >::update( m_timeStep, m_vel.x(), vel_bas.x(), tc_vel );
                m_vel.y() = Inertia< double >::update( m_timeStep, m_vel.y(), vel_bas.y(), tc_vel );
                m_vel.z() = Inertia< double >::update( m_timeStep, m_vel.z(), vel_bas.z(), tc_vel );

                Angles angles = leader->getAngles();

                if ( angles.psi() > 1.5 * M_PI && m_angles.psi() < M_PI_2 )
                    angles.psi() -= 2.0 * M_PI;
                else if ( angles.psi() < M_PI_2 && m_angles.psi() > 1.5 * M_PI )
                    angles.psi() += 2.0 * M_PI;

                m_angles.phi() = Inertia< double >::update( m_timeStep, m_angles.phi(), angles.phi(), tc_att );
                m_angles.tht() = Inertia< double >::update( m_timeStep, m_angles.tht(), angles.tht(), tc_att );
                m_angles.psi() = Inertia< double >::update( m_timeStep, m_angles.psi(), angles.psi(), tc_att );

                m_att = m_angles.getRotate();

                m_omg.x() = 0.0;
                m_omg.y() = 0.0;
                m_omg.z() = 0.0;
            }
            else
            {
                m_formation = false;

                Vec3 vel_bas = m_att.inverse() * ( leader->getAtt() * leader->getVel() );

                m_destination.first  = pos_enu;
                m_destination.second = getSpeed( -vel_bas.x() );

                float bear_rel = m_destBear - m_angles.psi();

                while ( bear_rel < -M_PI ) bear_rel += 2.0f * M_PI;
                while ( bear_rel >  M_PI ) bear_rel -= 2.0f * M_PI;

                if ( fabs( bear_rel ) < M_PI_4 )
                {
                    if ( m_destDist > 100.0f )
                    {
                        m_destination.second = 1.2f * m_destination.second;

                        if ( m_destDist > 200.0f )
                        {
                            m_destination.second = 1.2f * m_destination.second;
                        }
                    }
                }
            }
        }
        else
        {
            m_leaderValid = false;

            if ( m_waypointIndex < m_route.size() )
            {
                m_destination = m_route[ m_waypointIndex ];
            }
        }
    }
    else
    {
        m_wingman = false;
        m_formation = false;
    }
}
