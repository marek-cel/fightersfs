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

#include <gui/WidgetUnit.h>

#include <osgViewer/ViewerEventHandlers>

////////////////////////////////////////////////////////////////////////////////

WidgetUnit::WidgetUnit( QWidget *parent ) :
    WidgetCGI ( parent ),

    _layout ( NULLPTR ),
    _viewer ( NULLPTR )
{
    _viewer = new sim::Viewer( _gwin->getTraits()->width, _gwin->getTraits()->height );

    QWidget *widget = addViewWidget();

    _layout = new QGridLayout( this );
    _layout->setContentsMargins( 1, 1, 1, 1 );
    _layout->addWidget( widget, 0, 0 );

    setLayout( _layout );

    setCameraManipulator( _viewer->getCameraManipulator() );
    _viewer->resetViewer();
}

////////////////////////////////////////////////////////////////////////////////

WidgetUnit::~WidgetUnit() {}

////////////////////////////////////////////////////////////////////////////////

void WidgetUnit::update()
{
    _viewer->update( SIM_TIME_STEP );

    //////////////////
    QWidget::update();
    //////////////////
}

////////////////////////////////////////////////////////////////////////////////

void WidgetUnit::setUnit( int index )
{
    _viewer->setUnit( index );
    _viewer->resetViewer();
}

////////////////////////////////////////////////////////////////////////////////

QWidget* WidgetUnit::addViewWidget()
{
    createCamera();

    setSceneData( _viewer->getNode() );

    addEventHandler( new osgViewer::StatsHandler );
    //setKeyEventSetsDone( 0 );

    return _gwin->getGLWidget();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetUnit::createCamera()
{
    _camera = getCamera();

    _camera->setGraphicsContext( _gwin );

    const osg::GraphicsContext::Traits *traits = _gwin->getTraits();

    float w = traits->width  / 1.0f;
    float h = traits->height / 1.0f;

    float w2h = w / h;

    //_camera->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    _camera->setClearColor( osg::Vec4( 0.47, 0.71, 1.0, 1.0 ) );
    //_camera->setClearColor( osg::Vec4( 0.223529, 0.223529, 0.223529, 1.0 ) );
    _camera->setViewport( new osg::Viewport( 0, 0, w, h ) );
    _camera->setProjectionMatrixAsPerspective( 30.0f, w2h, _zNear, _zFar );
    _camera->setNearFarRatio( _zNear / _zFar );
}
