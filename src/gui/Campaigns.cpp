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

#include <gui/Campaigns.h>

#include <QFile>

#include <defs.h>

#include <gui/Utils.h>

////////////////////////////////////////////////////////////////////////////////

Campaigns::Campaigns()
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
                CampaignData data;

                data.file   = campaignNode.attributeNode( "file"   ).value();
                data.hidden = campaignNode.attributeNode( "hidden" ).value().toInt();

                data.name = getCampaignName( data.file );

                _data.push_back( data );

                campaignNode = campaignNode.nextSiblingElement( "campaign" );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

Campaigns::~Campaigns() {}

////////////////////////////////////////////////////////////////////////////////

Campaigns::CampaignData Campaigns::getCampaign( int campaign ) const
{
    CampaignData data;

    data = _data.at( campaign );

    return data;
}

////////////////////////////////////////////////////////////////////////////////

int Campaigns::getCampaignsCount() const
{
    return _data.size();
}

////////////////////////////////////////////////////////////////////////////////

int Campaigns::getRealIndex( int visible_index )
{
    int hidden = 0;

    for ( int i = 0; i < _data.size(); i++ )
    {
        if ( _data[ i ].hidden ) hidden++;

        if ( i >= visible_index + hidden ) break;
    }

    return visible_index + hidden;
}

////////////////////////////////////////////////////////////////////////////////

sim::Text Campaigns::getCampaignName( const QString &fileName )
{
    sim::Text name;

    QFile file( Path::get( ( "campaigns/" + fileName.toStdString() ).c_str() ).c_str() );

    if ( file.open(QFile::ReadOnly | QFile::Text) )
    {
        QDomDocument doc;

        doc.setContent( &file, false );

        QDomElement rootNode = doc.documentElement();

        if ( rootNode.tagName() == "campaign" )
        {
            QDomElement nameNode = rootNode.firstChildElement( "name" );

            Utils::read( nameNode, &name );
        }
    }

    return name;
}
