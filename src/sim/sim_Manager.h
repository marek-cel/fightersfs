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
#ifndef SIM_MANAGER_H
#define SIM_MANAGER_H

////////////////////////////////////////////////////////////////////////////////

/**
 * @mainpage
 *
 * @section FSG - Flight Simulation Game
 *
 * FSG is an arcade style combat flight simulation video game engine written
 * in C++ intended to be used with PC's, mobile devices such as smartphones and
 * tablets as well as smart TVs and set-top boxes.
 *
 * OpenGL ES or GLES for short and OpenSceneGraph or OSG are used as rendering
 * engine base and math library.
 */

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Simulation.h>

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Simulation manager singleton class.
 *`
 * This class should be used as the only interface to use FSG.
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

    /** @brief Destructor. */
    virtual ~Manager();

    /**
     * @brief Initializes mission.
     * @param width viewport width in pixels
     * @param height viewport height in pixels
     * @param mission_index mission file index as defined in campaign file
     */
    void init( int width, int height, UInt32 campaign_index, UInt32 mission_index );

    /** @brief Reloads model view or simulation (reloads models, textures, etc). */
    void reload();

    /**
     * @brief Updates model view or simulation. This function should be called every step.
     * @param timeStep [s] simulation time step
     */
    void update( double timeStep );

    /** @brief Pauses simulation. */
    void pause();

    /** @brief Unpauses simulation. */
    void unpause();

    /** @brief Restarts simulation. */
    void restart();

    /**
     * @brief Destroys simulation object, resets data, etc.
     * This function is not thread safe!
     */
    void destroy();

    /** @brief Returns current camera manipulator. */
    inline osgGA::CameraManipulator* getCameraManipulator()
    {
        return _cameraManipulator.get();
    }

    /** @brief Returns HUD (Head-up-Display) scene node. */
    inline osg::Group* getNodeHUD() { return _nodeHUD.get(); }

    /** @brief Returns OTW (Out-the-Window) scene node. */
    inline osg::Group* getNodeOTW() { return _nodeOTW.get(); }

    /** @brief Returns mission status. */
    inline Status getStatus() const { return _status; }

    /** @brief Returns Id of the currently orbited (by camera) unit. */
    inline UInt32 getOrbitedUnitId() const
    {
        if ( _simulation )
            return _simulation->getOrbitedUnitId();

        return 0;
    }

    /** @brief Returns true if mission is finished due to success ot failure. */
    inline bool isFinished() const { return _finished; }

    /** @brief Returns true if simulation is inited. */
    inline bool isInited() const
    {
        return _simulation != 0;
    }

    /** @brief Returns true if simulation is inited and ready. */
    inline bool isReady() const { return _simulation != 0 && _inited; }

    /** @brief Returns true if simulation is paused. */
    inline bool isPaused() const { return _paused; }

    /** @brief Returns true if mission is not finished. */
    inline bool isPending() const { return _pending; }

    /** @brief Sets autopilot state. */
    inline void setAutopilot( bool autopilot )
    {
        _autopilot = autopilot;
    }

    /**
     * @brief Sets controls input.
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
     * @brief Sets current language.
     * @param index
     */
    void setLanguage( int index );

    /** @brief Sets chase view. */
    void setViewChase();

    /** @brief Sets flyby view. */
    void setViewFlyby();

    /** @brief Sets front view. */
    void setViewFront();

    /** @brief Sets orbit view. */
    void setViewOrbit();

    /** @brief Sets pilot view. */
    void setViewPilot();

    /** @brief Sets shift view. */
    void setViewShift();

    /** @brief Sets world view. */
    void setViewWorld();

    /** @brief Toggles through units to be orbited. */
    void toggleOrbitUnit();

private:

    Simulation *_simulation;            ///< simulation object

    osg::ref_ptr<osg::Group> _nodeHUD;  ///< HUD root node
    osg::ref_ptr<osg::Group> _nodeOTW;  ///< OTW root node

    osg::ref_ptr<osgGA::CameraManipulator> _cameraManipulator;  ///< current camera manipulator

    UInt32 _campaign_index;             ///< campaign file index
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

    /** @brief Destroys simulation object, resets data, etc. */
    void reset();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_MANAGER_H
