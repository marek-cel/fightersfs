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

#include <gui/DialogConf.h>
#include <ui_DialogConf.h>

////////////////////////////////////////////////////////////////////////////////

DialogConf::DialogConf( QWidget *parent ) :
    QDialog ( parent, Qt::FramelessWindowHint ),
    _ui ( new Ui::DialogConf )
{
    _ui->setupUi(this);
}

////////////////////////////////////////////////////////////////////////////////

DialogConf::~DialogConf()
{
    DELPTR( _ui );
}

////////////////////////////////////////////////////////////////////////////////

void DialogConf::readData()
{
    _ui->widgetCtrl->readData();
}

////////////////////////////////////////////////////////////////////////////////

void DialogConf::saveData()
{
    _ui->widgetCtrl->saveData();
}

////////////////////////////////////////////////////////////////////////////////

void DialogConf::updateAssignments()
{
    _ui->widgetCtrl->updateAssignments();
}
