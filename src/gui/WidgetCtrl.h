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
#ifndef WIDGETCTRL_H
#define WIDGETCTRL_H

////////////////////////////////////////////////////////////////////////////////

#include <QSettings>
#include <QWidget>

#include <defs.h>

#include <hid/hid_Assignment.h>

////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class WidgetCtrl;
}

////////////////////////////////////////////////////////////////////////////////

/** Controls configuration widget class. */
class WidgetCtrl : public QWidget
{
    Q_OBJECT

public:

    /** Constructor. */
    explicit WidgetCtrl( QWidget *parent = NULLPTR );

    /** Destructor. */
    ~WidgetCtrl();

    void getAssignments( hid::Assignment assignments[] );

    void readData();
    void saveData();

    void updateAssignments();

private:

    Ui::WidgetCtrl *_ui;        ///< UI object

    bool _defaultAssignmentsInited;

    bool _comboDeviceInited;

    std::string     _actionNames[ HID_MAX_ACTIONS ];
    hid::Assignment _assignments[ HID_MAX_ACTIONS ];

    bool isButtonAssignmentClearEnabled( int action );
    bool isButtonAssignmentChangeEnabled( int action, int device );

    void changeAssignment( int action );
    void clearAssignment( int action );

    void createTableControls();
    void updateTableControls();

    void setButtonsState( int action, int device );

    void settingsRead();
    void settingsSave();

    void initDefaultAssignments();

    void assignmentsRead( QSettings &settings );
    void assignmentsSave( QSettings &settings );

    void assignmentRead( QSettings &settings, hid::Assignment &assignment, hid::Assignment::Action action );
    void assignmentSave( QSettings &settings, hid::Assignment &assignment, hid::Assignment::Action action );

private slots:

    void on_buttonAssignmentClear_clicked();
    void on_buttonAssignmentChange_clicked();
    void on_comboDevice_currentIndexChanged( int index );
    void on_tableControls_cellDoubleClicked( int row, int );
    void on_tableControls_currentCellChanged( int row, int, int, int );
};

////////////////////////////////////////////////////////////////////////////////

#endif // WIDGETCTRL_H
