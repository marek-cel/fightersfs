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
#ifndef WIDGETUNIT_H
#define WIDGETUNIT_H

////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>

#include <gui/WidgetCGI.h>

#include <sim/cgi/sim_Viewer.h>

////////////////////////////////////////////////////////////////////////////////

/** @brief Widget wrapper for database CGI. */
class WidgetUnit : public WidgetCGI
{
    Q_OBJECT

public:

    /** @brief Constructor. */
    WidgetUnit( QWidget *parent = NULLPTR );

    /** @brief Destructor. */
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
