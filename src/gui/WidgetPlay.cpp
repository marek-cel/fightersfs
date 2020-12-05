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

#include <gui/WidgetPlay.h>

#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <hid/hid_Manager.h>
#include <sim/sim_Manager.h>

////////////////////////////////////////////////////////////////////////////////

void keyPressCallback()
{
    sim::Manager::instance()->unpause();
}

////////////////////////////////////////////////////////////////////////////////

WidgetPlay::WidgetPlay( QWidget *parent ) :
    WidgetCGI( parent ),

    _layout ( NULLPTR )
{
    QWidget *widget = addViewWidget();

    _layout = new QGridLayout( this );
    _layout->setContentsMargins( 1, 1, 1, 1 );
    _layout->addWidget( widget, 0, 0 );

    _keyHandler = new KeyHandler( this );
    _keyHandler->registerKeyPressCallback( &keyPressCallback );
    getEventHandlers().push_front( _keyHandler.get() );

    setLayout( _layout );
}

////////////////////////////////////////////////////////////////////////////////

WidgetPlay::~WidgetPlay() {}

////////////////////////////////////////////////////////////////////////////////

void WidgetPlay::init()
{
    removeAllChildren( _cameraHUD.get() );
    removeAllChildren( _cameraOTW.get() );

    osg::ref_ptr<osg::Node> nodeHUD = sim::Manager::instance()->getNodeHUD();
    osg::ref_ptr<osg::Node> nodeOTW = sim::Manager::instance()->getNodeOTW();

    if ( nodeHUD.valid() )
    {
        _cameraHUD->addChild( nodeHUD.get() );
    }

    if ( nodeOTW.valid() )
    {
        setSceneData( nodeOTW.get() );
    }

    setCameraManipulator( sim::Manager::instance()->getCameraManipulator() );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetPlay::stop()
{
    removeAllChildren( _cameraHUD.get() );
    removeAllChildren( _cameraOTW.get() );

    setCameraManipulator( 0 );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetPlay::update()
{
    hid::Manager::instance()->setKeysState( _keyHandler->getKeysState() );

    //////////////////
    QWidget::update();
    //////////////////
}

////////////////////////////////////////////////////////////////////////////////

void WidgetPlay::mousePressEvent( QMouseEvent *event )
{
    //////////////////////////////////
    QWidget::mousePressEvent( event );
    //////////////////////////////////

    sim::Manager::instance()->unpause();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetPlay::resizeEvent( QResizeEvent *event )
{
    //////////////////////////////
    QWidget::resizeEvent( event );
    //////////////////////////////

    sim::Manager::instance()->reload();
}

////////////////////////////////////////////////////////////////////////////////

QWidget* WidgetPlay::addViewWidget()
{
    createCameraOTW();
    createCameraHUD();

    addEventHandler( new osgViewer::StatsHandler );
    setKeyEventSetsDone( 0 );

    return _gwin->getGLWidget();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetPlay::createCameraOTW()
{
    _cameraOTW = getCamera();

    _cameraOTW->setGraphicsContext( _gwin );

    const osg::GraphicsContext::Traits *traits = _gwin->getTraits();

    float w = traits->width  / 1.0f;
    float h = traits->height / 1.0f;

    float w2h = w / h;

    _cameraOTW->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //_cameraOTW->setClearColor( osg::Vec4( 0.47, 0.71, 1.0, 1.0 ) );
    //_cameraOTW->setClearColor( osg::Vec4( 0.223529, 0.223529, 0.223529, 1.0 ) );
    _cameraOTW->setViewport( new osg::Viewport( 0, 0, w, h ) );
    _cameraOTW->setProjectionMatrixAsPerspective( 30.0f, w2h, _zNear, _zFar );
    _cameraOTW->setNearFarRatio( _zNear / _zFar );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetPlay::createCameraHUD()
{
    _cameraHUD = new osg::Camera();

    _cameraHUD->setGraphicsContext( _gwin );

    const osg::GraphicsContext::Traits *traits = _gwin->getTraits();

    double size = 100.0;
    double w2h = (double)(traits->width) / (double)(traits->height);

    _cameraHUD->setProjectionMatrixAsOrtho2D( -size * w2h, size * w2h, -size, size );
    _cameraHUD->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    _cameraHUD->setViewMatrix( osg::Matrix::identity() );
    //_cameraHUD->setClearMask( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
    _cameraHUD->setClearMask( GL_DEPTH_BUFFER_BIT );
    _cameraHUD->setRenderOrder( osg::Camera::POST_RENDER );
    _cameraHUD->setAllowEventFocus( false );
    _cameraHUD->setProjectionResizePolicy( osg::Camera::HORIZONTAL );
    _cameraHUD->setViewport( new osg::Viewport( 0, 0, traits->width, traits->height ) );

    addSlave( _cameraHUD, false );
}
