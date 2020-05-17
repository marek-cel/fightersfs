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

#include <gui/Units.h>

#include <QFile>

#include <gui/Utils.h>

////////////////////////////////////////////////////////////////////////////////

Units::Data Units::getData( int index ) const
{
    Data result;

    if ( index < _units.size() )
    {
        result = _units[ index ];
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

Units::Units()
{
    readFile( Path::get( "units.xml" ).c_str() );
}

////////////////////////////////////////////////////////////////////////////////

void Units::readFile( const QString &path )
{
    QFile file( path );

    if ( file.open( QFile::ReadOnly | QFile::Text ) )
    {
        QDomDocument doc;

        doc.setContent( &file, false );

        QDomElement rootNode = doc.documentElement();

        if ( !rootNode.isNull() )
        {
            readUnits( rootNode );
        }
        else
        {
            Log::e() << "Reading file \"" << path.toStdString() << "\" failed. Invalid root node." << std::endl;
        }
    }
    else
    {
        Log::e() << "Reading file \"" << path.toStdString() << "\" failed." << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Units::readUnits( QDomElement &rootNode )
{
    QDomElement nodeAerial = rootNode.firstChildElement( "aerial" );
    QDomElement nodeMarine = rootNode.firstChildElement( "marine" );
    QDomElement nodeGround = rootNode.firstChildElement( "ground" );

    if ( !nodeAerial.isNull() ) readUnits( nodeAerial, Aerial );
    if ( !nodeMarine.isNull() ) readUnits( nodeMarine, Marine );
    if ( !nodeGround.isNull() ) readUnits( nodeGround, Ground );
}

////////////////////////////////////////////////////////////////////////////////

void Units::readUnits( QDomElement &nodeGroup, Type type )
{
    QDomElement nodeUnit = nodeGroup.firstChildElement( "unit" );

    while ( !nodeUnit.isNull() )
    {
        readUnit( nodeUnit, type );
        nodeUnit = nodeUnit.nextSiblingElement( "unit" );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Units::readUnit( QDomElement &nodeUnit, Type type )
{
    Data data;

    data.index = _units.size();

    data.type = type;

    QDomElement nodeName = nodeUnit.firstChildElement( "name" );

    int result = Utils::read( nodeName, data.name );

    if ( result == SIM_SUCCESS )
    {
        QDomElement nodeInfo = nodeUnit.firstChildElement( "info" );

        if ( !nodeInfo.isNull() )
        {
            if ( data.type == Aerial ) readUnit( nodeInfo, data.aerial );
            if ( data.type == Marine ) readUnit( nodeInfo, data.marine );
            if ( data.type == Ground ) readUnit( nodeInfo, data.ground );

            /////////////////////////
            _units.push_back( data );
            /////////////////////////
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Units::readUnit( QDomElement &nodeInfo, Data::DataAerial &data )
{
    int result = SIM_SUCCESS;

    QDomElement nodeRole         = nodeInfo.firstChildElement( "role" );
    QDomElement nodePowerplant   = nodeInfo.firstChildElement( "powerplant" );
    QDomElement nodeArmament     = nodeInfo.firstChildElement( "armament" );

    QDomElement nodeManufacturer = nodeInfo.firstChildElement( "manufacturer" );
    QDomElement nodeFirstFlight  = nodeInfo.firstChildElement( "first_flight" );
    QDomElement nodeIntroduction = nodeInfo.firstChildElement( "introduction" );
    QDomElement nodeRetired      = nodeInfo.firstChildElement( "retired" );
    QDomElement nodePrimaryUser  = nodeInfo.firstChildElement( "primary_user" );
    QDomElement nodeCrew         = nodeInfo.firstChildElement( "crew" );

    QDomElement nodeNumberBuilt  = nodeInfo.firstChildElement( "number_built" );

    QDomElement nodeLength       = nodeInfo.firstChildElement( "length" );
    QDomElement nodeWingspan     = nodeInfo.firstChildElement( "wingspan" );
    QDomElement nodeHeight       = nodeInfo.firstChildElement( "height" );
    QDomElement nodeMTOW         = nodeInfo.firstChildElement( "mtow" );
    QDomElement nodeMaxSpeed     = nodeInfo.firstChildElement( "max_speed" );
    QDomElement nodeRange        = nodeInfo.firstChildElement( "range" );

    if ( result == SIM_SUCCESS ) result = Utils::read( nodeRole       , data.role       );
    if ( result == SIM_SUCCESS ) result = Utils::read( nodePowerplant , data.powerplant );
    if ( result == SIM_SUCCESS ) result = Utils::read( nodeArmament   , data.armament   );

    data.manufacturer = nodeManufacturer .text();
    data.firstFlight  = nodeFirstFlight  .text();
    data.introduction = nodeIntroduction .text();
    data.retired      = nodeRetired      .text();
    data.primaryUser  = nodePrimaryUser  .text();
    data.crew         = nodeCrew         .text();

    data.numberBuilt = nodeNumberBuilt.text().toInt();

    data.length   = nodeLength   .text().toDouble();
    data.wingspan = nodeWingspan .text().toDouble();
    data.height   = nodeHeight   .text().toDouble();
    data.MTOW     = nodeMTOW     .text().toDouble();
    data.maxSpeed = nodeMaxSpeed .text().toDouble();
    data.range    = nodeRange    .text().toDouble();
}

////////////////////////////////////////////////////////////////////////////////

void Units::readUnit( QDomElement &nodeInfo, Data::DataMarine &data )
{
    int result = SIM_SUCCESS;

    QDomElement nodeType         = nodeInfo.firstChildElement( "type" );
    QDomElement nodeArmament     = nodeInfo.firstChildElement( "armament" );

    QDomElement nodeNumberBuilt  = nodeInfo.firstChildElement( "number_built" );
    QDomElement nodeComplement   = nodeInfo.firstChildElement( "complement" );

    QDomElement nodeDisplacement = nodeInfo.firstChildElement( "displacement" );
    QDomElement nodeLength       = nodeInfo.firstChildElement( "length" );
    QDomElement nodeBeam         = nodeInfo.firstChildElement( "beam" );
    QDomElement nodeDraft        = nodeInfo.firstChildElement( "draft" );
    QDomElement nodeMaxSpeed     = nodeInfo.firstChildElement( "max_speed" );

    if ( result == SIM_SUCCESS ) result = Utils::read( nodeType     , data.type     );
    if ( result == SIM_SUCCESS ) result = Utils::read( nodeArmament , data.armament );

    data.numberBuilt = nodeNumberBuilt .text().toInt();
    data.complement  = nodeComplement  .text().toInt();

    data.displacement = nodeDisplacement .text().toDouble();
    data.length       = nodeLength       .text().toDouble();
    data.beam         = nodeBeam         .text().toDouble();
    data.draft        = nodeDraft        .text().toDouble();
    data.maxSpeed     = nodeMaxSpeed     .text().toDouble();
}

////////////////////////////////////////////////////////////////////////////////

void Units::readUnit( QDomElement &nodeInfo, Data::DataGround &data )
{
    int result = SIM_SUCCESS;

    QDomElement nodeType         = nodeInfo.firstChildElement( "type" );
    QDomElement nodeArmament     = nodeInfo.firstChildElement( "armament" );

    QDomElement nodeNumberBuilt  = nodeInfo.firstChildElement( "number_built" );

    if ( result == SIM_SUCCESS ) result = Utils::read( nodeType     , data.type     );
    if ( result == SIM_SUCCESS ) result = Utils::read( nodeArmament , data.armament );

    data.numberBuilt = nodeNumberBuilt.text().toInt();
}
