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
#ifndef WIDGETCAMPAIGN_H
#define WIDGETCAMPAIGN_H

////////////////////////////////////////////////////////////////////////////////

#include <QWidget>

#include <defs.h>

////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class WidgetCampaign;
}

////////////////////////////////////////////////////////////////////////////////

class WidgetCampaign : public QWidget
{
    Q_OBJECT

public:

    explicit WidgetCampaign( QWidget *parent = NULLPTR );

    ~WidgetCampaign();

    void updateCampaign();

signals:

    void startClicked( int campaign );

protected:

    void resizeEvent( QResizeEvent *event );

private:

    Ui::WidgetCampaign *_ui;

    int _campaign;

    void initCampaigns();

    void updateCampaignImage();
    void updateCampaignText();

private slots:

    void on_comboBoxCampaigns_currentIndexChanged( int index );

    void on_pushButtonStartCampaign_clicked();
};

////////////////////////////////////////////////////////////////////////////////

#endif // WIDGETCAMPAIGN_H
