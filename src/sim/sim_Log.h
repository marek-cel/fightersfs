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
#ifndef SIM_LOG_H
#define SIM_LOG_H

////////////////////////////////////////////////////////////////////////////////

#include <sstream>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Logging class.
 */
class Log
{
public:

    static std::ostream &_out;  ///< log output stream

    inline static std::ostream& i() { return ( timeTag() << "[INFO] "    ); }
    inline static std::ostream& w() { return ( timeTag() << "[WARNING] " ); }
    inline static std::ostream& e() { return ( timeTag() << "[ERROR] "   ); }

    inline static std::ostream& out() { return _out; }

    /**
     * Creates time tag.
     * @return output stream
     */
    static std::ostream& timeTag();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_LOG_H
