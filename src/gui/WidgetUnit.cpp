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

#include <osgGA/TrackballManipulator>

////////////////////////////////////////////////////////////////////////////////

WidgetUnit::WidgetUnit( QWidget *parent ) :
    WidgetCGI ( parent ),

    _gridLayout ( 0 )
{
    QWidget *widget = addViewWidget();

    _gridLayout = new QGridLayout( this );
    _gridLayout->setContentsMargins( 1, 1, 1, 1 );
    _gridLayout->addWidget( widget, 0, 0 );

    setLayout( _gridLayout );
}

////////////////////////////////////////////////////////////////////////////////

WidgetUnit::~WidgetUnit() {}

////////////////////////////////////////////////////////////////////////////////

QWidget* WidgetUnit::addViewWidget()
{
    createCamera();

    setKeyEventSetsDone( 0 );

    return _graphicsWin->getGLWidget();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetUnit::createCamera()
{
    _camera = getCamera();

    _camera->setGraphicsContext( _graphicsWin );

    const osg::GraphicsContext::Traits *traits = _graphicsWin->getTraits();

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
