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
#ifndef WIDGETPLAY_H
#define WIDGETPLAY_H

////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>

#include <gui/KeyHandler.h>
#include <gui/WidgetCGI.h>

////////////////////////////////////////////////////////////////////////////////

/** This is widget wrapper for simulation CGI. */
class WidgetPlay : public WidgetCGI
{
    Q_OBJECT

public:

    /** Constructor. */
    WidgetPlay( QWidget *parent = NULLPTR );

    /** Destructor. */
    virtual ~WidgetPlay();

    /** */
    void init();

    /** */
    void stop();

    /** */
    void update();

protected:

    /** */
    void resizeEvent( QResizeEvent *event );

private:

    QGridLayout *_layout;

    osg::ref_ptr<KeyHandler> _keyHandler;

    osg::ref_ptr<osg::Camera> _cameraOTW;
    osg::ref_ptr<osg::Camera> _cameraHUD;

    QWidget* addViewWidget();

    void createCameraOTW();
    void createCameraHUD();
};

////////////////////////////////////////////////////////////////////////////////

#endif // WIDGETPLAY_H
