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
#ifndef UTILS_H
#define UTILS_H

////////////////////////////////////////////////////////////////////////////////

#include <QDomDocument>

#include <defs.h>

////////////////////////////////////////////////////////////////////////////////

class Utils
{
public:

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    static int read( const QDomElement &node, Text &text );

    static void m2ftin( double len_m, int *len_ft, int *len_in );

    static QString m2ftin( double len_m );
};

////////////////////////////////////////////////////////////////////////////////

#endif // UTILS_H
