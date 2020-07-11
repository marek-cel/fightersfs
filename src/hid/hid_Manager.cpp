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

#include <hid/hid_Manager.h>

#include <memory.h>

#include <sim/sim_Defines.h>
#include <hid/hid_Joysticks.h>

////////////////////////////////////////////////////////////////////////////////

using namespace hid;

////////////////////////////////////////////////////////////////////////////////

const std::string Manager::_actionNames[] = {
    "Primary Trigger",
    "Secondary Trigger",
    "Roll (Axis)",
    "Roll: Bank Left",
    "Roll: Bank Right",
    "Pitch (Axis)",
    "Pitch: Nose Up",
    "Pitch: Nose Down",
    "Yaw (Axis)",
    "Yaw: Turn Left",
    "Yaw: Turn Right",
    "Throttle (Axis)",
    "Throttle: Increase",
    "Throttle: Decrease"
};

#if ( HID_MAX_ACTIONS != 14 )
#   error 'HID_MAX_ACTIONS' has been changed! Check code above this line!
#endif

const std::string Manager::_keysNames[] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "`", "-", "=", "[", "]", ";", "'", ",", ".", "/",
    "Space",
    "Left", "Right", "Up", "Down",
    "Insert", "Delete", "Home", "End", "Page Up", "Page Down",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12"
};

#if ( HID_MAX_KEYS != 69 )
#   error 'HID_MAX_KEYS' has been changed! Check code following this line!
#endif

////////////////////////////////////////////////////////////////////////////////

Assignment::POVs Manager::getPOV( short pov_deg )
{
    Assignment::POVs result = Assignment::Centered;

    if ( !( pov_deg < 0 ) )
    {
        if ( pov_deg < 90 )
        {
            if ( pov_deg > 0 )
                result = Assignment::NorthEast;
            else
                result = Assignment::North;
        }
        else if ( pov_deg < 180 )
        {
            if ( pov_deg > 90 )
                result = Assignment::SouthEast;
            else
                result = Assignment::East;
        }
        else if ( pov_deg < 270 )
        {
            if ( pov_deg > 180 )
                result = Assignment::SouthWest;
            else
                result = Assignment::South;
        }
        else
        {
            if ( pov_deg > 270 )
                result = Assignment::NorthWest;
            else
                result = Assignment::West;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

bool Manager::isAxis( Assignment::Action action )
{
    return ( action == Assignment::RollAxis
          || action == Assignment::PitchAxis
          || action == Assignment::YawAxis
          || action == Assignment::ThrottleAxis );

#   if ( HID_MAX_ACTIONS != 14 )
#       error 'HID_MAX_ACTIONS' has been changed! Check code above this line!
#   endif
}

////////////////////////////////////////////////////////////////////////////////

Manager::Manager() :
    _speedControls ( 1.0f ),
    _speedThrottle ( 0.5f ),

    _timeStep ( 0.0 )
{
    for ( int i = 0; i < HID_MAX_ACTIONS; i++ )
    {
        _assignments[ i ].type = Assignment::None;

        memset( &(_assignments[ i ].data), 0, sizeof(Assignment::DeviceData) );
    }

    for ( int i = 0; i < HID_MAX_KEYS; i++ ) _keysState[ i ] = false;

    reset();
}

////////////////////////////////////////////////////////////////////////////////

Manager::~Manager() {}

////////////////////////////////////////////////////////////////////////////////

void Manager::init()
{
    Joysticks::instance()->init();
}

////////////////////////////////////////////////////////////////////////////////

void Manager::update( double timeStep )
{
    _timeStep =  timeStep;

    Joysticks::instance()->update();

    updateAxisActions();
    updateMiscActions();
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setThrottle( float throttle )
{
    _throttle = throttle;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::setAssingment( Assignment::Action action, const Assignment &assignment )
{
    _assignments[ action ] = assignment;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::getAxisValue( const Assignment &assignment, float &value, int absolute )
{
    if ( assignment.type == Assignment::Joystick )
    {
        Joysticks::Data joyData = Joysticks::instance()->getJoyData( assignment.data.joystick.joystickId );

        if ( joyData.active )
        {
            value = joyData.axis[ assignment.data.joystick.axisId ];

            if ( assignment.data.joystick.inverted ) value *= -1.0f;

            if ( absolute ) value = ( value + 1.0f ) / 2.0f;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Manager::getCtrlValue( Assignment::Action decreaseAction,
                         Assignment::Action increaseAction,
                         float &value,
                         float speed,
                         float min,
                         float max )
{
    bool tempDecrease = getButtState( _assignments[ decreaseAction ] );
    bool tempIncrease = getButtState( _assignments[ increaseAction ] );

    if ( !tempDecrease && !tempIncrease )
    {
        if      ( value > 0.0 ) value = value - speed * _timeStep;
        else if ( value < 0.0 ) value = value + speed * _timeStep;
    }
    else if ( tempDecrease && value > min ) value = value - speed * _timeStep;
    else if ( tempIncrease && value < max ) value = value + speed * _timeStep;

    if ( value < min ) value = min;
    if ( value > max ) value = max;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::getRealValue( Assignment::Action decreaseAction,
                         Assignment::Action increaseAction,
                         float &value,
                         float speed,
                         float min,
                         float max )
{
    bool tempDecrease = getButtState( _assignments[ decreaseAction ] );
    bool tempIncrease = getButtState( _assignments[ increaseAction ] );

    if ( tempDecrease && value > min ) value = value - speed * _timeStep;
    if ( tempIncrease && value < max ) value = value + speed * _timeStep;

    if ( value < min ) value = min;
    if ( value > max ) value = max;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::getRealValue( Assignment::Action applyAction,
                         float &value,
                         float speed,
                         float min,
                         float max )
{
    bool tempApply = getButtState( _assignments[ applyAction ] );

    if ( !tempApply && value > min ) value = value - speed * _timeStep;
    if (  tempApply && value < max ) value = value + speed * _timeStep;

    if ( value < min ) value = min;
    if ( value > max ) value = max;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::getRealValue( Assignment::Action toggleAction,
                         bool  &togglePrev,
                         bool  &state,
                         float &value,
                         float speed,
                         float min,
                         float max )
{
    bool toggle = getButtState( _assignments[ toggleAction ] );

    if ( toggle && !togglePrev )
    {
        state = !state;
    }

    togglePrev = toggle;

    if ( !state && value > min ) value  = value - speed * _timeStep;
    if (  state && value < max ) value  = value + speed * _timeStep;

    if ( value < min ) value = min;
    if ( value > max ) value = max;
}

////////////////////////////////////////////////////////////////////////////////

bool Manager::getButtState( const Assignment &assignment )
{
    bool state = false;

    if ( assignment.type == Assignment::Joystick )
    {
        Joysticks::Data joyData = Joysticks::instance()->getJoyData( assignment.data.joystick.joystickId );

        if ( joyData.active )
        {
            if ( assignment.data.joystick.buttonId >= 0
              && assignment.data.joystick.buttonId < joyData.buttCount )
            {
                state = joyData.butt[ assignment.data.joystick.buttonId ];
            }
            else if ( assignment.data.joystick.povId >= 0
                   && assignment.data.joystick.povId < joyData.povsCount )
            {
                if ( assignment.data.joystick.direction != Assignment::Centered )
                {
                    state = ( getPOV( joyData.povs[ assignment.data.joystick.povId ] )
                          & assignment.data.joystick.direction ) != 0;
                }
            }
        }
    }
    else if ( assignment.type == Assignment::Keyboard )
    {
        if ( assignment.data.keyboard.keyId != -1 )
        {
            state = _keysState[ assignment.data.keyboard.keyId ];
        }
    }

    return state;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::reset()
{
    _trigger_basic = false;
    _trigger_extra = false;

    _ctrlRoll  = 0.0f;
    _ctrlPitch = 0.0f;
    _ctrlYaw   = 0.0f;
    _throttle  = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::updateAxisActions()
{
    // roll
    if ( _assignments[ Assignment::RollAxis ].type == Assignment::None )
    {
        getCtrlValue( Assignment::RollBankLeft,
                      Assignment::RollBankRight,
                      _ctrlRoll, _speedControls, -1.0f, 1.0f );
    }
    else
    {
        getAxisValue( _assignments[ Assignment::RollAxis ], _ctrlRoll );
    }

    // pitch
    if ( _assignments[ Assignment::PitchAxis ].type == Assignment::None )
    {
        getCtrlValue( Assignment::PitchNoseDown,
                      Assignment::PitchNoseUp,
                      _ctrlPitch, _speedControls, -1.0f, 1.0f );
    }
    else
    {
        getAxisValue( _assignments[ Assignment::PitchAxis ], _ctrlPitch );
    }

    // yaw
    if ( _assignments[ Assignment::YawAxis ].type == Assignment::None )
    {
        getCtrlValue( Assignment::YawTurnLeft,
                      Assignment::YawTurnRight,
                      _ctrlYaw, _speedControls, -1.0f, 1.0f );
    }
    else
    {
        getAxisValue( _assignments[ Assignment::YawAxis ], _ctrlYaw );
    }

    // throttle
    if ( _assignments[ Assignment::ThrottleAxis ].type == Assignment::None )
    {
        getRealValue( Assignment::ThrottleDecrease,
                      Assignment::ThrottleIncrease,
                      _throttle, _speedThrottle, 0.0f, 1.0f );
    }

    getAxisValue( _assignments[ Assignment::ThrottleAxis ], _throttle, 1 );
}

////////////////////////////////////////////////////////////////////////////////

void Manager::updateMiscActions()
{
    _trigger_basic = getButtState( _assignments[ Assignment::TriggerBasic ] );
    _trigger_extra = getButtState( _assignments[ Assignment::TriggerExtra ] );
}
