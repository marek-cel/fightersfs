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

#include <Manager.h>

#include <iostream>

#include <QDir>
#include <QFontDatabase>
#include <QLibraryInfo>
#include <QMetaType>
#include <QSettings>
#include <QSplashScreen>
#include <QTimer>

#include <osg/Notify>

#include <sim/sim_Languages.h>

////////////////////////////////////////////////////////////////////////////////

int Manager::main( int argc, char *argv[] )
{
    Manager *mngr = new Manager();

    mngr->init( argc, argv );
    mngr->exec();

    int result = mngr->result();

    DELPTR( mngr );

    return result;
}

////////////////////////////////////////////////////////////////////////////////

Manager::Manager() :
    _app ( NULLPTR ),
    _win ( NULLPTR ),
    _result ( 0 )
{

}

////////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
    DELPTR( _win );
    DELPTR( _app );
}

////////////////////////////////////////////////////////////////////////////////

void Manager::init( int argc, char *argv[] )
{
    setlocale( LC_ALL, "C" );

    QLocale::setDefault( QLocale::system() );

    Path::setBasePath( SIM_BASE_PATH );

#   ifdef SIM_OSGDEBUGINFO
    osg::setNotifyLevel( osg::DEBUG_INFO );
#   else
    osg::setNotifyLevel( osg::WARN );
#   endif

    _app = new QApplication( argc, argv );

    _app->setApplicationName    ( SIM_APP_NAME   );
    _app->setApplicationVersion ( SIM_APP_VER    );
    _app->setOrganizationDomain ( SIM_ORG_DOMAIN );
    _app->setOrganizationName   ( SIM_ORG_NAME   );

    initTranslations();

    QFontDatabase::addApplicationFont( ":/gui/fonts/fsg_stencil.ttf" );

    QFile file( ":/gui/styles/main.qss" );
    file.open( QFile::ReadOnly );
    QString styleSheet = QLatin1String( file.readAll() );
    _app->setStyleSheet( styleSheet );

    showSlpashScreen();

    _win = new MainWindow();

#   ifdef SIM_TEST
    _win->show();
#   else
    _win->show();
    //_win->showFullScreen();
#   endif

    _win->init();
}

////////////////////////////////////////////////////////////////////////////////

void Manager::exec()
{
    _result = _app->exec();
}

////////////////////////////////////////////////////////////////////////////////

QString Manager::getLangCode()
{
    QString locale = QLocale::system().name();
    QString lang_code = locale.mid( 0, locale.indexOf('_') );

    QSettings settings( SIM_ORG_NAME, SIM_APP_NAME );

    settings.beginGroup( "settings" );

    lang_code = settings.value( "lang_code", lang_code ).toString();

    settings.endGroup();

    return lang_code;
}

////////////////////////////////////////////////////////////////////////////////

void Manager::initTranslations()
{
    QString lang_code = getLangCode();



    _translator.load( QString( ":/gui/translations/fightersfs_" ) + lang_code );

    _translatorQt.load( QLocale::system(),
                        QString::fromUtf8("qt"),
                        QString::fromUtf8("_"),
                        QLibraryInfo::location(QLibraryInfo::TranslationsPath) );

    _app->installTranslator( &_translator );
    _app->installTranslator( &_translatorQt );

    sim::Languages::instance()->setCurrentByCode( lang_code.toStdString() );
}

////////////////////////////////////////////////////////////////////////////////

void Manager::showSlpashScreen()
{
    QPixmap pixmap(":/gui/images/splashscreen.png");
    QSplashScreen splash( pixmap );
    splash.show();
    splash.showMessage( QString( SIM_APP_NAME ) + " " + QString( SIM_APP_VER ) );
    _app->processEvents();
    QTimer::singleShot( 1500, &splash, SLOT(close()) );
    while ( splash.isVisible() )
    {
        _app->processEvents();
    }
}
