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

#include <iostream>

#include <QApplication>
#include <QDir>
#include <QFontDatabase>
#include <QMetaType>

#include <defs.h>

#include <gui/MainWindow.h>

////////////////////////////////////////////////////////////////////////////////

/** This is application main function. */
int main( int argc, char *argv[] )
{
    setlocale( LC_ALL, "C" );

#   ifdef _LINUX_
    setenv( "LC_NUMERIC", "en_US", 1 );
#   endif

    QApplication *app = new QApplication( argc, argv );

    app->setApplicationName    ( SIM_APP_NAME   );
    app->setApplicationVersion ( SIM_APP_VER    );
    app->setOrganizationDomain ( SIM_ORG_DOMAIN );
    app->setOrganizationName   ( SIM_ORG_NAME   );

    QFontDatabase::addApplicationFont( ":/gui/fonts/fsg_stencil.ttf" );

    QFile file( ":/gui/styles/main.qss" );
    file.open( QFile::ReadOnly );
    QString styleSheet = QLatin1String( file.readAll() );
    qApp->setStyleSheet( styleSheet );

    MainWindow *win = new MainWindow();

#   ifdef SIM_TEST
    win->show();
#   else
    win->showFullScreen();
#   endif

    win->init();

    int result = app->exec();

    delete win; win = 0;
    delete app; app = 0;

    return result;
}
