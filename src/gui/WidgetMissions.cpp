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

#include <gui/WidgetMissions.h>
#include <ui_WidgetMissions.h>

////////////////////////////////////////////////////////////////////////////////

WidgetMissions::WidgetMissions( QWidget *parent ) :
    QWidget( parent ),
    _ui ( new Ui::WidgetMissions ),

    _campaign ( 0 )
{
    _ui->setupUi( this );
}

////////////////////////////////////////////////////////////////////////////////

WidgetMissions::~WidgetMissions()
{
    DELPTR( _ui );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::setCampaign( int campaign )
{
    _campaign = campaign;

    initMissions();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::resizeEvent( QResizeEvent *event )
{
    //////////////////////////////
    QWidget::resizeEvent( event );
    //////////////////////////////

    updateMissionImage();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::initMissions()
{
    _ui->comboBoxMissions->clear();

    for ( unsigned int i = 0; i < 3; i++ )
    {
        QString name = QString::number( i );
        _ui->comboBoxMissions->addItem( name );
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::updateMissionImage()
{
    QPixmap pixmap( Path::get( "missions/training_01_img.png" ).c_str() );

    int w = _ui->labelMissionImage->width();
    int h = _ui->labelMissionImage->height();

    pixmap = pixmap.scaled( w, h, Qt::KeepAspectRatio );

    //std::cout << w << " x " << h << std::endl;

    _ui->labelMissionImage->setPixmap( pixmap );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::on_comboBoxMissions_currentIndexChanged( int index )
{
    updateMissionImage();
}
