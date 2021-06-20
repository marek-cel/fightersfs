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
#ifndef CAMPAIGNS_H
#define CAMPAIGNS_H

////////////////////////////////////////////////////////////////////////////////

#include <QDomElement>
#include <QVector>

#include <sim/utils/sim_Singleton.h>
#include <sim/utils/sim_Text.h>

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Campaign class.
 */
class Campaigns : public sim::Singleton< Campaigns >
{
    friend class Singleton< Campaigns >;

public:

    struct CampaignData
    {
        QString file;
        bool hidden;

        sim::Text name;
        sim::Text synopsis;

        QString fileImage;      ///<
    };

    /** @brief Destructor. */
    virtual ~Campaigns();

    CampaignData getCampaign( int campaign ) const;

    int getCampaignsCount() const;

    int getRealIndex( int visible_index );

private:

    QVector< CampaignData > _data;

    /**
     * You should use static function instance() due to get refernce
     * to Airports class instance.
     */
    Campaigns();

    /** Using this constructor is forbidden. */
    Campaigns( const Campaigns & ) : sim::Singleton< Campaigns > () {}

    void readCampaign( const QString &fileName, CampaignData *data );
};

////////////////////////////////////////////////////////////////////////////////

#endif // CAMPAIGNS_H
