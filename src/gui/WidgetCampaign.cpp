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

#include <gui/WidgetCampaign.h>
#include <ui_WidgetCampaign.h>

#include <gui/Campaigns.h>

////////////////////////////////////////////////////////////////////////////////

WidgetCampaign::WidgetCampaign( QWidget *parent ) :
    QWidget( parent ),
    _ui ( new Ui::WidgetCampaign ),

    _campaign ( 0 )
{
    _ui->setupUi( this );

    initCampaigns();
}

////////////////////////////////////////////////////////////////////////////////

WidgetCampaign::~WidgetCampaign()
{
    DELPTR( _ui );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCampaign::updateCampaign()
{
    updateCampaignImage();
    updateCampaignText();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCampaign::resizeEvent( QResizeEvent *event )
{
    //////////////////////////////
    QWidget::resizeEvent( event );
    //////////////////////////////

    updateCampaignImage();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCampaign::initCampaigns()
{
    _ui->comboBoxCampaigns->clear();

    for ( int i = 0; i < Campaigns::instance()->getCampaignsCount(); i++ )
    {
        Campaigns::CampaignData data = Campaigns::instance()->getCampaign( i );

        if ( !data.hidden )
        {
            _ui->comboBoxCampaigns->addItem( data.name.get().c_str() );
        }
    }

    _campaign = Campaigns::instance()->getRealIndex( 0 );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCampaign::updateCampaignImage()
{
    QString fileImage = Campaigns::instance()->getCampaign( _campaign ).fileImage;

    QPixmap pixmap( Path::get( fileImage.toStdString() ).c_str() );

    if ( !pixmap.isNull() )
    {
        int w = _ui->labelCampaignImage->width();
        int h = _ui->labelCampaignImage->height();

        pixmap = pixmap.scaled( w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation );

        //std::cout << w << " x " << h << std::endl;
    }

    _ui->labelCampaignImage->setPixmap( pixmap );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCampaign::updateCampaignText()
{
    QString text = Campaigns::instance()->getCampaign( _campaign ).synopsis.get().c_str();

    _ui->textBrowserCampaign->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCampaign::on_comboBoxCampaigns_currentIndexChanged( int index )
{
    _campaign = Campaigns::instance()->getRealIndex( index );

    updateCampaignImage();
    updateCampaignText();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCampaign::on_pushButtonStartCampaign_clicked()
{
    startClicked( _campaign );
}
