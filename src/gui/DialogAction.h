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
#ifndef DIALOGACTION_H
#define DIALOGACTION_H

////////////////////////////////////////////////////////////////////////////////

#include <QDialog>

#include <defs.h>

#include <hid/hid_Manager.h>

////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class DialogAction;
}

////////////////////////////////////////////////////////////////////////////////

/** @brief Input action change dialog box class. */
class DialogAction : public QDialog
{
    Q_OBJECT

public:

    static hid::Assignment getJoyAxisAssignment( QWidget *parent = 0, hid::Assignment assignment = hid::Assignment(), short joystickId = 0 );
    static hid::Assignment getJoyButtAssignment( QWidget *parent = 0, hid::Assignment assignment = hid::Assignment(), short joystickId = 0 );
    static hid::Assignment getKeyAssignment( QWidget *parent = 0, hid::Assignment assignment = hid::Assignment() );

    /** @brief Constructor. */
    explicit DialogAction( QWidget *parent = NULLPTR, short joystickId = -1 );

    /** @brief Destructor. */
    ~DialogAction();

protected:

    void timerEvent( QTimerEvent *event );

private:

    Ui::DialogAction *_ui;      ///< UI object

    int _timerId;

    short _keyId;

    short _axisCount;
    short _buttCount;
    short _povsCount;

    short _joystickId;
    short _axisId;
    short _buttonId;
    short _povId;
    hid::Assignment::POVs _povDir;
    bool  _inverted;

private slots:

    void on_comboAxis_currentIndexChanged( int index );
    void on_comboButt_currentIndexChanged( int index );
    void on_comboKeys_currentIndexChanged( int index );
    void on_checkAxisInverted_toggled( bool checked );
};

////////////////////////////////////////////////////////////////////////////////

#endif // DIALOGACTION_H
