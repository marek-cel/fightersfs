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
