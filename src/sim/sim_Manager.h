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
#ifndef SIM_MANAGER_H
#define SIM_MANAGER_H

////////////////////////////////////////////////////////////////////////////////

/**
 * @mainpage
 * <h1>FSG - Flight Simulation Game</h1>
 *
 * <p>FSG is an arcade style combat flight simulation video game engine written
 * in C++ intended to be used with PC's, mobile devices such as smartphones and
 * tablets as well as smart TVs and set-top boxes.</p>
 *
 * <p>OpenGL ES or GLES for short and OpenSceneGraph or OSG are used
 * as rendering engine base and math library.</p>
 */

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Simulation.h>

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Simulation manager singleton class.
 * <p>This class should be used as the only interface to use FSG.</p>
 *
 * @see Simulation
 * @see UnitViewer
 */
class Manager : public Singleton< Manager >
{
    friend class Singleton< Manager >;

private:

    /**
     * You should use static function getInstance() due to get refernce
     * to Manager class instance.
     */
    Manager();

    /** Using this constructor is forbidden. */
    Manager( const Manager & ) : Singleton< Manager >() {}

public:

    /** Destructor. */
    virtual ~Manager();

    /**
     * Initializes mission.
     * @param width viewport width in pixels
     * @param height viewport height in pixels
     * @param mission_index mission file index as defined in missions/campaign.xml
     */
    void init( int width, int height, UInt32 mission_index );

    /** Reloads model view or simulation (reloads models, textures, etc). */
    void reload();

    /**
     * Updates model view or simulation. This function should be called every step.
     * @param timeStep [s] simulation time step
     */
    void update( double timeStep );

    /** Pauses simulation. */
    void pause();

    /** Unpauses simulation. */
    void unpause();

    /** Restarts simulation. */
    void restart();

    /**
     * Destroys simulation object, resets data, etc.
     * This function is not thread safe!
     */
    void destroy();

    /** Returns current camera manipulator. */
    inline osgGA::CameraManipulator* getCameraManipulator()
    {
        return _cameraManipulator.get();
    }

    /** Returns HUD (Head-up-Display) scene node. */
    inline osg::Group* getNodeHUD() { return _nodeHUD.get(); }

    /** Returns OTW (Out-the-Window) scene node. */
    inline osg::Group* getNodeOTW() { return _nodeOTW.get(); }

    /** Returns mission status. */
    inline Status getStatus() const { return _status; }

    /** Returns Id of the currently orbited (by camera) unit. */
    inline UInt32 getOrbitedUnitId() const
    {
        if ( _simulation )
            return _simulation->getOrbitedUnitId();

        return 0;
    }

    /** Returns true if mission is finished due to success ot failure. */
    inline bool isFinished() const { return _finished; }

    /** Returns true if simulation is inited. */
    inline bool isInited() const
    {
        return _simulation != 0;
    }

    /** Returns true if simulation is inited and ready. */
    inline bool isReady() const { return _simulation != 0 && _inited; }

    /** Returns true if simulation is paused. */
    inline bool isPaused() const { return _paused; }

    /** Returns true if mission is not finished. */
    inline bool isPending() const { return _pending; }

    /** Sets autopilot state. */
    inline void setAutopilot( bool autopilot )
    {
        _autopilot = autopilot;
    }

    /**
     * Sets controls input.
     * @param trigger basic trigger
     * @param ctrlRoll <-1;1> roll control
     * @param ctrlPitch <-1;1> pitch control
     * @param ctrlYaw <-1;1> yaw control
     * @param throttle <0;1> throttle
     */
    void setControls( bool trigger,
                      float ctrlRoll,
                      float ctrlPitch,
                      float ctrlYaw,
                      float throttle );

    /**
     * Sets current language.
     * @param index
     */
    void setLanguage( int index );

    /** Sets chase view. */
    void setViewChase();

    /** Sets flyby view. */
    void setViewFlyby();

    /** Sets front view. */
    void setViewFront();

    /** Sets orbit view. */
    void setViewOrbit();

    /** Sets pilot view. */
    void setViewPilot();

    /** Sets shift view. */
    void setViewShift();

    /** Sets world view. */
    void setViewWorld();

    /** Toggles through units to be orbited. */
    void toggleOrbitUnit();

private:

    Simulation *_simulation;            ///< simulation object

    osg::ref_ptr<osg::Group> _nodeHUD;  ///< HUD root node
    osg::ref_ptr<osg::Group> _nodeOTW;  ///< OTW root node

    osg::ref_ptr<osgGA::CameraManipulator> _cameraManipulator;  ///< current camera manipulator

    UInt32 _mission_index;              ///< mission file index

    Status _status;                     ///< last mission status

    double _timeStep;                   ///< [s] simulation time step
    double _timeInit;                   ///< [s] time since init

    bool _autopilot;                    ///< specifies if autopilot is engaged
    bool _finished;                     ///< specifies if simulation is finished
    bool _inited;                       ///<
    bool _paused;                       ///< specifies if simulation is paused
    bool _pending;                      ///< specifies if mission is pending
    bool _started;                      ///< specifies if simulation have been started after initial pause

    /** Destroys simulation object, resets data, etc. */
    void reset();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_MANAGER_H
