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

#include <sim/cgi/sim_Color.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const osg::Vec3 Color::amber  = osg::Vec3( 1.00f, 0.75f, 0.00f );
const osg::Vec3 Color::black  = osg::Vec3( 0.00f, 0.00f, 0.00f );
const osg::Vec3 Color::cyan   = osg::Vec3( 0.00f, 1.00f, 1.00f );
const osg::Vec3 Color::green  = osg::Vec3( 0.00f, 0.50f, 0.00f );
const osg::Vec3 Color::grey   = osg::Vec3( 0.50f, 0.50f, 0.50f );
const osg::Vec3 Color::lime   = osg::Vec3( 0.00f, 1.00f, 0.00f );
const osg::Vec3 Color::orange = osg::Vec3( 1.00f, 0.65f, 0.00f );
const osg::Vec3 Color::red    = osg::Vec3( 1.00f, 0.00f, 0.00f );
const osg::Vec3 Color::white  = osg::Vec3( 1.00f, 1.00f, 1.00f );
const osg::Vec3 Color::yellow = osg::Vec3( 1.00f, 1.00f, 0.00f );

const osg::Vec3 Color::fog_light = osg::Vec3( 164.0f/255.0f, 191.0f/255.0f, 220.0f/255.0f );
const osg::Vec3 Color::fog_heavy = osg::Vec3( 219.0f/255.0f, 219.0f/255.0f, 219.0f/255.0f );

const osg::Vec3 Color::sun = osg::Vec3( 255.0f/255.0f, 253.0f/255.0f, 250.0f/255.0f );
