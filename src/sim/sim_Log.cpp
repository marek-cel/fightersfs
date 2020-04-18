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

#include <sim/sim_Log.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <cmath>

#ifdef _LINUX_
#   include <sys/time.h>
#endif

#ifdef WIN32
#   include <Windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

std::ostream& Log::_out = std::cerr;

////////////////////////////////////////////////////////////////////////////////

std::ostream& Log::timeTag()
{
    int year = 2000;
    int mon  = 1;
    int day  = 1;
    int hour = 0;
    int min  = 0;
    int sec  = 0;
    int msec = 0;

#   ifdef _LINUX_
    struct timeval tp;
    gettimeofday( &tp, NULL );
    std::tm *tm = std::localtime( &tp.tv_sec );

    year = 1900 + tm->tm_year;
    mon  = tm->tm_mon + 1;
    day  = tm->tm_mday;
    hour = tm->tm_hour;
    min  = tm->tm_min;
    sec  = tm->tm_sec;
    msec = floor( tp.tv_usec * 0.001 );
#   endif

#   ifdef WIN32
    SYSTEMTIME st;
    GetLocalTime( &st );

    year = st.wYear;
    mon  = st.wMonth;
    day  = st.wDay;
    hour = st.wHour;
    min  = st.wMinute;
    sec  = st.wSecond;
    msec = st.wMilliseconds;
#   endif

    _out << "[";
    _out << year;
    _out << "-";
    _out << std::setfill('0') << std::setw( 2 ) << mon;
    _out << "-";
    _out << std::setfill('0') << std::setw( 2 ) << day;
    _out << " ";
    _out << std::setfill('0') << std::setw( 2 ) << hour;
    _out << ":";
    _out << std::setfill('0') << std::setw( 2 ) << min;
    _out << ":";
    _out << std::setfill('0') << std::setw( 2 ) << sec;
    _out << ".";
    _out << std::setfill('0') << std::setw( 3 ) << msec;
    _out << "]";

    return _out;
}
