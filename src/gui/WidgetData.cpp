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

#include <gui/WidgetData.h>
#include <ui_WidgetData.h>

#include <gui/Utils.h>

////////////////////////////////////////////////////////////////////////////////

WidgetData::WidgetData( QWidget *parent ) :
    QWidget( parent ),
    _ui( new Ui::WidgetData )
{
    _ui->setupUi( this );

#   ifdef WIN32
    _ui->textBrowserUnitData->setFontFamily( "Courier" );
#   else
    _ui->textBrowserUnitData->setFontFamily( "Monospace" );
#   endif

    initDatabase();
}

////////////////////////////////////////////////////////////////////////////////

WidgetData::~WidgetData()
{
    DELPTR( _ui );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::timeStep()
{
    _ui->widgetUnit->update();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::initDatabase()
{
    for ( unsigned int i = 0; i < Units::instance()->getCount(); i++ )
    {
        QString name = Units::instance()->getData( i ).name.get().c_str();
        _ui->comboBoxUnits->addItem( name );
    }
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::updateDataAerial( const Units::Data::DataAerial &data )
{
    QString length   = locale().toString( data.length   , 'f', 2 ) + " m";
    QString wingspan = locale().toString( data.wingspan , 'f', 2 ) + " m";
    QString height   = locale().toString( data.height   , 'f', 2 ) + " m";
    QString mtow     = locale().toString( data.MTOW     , 'f', 0 ) + " kg";
    QString maxSpeed = locale().toString( data.maxSpeed , 'f', 0 ) + " km/h";
    QString range    = locale().toString( data.range    , 'f', 0 ) + " km";

    double mtow_lbs  = Convert::kg2lb   ( data.MTOW     );
    double speed_kts = Convert::kmh2kts ( data.maxSpeed );
    double range_nmi = Convert::km2nmi  ( data.range    );

    length   += " (" + Utils::m2ftin( data.length   ) + ")";
    wingspan += " (" + Utils::m2ftin( data.wingspan ) + ")";
    height   += " (" + Utils::m2ftin( data.height   ) + ")";
    mtow     += " (" + locale().toString( 10.0 * floor( mtow_lbs  / 10.0 ), 'f', 0 ) + " lbs)";
    maxSpeed += " (" + locale().toString( 10.0 * floor( speed_kts / 10.0 ), 'f', 0 ) + " kts)";
    range    += " (" + locale().toString( 10.0 * floor( range_nmi / 10.0 ), 'f', 0 ) + " nmi)";

    QVector< QString > labels;

    labels.push_back( tr( "Role"          ) );
    labels.push_back( tr( "Manufacturer"  ) );
    labels.push_back( tr( "First Flight"  ) );
    labels.push_back( tr( "Introduction"  ) );
    labels.push_back( tr( "Retired"       ) );
    labels.push_back( tr( "Number Built"  ) );
    labels.push_back( tr( "Primary User"  ) );
    labels.push_back( tr( "Crew"          ) );
    labels.push_back( tr( "Length"        ) );
    labels.push_back( tr( "Wingspan"      ) );
    labels.push_back( tr( "Height"        ) );
    labels.push_back( tr( "MTOW"          ) );
    labels.push_back( tr( "Max Speed"     ) );
    labels.push_back( tr( "Range"         ) );
    labels.push_back( tr( "Powerplant"    ) );
    labels.push_back( tr( "Armament"      ) );

    int l_max = 0;

    for ( auto l : labels )
    {
        l_max = std::max( l_max, l.length() );
    }

    QString text;

    text += QString( "%1" ).arg( labels[  0 ], l_max, ' ' ) + ": " + QString( data.role.get().c_str() )       + "\n";
    text += QString( "%1" ).arg( labels[  1 ], l_max, ' ' ) + ": " + data.manufacturer                        + "\n";
    text += QString( "%1" ).arg( labels[  2 ], l_max, ' ' ) + ": " + data.firstFlight                         + "\n";
    text += QString( "%1" ).arg( labels[  3 ], l_max, ' ' ) + ": " + data.introduction                        + "\n";
    text += QString( "%1" ).arg( labels[  4 ], l_max, ' ' ) + ": " + data.retired                             + "\n";
    text += QString( "%1" ).arg( labels[  5 ], l_max, ' ' ) + ": " + locale().toString( data.numberBuilt )    + "\n";
    text += QString( "%1" ).arg( labels[  6 ], l_max, ' ' ) + ": " + data.primaryUser                         + "\n";
    text += QString( "%1" ).arg( labels[  7 ], l_max, ' ' ) + ": " + data.crew                                + "\n";
    text += QString( "%1" ).arg( labels[  8 ], l_max, ' ' ) + ": " + length                                   + "\n";
    text += QString( "%1" ).arg( labels[  9 ], l_max, ' ' ) + ": " + wingspan                                 + "\n";
    text += QString( "%1" ).arg( labels[ 10 ], l_max, ' ' ) + ": " + height                                   + "\n";
    text += QString( "%1" ).arg( labels[ 11 ], l_max, ' ' ) + ": " + mtow                                     + "\n";
    text += QString( "%1" ).arg( labels[ 12 ], l_max, ' ' ) + ": " + maxSpeed                                 + "\n";
    text += QString( "%1" ).arg( labels[ 13 ], l_max, ' ' ) + ": " + range                                    + "\n";
    text += QString( "%1" ).arg( labels[ 14 ], l_max, ' ' ) + ": " + QString( data.powerplant.get().c_str() ) + "\n";
    text += QString( "%1" ).arg( labels[ 15 ], l_max, ' ' ) + ": " + QString( data.armament.get().c_str() )   + "\n";

    _ui->textBrowserUnitData->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::updateDataMarine( const Units::Data::DataMarine &data )
{
    QString displacement = locale().toString( data.displacement , 'f', 0 ) + " t";
    QString length       = locale().toString( data.length       , 'f', 0 ) + " m";
    QString beam         = locale().toString( data.beam         , 'f', 2 ) + " m";
    QString draft        = locale().toString( data.draft        , 'f', 2 ) + " m";
    QString maxSpeed     = locale().toString( data.maxSpeed     , 'f', 0 ) + " kts";

    length += " (" + QString::number( floor( Convert::m2ft( data.length ) + 0.5 ) ) + " ft)";
    beam   += " (" + Utils::m2ftin( data.beam  ) + ")";
    draft  += " (" + Utils::m2ftin( data.draft ) + ")";

    QVector< QString > labels;

    labels.push_back( tr( "Type"         ) );
    labels.push_back( tr( "Armament"     ) );
    labels.push_back( tr( "Number Built" ) );
    labels.push_back( tr( "Complement"   ) );
    labels.push_back( tr( "Displacement" ) );
    labels.push_back( tr( "Length"       ) );
    labels.push_back( tr( "Beam"         ) );
    labels.push_back( tr( "Draft"        ) );
    labels.push_back( tr( "Max Speed"    ) );

    int l_max = 0;

    for ( auto l : labels )
    {
        l_max = std::max( l_max, l.length() );
    }

    QString text;

    text += QString( "%1" ).arg( labels[ 0 ], l_max, ' ' ) + ": " + QString( data.type.get().c_str() )     + "\n";
    text += QString( "%1" ).arg( labels[ 1 ], l_max, ' ' ) + ": " + QString( data.armament.get().c_str() ) + "\n";
    text += QString( "%1" ).arg( labels[ 2 ], l_max, ' ' ) + ": " + locale().toString( data.numberBuilt )  + "\n";
    text += QString( "%1" ).arg( labels[ 3 ], l_max, ' ' ) + ": " + locale().toString( data.complement )   + "\n";
    text += QString( "%1" ).arg( labels[ 4 ], l_max, ' ' ) + ": " + displacement                           + "\n";
    text += QString( "%1" ).arg( labels[ 5 ], l_max, ' ' ) + ": " + length                                 + "\n";
    text += QString( "%1" ).arg( labels[ 6 ], l_max, ' ' ) + ": " + beam                                   + "\n";
    text += QString( "%1" ).arg( labels[ 7 ], l_max, ' ' ) + ": " + draft                                  + "\n";
    text += QString( "%1" ).arg( labels[ 8 ], l_max, ' ' ) + ": " + maxSpeed                               + "\n";

    _ui->textBrowserUnitData->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::updateDataGround( const Units::Data::DataGround &data )
{
    QVector< QString > labels;

    labels.push_back( tr( "Type"         ) );
    labels.push_back( tr( "Armament"     ) );
    labels.push_back( tr( "Number Built" ) );

    int l_max = 0;

    for ( auto l : labels )
    {
        l_max = std::max( l_max, l.length() );
    }

    QString text;

    text += QString( "%1" ).arg( labels[ 0 ], l_max, ' ' ) + ": " + QString( data.type.get().c_str() )     + "\n";
    text += QString( "%1" ).arg( labels[ 1 ], l_max, ' ' ) + ": " + QString( data.armament.get().c_str() ) + "\n";
    text += QString( "%1" ).arg( labels[ 2 ], l_max, ' ' ) + ": " + locale().toString( data.numberBuilt )  + "\n";

    _ui->textBrowserUnitData->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::on_comboBoxUnits_currentIndexChanged(int index)
{
    _ui->widgetUnit->setUnit( index );

    Units::Data data = Units::instance()->getData( index );

    switch ( data.type )
    {
        case Units::Aerial: updateDataAerial( data.aerial ); break;
        case Units::Marine: updateDataMarine( data.marine ); break;
        case Units::Ground: updateDataGround( data.ground ); break;
    }
}
