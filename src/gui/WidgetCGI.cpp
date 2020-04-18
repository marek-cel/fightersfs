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

#include <gui/WidgetCGI.h>

#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <hid/hid_Manager.h>
#include <sim/sim_Manager.h>

////////////////////////////////////////////////////////////////////////////////

const double WidgetCGI::m_zNear = 0.55;
const double WidgetCGI::m_zFar  = SIM_SKYDOME_RAD + 0.1f * SIM_SKYDOME_RAD;

////////////////////////////////////////////////////////////////////////////////

WidgetCGI::WidgetCGI( QWidget *parent ) :
    QWidget ( parent ),

    _gridLayout ( 0 )
{
#   ifdef SIM_OSGDEBUGINFO
    osg::setNotifyLevel( osg::DEBUG_INFO );
#   else
    osg::setNotifyLevel( osg::WARN );
#   endif

    //setThreadingModel( osgViewer::ViewerBase::SingleThreaded );
    setThreadingModel( osgViewer::ViewerBase::ThreadPerContext );

    _graphicsWin = createGraphicsWindow( x(), y(), width(), height() );

    QWidget *widget = addViewWidget();

    _gridLayout = new QGridLayout( this );
    _gridLayout->setContentsMargins( 1, 1, 1, 1 );
    _gridLayout->addWidget( widget, 0, 0 );

    _keyHandler = new KeyHandler( this );
    getEventHandlers().push_front( _keyHandler.get() );

    setLayout( _gridLayout );
}

////////////////////////////////////////////////////////////////////////////////

WidgetCGI::~WidgetCGI() {}

////////////////////////////////////////////////////////////////////////////////

void WidgetCGI::init()
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

void WidgetCGI::stop()
{
    removeAllChildren( _cameraHUD.get() );
    removeAllChildren( _cameraOTW.get() );

    setCameraManipulator( 0 );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCGI::update()
{
    hid::Manager::instance()->setKeysState( _keyHandler->getKeysState() );

    //////////////////
    QWidget::update();
    //////////////////
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCGI::paintEvent( QPaintEvent *event )
{
    /////////////////////////////
    QWidget::paintEvent( event );
    /////////////////////////////

    frame();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCGI::resizeEvent( QResizeEvent *event )
{
    //////////////////////////////
    QWidget::resizeEvent( event );
    //////////////////////////////

    sim::Manager::instance()->reload();
}

////////////////////////////////////////////////////////////////////////////////

QWidget* WidgetCGI::addViewWidget()
{
    createCameraOTW();
    createCameraHUD();

    addEventHandler( new osgViewer::StatsHandler );
    setKeyEventSetsDone( 0 );

    return _graphicsWin->getGLWidget();
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCGI::createCameraOTW()
{
    _cameraOTW = getCamera();

    _cameraOTW->setGraphicsContext( _graphicsWin );

    const osg::GraphicsContext::Traits *traits = _graphicsWin->getTraits();

    float w = traits->width  / 1.0f;
    float h = traits->height / 1.0f;

    _cameraOTW->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //_cameraOTW->setClearColor( osg::Vec4( 0.47, 0.71, 1.0, 1.0 ) );
    //_cameraOTW->setClearColor( osg::Vec4( 0.223529, 0.223529, 0.223529, 1.0 ) );
    _cameraOTW->setViewport( new osg::Viewport( 0, 0, w, h ) );
    _cameraOTW->setProjectionMatrixAsPerspective( 30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), m_zNear, m_zFar );
    _cameraOTW->setNearFarRatio( m_zNear / m_zFar );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCGI::createCameraHUD()
{
    _cameraHUD = new osg::Camera();

    _cameraHUD->setGraphicsContext( _graphicsWin );

    const osg::GraphicsContext::Traits *traits = _graphicsWin->getTraits();

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

////////////////////////////////////////////////////////////////////////////////

osgQt::GraphicsWindowQt* WidgetCGI::createGraphicsWindow( int x, int y, int w, int h,
                                                          const std::string &name,
                                                          bool windowDecoration )
{
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();

    traits->windowName       = name;
    traits->windowDecoration = windowDecoration;
    traits->x                = x;
    traits->y                = y;
    traits->width            = w;
    traits->height           = h;
    traits->doubleBuffer     = true;
    traits->alpha            = osg::DisplaySettings::instance().get()->getMinimumNumAlphaBits();
    traits->stencil          = osg::DisplaySettings::instance().get()->getMinimumNumStencilBits();
    traits->sampleBuffers    = osg::DisplaySettings::instance().get()->getMultiSamples();
    traits->samples          = 4;
    traits->vsync            = true;

    return new osgQt::GraphicsWindowQt( traits.get() );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCGI::removeAllChildren( osg::Camera *camera )
{
    if ( camera->getNumChildren() > 0 )
    {
        camera->removeChildren( 0, camera->getNumChildren() );
    }
}
