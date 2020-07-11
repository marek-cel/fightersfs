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
#ifndef WIDGETCGI_H
#define WIDGETCGI_H

////////////////////////////////////////////////////////////////////////////////

#include <QDateTime>
#include <QGridLayout>
#include <QWidget>

#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>

#include <defs.h>

#include <gui/GraphicsWinQt.h>
#include <hid/hid_Assignment.h>

////////////////////////////////////////////////////////////////////////////////

/** This is widget wrapper for CGI. */
class WidgetCGI : public QWidget, public osgViewer::Viewer
{
    Q_OBJECT

public:

    static const double _zNear;
    static const double _zFar;

    /** Constructor. */
    WidgetCGI( QWidget *parent = NULLPTR );

    /** Destructor. */
    virtual ~WidgetCGI();

protected:

    osg::ref_ptr< GraphicsWinQt > _gwin;

    /** */
    void paintEvent( QPaintEvent *event );

    GraphicsWinQt* createGraphicsWindow( int x, int y, int w, int h,
                                         const std::string &name = "",
                                         bool windowDecoration = false );

    void removeAllChildren( osg::Camera *camera );
};

////////////////////////////////////////////////////////////////////////////////

#endif // WIDGETCGI_H
