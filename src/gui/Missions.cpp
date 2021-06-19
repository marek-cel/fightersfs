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

#include <gui/Missions.h>

#include <QFile>

#include <gui/Utils.h>

////////////////////////////////////////////////////////////////////////////////

Missions::Missions()
{
    QFile file( Path::get( "campaigns.xml" ).c_str() );

    if ( file.open(QFile::ReadOnly | QFile::Text) )
    {
        QDomDocument doc;

        doc.setContent( &file, false );

        QDomElement rootNode = doc.documentElement();

        if ( rootNode.tagName() == "campaigns" )
        {
            QDomElement campaignNode = rootNode.firstChildElement( "campaign" );

            while ( !campaignNode.isNull() )
            {
                QString campaignFile = campaignNode.attributeNode( "file" ).value();

                readCampaign( campaignFile );

                campaignNode = campaignNode.nextSiblingElement( "campaign" );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

Missions::~Missions() {}

////////////////////////////////////////////////////////////////////////////////

Missions::MissionData Missions::getMission( int campaign, int mission ) const
{
    MissionData data;

    if ( campaign < _data.size() )
    {
        data = _data[ campaign ].at( mission );
    }

    return data;
}

////////////////////////////////////////////////////////////////////////////////

int Missions::getMissionsCount( int campaign ) const
{
    if ( campaign < _data.size() )
    {
        return _data[ campaign ].size();
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void Missions::readCampaign( QString campaignFile )
{
    QVector< MissionData > campaignData;

    QFile file( Path::get( "campaigns/" + campaignFile.toStdString() ).c_str() );

    if ( file.open(QFile::ReadOnly | QFile::Text) )
    {
        QDomDocument doc;

        doc.setContent( &file, false );

        QDomElement rootNode = doc.documentElement();

        if ( rootNode.tagName() == "campaign" )
        {
            QDomElement missionNode = rootNode.firstChildElement( "mission" );

            while ( !missionNode.isNull() )
            {
                MissionData missionData;

                QString missionFile = missionNode.attributeNode( "file" ).value();

                if ( readMissionData( missionFile, &missionData ) )
                {
                    campaignData.push_back( missionData );
                }

                missionNode = missionNode.nextSiblingElement( "mission" );
            }
        }
    }

    _data.push_back( campaignData );
}

////////////////////////////////////////////////////////////////////////////////

bool Missions::readMissionData( QString missionFile, MissionData *missionData )
{
    QFile file( Path::get( "missions/" + missionFile.toStdString() ).c_str() );

    if ( file.open(QFile::ReadOnly | QFile::Text) )
    {
        QDomDocument doc;

        doc.setContent( &file, false );

        QDomElement rootNode = doc.documentElement();

        if ( rootNode.tagName() == "mission" )
        {
            QDomElement dataNode = rootNode.firstChildElement( "data" );

            if ( !dataNode.isNull() )
            {
                QDomElement nameNode           = dataNode.firstChildElement( "name"            );
                QDomElement introductionNode   = dataNode.firstChildElement( "introduction"    );
                QDomElement summaryFailureNode = dataNode.firstChildElement( "summary_failure" );
                QDomElement summarySuccessNode = dataNode.firstChildElement( "summary_success" );
                QDomElement imageNode          = dataNode.firstChildElement( "image"           );

                Utils::read( nameNode           , &missionData->name           );
                Utils::read( introductionNode   , &missionData->introduction   );
                Utils::read( summaryFailureNode , &missionData->summaryFailure );
                Utils::read( summarySuccessNode , &missionData->summarySuccess );

                missionData->fileImage = imageNode.text();

                return true;
            }
        }
    }

    return false;
}
