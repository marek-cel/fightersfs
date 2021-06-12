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

#include <gui/WidgetCtrl.h>
#include <ui_WidgetCtrl.h>

#include <hid/hid_Joysticks.h>
#include <gui/DialogAction.h>

////////////////////////////////////////////////////////////////////////////////

WidgetCtrl::WidgetCtrl( QWidget *parent ) :
    QWidget( parent ),
    _ui( new Ui::WidgetCtrl ),

    _defaultAssignmentsInited ( false ),
    _comboDeviceInited ( false )
{
    _actionIds[ hid::Assignment::TriggerBasic     ] = "trigger_basic";        // 0
    //_actionIds[ hid::Assignment::TriggerExtra     ] = "trigger_extra";        // 1
    _actionIds[ hid::Assignment::RollAxis         ] = "roll_axis";            // 2
    _actionIds[ hid::Assignment::RollBankLeft     ] = "roll_bank_left";       // 3
    _actionIds[ hid::Assignment::RollBankRight    ] = "roll_bank_right";      // 4
    _actionIds[ hid::Assignment::PitchAxis        ] = "pitch_axis";           // 5
    _actionIds[ hid::Assignment::PitchNoseUp      ] = "pitch_nose_up";        // 6
    _actionIds[ hid::Assignment::PitchNoseDown    ] = "pitch_nose_down";      // 7
    _actionIds[ hid::Assignment::YawAxis          ] = "yaw_axis";             // 8
    _actionIds[ hid::Assignment::YawTurnLeft      ] = "yaw_turn_left";        // 9
    _actionIds[ hid::Assignment::YawTurnRight     ] = "yaw_turn_right";       // 10
    _actionIds[ hid::Assignment::ThrottleAxis     ] = "throttle_axis";        // 11
    _actionIds[ hid::Assignment::ThrottleIncrease ] = "throttle_increase";    // 12
    _actionIds[ hid::Assignment::ThrottleDecrease ] = "throttle_decrease";    // 13

    _actionNames[ hid::Assignment::TriggerBasic     ] = tr( "Primary Trigger"    );
    //_actionNames[ hid::Assignment::TriggerExtra     ] = tr( "Secondary Trigger"  );
    _actionNames[ hid::Assignment::RollAxis         ] = tr( "Roll (Axis)"        );
    _actionNames[ hid::Assignment::RollBankLeft     ] = tr( "Roll: Bank Left"    );
    _actionNames[ hid::Assignment::RollBankRight    ] = tr( "Roll: Bank Right"   );
    _actionNames[ hid::Assignment::PitchAxis        ] = tr( "Pitch (Axis)"       );
    _actionNames[ hid::Assignment::PitchNoseUp      ] = tr( "Pitch: Nose Up"     );
    _actionNames[ hid::Assignment::PitchNoseDown    ] = tr( "Pitch: Nose Down"   );
    _actionNames[ hid::Assignment::YawAxis          ] = tr( "Yaw (Axis)"         );
    _actionNames[ hid::Assignment::YawTurnLeft      ] = tr( "Yaw: Turn Left"     );
    _actionNames[ hid::Assignment::YawTurnRight     ] = tr( "Yaw: Turn Right"    );
    _actionNames[ hid::Assignment::ThrottleAxis     ] = tr( "Throttle (Axis)"    );
    _actionNames[ hid::Assignment::ThrottleIncrease ] = tr( "Throttle: Increase" );
    _actionNames[ hid::Assignment::ThrottleDecrease ] = tr( "Throttle: Decrease" );

#   if ( HID_MAX_ACTIONS != 13 )
#       error 'HID_MAX_ACTIONS' has been changed! Check code above this line!
#   endif

    _ui->setupUi( this );

    for ( short i = 0; i < HID_MAX_ACTIONS; i++ )
    {
        memset( &(_assignments[ i ]), 0, sizeof(hid::Assignment) );
        _assignments[ i ].type = hid::Assignment::None;
    }

    createTableControls();

    if ( _ui->tableControls->rowCount() != HID_MAX_ACTIONS )
    {
        Log::e() << "Wrong actions count: " << HID_MAX_ACTIONS << std::endl;

#       ifdef SIM_EXITONERROR
        exit( EXIT_FAILURE );
#       endif
    }

    settingsRead();
}

////////////////////////////////////////////////////////////////////////////////

WidgetCtrl::~WidgetCtrl()
{
    settingsSave();

    DELPTR( _ui );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::getAssignments( hid::Assignment assignments[] )
{
    for ( short i = 0; i < HID_MAX_ACTIONS; i++ )
    {
        assignments[ i ] = _assignments[ i ];
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::readData()
{
    if ( !_comboDeviceInited )
    {
        short joysCount = hid::Joysticks::instance()->getJoysCount();

        for ( short i = 0; i < joysCount; i++ )
        {
            hid::Joysticks::Data joyData = hid::Joysticks::instance()->getJoyData( i );

            _ui->comboDevice->addItem( QString( joyData.name.c_str() ) );
        }

        _comboDeviceInited = true;
    }

    settingsRead();

    updateTableControls();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::saveData()
{
    settingsSave();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::updateAssignments()
{
    for ( int i = 0; i < HID_MAX_ACTIONS; i++ )
    {
        hid::Manager::instance()->setAssingment( (hid::Assignment::Action)i, _assignments[ i ] );
    }
}

////////////////////////////////////////////////////////////////////////////////

bool WidgetCtrl::isButtonAssignmentClearEnabled( int action )
{
    if ( action >= 0 && action < HID_MAX_ACTIONS )
    {
        if ( _assignments[ action ].type == hid::Assignment::None )
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////

bool WidgetCtrl::isButtonAssignmentChangeEnabled( int action, int device )
{
    if ( action >= 0 && action < HID_MAX_ACTIONS )
    {
        if ( device == 0 )
        {
            if ( hid::Manager::isAxis( (hid::Assignment::Action)action ) )
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }
    else

    return false;
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::changeAssignment( int action )
{
    if ( action >= 0 && action < HID_MAX_ACTIONS )
    {
        hid::Assignment newAssignment;

        if ( _ui->comboDevice->currentIndex() < 1 )
        {
            newAssignment = DialogAction::getKeyAssignment( this, _assignments[ action ] );
        }
        else
        {
            int joystickId = _ui->comboDevice->currentIndex() - 1;

            if ( hid::Manager::isAxis( (hid::Assignment::Action)action ) )
            {
                newAssignment = DialogAction::getJoyAxisAssignment( this, _assignments[ action ], joystickId );
            }
            else
            {
                newAssignment = DialogAction::getJoyButtAssignment( this, _assignments[ action ], joystickId );
            }
        }

        if ( newAssignment.type != hid::Assignment::None )
        {
            _assignments[ action ] = newAssignment;
        }

        updateTableControls();

        setButtonsState( _ui->tableControls->currentRow(), _ui->comboDevice->currentIndex() );
    }
    else
    {
        Log::e() << "Wrong action number: " << action << std::endl;

#       ifdef SIM_EXITONERROR
        exit( EXIT_FAILURE );
#       endif
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::clearAssignment( int action )
{
    if ( action >= 0 && action < HID_MAX_ACTIONS )
    {
        memset( &(_assignments[ action ]), 0, sizeof(hid::Assignment) );

        _assignments[ action ].type = hid::Assignment::None;

        updateTableControls();
        setButtonsState( _ui->tableControls->currentRow(), _ui->comboDevice->currentIndex() );
    }
    else
    {
        Log::e() << "Wrong action number: " << action << std::endl;

#       ifdef SIM_EXITONERROR
        exit( EXIT_FAILURE );
#       endif
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::createTableControls()
{
    if ( _ui->tableControls->rowCount() < HID_MAX_ACTIONS )
    {
        _ui->tableControls->setRowCount( HID_MAX_ACTIONS );
    }

    for ( int i = 0; i < HID_MAX_ACTIONS; i++ )
    {
        QTableWidgetItem *tableRowHeader = new QTableWidgetItem();
        tableRowHeader->setText( _actionNames[ i ] );
        _ui->tableControls->setVerticalHeaderItem( i, tableRowHeader );

        QTableWidgetItem *tableItem = new QTableWidgetItem();
        _ui->tableControls->setItem( i, 0, tableItem );
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::updateTableControls()
{
    for ( int i = 0; i < HID_MAX_ACTIONS; i++ )
    {
        QString itemText( "" );

        hid::Joysticks::Data joyData;

        switch ( _assignments[ i ].type )
        {
        case hid::Assignment::Keyboard:
            itemText += tr("Keyboard: ");
            if ( _assignments[ i ].data.keyboard.keyId >= 0
              && _assignments[ i ].data.keyboard.keyId < HID_MAX_KEYS )
            {
                itemText += "\"";
                itemText += hid::Manager::_keysNames[ _assignments[ i ].data.keyboard.keyId ].c_str();
                itemText += "\"";
            }
            else
            {
                itemText += tr("NONE");
            }
            break;

        case hid::Assignment::Joystick:
            joyData = hid::Joysticks::instance()->getJoyData( _assignments[ i ].data.joystick.joystickId );

            itemText += joyData.name.c_str();
            itemText += ": ";
            if ( hid::Manager::isAxis( (hid::Assignment::Action)i ) )
            {
                itemText += hid::Joysticks::_axisNames[ _assignments[ i ].data.joystick.axisId ].c_str();
                if ( _assignments[ i ].data.joystick.inverted ) itemText += tr(" (Inverted)");
            }
            else
            {
                if ( _assignments[ i ].data.joystick.buttonId >= 0
                  && _assignments[ i ].data.joystick.buttonId < joyData.buttCount )
                {
                    itemText += tr("Button ");
                    itemText += QString::number( _assignments[ i ].data.joystick.buttonId + 1 );
                }
                else if ( _assignments[ i ].data.joystick.povId >= 0
                       && _assignments[ i ].data.joystick.povId < joyData.povsCount )
                {
                    itemText += tr("POV ");
                    itemText += QString::number( _assignments[ i ].data.joystick.povId + 1 );

                    switch ( _assignments[ i ].data.joystick.direction )
                    {
                        case hid::Assignment::North: itemText += " - N";  break;
                        case hid::Assignment::East:  itemText += " - E";  break;
                        case hid::Assignment::South: itemText += " - S";  break;
                        case hid::Assignment::West:  itemText += " - W";  break;
                        default:  itemText += tr(" - NONE"); break;
                    }
                }
            }
            break;

        default:
            break;
        }

        _ui->tableControls->item( i, 0 )->setText( itemText );
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::setButtonsState( int action, int device )
{
    _ui->buttonAssignmentClear->setEnabled( isButtonAssignmentClearEnabled( action ) );
    _ui->buttonAssignmentChange->setEnabled( isButtonAssignmentChangeEnabled( action, device ) );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::settingsRead()
{
    QSettings settings( SIM_ORG_NAME, SIM_APP_NAME );

    settings.beginGroup( "dialog_conf" );

    restoreGeometry( settings.value( "geometry" ).toByteArray() );

    assignmentsRead( settings );

    settings.endGroup();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::settingsSave()
{
    QSettings settings( SIM_ORG_NAME, SIM_APP_NAME );

    settings.beginGroup( "dialog_conf" );

    settings.setValue( "geometry" , saveGeometry() );

    assignmentsSave( settings );

    settings.endGroup();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::initDefaultAssignments()
{
    if ( !_defaultAssignmentsInited )
    {
        QSettings settings( SIM_ORG_NAME, SIM_APP_NAME );

        settings.beginGroup( "page_controls" );
        {
            _defaultAssignmentsInited = settings.value( "default_assignments_inited", 0 ).toBool();
        }
        settings.endGroup(); // page_controls
    }

    if ( !_defaultAssignmentsInited )
    {
        QSettings settings( SIM_ORG_NAME, SIM_APP_NAME );

        settings.beginGroup( "page_controls" );
        {
            settings.setValue( "default_assignments_inited" , (int)true );
        }
        settings.endGroup(); // page_controls

        int joysCount = hid::Joysticks::instance()->getJoysCount();

        if ( joysCount > 0 )
        {
            _assignments[ hid::Assignment::TriggerBasic ].type = hid::Assignment::Joystick;
            _assignments[ hid::Assignment::TriggerBasic ].data.joystick.joystickId = 0;
            _assignments[ hid::Assignment::TriggerBasic ].data.joystick.axisId     = -1;
            _assignments[ hid::Assignment::TriggerBasic ].data.joystick.inverted   = false;
            _assignments[ hid::Assignment::TriggerBasic ].data.joystick.buttonId   = 0;
            _assignments[ hid::Assignment::TriggerBasic ].data.joystick.povId      = -1;
            _assignments[ hid::Assignment::TriggerBasic ].data.joystick.direction  = hid::Assignment::Centered;

            //_assignments[ hid::Assignment::TriggerExtra ].type = hid::Assignment::Joystick;
            //_assignments[ hid::Assignment::TriggerExtra ].data.joystick.joystickId = 0;
            //_assignments[ hid::Assignment::TriggerExtra ].data.joystick.axisId     = -1;
            //_assignments[ hid::Assignment::TriggerExtra ].data.joystick.inverted   = false;
            //_assignments[ hid::Assignment::TriggerExtra ].data.joystick.buttonId   = 1;
            //_assignments[ hid::Assignment::TriggerExtra ].data.joystick.povId      = -1;
            //_assignments[ hid::Assignment::TriggerExtra ].data.joystick.direction  = hid::Assignment::Centered;

            _assignments[ hid::Assignment::RollAxis ].type = hid::Assignment::Joystick;
            _assignments[ hid::Assignment::RollAxis ].data.joystick.joystickId = 0;
            _assignments[ hid::Assignment::RollAxis ].data.joystick.axisId     = hid::Joysticks::AxisX;
            _assignments[ hid::Assignment::RollAxis ].data.joystick.inverted   = false;
            _assignments[ hid::Assignment::RollAxis ].data.joystick.buttonId   = -1;
            _assignments[ hid::Assignment::RollAxis ].data.joystick.povId      = -1;
            _assignments[ hid::Assignment::RollAxis ].data.joystick.direction  = hid::Assignment::Centered;

            _assignments[ hid::Assignment::PitchAxis ].type = hid::Assignment::Joystick;
            _assignments[ hid::Assignment::PitchAxis ].data.joystick.joystickId = 0;
            _assignments[ hid::Assignment::PitchAxis ].data.joystick.axisId     = hid::Joysticks::AxisY;
            _assignments[ hid::Assignment::PitchAxis ].data.joystick.inverted   = false;
            _assignments[ hid::Assignment::PitchAxis ].data.joystick.buttonId   = -1;
            _assignments[ hid::Assignment::PitchAxis ].data.joystick.povId      = -1;
            _assignments[ hid::Assignment::PitchAxis ].data.joystick.direction  = hid::Assignment::Centered;
        }
        else
        {
            _assignments[ hid::Assignment::TriggerBasic ].type = hid::Assignment::Keyboard;
            _assignments[ hid::Assignment::TriggerBasic ].data.keyboard.keyId = hid::Assignment::KeySpace;

            // TODO
            //_assignments[ hid::Assignment::TriggerExtra ].type = hid::Assignment::Keyboard;
            //_assignments[ hid::Assignment::TriggerExtra ].data.keyboard.keyId = hid::Assignment::KeySpace;

            _assignments[ hid::Assignment::RollBankLeft ].type = hid::Assignment::Keyboard;
            _assignments[ hid::Assignment::RollBankLeft ].data.keyboard.keyId = hid::Assignment::KeyLeft;

            _assignments[ hid::Assignment::RollBankRight ].type = hid::Assignment::Keyboard;
            _assignments[ hid::Assignment::RollBankRight ].data.keyboard.keyId = hid::Assignment::KeyRight;

            _assignments[ hid::Assignment::PitchNoseUp ].type = hid::Assignment::Keyboard;
            _assignments[ hid::Assignment::PitchNoseUp ].data.keyboard.keyId = hid::Assignment::KeyDown;

            _assignments[ hid::Assignment::PitchNoseDown ].type = hid::Assignment::Keyboard;
            _assignments[ hid::Assignment::PitchNoseDown ].data.keyboard.keyId = hid::Assignment::KeyUp;
        }

        _assignments[ hid::Assignment::YawTurnLeft ].type = hid::Assignment::Keyboard;
        _assignments[ hid::Assignment::YawTurnLeft ].data.keyboard.keyId = hid::Assignment::KeyComma;

        _assignments[ hid::Assignment::YawTurnRight ].type = hid::Assignment::Keyboard;
        _assignments[ hid::Assignment::YawTurnRight ].data.keyboard.keyId = hid::Assignment::KeyPeriod;

        _assignments[ hid::Assignment::ThrottleIncrease ].type = hid::Assignment::Keyboard;
        _assignments[ hid::Assignment::ThrottleIncrease ].data.keyboard.keyId = hid::Assignment::KeyEquals;

        _assignments[ hid::Assignment::ThrottleDecrease ].type = hid::Assignment::Keyboard;
        _assignments[ hid::Assignment::ThrottleDecrease ].data.keyboard.keyId = hid::Assignment::KeyMinus;

#       if ( HID_MAX_ACTIONS != 13 )
#           error 'HID_MAX_ACTIONS' has been changed! Check code above this line!
#       endif

        settingsSave();
    }

    _defaultAssignmentsInited = true;
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::assignmentsRead( QSettings &settings )
{
    initDefaultAssignments();

    for ( int i = 0; i < HID_MAX_ACTIONS; i++ )
    {
        assignmentRead( settings, _assignments[ i ], (hid::Assignment::Action)i );
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::assignmentsSave( QSettings &settings )
{
    for ( int i = 0; i < HID_MAX_ACTIONS; i++ )
    {
        assignmentSave( settings, _assignments[ i ], (hid::Assignment::Action)i );
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::assignmentRead( QSettings &settings, hid::Assignment &assignment, hid::Assignment::Action action )
{
    settings.beginGroup( "controls" );
    {
        settings.beginGroup( _actionIds[ action ].c_str() );
        {
            int joysCount = hid::Joysticks::instance()->getJoysCount();

            int tempType       = 0;
            int tempKeyId      = 0;
            int tempJoystickId = 0;
            int tempAxisId     = 0;
            int tempInverted   = 0;
            int tempButtonId   = 0;
            int tempPovId      = 0;
            int tempDirection  = 0;

            tempType = settings.value( "type", hid::Assignment::None ).toInt();

            switch ( tempType )
            {
            case hid::Assignment::Keyboard:
                tempKeyId = settings.value( "key_id", -1 ).toInt();

                assignment.type = hid::Assignment::Keyboard;
                assignment.data.keyboard.keyId = (short)tempKeyId;
                break;

            case hid::Assignment::Joystick:
                tempJoystickId = settings.value( "joystick_id" , -1 ).toInt();
                tempAxisId     = settings.value( "axis_id"     , -1 ).toInt();
                tempInverted   = settings.value( "inverted"    , -1 ).toInt();
                tempButtonId   = settings.value( "button_id"   , -1 ).toInt();
                tempPovId      = settings.value( "pov_id"      , -1 ).toInt();
                tempDirection  = settings.value( "direction"   , -1 ).toInt();

                if ( tempJoystickId < joysCount )
                {
                    assignment.type = hid::Assignment::Joystick;
                    assignment.data.joystick.joystickId = (short)tempJoystickId;
                    assignment.data.joystick.axisId     = (short)tempAxisId;
                    assignment.data.joystick.inverted   = (bool)tempInverted;
                    assignment.data.joystick.buttonId   = (short)tempButtonId;
                    assignment.data.joystick.povId      = (short)tempPovId;
                    assignment.data.joystick.direction  = (hid::Assignment::POVs)tempDirection;
                }
                else
                {
                    assignment.type = hid::Assignment::None;
                    memset( &(assignment.data), 0, sizeof(hid::Assignment::DeviceData) );
                }
                break;

            default:
                assignment.type = hid::Assignment::None;
                break;
            }
        }
        settings.endGroup();
    }
    settings.endGroup(); // controls
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::assignmentSave( QSettings &settings, hid::Assignment &assignment, hid::Assignment::Action action )
{
    settings.beginGroup( "controls" );
    {
        settings.beginGroup( _actionIds[ action ].c_str() );
        {
            switch ( assignment.type )
            {
            case hid::Assignment::Keyboard:
                settings.setValue( "type", hid::Assignment::Keyboard );
                settings.setValue( "key_id", (int)assignment.data.keyboard.keyId );
                break;

            case hid::Assignment::Joystick:
                settings.setValue( "type", hid::Assignment::Joystick );
                settings.setValue( "joystick_id" , (int)assignment.data.joystick.joystickId );
                settings.setValue( "axis_id"     , (int)assignment.data.joystick.axisId     );
                settings.setValue( "inverted"    , (int)assignment.data.joystick.inverted   );
                settings.setValue( "button_id"   , (int)assignment.data.joystick.buttonId   );
                settings.setValue( "pov_id"      , (int)assignment.data.joystick.povId      );
                settings.setValue( "direction"   , (int)assignment.data.joystick.direction  );
                break;

            default:
                settings.setValue( "type", hid::Assignment::None );
                settings.setValue( "key_id"      , 0 );
                settings.setValue( "joystick_id" , 0 );
                settings.setValue( "axis_id"     , 0 );
                settings.setValue( "inverted"    , 0 );
                settings.setValue( "button_id"   , 0 );
                settings.setValue( "pov_id"      , 0 );
                settings.setValue( "direction"   , 0 );
                break;
            }
        }
        settings.endGroup();

    }
    settings.endGroup(); // controls
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::on_buttonAssignmentClear_clicked()
{
    clearAssignment( _ui->tableControls->currentRow() );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::on_buttonAssignmentChange_clicked()
{
    changeAssignment( _ui->tableControls->currentRow() );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::on_comboDevice_currentIndexChanged( int index )
{
    setButtonsState( _ui->tableControls->currentRow(), index );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::on_tableControls_cellDoubleClicked( int row, int )
{
    if ( isButtonAssignmentChangeEnabled( row, _ui->comboDevice->currentIndex() ) )
    {
        changeAssignment( row );
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCtrl::on_tableControls_currentCellChanged( int row, int, int, int )
{
    setButtonsState( row, _ui->comboDevice->currentIndex() );
}

