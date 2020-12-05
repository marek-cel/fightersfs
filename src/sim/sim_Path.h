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
#ifndef SIM_PATH_H
#define SIM_PATH_H

////////////////////////////////////////////////////////////////////////////////

#include <string>

#ifndef SIM_BASE_PATH
#   ifdef SIM_TEST
#       define SIM_BASE_PATH "../data/"
#   else
#       if defined(__ANDROID__)
#           define SIM_DATA_PATH "/sdcard/Download/data/"
#       elif defined(_LINUX_)
#           define SIM_BASE_PATH "/usr/share/fightersfs/"
#       elif defined(WIN32)
#           define SIM_BASE_PATH "../data/"
#       endif
#   endif
#endif

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Files path class.
 */
class Path
{
public:

    /**
     * @brief Returns file path prefixed with base path.
     * @param path file path relative to the base path.
     */
    inline static std::string get( const char *path = "" )
    {
        return get( std::string( path ) );
    }

    /**
     * @brief Returns file path prefixed with base path.
     * @param path file path relative to the base path.
     */
    inline static std::string get( const std::string &path )
    {
        return _basePath + path;
    }

    inline static void setBasePath( const char *basePath )
    {
        _basePath = basePath;
    }

private:

    static std::string _basePath;
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_PATH_H
