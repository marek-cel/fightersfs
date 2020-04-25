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
#ifndef WIDGETUNIT_H
#define WIDGETUNIT_H

////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>

#include <gui/WidgetCGI.h>

#include <sim/cgi/sim_Viewer.h>

////////////////////////////////////////////////////////////////////////////////

/** This is widget wrapper for database CGI. */
class WidgetUnit : public WidgetCGI
{
    Q_OBJECT

public:

    /** Constructor. */
    WidgetUnit( QWidget *parent = NULLPTR );

    /** Destructor. */
    virtual ~WidgetUnit();

    void update();

    void setUnit( int index );

private:

    QGridLayout *_layout;

    osg::ref_ptr<osg::Camera> _camera;

    sim::Viewer *_viewer;

    QWidget* addViewWidget();

    void createCamera();
};

////////////////////////////////////////////////////////////////////////////////

#endif // WIDGETUNIT_H
