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

#include <sim/sim_Manager.h>

#include <sim/sim_Captions.h>
#include <sim/sim_Languages.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Data::SimulationData Data::_data;

////////////////////////////////////////////////////////////////////////////////

Manager::Manager() :
    _simulation ( 0 ),

    _mission_index ( 0 ),

    _status ( Pending ),

    _timeStep ( 0.0 ),
    _timeInit ( 0.0 ),

    _autopilot ( false ),
    _finished  ( false ),
    _inited    ( false ),
    _paused    ( false ),
    _pending   ( false ),
    _started   ( false )
{
    Data::reset();
}

////////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
    DELPTR( _simulation );
}

////////////////////////////////////////////////////////////////////////////////

void Manager::init( int width, int height, UInt32 mission_index )
{
    if ( _simulation != 0 )
    {
        reset();
    }

    _simulation = new Simulation( width, height );

    _mission_index = mission_index;

    setViewChase();

    _cameraManipulator = _simulation->getCameraManipulator();

    _nodeHUD = _simulation->getNodeHUD();
    _nodeOTW = _simulation->getNodeOTW();

    _status = Pending;

    _autopilot = false;
    _finished  = false;
    _inited    = false;
    _paused    = true;
    _pending   = true;
    _started   = false;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::reload()
{
    if ( _simulation )
    {
        _simulation->load();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::update( double timeStep )
{
    _timeStep = timeStep;

    if ( _timeStep < SIM_TIME_STEP_MIN ) _timeStep = SIM_TIME_STEP_MIN;
    if ( _timeStep > SIM_TIME_STEP_MAX ) _timeStep = SIM_TIME_STEP_MAX;

    if ( _simulation != 0 )
    {
        if ( !_inited )
        {
            if ( _timeInit > 1.0 )
            {
                _simulation->init( _mission_index );
                _simulation->load();

                _inited = true;
            }

            _timeInit += _timeStep;
        }
        else
        {
            Data::get()->controls.autopilot = _autopilot;

            if ( _paused )
            {
                if ( !Data::get()->paused )
                {
                    Data::get()->message.visible = true;
                    Data::get()->message.overlay = true;

                    if ( _started )
                    {
                        strcpy( Data::get()->message.text, Captions::instance()->getResume().c_str() );
                    }
                    else
                    {
                        strcpy( Data::get()->message.text, Captions::instance()->getBegin().c_str() );
                    }
                }
            }
            else
            {
                _started = true;

                if ( !_pending )
                {
                    if ( Data::get()->mission.time_end > SIM_END_DELAY )
                    {
                        _finished = true;
                        _paused   = true;
                    }
                }
            }

            Data::get()->paused = _paused;

            /////////////////////////////////
            _simulation->update( _timeStep );
            /////////////////////////////////

            _status = Data::get()->mission.status;

            _pending = _status == Pending;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::pause()
{
    if ( _inited )
    {
        _paused = true;

        if ( _simulation )
        {
            _simulation->pause();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::unpause()
{
    if ( _inited )
    {
        _paused = false;

        if ( _simulation )
        {
            _simulation->unpause();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::restart()
{
    if ( _simulation )
    {
        _simulation->restart();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::destroy()
{
    reset();
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setControls( bool trigger,
                           float ctrlRoll,
                           float ctrlPitch,
                           float ctrlYaw,
                           float throttle )
{
    Data::get()->controls.trigger = trigger;

    Data::get()->controls.ctrlRoll  = ctrlRoll;
    Data::get()->controls.ctrlPitch = ctrlPitch;
    Data::get()->controls.ctrlYaw   = ctrlYaw;
    Data::get()->controls.throttle  = throttle;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setLanguage( int index )
{
    sim::Languages::instance()->setCurrent( index );
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setViewChase()
{
    if ( _simulation )
    {
        _simulation->setViewChase();
        _cameraManipulator = _simulation->getCameraManipulator();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setViewFlyby()
{
    if ( _simulation )
    {
        _simulation->setViewFlyby();
        _cameraManipulator = _simulation->getCameraManipulator();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setViewFront()
{
    if ( _simulation )
    {
        _simulation->setViewFront();
        _cameraManipulator = _simulation->getCameraManipulator();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setViewOrbit()
{
    if ( _simulation )
    {
        _simulation->setViewOrbit();
        _cameraManipulator = _simulation->getCameraManipulator();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setViewPilot()
{
    if ( _simulation )
    {
        _simulation->setViewPilot();
        _cameraManipulator = _simulation->getCameraManipulator();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setViewShift()
{
    if ( _simulation )
    {
        _simulation->setViewShift();
        _cameraManipulator = _simulation->getCameraManipulator();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setViewWorld()
{
    if ( _simulation )
    {
        _simulation->setViewWorld();
        _cameraManipulator = _simulation->getCameraManipulator();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::toggleOrbitUnit()
{
    if ( _simulation )
    {
        _simulation->toggleOrbitUnit();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::reset()
{
    Data::reset();

    DELPTR( _simulation );

    _nodeHUD = 0;
    _nodeOTW = 0;

    _timeStep = 0.0;
    _timeInit = 0.0;

    _autopilot = false;
    _finished  = false;
    _inited    = false;
    _paused    = false;
    _pending   = false;
    _started   = false;
}
