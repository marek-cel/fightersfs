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
    m_otw ( 0 ),
    m_hud ( 0 ),
    m_sfx ( 0 ),

    m_camera  ( 0 ),
    m_mission ( 0 ),

    m_orbitedUnitId ( 0 )
{
    readMissions();

    float linesWidth = floor( (float)height / 200.0f + 0.5f );

    Models::createTracer( 1.2f * linesWidth );

    m_otw = new OTW( linesWidth );
    m_hud = new HUD( linesWidth, width, height );
    m_sfx = new SFX();

    m_camera = new Camera();
}

////////////////////////////////////////////////////////////////////////////////

Simulation::~Simulation()
{
    Models::reset();

    DELPTR( m_otw );
    DELPTR( m_hud );
    DELPTR( m_sfx );
    DELPTR( m_camera );
    DELPTR( m_mission );
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::init() {}

////////////////////////////////////////////////////////////////////////////////

void Simulation::init( UInt32 mission_index )
{
    if ( mission_index < m_missions.size() )
    {
        m_mission = new Mission();
        m_mission->init( m_missions[ mission_index ] );
    }

    Ownship::instance()->init();

    m_otw->init();
    m_hud->init( m_mission->isTutorial() );
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::load()
{
    Models::instance()->reset();

    // entities must be loaded before OTW
    Entities::instance()->load();

    m_otw->load();
    m_hud->load();

    m_sfx->init();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::pause()
{
    m_sfx->pause();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::unpause()
{
    m_sfx->unpause();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::restart()
{

}

////////////////////////////////////////////////////////////////////////////////

void Simulation::update( double timeStep )
{
    m_mission->update( timeStep );

    // ownship (after mission!)
    Ownship::instance()->update( timeStep );

    m_otw->update();
    m_hud->update();
    m_sfx->update();

    m_camera->update();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewChase()
{
    if ( m_camera ) m_camera->setViewChase();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewFlyby()
{
    if ( m_camera ) m_camera->setViewFlyby();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewFront()
{
    if ( m_camera ) m_camera->setViewFront();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewOrbit()
{
    if ( m_camera )
    {
        m_orbitedUnitId = 0;

        Entity *ownship = Ownship::instance()->getEntity();

        if ( ownship )
        {
            if ( ownship->isActive() )
            {
                m_camera->setTrackNode( ownship->getEntityNode() );
                m_camera->setViewOrbit();

                Unit *unit = dynamic_cast< Unit* >( ownship );

                if ( unit )
                {
                    m_orbitedUnitId = unit->getId();
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewPilot()
{
    if ( m_camera ) m_camera->setViewPilot();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewShift()
{
    if ( m_camera ) m_camera->setViewShift();
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::setViewWorld()
{
    if ( m_camera )
    {
        if ( !m_camera->isWorldNodeValid() )
        {
            osg::ref_ptr<osg::Node> worldNode = Scenery::getTerrainNode();

            if ( worldNode.valid() )
            {
                m_camera->setWorldNode( worldNode.get() );
            }
        }

        m_camera->setViewWorld();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Simulation::toggleOrbitUnit()
{
    if ( m_camera )
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

                    if ( unit->getId() == m_orbitedUnitId )
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

            m_orbitedUnitId = units[ nextUnitIndex ]->getId();

            m_camera->setTrackNode( units[ nextUnitIndex ]->getEntityNode() );
        }
        else
        {
            m_orbitedUnitId = 0;
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
                        m_missions.push_back( "missions/" + missionFile );
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
