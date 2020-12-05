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
#ifndef SIM_SIMULATION_H
#define SIM_SIMULATION_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Base.h>
#include <sim/sim_Statistics.h>

#include <sim/cgi/sim_Camera.h>
#include <sim/cgi/sim_HUD.h>
#include <sim/cgi/sim_OTW.h>

#include <sim/missions/sim_Mission.h>

#include <sim/sfx/sim_SFX.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Main simulation class. */
class Simulation : public Base
{
public:

    /**
     * @brief Constructor.
     * @param width viewport width in pixels
     * @param height viewport height in pixels
     */
    Simulation( int width, int height );

    /** @brief Destructor. */
    virtual ~Simulation();

    /**
     * @brief Initialized simulation (reads config files, creates scene, etc.).
     */
    void init();

    /**
     * @brief Initialized simulation (reads config files, creates scene, etc.).
     * @param mission_index mission file index
     */
    void init( UInt32 mission_index );

    /** @brief Loads and reloads simulation (models, textures, etc.). */
    void load();

    /** @brief Pauses simulation. */
    void pause();

    /** @brief Unpauses simulation. */
    void unpause();

    /** @brief Restarts simulation after freeze (resumes sound replaying, etc.). */
    void restart();

    /**
     * @brief Updates simulation.
     * @param timeStep [s] simulation time step
     */
    void update( double timeStep );

    /** */
    inline osgGA::CameraManipulator* getCameraManipulator()
    {
        return _camera->getManipulator();
    }

    inline osg::Group* getNodeHUD() { return _hud->getNode(); }
    inline osg::Group* getNodeOTW() { return _otw->getNode(); }

    inline UInt32 getOrbitedUnitId() const { return _orbitedUnitId; }

    void setViewChase();
    void setViewFlyby();
    void setViewFront();
    void setViewOrbit();
    void setViewPilot();
    void setViewShift();
    void setViewWorld();

    /** Toggles through units to be orbited. */
    void toggleOrbitUnit();

private:

    typedef std::vector< std::string > Missions;

    Missions _missions;         ///< missions files list

    OTW *_otw;                  ///< Out-the-Window
    HUD *_hud;                  ///< Head-up-Display
    SFX *_sfx;                  ///< sound effects

    Camera  *_camera;           ///< camera control
    Mission *_mission;          ///< mission management

    UInt32 _orbitedUnitId;      ///< current orbited unit ID

    /** Using this constructor is forbidden. */
    Simulation( const Simulation & ) {}

    /**
     * @brief create
     * @param width viewport width in pixels
     * @param height viewport height in pixels
     * @param mission
     */
    void createSimulation( int width, int height );

    /** @brief Reads campaign XML file and initializes missions files list.*/
    void readMissions();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_SIMULATION_H
