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

    int result = Utils::read( nodeName, &data.name );

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

    if ( result == SIM_SUCCESS ) result = Utils::read( nodeRole       , &data.role       );
    if ( result == SIM_SUCCESS ) result = Utils::read( nodePowerplant , &data.powerplant );
    if ( result == SIM_SUCCESS ) result = Utils::read( nodeArmament   , &data.armament   );

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

    if ( result == SIM_SUCCESS ) result = Utils::read( nodeType     , &data.type     );
    if ( result == SIM_SUCCESS ) result = Utils::read( nodeArmament , &data.armament );

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

    if ( result == SIM_SUCCESS ) result = Utils::read( nodeType     , &data.type     );
    if ( result == SIM_SUCCESS ) result = Utils::read( nodeArmament , &data.armament );

    data.numberBuilt = nodeNumberBuilt.text().toInt();
}
