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

#include <sim/sim_Manager.h>

#include <sim/sim_Captions.h>
#include <sim/sim_Languages.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Data::SimulationData Data::_data;

////////////////////////////////////////////////////////////////////////////////

Manager::Manager() :
    _simulation ( nullptr ),

    _campaign_index ( 0 ),
    _mission_index  ( 0 ),

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

void Manager::init( int width, int height, UInt32 campaign_index, UInt32 mission_index )
{
    if ( _simulation != nullptr )
    {
        reset();
    }

    _simulation = new Simulation( width, height );

    _campaign_index = campaign_index;
    _mission_index  = mission_index;

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
                _simulation->init( _campaign_index, _mission_index );
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

    _nodeHUD = nullptr;
    _nodeOTW = nullptr;

    _timeStep = 0.0;
    _timeInit = 0.0;

    _autopilot = false;
    _finished  = false;
    _inited    = false;
    _paused    = false;
    _pending   = false;
    _started   = false;
}
