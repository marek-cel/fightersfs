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
#ifndef SIM_COLOR_H
#define SIM_COLOR_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Vec3>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Colors class. */
class Color
{
public:

    static const osg::Vec3 amber;       ///< amber
    static const osg::Vec3 black;       ///< black  (according to W3C)
    static const osg::Vec3 cyan;        ///< cyan   (according to W3C)
    static const osg::Vec3 green;       ///< green  (according to W3C)
    static const osg::Vec3 grey;        ///< grey   (according to W3C)
    static const osg::Vec3 lime;        ///< lime   (according to W3C)
    static const osg::Vec3 orange;      ///< orange (according to W3C)
    static const osg::Vec3 red;         ///< red    (according to W3C)
    static const osg::Vec3 white;       ///< white  (according to W3C)
    static const osg::Vec3 yellow;      ///< yellow (according to W3C)

    static const osg::Vec3 fog_light;   ///< light fog color
    static const osg::Vec3 fog_heavy;   ///< heavy fog color

    static const osg::Vec3 sun;         ///< sun light color
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_COLOR_H
