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

#include <gui/Utils.h>

////////////////////////////////////////////////////////////////////////////////

WidgetData::WidgetData( QWidget *parent ) :
    QWidget ( parent ),
    _ui ( new Ui::WidgetData )
{
    _ui->setupUi( this );

    for ( unsigned int i = 0; i < Units::instance()->getCount(); i++ )
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

void WidgetData::step()
{
    _ui->widgetUnit->update();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::updateDataAerial( const Units::Data::DataAerial &data )
{
    _ui->stackedWidget->setCurrentIndex( 0 );

    QString length   = QString::number( data.length   , 'f', 2 ) + " m";
    QString wingspan = QString::number( data.wingspan , 'f', 2 ) + " m";
    QString height   = QString::number( data.height   , 'f', 2 ) + " m";
    QString mtow     = QString::number( data.MTOW     , 'f', 0 ) + " kg";
    QString maxSpeed = QString::number( data.maxSpeed , 'f', 0 ) + " km/h";
    QString range    = QString::number( data.range    , 'f', 0 ) + " km";

    double mtow_lbs  = Convert::kg2lb   ( data.MTOW     );
    double speed_kts = Convert::kmh2kts ( data.maxSpeed );
    double range_nmi = Convert::km2nmi  ( data.range    );

    length   += " (" + Utils::m2ftin( data.length   ) + ")";
    wingspan += " (" + Utils::m2ftin( data.wingspan ) + ")";
    height   += " (" + Utils::m2ftin( data.height   ) + ")";
    mtow     += " (" + QString::number( 10.0 * floor( mtow_lbs  / 10.0 ), 'f', 0 ) + " lbs)";
    maxSpeed += " (" + QString::number( 10.0 * floor( speed_kts / 10.0 ), 'f', 0 ) + " kts)";
    range    += " (" + QString::number( 10.0 * floor( range_nmi / 10.0 ), 'f', 0 ) + " nmi)";

    _ui->labelAerialValueRole         ->setText( data.role.get().c_str() );
    _ui->labelAerialValueManufacturer ->setText( data.manufacturer );
    _ui->labelAerialValueFirstFlight  ->setText( data.firstFlight );
    _ui->labelAerialValueIntroduction ->setText( data.introduction );
    _ui->labelAerialValueRetired      ->setText( data.retired );
    _ui->labelAerialValueNumberBuilt  ->setText( QString::number( data.numberBuilt ) );
    _ui->labelAerialValuePrimaryUser  ->setText( data.primaryUser );
    _ui->labelAerialValueCrew         ->setText( data.crew );
    _ui->labelAerialValueLength       ->setText( length );
    _ui->labelAerialValueWingspan     ->setText( wingspan );
    _ui->labelAerialValueHeight       ->setText( height );
    _ui->labelAerialValueMTOW         ->setText( mtow );
    _ui->labelAerialValueMaxSpeed     ->setText( maxSpeed );
    _ui->labelAerialValueRange        ->setText( range );
    _ui->labelAerialValuePowerplant   ->setText( data.powerplant.get().c_str() );
    _ui->labelAerialValueArmament     ->setText( data.armament.get().c_str() );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::updateDataMarine( const Units::Data::DataMarine &data )
{
    _ui->stackedWidget->setCurrentIndex( 1 );

    QString displacement = QString::number( data.displacement , 'f', 0 ) + " t";
    QString length       = QString::number( data.length       , 'f', 0 ) + " m";
    QString beam         = QString::number( data.beam         , 'f', 2 ) + " m";
    QString draft        = QString::number( data.draft        , 'f', 2 ) + " m";
    QString maxSpeed     = QString::number( data.maxSpeed     , 'f', 0 ) + " kts";

    length += " (" + QString::number( floor( Convert::m2ft( data.length ) + 0.5 ) ) + " ft)";
    beam   += " (" + Utils::m2ftin( data.beam  ) + ")";
    draft  += " (" + Utils::m2ftin( data.draft ) + ")";

    _ui->labelMarineValueType         ->setText( data.type.get().c_str() );
    _ui->labelMarineValueNumberBuilt  ->setText( QString::number( data.numberBuilt ) );
    _ui->labelMarineValueDisplacement ->setText( displacement );
    _ui->labelMarineValueLength       ->setText( length );
    _ui->labelMarineValueBeam         ->setText( beam );
    _ui->labelMarineValueDraft        ->setText( draft );
    _ui->labelMarineValueMaxSpeed     ->setText( maxSpeed );
    _ui->labelMarineValueComplement   ->setText( QString::number( data.complement ) );
    _ui->labelMarineValueArmament     ->setText( data.armament.get().c_str() );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::updateDataGround( const Units::Data::DataGround &data )
{
    _ui->stackedWidget->setCurrentIndex( 2 );

    _ui->labelGroundValueType         ->setText( data.type.get().c_str() );
    _ui->labelGroundValueNumberBuilt  ->setText( QString::number( data.numberBuilt ) );
    _ui->labelGroundValueArmament     ->setText( data.armament.get().c_str() );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::on_pushButtonBack_clicked()
{
    emit back();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetData::on_comboBoxUnits_currentIndexChanged( int index )
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
