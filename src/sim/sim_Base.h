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
#ifndef SIM_BASE_H
#define SIM_BASE_H

////////////////////////////////////////////////////////////////////////////////

#include <memory.h>
#include <string>

#include <sim/sim_Defs.h>
#include <sim/sim_Types.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Base class. */
class Base
{
public:

    /**
     * Returns file path prefixed with base path.
     * @param path file path relative to the base path.
     */
    inline static std::string getPath( const std::string &path )
    {
        return m_basePath + path;
    }

    /**
     * @brief setBasePath
     * @param basePath
     */
    inline static void setBasePath( const std::string &basePath )
    {
        m_basePath = basePath;
    }

    /** Overloaded operator which initializes memory. */
    void* operator new( size_t st )
    {
        void *pv = ::operator new( st );
        if ( pv ) memset( pv , 0 , st );
        return pv;
    }

private:

    static std::string m_basePath;
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_BASE_H
