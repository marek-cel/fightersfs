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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

////////////////////////////////////////////////////////////////////////////////

#include <QMainWindow>

#include <QElapsedTimer>
#include <QShortcut>

#include <defs.h>

#include <gui/Units.h>

////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class MainWindow;
}

////////////////////////////////////////////////////////////////////////////////

/** @brief Main window class. */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    enum Pages
    {
        PageHome     = 0,
        PageCampaign = 1,
        PageMissions = 2,
        PageDatabase = 3,
        PageSettings = 4
    };

    /** @brief Constructor. */
    explicit MainWindow( QWidget *parent = NULLPTR );

    /** @brief Destructor. */
    virtual ~MainWindow();

    /** @brief Initializes main window. */
    void init();

protected:

    /** */
    void closeEvent( QCloseEvent *event );

    /** */
    void timerEvent( QTimerEvent *event );

private:

    Ui::MainWindow *_ui;                    ///< UI object

    QShortcut *_shortcutPause;              ///< pause keybaord shortcut
    QShortcut *_shortcutAbort;              ///< abort keybaord shortcut

#   ifdef SIM_TEST
    QShortcut *_shortcutAuto;               ///< toggle autopilot keybaord shortcut

    QShortcut *_shortcutTimeFaster;         ///< faster time elapsing keyboard shortcut
    QShortcut *_shortcutTimeSlower;         ///< slower time elapsing keyboard shortcut
    QShortcut *_shortcutTimeNormal;         ///< normal time elapsing keyboard shortcut
#   endif

    QElapsedTimer *_timer;                  ///< timer object

    double _timeCoef;                       ///< [-] time coefficient
    double _heading;                        ///< [rad] last valid heading

    bool _autopilot;                        ///< specifies if autopilot is enabled
    bool _inited;                           ///< specifies if simulation is initialized
    bool _throttle;                         ///< specifies if throttle is inited
    bool _pending;                          ///< specifies if mission is pending

    int _timerId;                           ///< timer ID

    int _backPage;

    void askIfAbort();

    void settingsRead();
    void settingsSave();

    void simulationStart( int campaignIndex, int missionIndex );
    void simulationPause();
    void simulationAbort();

    void toggleAutopilot();

private slots:

    void shortcutPause_activated();
    void shortcutAbort_activated();

#   ifdef SIM_TEST
    void shortcutAuto_activated();

    void shortcutTimeFaster_activated();
    void shortcutTimeSlower_activated();
    void shortcutTimeNormal_activated();
#   endif

    void on_pushButtonTutorial_clicked();
    void on_pushButtonTraining_clicked();
    void on_pushButtonCampaign_clicked();
    void on_pushButtonDatabase_clicked();
    void on_pushButtonSettings_clicked();
    void on_pushButtonAbout_clicked();

    void on_pushButtonExit_clicked();

    void on_pushButtonConfDiscard_clicked();
    void on_pushButtonConfSave_clicked();
};

////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
