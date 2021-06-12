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

    typedef std::vector< std::string > Campaigns;
    typedef std::vector< std::string > Missions;

    /** */
    static std::string getMissionFile( UInt32 campaign_index, UInt32 mission_index );

    /** */
    static Campaigns getCampaigns();

    /** */
    static Missions  getMissions( UInt32 campaign_index );

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
     * @param campaign_index campaign file index
     * @param mission_index mission file index
     */
    void init( UInt32 campaign_index, UInt32 mission_index );

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
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_SIMULATION_H
