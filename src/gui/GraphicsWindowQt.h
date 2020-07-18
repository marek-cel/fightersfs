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
#ifndef GRAPHICSWINDOWQT_H
#define GRAPHICSWINDOWQT_H

////////////////////////////////////////////////////////////////////////////////

#include <osgViewer/GraphicsWindow>

#include <QEvent>
#include <QGLWidget>
#include <QInputEvent>
#include <QMutex>
#include <QQueue>
#include <QSet>

#include <defs.h>

////////////////////////////////////////////////////////////////////////////////

class GraphicsWindowQt : public osgViewer::GraphicsWindow
{
    class GLWidget;

    friend class GLWidget;

public:

    struct WinData : public osg::Referenced
    {
        WinData( GLWidget *widget = NULLPTR, QWidget *parent = NULLPTR ) :
            _widget ( widget ),
            _parent ( parent )
        {}

        GLWidget *_widget;      ///<
        QWidget  *_parent;      ///<
    };

    /**
     * Constructor.
     * @param traits
     */
    GraphicsWindowQt( osg::GraphicsContext::Traits *traits );

    /** Destructor. */
    virtual ~GraphicsWindowQt();

    virtual bool setWindowRectangleImplementation( int x, int y, int w, int h );
    virtual void getWindowRectangle( int &x, int &y, int &w, int &h );
    virtual void grabFocus();
    virtual void grabFocusIfPointerInWindow();
    virtual void raiseWindow();
    virtual void useCursor( bool cursorOn );
    virtual void setCursor( MouseCursor cursor );

    virtual bool valid() const;
    virtual bool realizeImplementation();
    virtual bool isRealizedImplementation() const;
    virtual void closeImplementation();
    virtual bool makeCurrentImplementation();
    virtual bool releaseContextImplementation();
    virtual void swapBuffersImplementation();
    virtual void runOperations();

    virtual void requestWarpPointer( float x, float y );

    inline       GLWidget* getGLWidget()       { return _widget; }
    inline const GLWidget* getGLWidget() const { return _widget; }

private:

    class GLWidget : public QGLWidget
    {
        friend class GraphicsWindowQt;

    public:

        GLWidget( const QGLFormat &format,
                  QWidget *parent = NULLPTR, const QGLWidget *shareWidget = NULLPTR,
                  Qt::WindowFlags flags = 0 );

        virtual ~GLWidget();

        inline       GraphicsWindowQt* getGraphicsWindow()       { return _gwin; }
        inline const GraphicsWindowQt* getGraphicsWindow() const { return _gwin; }

        inline void setGraphicsWindow( GraphicsWindowQt *gwin ) { _gwin = gwin; }

        void setKeyboardModifiers( QInputEvent *event );

    protected:

        virtual bool event( QEvent *event );

        virtual void keyPressEvent   ( QKeyEvent *event );
        virtual void keyReleaseEvent ( QKeyEvent *event );

        virtual void mousePressEvent       ( QMouseEvent *event );
        virtual void mouseReleaseEvent     ( QMouseEvent *event );
        virtual void mouseDoubleClickEvent ( QMouseEvent *event );
        virtual void mouseMoveEvent        ( QMouseEvent *event );

        virtual void moveEvent( QMoveEvent *event );

        virtual void resizeEvent( QResizeEvent *event );

        virtual void wheelEvent( QWheelEvent *event );

        virtual void glDraw();

    private:

        GraphicsWindowQt *_gwin;                        ///<

        QMutex _deferredEventQueueMutex;                ///<

        QQueue < QEvent::Type > _deferredEventQueue;    ///<
        QSet   < QEvent::Type > _eventCompressor;       ///<

        int getNumDeferredEvents();

        void enqueueDeferredEvent( QEvent::Type eventType,
                                   QEvent::Type removeEventType = QEvent::None );

        void processDeferredEvents();
    };

    GLWidget *_widget;          ///<
    QCursor _currentCursor;     ///<

    bool _ownsWidget;           ///<
    bool _realized;             ///<
};

////////////////////////////////////////////////////////////////////////////////

#endif // GRAPHICSWINDOWQT_H
