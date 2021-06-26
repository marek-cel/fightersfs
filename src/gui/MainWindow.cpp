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

#include <gui/MainWindow.h>
#include <ui_MainWindow.h>

#include <iostream>

#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>

#include <gui/ScreenSaver.h>
#include <gui/Utils.h>

#include <hid/hid_Manager.h>
#include <sim/sim_Manager.h>

////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    _ui ( new Ui::MainWindow ),

    _shortcutPause ( NULLPTR ),
    _shortcutAbort ( NULLPTR ),

#   ifdef SIM_TEST
    _shortcutAuto ( NULLPTR ),

    _shortcutFullScreen ( NULLPTR ),
    _shortcutTimeFaster ( NULLPTR ),
    _shortcutTimeSlower ( NULLPTR ),
    _shortcutTimeNormal ( NULLPTR ),
    _shortcutToggleView ( NULLPTR ),
#   endif

    _timer ( NULLPTR ),

    _timeCoef ( 0.0 ),
    _heading  ( 0.0 ),

    _autopilot ( false ),
    _inited    ( false ),
    _throttle  ( false ),
    _pending   ( true  ),

    _timerId ( 0 ),

    _backPage ( PageHome )
{
    _ui->setupUi( this );

    _shortcutPause = new QShortcut( QKeySequence(Qt::Key_Pause)  , this, SLOT(shortcutPause_activated()) );
    _shortcutAbort = new QShortcut( QKeySequence(Qt::Key_Escape) , this, SLOT(shortcutAbort_activated()) );

#   ifdef SIM_TEST
    _shortcutAuto = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_A), this, SLOT(shortcutAuto_activated()) );

    _shortcutFullScreen = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_F)     , this, SLOT(shortcutFullScreen_activated()) );
    _shortcutTimeFaster = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_Equal) , this, SLOT(shortcutTimeFaster_activated()) );
    _shortcutTimeSlower = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_Minus) , this, SLOT(shortcutTimeSlower_activated()) );
    _shortcutTimeNormal = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_0)     , this, SLOT(shortcutTimeNormal_activated()) );
    _shortcutToggleView = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_V)     , this, SLOT(shortcutToggleView_activated()) );
#   endif

    _timer = new QElapsedTimer();

    settingsRead();

    _ui->stackedWidgetMenu->setCurrentIndex( PageHome );
}

////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    settingsSave();

    DELPTR( _timer );

    if ( _timerId ) killTimer( _timerId );

    DELPTR( _shortcutPause );
    DELPTR( _shortcutAbort );

#   ifdef SIM_TEST
    DELPTR( _shortcutAuto );

    DELPTR( _shortcutFullScreen );
    DELPTR( _shortcutTimeFaster );
    DELPTR( _shortcutTimeSlower );
    DELPTR( _shortcutTimeNormal );
    DELPTR( _shortcutToggleView );
#   endif

    DELPTR( _ui );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::init()
{
    hid::Manager::instance()->init();

    _ui->widgetCtrl->readData();
    _ui->widgetCtrl->updateAssignments();

    _timer->start();
    _timerId = startTimer( 1000.0 * SIM_TIME_STEP );

    resizeHomeImage();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent( QCloseEvent *event )
{
    QString title = windowTitle();
    QString text = tr( "Do you want to quit?" );

    QMessageBox::StandardButton result = QMessageBox::question( this, title, text );

    if ( result == QMessageBox::Yes )
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

void MainWindow::resizeEvent( QResizeEvent *event )
{
    //////////////////////////////////
    QMainWindow::resizeEvent( event );
    //////////////////////////////////

    resizeHomeImage();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::timerEvent( QTimerEvent *event )
{
    /////////////////////////////////
    QMainWindow::timerEvent( event );
    /////////////////////////////////

    double timeStep = (double)_timer->restart() / 1000.0;

    if ( _ui->stackedWidgetMain->currentIndex() == 1 && _inited )
    {
        _ui->widgetPlay->update();
    }

    if ( _ui->stackedWidgetMenu->currentIndex() == PageDatabase )
    {
        _ui->widgetData->timeStep();
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

    QMessageBox::StandardButton result = QMessageBox::question( this, title, text );

    if ( result == QMessageBox::Yes )
    {
        simulationAbort();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::resizeHomeImage()
{
    QPixmap pixmap( ":/gui/images/home.png" );

    int w = _ui->labelLogo->width();
    int h = _ui->labelLogo->height();

    //if ( w < pixmap.width() )
    {
        pixmap = pixmap.scaled( w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    }

    //std::cout << w << " x " << h << std::endl;

    _ui->labelLogo->setPixmap( pixmap );
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

void MainWindow::simulationStart( int campaignIndex, int missionIndex )
{
    if ( !_inited )
    {
        _inited   = true;
        _throttle = false;
        _pending  = true;

        _heading  = 0.0;
        _timeCoef = 1.0;

        _ui->stackedWidgetMain->setCurrentIndex( 1 );

        int w = _ui->widgetPlay->width();
        int h = _ui->widgetPlay->height();

        sim::Manager::instance()->init( w, h, campaignIndex, missionIndex );

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

    _ui->stackedWidgetMain->setCurrentIndex( 0 );
    _ui->stackedWidgetMenu->setCurrentIndex( _backPage );

    _ui->widgetMissions->setStatus( sim::Data::get()->mission.status );

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
void MainWindow::shortcutFullScreen_activated()
{
    if ( isFullScreen() )
    {
        showNormal();
    }
    else
    {
        showFullScreen();
    }
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

#ifdef SIM_TEST
void MainWindow::shortcutToggleView_activated()
{
    static int view = 0;

    if ( view == 0 )
    {
        sim::Manager::instance()->setViewFlyby();
        view = 1;
    }
    else if ( view == 1 )
    {
        sim::Manager::instance()->setViewFront();
        view = 2;
    }
    else if ( view == 2 )
    {
        sim::Manager::instance()->setViewOrbit();
        view = 3;
    }
    else if ( view == 3 )
    {
        sim::Manager::instance()->setViewPilot();
        view = 4;
    }
    else if ( view == 4 )
    {
        sim::Manager::instance()->setViewShift();
        view = 5;
    }
    else if ( view == 5 )
    {
        sim::Manager::instance()->setViewWorld();
        view = 6;
    }
    else if ( view == 6 )
    {
        sim::Manager::instance()->setViewChase();
        view = 0;
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonTutorial_clicked()
{
    _backPage = PageHome;

    simulationStart( 0, 0 );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonTraining_clicked()
{
    _ui->stackedWidgetMenu->setCurrentIndex( PageMissions );
    _ui->widgetMissions->setCampaign( 1 );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonCampaign_clicked()
{
    _ui->stackedWidgetMenu->setCurrentIndex( PageCampaign );
    _ui->widgetCampaign->updateCampaign();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonDatabase_clicked()
{
    _ui->stackedWidgetMenu->setCurrentIndex( PageDatabase );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonSettings_clicked()
{
    _ui->widgetCtrl->readData();
    _ui->stackedWidgetMenu->setCurrentIndex( PageSettings );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonAbout_clicked()
{
    QFile aboutHtmlFile( ":/gui/html/about.html" );

    QString aboutWinTitle;
    QString aboutInfoText;

    aboutWinTitle = tr( "About" );

    if ( aboutHtmlFile.open( QIODevice::ReadOnly ) )
    {
        aboutInfoText = aboutHtmlFile.readAll();
        aboutHtmlFile.close();
    }

    QMessageBox::about( this, aboutWinTitle, aboutInfoText );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonExit_clicked()
{
    close();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonConfDiscard_clicked()
{
    _ui->stackedWidgetMenu->setCurrentIndex( PageHome );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonConfSave_clicked()
{
    _ui->widgetCtrl->saveData();
    _ui->stackedWidgetMenu->setCurrentIndex( PageHome );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_widgetCampaign_startClicked( int campaign )
{
    _ui->stackedWidgetMenu->setCurrentIndex( PageMissions );
    _ui->widgetMissions->setCampaign( campaign );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_widgetMissions_startClicked( int campaign, int mission )
{
    _backPage = PageMissions;

    simulationStart( campaign, mission );
}
