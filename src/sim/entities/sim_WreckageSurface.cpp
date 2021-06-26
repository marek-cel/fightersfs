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

#include <sim/entities/sim_WreckageSurface.h>

#ifdef SIM_DESKTOP
#   include <sim/entities/sim_UnitMarine.h>
#endif

#include <sim/cgi/sim_Effects.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

WreckageSurface::WreckageSurface( osg::Node *model , osg::Group *smoke ) :
    Wreckage( model )
{
    osg::ref_ptr<osg::Group> smokeTmp = smoke;

    if ( !smokeTmp.valid() )
    {
        smokeTmp = Effects::createSmoke( 60.0f, 10.0f, 150.0f, 1.0f, 0.04f, 0.5f );
    }

    _switch->addChild( smokeTmp.get() );

#   ifdef SIM_DESKTOP
    UnitMarine::createReflection( model, _switch.get() );
#   endif
}

////////////////////////////////////////////////////////////////////////////////

WreckageSurface::~WreckageSurface() {}
