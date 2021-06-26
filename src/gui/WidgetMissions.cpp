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

#include <gui/Missions.h>

////////////////////////////////////////////////////////////////////////////////

WidgetMissions::WidgetMissions( QWidget *parent ) :
    QWidget( parent ),
    _ui ( new Ui::WidgetMissions ),

    _campaign ( 0 ),
    _mission  ( 0 ),

    _status ( sim::Pending )
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
    _mission  = 0;
    _status   = sim::Pending;

    initMissions();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::setStatus( sim::Status status )
{
    _status = status;

    updateMissionText();
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

    for ( int i = 0; i < Missions::instance()->getMissionsCount( _campaign ); i++ )
    {
        QString name = Missions::instance()->getMission( _campaign, i ).name.get().c_str();
        _ui->comboBoxMissions->addItem( name );
    }

    _ui->comboBoxMissions->setCurrentIndex( 0 );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::updateMissionImage()
{
    QString fileImage = Missions::instance()->getMission( _campaign, _mission ).fileImage;

    QPixmap pixmap( Path::get( fileImage.toStdString() ).c_str() );

    if ( !pixmap.isNull() )
    {
        int w = _ui->labelMissionImage->width();
        int h = _ui->labelMissionImage->height();

        pixmap = pixmap.scaled( w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation );

        //std::cout << w << " x " << h << std::endl;
    }

    _ui->labelMissionImage->setPixmap( pixmap );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::updateMissionText()
{
    QString text;

    switch ( _status )
    {
    case sim::Pending:
        text = Missions::instance()->getMission( _campaign, _mission ).introduction.get().c_str();
        break;

    case sim::Failure:
        text = Missions::instance()->getMission( _campaign, _mission ).summaryFailure.get().c_str();
        break;

    case sim::Success:
        text = Missions::instance()->getMission( _campaign, _mission ).summarySuccess.get().c_str();
        break;
    }

    _ui->textBrowserMission->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::on_comboBoxMissions_currentIndexChanged( int index )
{
    _mission = ( index >= 0 ) ? index : 0;
    _status  = sim::Pending;

    updateMissionImage();
    updateMissionText();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetMissions::on_pushButtonStartMission_clicked()
{
    startClicked( _campaign, _mission );
}
