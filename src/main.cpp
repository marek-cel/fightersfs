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

#include <iostream>

#include <QApplication>
#include <QDir>
#include <QFontDatabase>
#include <QLibraryInfo>
#include <QMetaType>
#include <QSettings>
#include <QTranslator>

#include <osg/Notify>

#include <defs.h>

#include <gui/MainWindow.h>

#include <sim/sim_Languages.h>

////////////////////////////////////////////////////////////////////////////////

QString getLangCode();

////////////////////////////////////////////////////////////////////////////////

/** This is application main function. */
int main( int argc, char *argv[] )
{
    setlocale( LC_ALL, "C" );

    QLocale::setDefault( QLocale::system() );

    Path::setBasePath( SIM_BASE_PATH );

    QApplication *app = new QApplication( argc, argv );

    app->setApplicationName    ( SIM_APP_NAME   );
    app->setApplicationVersion ( SIM_APP_VER    );
    app->setOrganizationDomain ( SIM_ORG_DOMAIN );
    app->setOrganizationName   ( SIM_ORG_NAME   );

    QString lang_code = getLangCode();

    QTranslator translator;
    QTranslator translatorQt;

    translator.load( QString( ":/gui/translations/fightersfs_" ) + lang_code );

    translatorQt.load( QLocale::system(),
                       QString::fromUtf8("qt"),
                       QString::fromUtf8("_"),
                       QLibraryInfo::location(QLibraryInfo::TranslationsPath) );

    app->installTranslator( &translator );
    app->installTranslator( &translatorQt );

    sim::Languages::instance()->setCurrentByCode( lang_code.toStdString() );

    QFontDatabase::addApplicationFont( ":/gui/fonts/fsg_stencil.ttf" );

    QFile file( ":/gui/styles/main.qss" );
    file.open( QFile::ReadOnly );
    QString styleSheet = QLatin1String( file.readAll() );
    qApp->setStyleSheet( styleSheet );

#   ifdef SIM_OSGDEBUGINFO
    osg::setNotifyLevel( osg::DEBUG_INFO );
#   else
    osg::setNotifyLevel( osg::WARN );
#   endif

    MainWindow *win = new MainWindow();

#   ifdef SIM_TEST
    win->show();
#   else
    win->showFullScreen();
#   endif

    win->init();

    int result = app->exec();

    DELPTR( win );
    DELPTR( app );

    return result;
}

////////////////////////////////////////////////////////////////////////////////

QString getLangCode()
{
    QString locale = QLocale::system().name();
    QString lang_code = locale.mid( 0, locale.indexOf('_') );

    QSettings settings( SIM_ORG_NAME, SIM_APP_NAME );

    settings.beginGroup( "settings" );

    lang_code = settings.value( "lang_code", lang_code ).toString();

    settings.endGroup();

    return lang_code;
}
