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
#ifndef WIDGETDATA_H
#define WIDGETDATA_H

////////////////////////////////////////////////////////////////////////////////

#include <QWidget>

#include <defs.h>

#include <gui/Units.h>

////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class WidgetData;
}

////////////////////////////////////////////////////////////////////////////////

/** @brief Data widget class. */
class WidgetData : public QWidget
{
    Q_OBJECT

public:

    explicit WidgetData( QWidget *parent = NULLPTR );

    ~WidgetData();

    void step();

signals:

    void back();

private:

    Ui::WidgetData *_ui;                    ///< UI object

    void updateDataAerial( const Units::Data::DataAerial &data );
    void updateDataMarine( const Units::Data::DataMarine &data );
    void updateDataGround( const Units::Data::DataGround &data );

private slots:

    void on_pushButtonBack_clicked();
    void on_comboBoxUnits_currentIndexChanged( int index );
};

////////////////////////////////////////////////////////////////////////////////

#endif // WIDGETDATA_H
