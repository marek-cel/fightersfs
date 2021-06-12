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

std::string Simulation::getMissionFile( UInt32 campaign_index, UInt32 mission_index )
{
    std::string missionFile = "";

    Missions missions = getMissions( campaign_index );

    if ( mission_index < missions.size() )
    {
        missionFile = missions[ mission_index ];
    }

    return missionFile;
}

////////////////////////////////////////////////////////////////////////////////

Simulation::Campaigns Simulation::getCampaigns()
{
    Campaigns campaigns;

    std::string campaignsFile = Base::getPath( "campaigns.xml" );

    XmlDoc doc( campaignsFile );

    if ( doc.isOpen() )
    {
        XmlNode rootNode = doc.getRootNode();

        if ( rootNode.isValid() )
        {
            if ( 0 == String::icompare( rootNode.getName(), "campaigns" ) )
            {
                XmlNode campaignNode = rootNode.getFirstChildElement( "campaign" );

                while ( campaignNode.isValid() )
                {
                    std::string campaignFile = campaignNode.getAttribute( "file" );

                    if ( campaignFile.length() > 0 )
                    {
                        campaigns.push_back( "campaigns/" + campaignFile );
                    }

                    campaignNode = campaignNode.getNextSiblingElement( "campaign" );
                }
            }
        }
    }
    else
    {
        Log::e() << "Cannot open campaigns file: " << campaignsFile << std::endl;
    }

    return campaigns;
}

////////////////////////////////////////////////////////////////////////////////

Simulation::Missions Simulation::getMissions( UInt32 campaign_index )
{
    Missions missions;

    Campaigns campaigns = getCampaigns();

    std::string campaignFile;

    if ( campaign_index < campaigns.size() )
    {
        campaignFile = Base::getPath( campaigns[ campaign_index ] );
    }

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
                        missions.push_back( "missions/" + missionFile );
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

    return missions;
}


////////////////////////////////////////////////////////////////////////////////

Simulation::Simulation( int width, int height ) :
    _otw ( 0 ),
    _hud ( 0 ),
    _sfx ( 0 ),

    _camera  ( 0 ),
    _mission ( 0 ),

    _orbitedUnitId ( 0 )
{
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

void Simulation::init( UInt32 campaign_index, UInt32 mission_index )
{
    std::string missionFile = getMissionFile( campaign_index, mission_index );

    if ( missionFile.length() > 0 )
    {
        _mission = new Mission();
        _mission->init( missionFile );
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
    _hud->update( timeStep );
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
