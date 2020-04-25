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
