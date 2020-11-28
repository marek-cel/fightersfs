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

#include <gui/MainWindow.h>
#include <ui_MainWindow.h>

#include <iostream>

#include <QCloseEvent>
#include <QSettings>

#include <gui/KeyMap.h>
#include <gui/MessageBox.h>
#include <gui/ScreenSaver.h>

#include <hid/hid_Manager.h>
#include <sim/sim_Manager.h>

////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent, Qt::FramelessWindowHint ),
    _ui ( new Ui::MainWindow ),

    _dialogConf ( NULLPTR ),

    _shortcutPause ( NULLPTR ),
    _shortcutAbort ( NULLPTR ),

#   ifdef SIM_TEST
    _shortcutAuto ( NULLPTR ),

    _shortcutTimeFaster ( NULLPTR ),
    _shortcutTimeSlower ( NULLPTR ),
    _shortcutTimeNormal ( NULLPTR ),
#   endif

    _timer ( NULLPTR ),

    _timerId ( 0 ),

    _timeCoef ( 0.0 ),
    _heading  ( 0.0 ),

    _autopilot ( false ),
    _inited    ( false ),
    _throttle  ( false ),
    _pending   ( true  )
{
    _ui->setupUi( this );

    _dialogConf = new DialogConf( this );

    _shortcutPause = new QShortcut( QKeySequence(Qt::Key_Pause)  , this, SLOT(shortcutPause_activated()) );
    _shortcutAbort = new QShortcut( QKeySequence(Qt::Key_Escape) , this, SLOT(shortcutAbort_activated()) );

#   ifdef SIM_TEST
    _shortcutAuto = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_A), this, SLOT(shortcutAuto_activated()) );

    _shortcutTimeFaster = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_Equal) , this, SLOT(shortcutTimeFaster_activated()) );
    _shortcutTimeSlower = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_Minus) , this, SLOT(shortcutTimeSlower_activated()) );
    _shortcutTimeNormal = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_0)     , this, SLOT(shortcutTimeNormal_activated()) );
#   endif

    _timer = new QElapsedTimer();

    settingsRead();
}

////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    DELPTR( _timer );

    if ( _timerId ) killTimer( _timerId );

    settingsSave();

    DELPTR( _dialogConf );

    DELPTR( _shortcutPause );
    DELPTR( _shortcutAbort );

#   ifdef SIM_TEST
    DELPTR( _shortcutAuto );

    DELPTR( _shortcutTimeFaster );
    DELPTR( _shortcutTimeSlower );
    DELPTR( _shortcutTimeNormal );
#   endif

    DELPTR( _ui );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::init()
{
    _ui->stackedMain->setCurrentIndex( PageHome );

    hid::Manager::instance()->init();

    _dialogConf->readData();
    _dialogConf->updateAssignments();

    _timer->start();
    _timerId = startTimer( 1000.0 * SIM_TIME_STEP );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent( QCloseEvent *event )
{
    QString title = windowTitle();
    QString text = tr( "Do you want to quit?" );

    MessageBox::StandardButton result = MessageBox::question( this, title, text );

    if ( result == MessageBox::Yes )
    {
        /////////////////////////////////
        QMainWindow::closeEvent( event );
        /////////////////////////////////
    }
    else
    {
        event->ignore();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::timerEvent( QTimerEvent *event )
{
    /////////////////////////////////
    QMainWindow::timerEvent( event );
    /////////////////////////////////

    double timeStep = (double)_timer->restart() / 1000.0;

    if ( _ui->widgetPlay->isVisible() && _inited )
    {
        _ui->widgetPlay->update();
    }

    if ( _ui->widgetData->isVisible() )
    {
        _ui->widgetData->step();
    }

    if ( _pending && !sim::Data::get()->ownship.destroyed )
    {
        _heading = sim::Data::get()->ownship.heading;
    }
    else
    {
        if ( sim::Success == sim::Data::get()->mission.status
          || sim::Data::get()->ownship.destroyed )
        {
            if ( !_autopilot ) toggleAutopilot();

            if ( sim::Manager::instance()->isReady()
              && sim::Data::get()->camera.type != sim::ViewOrbit )
            {
                sim::Manager::instance()->setViewOrbit();

                osg::ref_ptr<sim::ManipulatorOrbit> manipulator =
                        dynamic_cast< sim::ManipulatorOrbit* >( sim::Manager::instance()->getCameraManipulator() );

                if ( manipulator.valid() )
                {
                    _ui->widgetPlay->setCameraManipulator( manipulator.get() );

                    manipulator->setElevation( osg::DegreesToRadians( 10.0 ) );
                    manipulator->setDistance( 25.0 );
                    manipulator->setHeading( -M_PI_2 - _heading - M_PI_4 );
                }
            }
        }

        if ( sim::Manager::instance()->isFinished() )
        {
            simulationAbort();
        }
    }

    if ( sim::Manager::instance()->isInited() && !sim::Manager::instance()->isFinished() )
    {
        ScreenSaver::reset();
    }

    hid::Manager::instance()->update( _timeCoef * timeStep );
    sim::Manager::instance()->update( _timeCoef * timeStep );

    if ( !_throttle && sim::Manager::instance()->isReady() )
    {
        hid::Manager::instance()->setThrottle( sim::Data::get()->ownship.init_throttle );

        _throttle = true;
    }

    sim::Manager::instance()->setControls( hid::Manager::instance()->getTriggerBasic(),
                                           hid::Manager::instance()->getCtrlRoll(),
                                           hid::Manager::instance()->getCtrlPitch(),
                                           hid::Manager::instance()->getCtrlYaw(),
                                           hid::Manager::instance()->getThrottle() );

    _pending = sim::Manager::instance()->isPending();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::askIfAbort()
{
    sim::Manager::instance()->pause();

    QString title = windowTitle();
    QString text = tr( "Do you want to abort mission?" );

    MessageBox::StandardButton result = MessageBox::question( this, title, text );

    if ( result == QMessageBox::Yes )
    {
        simulationAbort();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::settingsRead()
{
    QSettings settings( SIM_ORG_NAME, SIM_APP_NAME );

    settings.beginGroup( "main_window" );

    restoreState( settings.value( "state" ).toByteArray() );
    restoreGeometry( settings.value( "geometry" ).toByteArray() );

    settings.endGroup();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::settingsSave()
{
    QSettings settings( SIM_ORG_NAME, SIM_APP_NAME );

    settings.beginGroup( "main_window" );

    settings.setValue( "state", saveState() );
    settings.setValue( "geometry", saveGeometry() );

    settings.endGroup();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::simulationStart( int missionIndex )
{
    if ( !_inited )
    {
        _inited   = true;
        _throttle = false;
        _pending  = true;

        _heading  = 0.0;
        _timeCoef = 1.0;

        _ui->stackedMain->setCurrentIndex( PagePlay );

        int w = _ui->widgetPlay->width();
        int h = _ui->widgetPlay->height();

        sim::Manager::instance()->init( w, h, missionIndex );

        _ui->widgetPlay->init();

        ScreenSaver::disable();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::simulationPause()
{
    if ( _inited )
    {
        bool paused = !sim::Manager::instance()->isPaused();

        if ( paused )
        {
            sim::Manager::instance()->pause();
        }
        else
        {
            sim::Manager::instance()->unpause();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::simulationAbort()
{
    if ( _autopilot )
    {
        toggleAutopilot();
    }

    _inited   = false;
    _throttle = false;
    _pending  = true;

    _timeCoef = 1.0;

    _ui->stackedMain->setCurrentIndex( PageHome );

    _ui->widgetPlay->stop();

    sim::Manager::instance()->destroy();

    ScreenSaver::enable();

    std::cout << "Friendly units destroyed: "
              << sim::Statistics::instance()->getFriendsDestroyed()
              << " out of "
              << sim::Statistics::instance()->getFriendsActivated()
              << std::endl;

    std::cout << "Hostile  units destroyed: "
              << sim::Statistics::instance()->getEnemiesDestroyed()
              << " out of "
              << sim::Statistics::instance()->getEnemiesActivated()
              << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::toggleAutopilot()
{
    if ( _inited )
    {
        _autopilot = !_autopilot;
        sim::Manager::instance()->setAutopilot( _autopilot );
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::shortcutPause_activated()
{
    simulationPause();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::shortcutAbort_activated()
{
    if ( _inited )
    {
        askIfAbort();
    }
}

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_TEST
void MainWindow::shortcutAuto_activated()
{
    toggleAutopilot();
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_TEST
void MainWindow::shortcutTimeFaster_activated()
{
    if ( _inited )
    {
        if ( _timeCoef > 0.9 )
        {
            if ( _timeCoef < 10.0 )
            {
                _timeCoef += 1.0;
            }
            else
            {
                _timeCoef = 10.0;
            }
        }
        else
        {
            _timeCoef += 0.1;
        }
    }

    if ( _timeCoef > 10.0 ) _timeCoef = 10.0;
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_TEST
void MainWindow::shortcutTimeSlower_activated()
{
    if ( _inited )
    {
        if ( _timeCoef > 1.0 )
        {
            _timeCoef -= 1.0;
        }
        else
        {
            if ( _timeCoef > 0.0 )
            {
                _timeCoef -= 0.1;
            }
            else
            {
                _timeCoef = 0.0;
            }
        }
    }

    if ( _timeCoef < 0.0 ) _timeCoef = 0.0;
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_TEST
void MainWindow::shortcutTimeNormal_activated()
{
    _timeCoef = 1.0;
}
#endif

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonMenuTutorial_clicked()
{
    simulationStart( 0 );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonMenuData_clicked()
{
    _ui->stackedMain->setCurrentIndex( PageData );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonMenuConf_clicked()
{
    _dialogConf->readData();

    if ( _dialogConf->exec() == QDialog::Accepted )
    {
        _dialogConf->saveData();
        _dialogConf->updateAssignments();
    }
    else
    {
        _dialogConf->readData();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonMenuExit_clicked()
{
    close();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_widgetData_back()
{
    _ui->stackedMain->setCurrentIndex( PageHome );
}
