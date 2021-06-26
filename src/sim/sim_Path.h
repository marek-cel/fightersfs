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
#           define SIM_BASE_PATH "/usr/share/fightersfs/data/"
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
