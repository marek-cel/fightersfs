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

#include <sim/missions/sim_Mission.h>

#include <sim/sim_Elevation.h>
#include <sim/sim_Log.h>
#include <sim/sim_Ownship.h>
#include <sim/sim_Statistics.h>
#include <sim/sim_Creator.h>

#include <sim/cgi/sim_FogScene.h>
#include <sim/cgi/sim_Scenery.h>
#include <sim/cgi/sim_SkyDome.h>

#include <sim/missions/sim_ObjectiveWaypoint.h>

#include <sim/entities/sim_Aircraft.h>
#include <sim/entities/sim_Entities.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlDoc.h>
#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Mission::Mission() :
    m_ownshipRoute ( 0 ),
    m_stageIndex ( 0 ),
    m_messageIndex ( 0 ),
    m_status ( Pending ),
    m_tutorial ( false ),
    m_tutorialRoute ( false ),
    m_ready ( false ),
    m_realTime ( 0.0f ),
    m_timeEnd  ( 0.0f ),
    m_timeLeft ( 0.0f )
{
    Elevation::instance()->reset();
    Statistics::instance()->reset();
}

////////////////////////////////////////////////////////////////////////////////

Mission::~Mission()
{
    Entities::instance()->deleteAllEntities();

    // delete routes
    for ( Routes::iterator it = m_routes.begin(); it != m_routes.end(); ++it )
    {
        DELPTR( it->second );
    }

    m_routes.clear();

    // delete stages
    for ( Stages::iterator it = m_stages.begin(); it != m_stages.end(); ++it )
    {
        DELPTR( *it );
    }

    m_stages.clear();
}

////////////////////////////////////////////////////////////////////////////////

void Mission::addStage( Stage *stage )
{
    m_stages.push_back( stage );
}

////////////////////////////////////////////////////////////////////////////////

void Mission::init()
{
    m_ready = true;

    Data::get()->message.pointer_custom  = false;
    Data::get()->message.pointer_target  = false;
    Data::get()->message.pointer_rpm_dec = false;
    Data::get()->message.pointer_rpm_inc = false;
    Data::get()->message.pointer_trigger = false;

    Data::get()->message.pointer_x   = 0.0f;
    Data::get()->message.pointer_y   = 0.0f;
    Data::get()->message.pointer_phi = 0.0f;

    Data::get()->message.tip = NoTip;
}

////////////////////////////////////////////////////////////////////////////////

void Mission::init( const std::string &missionFile )
{
    std::string file = getPath( missionFile );

    XmlDoc doc( file );

    if ( doc.isOpen() )
    {
        if ( SIM_SUCCESS == readMission( doc.getRootNode() ) )
        {
            if ( m_stages.size() > 0 )
            {
                initStageTutorial();
                m_stages[ 0 ]->init();
            }

            m_ready = true;
        }
    }
    else
    {
        Log::e() << "Cannot open mission file: " << file << std::endl;
    }

    Data::get()->message.pointer_custom  = false;
    Data::get()->message.pointer_target  = false;
    Data::get()->message.pointer_rpm_dec = false;
    Data::get()->message.pointer_rpm_inc = false;
    Data::get()->message.pointer_trigger = false;

    Data::get()->message.pointer_x   = 0.0f;
    Data::get()->message.pointer_y   = 0.0f;
    Data::get()->message.pointer_phi = 0.0f;

    Data::get()->message.tip = NoTip;
}

////////////////////////////////////////////////////////////////////////////////

void Mission::update( double timeStep )
{
    if ( m_ready )
    {
        if ( !Data::get()->paused )
        {
            m_realTime += (float)timeStep;

            Statistics::instance()->update( timeStep );

            if ( m_status != Pending )
            {
                m_timeEnd += (float)timeStep;
            }

            // stages (before entities!)
            updateStages( timeStep );
            updateMessage();

            // entities (after stages!)
            Entities::instance()->update( timeStep );
        }
    }

    // mission
    Data::get()->mission.status = m_status;

    Data::get()->mission.time_step = timeStep;
    Data::get()->mission.real_time = m_realTime;
    Data::get()->mission.time_end  = m_timeEnd;
    Data::get()->mission.time_left = m_timeLeft;
}

////////////////////////////////////////////////////////////////////////////////

Route* Mission::getRouteByName( const std::string &name )
{
    Routes::iterator it = m_routes.find( name );

    if ( it != m_routes.end() )
    {
        return it->second;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void Mission::initStageTutorial()
{
    if ( m_tutorial && !m_tutorialRoute )
    {
        bool waypointObj = false;

        const Stage::Objectives &objectives = m_stages[ m_stageIndex ]->getObjectives();

        for ( unsigned int i = 0; i < objectives.size(); i++ )
        {
            const ObjectiveWaypoint *obj = dynamic_cast< ObjectiveWaypoint* >( objectives.at( i ) );

            if ( obj )
            {
                waypointObj = true;
                break;
            }
        }

        if ( waypointObj )
        {
            Aircraft *aircraft = Ownship::instance()->getAircraft();

            if ( aircraft )
            {
                double psi = aircraft->getAngles().psi();

                Quat dir = Quat( psi, osg::Z_AXIS );
                Vec3 pos = Vec3( aircraft->getAbsPos().x(), aircraft->getAbsPos().y(), 0.0f );

                // routes
                for ( Routes::iterator it = m_routes.begin(); it != m_routes.end(); ++it )
                {
                    Route *route = it->second;

                    for ( unsigned int i = 0; i < route->size(); i++ )
                    {
                        WaypointData wpy = route->at( i );

                        route->at( i ) = WaypointData( pos + dir * wpy.first, wpy.second );
                    }
                }

                // entities
                Entities::List::iterator it = Entities::instance()->getEntities()->begin();

                while ( it != Entities::instance()->getEntities()->end() )
                {
                    Unit *unit = dynamic_cast< Unit* >(*it);

                    if ( unit )
                    {
                        if ( Standby == unit->getState() && unit->isTopLevel() )
                        {
                            float hdg = -M_PI_2 - unit->getAngles().psi() - psi;

                            unit->setHeading( hdg );
                            unit->setPos( pos + dir * unit->getAbsPos() );
                        }
                    }

                    ++it;
                }

                // ownship route
                if ( m_ownshipRoute )
                {
                    aircraft->setRoute( m_ownshipRoute );
                }
            }

            m_tutorialRoute = true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int Mission::readMission( const XmlNode &node )
{
    if ( node.isValid() )
    {
        if ( 0 == String::icompare( node.getName(), "mission" ) )
        {
            // tutorial
            m_tutorial = String::toInt( node.getAttribute( "tutorial" ) ) == 1 ? true : false;

            // scenery
            XmlNode sceneryNode = node.getFirstChildElement( "scenery" );

            if ( sceneryNode.isValid() )
            {
                if ( SIM_SUCCESS != readScenery( sceneryNode ) )
                {
                    return SIM_FAILURE;
                }
            }
            else
            {
                return SIM_FAILURE;
            }

            // routes
            XmlNode routesNode = node.getFirstChildElement( "routes" );

            if ( routesNode.isValid() )
            {
                if ( SIM_SUCCESS != readRoutes( routesNode ) )
                {
                    return SIM_FAILURE;
                }
            }

            // units (after routes!!!)
            XmlNode unitsNode = node.getFirstChildElement( "units" );

            if ( unitsNode.isValid() )
            {
                if ( SIM_SUCCESS != readUnits( unitsNode ) )
                {
                    return SIM_FAILURE;
                }
            }

            // stages (after units!!!)
            XmlNode stagesNode = node.getFirstChildElement( "stages" );

            if ( stagesNode.isValid() )
            {
                if ( SIM_SUCCESS != readStages( stagesNode ) )
                {
                    return SIM_FAILURE;
                }
            }

            return SIM_SUCCESS;
        }
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int Mission::readRoutes( const XmlNode &node )
{
    if ( node.isValid() )
    {
        XmlNode routeNode = node.getFirstChildElement( "route" );

        while ( routeNode.isValid() )
        {
            std::string routeName = routeNode.getAttribute( "name" );

            if ( routeName.length() > 0 )
            {
                Route *route = Route::read( routeNode );

                if ( route )
                {
                    m_routes.insert( std::pair< std::string, Route* >( routeName, route ) );
                }
                else
                {
                    return SIM_FAILURE;
                }
            }

            routeNode = routeNode.getNextSiblingElement( "route" );
        }

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int Mission::readScenery( const XmlNode &node )
{
    if ( node.isValid() )
    {
        XmlNode terrainNode = node.getFirstChildElement( "terrain" );
        XmlNode genericNode = node.getFirstChildElement( "generic" );
        XmlNode skyDomeNode = node.getFirstChildElement( "sky_dome" );

        if ( terrainNode.isValid() && genericNode.isValid() && skyDomeNode.isValid() )
        {
            std::string terrainFile;
            std::string genericFile;
            std::string skyDomeFile;

            if ( SIM_SUCCESS == XmlUtils::read( terrainNode, terrainFile )
              && SIM_SUCCESS == XmlUtils::read( genericNode, genericFile )
              && SIM_SUCCESS == XmlUtils::read( skyDomeNode, skyDomeFile ) )
            {
                Scenery::_objectFiles.clear();
                Scenery::_terrainFile = terrainFile;
                Scenery::_genericFile = genericFile;
                SkyDome::_skyDomeFile = skyDomeFile;

                // default values
                FogScene::_visibility = 0.9f * SIM_SKYDOME_RAD;
                SkyDome::_sunCoef = 1.0f;

                // visibility
                XmlNode visibilityNode = node.getFirstChildElement( "visibility" );

                if ( visibilityNode.isValid() )
                {
                    int visibility;

                    if ( SIM_SUCCESS == XmlUtils::read( visibilityNode, visibility ) )
                    {
                        FogScene::_visibility = visibility;
                    }
                }

                // sun coef
                XmlNode sunCoefNode = node.getFirstChildElement( "sun_coef" );

                if ( sunCoefNode.isValid() )
                {
                    float sunCoef;

                    if ( SIM_SUCCESS == XmlUtils::read( sunCoefNode, sunCoef ) )
                    {
                        SkyDome::_sunCoef = sunCoef;
                    }
                }

                // elevation
                XmlNode elevationNode = node.getFirstChildElement( "elevation" );

                if ( elevationNode.isValid() )
                {
                    std::string elevationFile;

                    if ( SIM_SUCCESS == XmlUtils::read( elevationNode, elevationFile ) )
                    {
                        Elevation::instance()->readFile( getPath( elevationFile ) );
                    }
                }

                // objects
                XmlNode objectsNode = node.getFirstChildElement( "objects" );

                if ( objectsNode.isValid() )
                {
                    XmlNode objectNode = objectsNode.getFirstChildElement( "object" );

                    while ( objectNode.isValid() )
                    {
                        std::string objectFile;

                        if ( SIM_SUCCESS == XmlUtils::read( objectNode, objectFile ) )
                        {
                            Scenery::_objectFiles.push_back( objectFile );
                        }
                        else
                        {
                            return SIM_FAILURE;
                        }

                        objectNode = objectNode.getNextSiblingElement( "object" );
                    }
                }

                return SIM_SUCCESS;
            }
        }
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int Mission::readStages( const XmlNode &node )
{
    if ( node.isValid() )
    {
        XmlNode stageNode = node.getFirstChildElement( "stage" );

        while ( stageNode.isValid() )
        {
            Stage *stage = Stage::read( stageNode, m_tutorial );

            if ( stage )
            {
                m_stages.push_back( stage );
            }
            else
            {
                return SIM_FAILURE;
            }

            stageNode = stageNode.getNextSiblingElement( "stage" );
        }

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int Mission::readUnits( const XmlNode &node )
{
    if ( node.isValid() )
    {
        // ownship
        XmlNode ownshipNode = node.getFirstChildElement( "ownship" );

        if ( ownshipNode.isValid() )
        {
            XmlNode unitNode = ownshipNode.getFirstChildElement();

            if ( SIM_SUCCESS != readUnit( unitNode, Friend, true ) )
            {
                return SIM_FAILURE;
            }
        }
        else
        {
            return SIM_FAILURE;
        }

        // friends
        XmlNode friendsNode = node.getFirstChildElement( "friends" );

        if ( friendsNode.isValid() )
        {
            if ( SIM_SUCCESS != readUnits( friendsNode, Friend ) )
            {
                return SIM_FAILURE;
            }
        }

        // hostiles
        XmlNode hostilesNode = node.getFirstChildElement( "hostiles" );

        if ( hostilesNode.isValid() )
        {
            if ( SIM_SUCCESS != readUnits( hostilesNode, Hostile ) )
            {
                return SIM_FAILURE;
            }
        }

        // neutrals
        XmlNode neutralsNode = node.getFirstChildElement( "neutrals" );

        if ( neutralsNode.isValid() )
        {
            if ( SIM_SUCCESS != readUnits( neutralsNode, Neutral ) )
            {
                return SIM_FAILURE;
            }
        }

        // leaders (this must be done after creating all units)
        if ( ownshipNode.isValid()  ) readUnitLeaders( ownshipNode  );
        if ( friendsNode.isValid()  ) readUnitLeaders( friendsNode  );
        if ( hostilesNode.isValid() ) readUnitLeaders( hostilesNode );
        if ( neutralsNode.isValid() ) readUnitLeaders( neutralsNode );

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int Mission::readUnits( const XmlNode &node, Affiliation affiliation, bool ownship )
{
    if ( node.isValid() )
    {
        XmlNode unitNode = node.getFirstChildElement();

        while ( unitNode.isValid() )
        {
            if ( SIM_SUCCESS != readUnit( unitNode, affiliation, ownship ) )
            {
                return SIM_FAILURE;
            }

            unitNode = unitNode.getNextSiblingElement();
        }

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int Mission::readUnit( const XmlNode &node, Affiliation affiliation, bool ownship )
{
    if ( node.isValid() )
    {
        Unit *unit = 0;

        std::string unitType = node.getName();
        std::string unitFile = "";

        XmlNode fileNode = node.getFirstChildElement( "file" );

        if ( SIM_SUCCESS == XmlUtils::read( fileNode, unitFile ) )
        {
            unit = Creator::createUnit( unitType, unitFile, affiliation );
        }

        if ( unit )
        {
            unit->setState( Standby );

            std::string unitName = node.getAttribute( "name" );
            std::string unitHP   = node.getAttribute( "hp" );

            unit->setName( unitName );

            if ( unitHP.length() > 0 )
            {
                unit->setHP( String::toInt( unitHP ) );
            }

            XmlNode posNode = node.getFirstChildElement( "position" );
            XmlNode hdgNode = node.getFirstChildElement( "heading" );
            XmlNode velNode = node.getFirstChildElement( "velocity" );

            Vec3 pos;

            float hdg = 0.0;
            float vel = 0.0;

            if ( SIM_SUCCESS == XmlUtils::read( posNode, pos ) )
            {
                unit->setPos( pos );
            }

            if ( SIM_SUCCESS == XmlUtils::read( hdgNode, hdg ) )
            {
                unit->setHeading( Convert::deg2rad( hdg ) );
            }

            if ( SIM_SUCCESS == XmlUtils::read( velNode, vel ) )
            {
                unit->setVelocity( vel );
            }

            Aircraft *aircraft = dynamic_cast<Aircraft*>( unit );

            if ( aircraft )
            {
                // route
                if ( !ownship || !m_tutorial )
                {
                    aircraft->setRoute( getRouteByName( node.getAttribute( "route" ) ) );
                }
                else
                {
                    m_ownshipRoute = getRouteByName( node.getAttribute( "route" ) );
                }

                // unique
                if ( String::toBool( node.getAttribute( "unique" ) ) )
                {
                    aircraft->setUnique( true );
                }

                // offset
                XmlNode offsetNode = node.getFirstChildElement( "offset" );

                if ( offsetNode.isValid() )
                {
                    Vec3 offset;

                    if ( SIM_SUCCESS == XmlUtils::read( offsetNode, offset ) )
                    {
                        aircraft->setOffset( offset );
                    }
                }

                // livery
                XmlNode liveryNode = node.getFirstChildElement( "livery" );

                if ( liveryNode.isValid() )
                {
                    std::string livery;

                    if ( SIM_SUCCESS == XmlUtils::read( liveryNode, livery ) )
                    {
                        aircraft->setLivery( livery );
                    }
                }
            }

            // after setting aircraft data (especially route)
            if ( ownship )
            {
                unit->makeOwnship();
            }

            return SIM_SUCCESS;
        }
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

int Mission::readUnitLeaders( const XmlNode &node )
{
    if ( node.isValid() )
    {
        XmlNode unitNode = node.getFirstChildElement();

        while ( unitNode.isValid() )
        {
            std::string unitName   = unitNode.getAttribute( "name" );
            std::string leaderName = unitNode.getAttribute( "leader" );

            if ( unitName.length() > 0 && leaderName.length() > 0 )
            {
                Aircraft *unit   = dynamic_cast< Aircraft* >( Entities::instance()->getUnitByName( unitName ) );
                Aircraft *leader = dynamic_cast< Aircraft* >( Entities::instance()->getUnitByName( leaderName ) );

                if ( unit && leader )
                {
                    unit->setLeader( leader->getId() );
                }
            }

            unitNode = unitNode.getNextSiblingElement();
        }

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void Mission::updateMessage()
{
    Data::get()->message.visible = false;
    Data::get()->message.overlay = false;

    strcpy( Data::get()->message.text, "" );

    if ( m_tutorial )
    {
        Data::get()->message.pointer_custom  = false;
        Data::get()->message.pointer_target  = false;
        Data::get()->message.pointer_rpm_dec = false;
        Data::get()->message.pointer_rpm_inc = false;
        Data::get()->message.pointer_trigger = false;

        Data::get()->message.pointer_x   = 0.0f;
        Data::get()->message.pointer_y   = 0.0f;
        Data::get()->message.pointer_phi = 0.0f;

        Data::get()->message.tip = NoTip;
    }

    if ( m_status == Pending )
    {
        if ( m_stageIndex < m_stages.size() )
        {
            if ( m_stages[ m_stageIndex ] )
            {
                if ( m_messageIndex < m_stages[ m_stageIndex ]->getMessages().size() )
                {
                    const Stage::Message &message = m_stages[ m_stageIndex ]->getMessages()[ m_messageIndex ];

                    if ( m_stages[ m_stageIndex ]->getStageTime() > message.delay )
                    {
                        if ( m_stages[ m_stageIndex ]->getStageTime() < message.delay + message.duration )
                        {
                            Data::get()->message.visible = true;
                            Data::get()->message.overlay = message.overlay;

                            strncpy( Data::get()->message.text, message.text.get().c_str(), SIM_MSG_LEN );

                            if ( m_tutorial )
                            {
                                Data::get()->message.pointer_custom  = message.pointer_custom;
                                Data::get()->message.pointer_target  = message.pointer_target;
                                Data::get()->message.pointer_rpm_dec = message.pointer_rpm_dec;
                                Data::get()->message.pointer_rpm_inc = message.pointer_rpm_inc;
                                Data::get()->message.pointer_trigger = message.pointer_trigger;

                                Data::get()->message.pointer_x   = message.pointer_x;
                                Data::get()->message.pointer_y   = message.pointer_y;
                                Data::get()->message.pointer_phi = message.pointer_phi;

                                Data::get()->message.tip = message.tip;
                            }
                        }
                        else
                        {
                            if ( m_messageIndex + 1 < m_stages[ m_stageIndex ]->getMessages().size() )
                            {
                                m_messageIndex++;
                            }
                        }
                    }
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Mission::updateStages( double timeStep )
{
    m_timeLeft = 0.0f;

    if ( m_status != Failure && !Data::get()->ownship.destroyed )
    {
        if ( m_stages.size() > 0 )
        {
            if ( m_status == Pending )
            {
                if ( m_stageIndex < m_stages.size() )
                {
                    Stage *stage = m_stages[ m_stageIndex ];

                    if ( stage )
                    {
                        stage->update( timeStep );

                        if ( stage->getStatus() == Success )
                        {
                            m_stageIndex++;
                            m_messageIndex = 0;

                            if ( m_stageIndex < m_stages.size() )
                            {
                                initStageTutorial();
                                m_stages[ m_stageIndex ]->init();
                            }
                        }
                        else
                        {
                            m_timeLeft = stage->getTimeLimit() - stage->getStageTime();
                        }
                    }
                }
            }

            bool failure = false;
            bool success = true;

            Stages::iterator it = m_stages.begin();

            while( it != m_stages.end() )
            {
                if ( (*it) )
                {
                    if ( (*it)->getStatus() == Failure ) failure = true;
                    if ( (*it)->getStatus() != Success ) success = false;
                }

                ++it;
            }

            m_status = Pending;

            if ( success ) m_status = Success;
            if ( failure ) m_status = Failure;
        }
        else
        {
            m_status = Success;
        }
    }
    else
    {
        m_status = Failure;
    }
}
