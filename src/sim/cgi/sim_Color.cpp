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
