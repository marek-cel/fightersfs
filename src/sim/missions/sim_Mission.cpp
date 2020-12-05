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
    _ownshipRoute ( 0 ),
    _stageIndex ( 0 ),
    _messageIndex ( 0 ),
    _status ( Pending ),
    _tutorial ( false ),
    _tutorialRoute ( false ),
    _ready ( false ),
    _realTime ( 0.0f ),
    _timeEnd  ( 0.0f ),
    _timeLeft ( 0.0f )
{
    Elevation::instance()->reset();
    Statistics::instance()->reset();
}

////////////////////////////////////////////////////////////////////////////////

Mission::~Mission()
{
    Entities::instance()->deleteAllEntities();

    // delete routes
    for ( Routes::iterator it = _routes.begin(); it != _routes.end(); ++it )
    {
        DELPTR( it->second );
    }

    _routes.clear();

    // delete stages
    for ( Stages::iterator it = _stages.begin(); it != _stages.end(); ++it )
    {
        DELPTR( *it );
    }

    _stages.clear();
}

////////////////////////////////////////////////////////////////////////////////

void Mission::addStage( Stage *stage )
{
    _stages.push_back( stage );
}

////////////////////////////////////////////////////////////////////////////////

void Mission::init()
{
    _ready = true;

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
            if ( _stages.size() > 0 )
            {
                initStageTutorial();
                _stages[ 0 ]->init();
            }

            _ready = true;
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
    if ( _ready )
    {
        if ( !Data::get()->paused )
        {
            _realTime += (float)timeStep;

            Statistics::instance()->update( timeStep );

            if ( _status != Pending )
            {
                _timeEnd += (float)timeStep;
            }

            // stages (before entities!)
            updateStages( timeStep );
            updateMessage();

            // entities (after stages!)
            Entities::instance()->update( timeStep );
        }
    }

    // mission
    Data::get()->mission.status = _status;

    Data::get()->mission.time_step = timeStep;
    Data::get()->mission.real_time = _realTime;
    Data::get()->mission.time_end  = _timeEnd;
    Data::get()->mission.time_left = _timeLeft;
}

////////////////////////////////////////////////////////////////////////////////

Route* Mission::getRouteByName( const std::string &name )
{
    Routes::iterator it = _routes.find( name );

    if ( it != _routes.end() )
    {
        return it->second;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void Mission::initStageTutorial()
{
    if ( _tutorial && !_tutorialRoute )
    {
        bool waypointObj = false;

        const Stage::Objectives &objectives = _stages[ _stageIndex ]->getObjectives();

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
                for ( Routes::iterator it = _routes.begin(); it != _routes.end(); ++it )
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
                if ( _ownshipRoute )
                {
                    aircraft->setRoute( _ownshipRoute );
                }
            }

            _tutorialRoute = true;
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
            _tutorial = String::toInt( node.getAttribute( "tutorial" ) ) == 1 ? true : false;

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
                    _routes.insert( std::pair< std::string, Route* >( routeName, route ) );
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
            Stage *stage = Stage::read( stageNode, _tutorial );

            if ( stage )
            {
                _stages.push_back( stage );
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
                if ( !ownship || !_tutorial )
                {
                    aircraft->setRoute( getRouteByName( node.getAttribute( "route" ) ) );
                }
                else
                {
                    _ownshipRoute = getRouteByName( node.getAttribute( "route" ) );
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

    if ( _tutorial )
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

    if ( _status == Pending )
    {
        if ( _stageIndex < _stages.size() )
        {
            if ( _stages[ _stageIndex ] )
            {
                if ( _messageIndex < _stages[ _stageIndex ]->getMessages().size() )
                {
                    const Stage::Message &message = _stages[ _stageIndex ]->getMessages()[ _messageIndex ];

                    if ( _stages[ _stageIndex ]->getStageTime() > message.delay )
                    {
                        if ( _stages[ _stageIndex ]->getStageTime() < message.delay + message.duration )
                        {
                            Data::get()->message.visible = true;
                            Data::get()->message.overlay = message.overlay;

                            strncpy( Data::get()->message.text, message.text.get().c_str(), SIM_MSG_LEN );

                            if ( _tutorial )
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
                            if ( _messageIndex + 1 < _stages[ _stageIndex ]->getMessages().size() )
                            {
                                _messageIndex++;
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
    _timeLeft = 0.0f;

    if ( _status != Failure && !Data::get()->ownship.destroyed )
    {
        if ( _stages.size() > 0 )
        {
            if ( _status == Pending )
            {
                if ( _stageIndex < _stages.size() )
                {
                    Stage *stage = _stages[ _stageIndex ];

                    if ( stage )
                    {
                        stage->update( timeStep );

                        if ( stage->getStatus() == Success )
                        {
                            _stageIndex++;
                            _messageIndex = 0;

                            if ( _stageIndex < _stages.size() )
                            {
                                initStageTutorial();
                                _stages[ _stageIndex ]->init();
                            }
                        }
                        else
                        {
                            _timeLeft = stage->getTimeLimit() - stage->getStageTime();
                        }
                    }
                }
            }

            bool failure = false;
            bool success = true;

            Stages::iterator it = _stages.begin();

            while( it != _stages.end() )
            {
                if ( (*it) )
                {
                    if ( (*it)->getStatus() == Failure ) failure = true;
                    if ( (*it)->getStatus() != Success ) success = false;
                }

                ++it;
            }

            _status = Pending;

            if ( success ) _status = Success;
            if ( failure ) _status = Failure;
        }
        else
        {
            _status = Success;
        }
    }
    else
    {
        _status = Failure;
    }
}
