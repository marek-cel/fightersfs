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
#ifndef DIALOGCONF_H
#define DIALOGCONF_H

////////////////////////////////////////////////////////////////////////////////

#include <QDialog>

#include <defs.h>

////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class DialogConf;
}

////////////////////////////////////////////////////////////////////////////////

/** @brief Configuration dialog box class. */
class DialogConf : public QDialog
{
    Q_OBJECT

public:

    /** @brief Constructor. */
    explicit DialogConf( QWidget *parent = NULLPTR );

    /** @brief Destructor. */
    ~DialogConf();

    void readData();
    void saveData();

    void updateAssignments();

private:

    Ui::DialogConf *_ui;        ///< UI object
};

////////////////////////////////////////////////////////////////////////////////

#endif // DIALOGCONF_H
