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

#include <sim/sim_Simulation.h>

#include <math.h>

#include <sim/sim_Creator.h>
#include <sim/sim_Elevation.h>
#include <sim/sim_ListScenery.h>
#include <sim/sim_ListUnits.h>
#include <sim/sim_Log.h>
#include <sim/sim_Ownship.h>

#include <sim/cgi/sim_FogScene.h>
#include <sim/cgi/sim_Models.h>
#include <sim/cgi/sim_Scenery.h>
#include <sim/cgi/sim_SkyDome.h>

#include <sim/missions/sim_ObjectiveDestroy.h>
#include <sim/missions/sim_ObjectiveTimeout.h>

#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlDoc.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Simulation::Simulation( int width, int height ) :
    _otw ( 0 ),
    _hud ( 0 ),
    _sfx ( 0 ),

    _camera  ( 0 ),
    _mission ( 0 ),

    _orbitedUnitId ( 0 )
{
    readMissions();

    float linesWidth = floor( (float)height / 200.0f + 0.5f );

    Models::createTracer( 1.2f * linesWidth );

    _otw = new OTW( linesWidth );
    _hud = new HUD( linesWidth, width, height );
    _sfx = new SFX();

    _camera = new Camera();
}

////////////////////////////////////////////////////////////////////////////////

Simulation::~Simulation()
{
    Models::reset();

    DELPTR( _otw );
    DELPTR( _hud );
    DELPTR( _sfx );
    DELPTR( _camera );
    DELPTR( _mission );
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::init() {}

////////////////////////////////////////////////////////////////////////////////

void Simulation::init( UInt32 mission_index )
{
    if ( mission_index < _missions.size() )
    {
        _mission = new Mission();
        _mission->init( _missions[ mission_index ] );
    }

    Ownship::instance()->init();

    _otw->init();
    _hud->init( _mission->isTutorial() );
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::load()
{
    Models::instance()->reset();

    // entities must be loaded before OTW
    Entities::instance()->load();

    _otw->load();
    _hud->load();

    _sfx->init();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::pause()
{
    _sfx->pause();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::unpause()
{
    _sfx->unpause();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::restart()
{

}

////////////////////////////////////////////////////////////////////////////////

void Simulation::update( double timeStep )
{
    _mission->update( timeStep );

    // ownship (after mission!)
    Ownship::instance()->update( timeStep );

    _otw->update();
    _hud->update();
    _sfx->update();

    _camera->update();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewChase()
{
    if ( _camera ) _camera->setViewChase();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewFlyby()
{
    if ( _camera ) _camera->setViewFlyby();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewFront()
{
    if ( _camera ) _camera->setViewFront();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewOrbit()
{
    if ( _camera )
    {
        _orbitedUnitId = 0;

        Entity *ownship = Ownship::instance()->getEntity();

        if ( ownship )
        {
            if ( ownship->isActive() )
            {
                _camera->setTrackNode( ownship->getEntityNode() );
                _camera->setViewOrbit();

                Unit *unit = dynamic_cast< Unit* >( ownship );

                if ( unit )
                {
                    _orbitedUnitId = unit->getId();
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewPilot()
{
    if ( _camera ) _camera->setViewPilot();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewShift()
{
    if ( _camera ) _camera->setViewShift();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewWorld()
{
    if ( _camera )
    {
        if ( !_camera->isWorldNodeValid() )
        {
            osg::ref_ptr<osg::Node> worldNode = Scenery::getTerrainNode();

            if ( worldNode.valid() )
            {
                _camera->setWorldNode( worldNode.get() );
            }
        }

        _camera->setViewWorld();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::toggleOrbitUnit()
{
    if ( _camera )
    {
        std::vector< Unit* > units;

        unsigned int nextUnitIndex = 0;

        for ( unsigned int i = 0; i < Entities::instance()->getEntities()->size(); i++ )
        {
            Unit *unit = dynamic_cast< Unit* >( Entities::instance()->getEntities()->at( i ) );

            if ( unit )
            {
                if ( unit->isActive() )
                {
                    units.push_back( unit );

                    if ( unit->getId() == _orbitedUnitId )
                    {
                        nextUnitIndex = i + 1;
                    }
                }
            }
        }

        if ( units.size() > 0 )
        {
            if ( nextUnitIndex > units.size() - 1 )
            {
                nextUnitIndex = 0;
            }

            _orbitedUnitId = units[ nextUnitIndex ]->getId();

            _camera->setTrackNode( units[ nextUnitIndex ]->getEntityNode() );
        }
        else
        {
            _orbitedUnitId = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::readMissions()
{
    std::string campaignFile = Base::getPath( "missions/campaign.xml" );

    XmlDoc doc( campaignFile );

    if ( doc.isOpen() )
    {
        XmlNode rootNode = doc.getRootNode();

        if ( rootNode.isValid() )
        {
            if ( 0 == String::icompare( rootNode.getName(), "campaign" ) )
            {
                XmlNode missionNode = rootNode.getFirstChildElement( "mission" );

                while ( missionNode.isValid() )
                {
                    std::string missionFile = missionNode.getAttribute( "file" );

                    if ( missionFile.length() > 0 )
                    {
                        _missions.push_back( "missions/" + missionFile );
                    }

                    missionNode = missionNode.getNextSiblingElement( "mission" );
                }
            }
        }
    }
    else
    {
        Log::e() << "Cannot open campaign file: " << campaignFile << std::endl;
    }
}
