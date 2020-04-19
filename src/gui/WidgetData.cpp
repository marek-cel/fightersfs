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

#include <gui/WidgetData.h>
#include <ui_WidgetData.h>

#include <gui/Units.h>

////////////////////////////////////////////////////////////////////////////////

WidgetData::WidgetData( QWidget *parent ) :
    QWidget ( parent ),
    _ui ( new Ui::WidgetData )
{
    _ui->setupUi( this );

    for ( int i = 0; i < Units::instance()->getCount(); i++ )
    {
        QString name = Units::instance()->getData( i ).name.get().c_str();
        _ui->comboBoxUnits->addItem( name );
    }
}

////////////////////////////////////////////////////////////////////////////////

WidgetData::~WidgetData()
{
    DELPTR( _ui );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::on_pushButtonBack_clicked()
{
    emit back();
}
