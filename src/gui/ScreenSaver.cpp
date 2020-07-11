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

#include <gui/ScreenSaver.h>

#ifdef _MSC_VER
#   include <windows.h>
#endif

#ifdef _LINUX_
#   include <QX11Info>
#   include <X11/extensions/scrnsaver.h>
#   include <X11/Xlib.h>
#endif

////////////////////////////////////////////////////////////////////////////////

void ScreenSaver::disable()
{
#   ifdef _MSC_VER
    SetThreadExecutionState( ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED );
#   endif

#   ifdef _LINUX_
    XScreenSaverSuspend( QX11Info::display(), True );
#   endif
}

////////////////////////////////////////////////////////////////////////////////

void ScreenSaver::enable()
{
#   ifdef _MSC_VER
    SetThreadExecutionState( ES_CONTINUOUS );
#   endif

#   ifdef _LINUX_
    XScreenSaverSuspend( QX11Info::display(), False );
#   endif
}

////////////////////////////////////////////////////////////////////////////////

void ScreenSaver::reset()
{
#   ifdef _LINUX_
    XResetScreenSaver( QX11Info::display() );
#   endif
}
