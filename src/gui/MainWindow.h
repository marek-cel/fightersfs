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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

////////////////////////////////////////////////////////////////////////////////

#include <QElapsedTimer>
#include <QMainWindow>
#include <QShortcut>

#include <defs.h>

#include <gui/DialogConf.h>

////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class MainWindow;
}

////////////////////////////////////////////////////////////////////////////////

/** This is main window class. */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    enum
    {
        PageHome = 0,
        PageData = 1,
        PagePlay = 2
    };

    /** Constructor. */
    explicit MainWindow( QWidget *parent = NULLPTR );

    /** Destructor. */
    virtual ~MainWindow();

    /** */
    void init();

protected:

    /** */
    void closeEvent( QCloseEvent *event );

    /** */
    void timerEvent( QTimerEvent *event );

private:

    Ui::MainWindow *_ui;                    ///< UI object

    DialogConf *_dialogConf;                ///<

    QShortcut *_shortcutPause;              ///<
    QShortcut *_shortcutAbort;              ///<

    QElapsedTimer *_timer;                  ///<

    int _timerId;                           ///< timer ID

    double _timeCoef;                       ///< [-] time coefficient
    double _heading;                        ///< [rad] last valid heading

    bool _autopilot;                        ///<
    bool _inited;                           ///<
    bool _paused;                           ///<
    bool _throttle;                         ///< specifies if throttle is inited
    bool _pending;                          ///< specifies if mission is pending

    void askIfAbort();

    void settingsRead();
    void settingsSave();

    void simulationStart( int missionIndex );
    void simulationPause();
    void simulationAbort();

    void toggleAutopilot();

private slots:

    void shortcutPause_activated();
    void shortcutAbort_activated();

    void on_pushButtonMenuTutorial_clicked();
    void on_pushButtonMenuData_clicked();
    void on_pushButtonMenuConf_clicked();
    void on_pushButtonMenuExit_clicked();

    void on_pushButtonDataBack_clicked();
};

////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
