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
#include <osgQt/GraphicsWindowQt>

#include <hid/hid_Assignment.h>

////////////////////////////////////////////////////////////////////////////////

/** This is widget wrapper for CGI. */
class WidgetCGI : public QWidget, public osgViewer::Viewer
{
    Q_OBJECT

public:

    static const double m_zNear;
    static const double m_zFar;

    /** */
    class KeyHandler : public osgGA::GUIEventHandler
    {
    public:

        /** */
        KeyHandler( WidgetCGI *widgetCGI );

        /** */
        inline const bool* getKeysState() const { return m_keysState; }

        /** */
        bool handle( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter & );

    private:

        WidgetCGI *m_widgetCGI;

        bool m_keysState[ HID_MAX_KEYS ];

        bool handleKeyDn( const osgGA::GUIEventAdapter &ea );
        bool handleKeyUp( const osgGA::GUIEventAdapter &ea );
    };

    /** Constructor. */
    WidgetCGI( QWidget *parent = 0 );

    /** Destructor. */
    virtual ~WidgetCGI();

    /** */
    void init();

    /** */
    void stop();

    /** */
    void update();

protected:

    /** */
    void paintEvent( QPaintEvent *event );

    /** */
    void resizeEvent( QResizeEvent *event );

private:

    QGridLayout *_gridLayout;

    osg::ref_ptr<osgQt::GraphicsWindowQt> _graphicsWin;
    osg::ref_ptr<KeyHandler> _keyHandler;

    osg::ref_ptr<osg::Camera> _cameraOTW;
    osg::ref_ptr<osg::Camera> _cameraHUD;

    QWidget* addViewWidget();

    void createCameraOTW();
    void createCameraHUD();

    osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h,
                                                   const std::string &name = "",
                                                   bool windowDecoration = false );

    void removeAllChildren( osg::Camera *camera );
};

////////////////////////////////////////////////////////////////////////////////

#endif // WIDGETCGI_H
