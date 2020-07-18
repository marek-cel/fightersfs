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

const double WidgetCGI::_zNear = 0.55;
const double WidgetCGI::_zFar  = SIM_SKYDOME_RAD + 0.1f * SIM_SKYDOME_RAD;

////////////////////////////////////////////////////////////////////////////////

WidgetCGI::WidgetCGI( QWidget *parent ) :
    QWidget( parent )
{
    setThreadingModel( osgViewer::ViewerBase::SingleThreaded );
    //setThreadingModel( osgViewer::ViewerBase::ThreadPerContext );

    _gwin = createGraphicsWindow( x(), y(), width(), height() );
}

////////////////////////////////////////////////////////////////////////////////

WidgetCGI::~WidgetCGI() {}

////////////////////////////////////////////////////////////////////////////////

void WidgetCGI::paintEvent( QPaintEvent *event )
{
    /////////////////////////////
    QWidget::paintEvent( event );
    /////////////////////////////

    frame();
}

////////////////////////////////////////////////////////////////////////////////

GraphicsWindowQt* WidgetCGI::createGraphicsWindow( int x, int y, int w, int h,
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

    return new GraphicsWindowQt( traits.get() );
}

////////////////////////////////////////////////////////////////////////////////

void WidgetCGI::removeAllChildren( osg::Camera *camera )
{
    if ( camera->getNumChildren() > 0 )
    {
        camera->removeChildren( 0, camera->getNumChildren() );
    }
}
