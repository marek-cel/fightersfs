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

#include <sim/cgi/sim_OTW.h>

#include <sim/cgi/sim_FogScene.h>
#include <sim/cgi/sim_Gates.h>
#include <sim/cgi/sim_SkyDome.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

OTW::OTW( float linesWidth, Module *parent ) :
    Module( new osg::Group(), parent ),

    _linesWidth ( linesWidth )
{
    osg::ref_ptr<osg::StateSet> stateSet = _root->getOrCreateStateSet();

    stateSet->setMode( GL_RESCALE_NORMAL , osg::StateAttribute::ON  );
    stateSet->setMode( GL_LIGHTING       , osg::StateAttribute::ON  );
    stateSet->setMode( GL_LIGHT0         , osg::StateAttribute::ON  );
    stateSet->setMode( GL_BLEND          , osg::StateAttribute::ON  );
    stateSet->setMode( GL_ALPHA_TEST     , osg::StateAttribute::ON  );
    stateSet->setMode( GL_DEPTH_TEST     , osg::StateAttribute::ON  );
    stateSet->setMode( GL_DITHER         , osg::StateAttribute::OFF );
}

////////////////////////////////////////////////////////////////////////////////

OTW::~OTW() {}

////////////////////////////////////////////////////////////////////////////////

void OTW::init()
{
    addChild( new FogScene( this ) );
    addChild( new Gates( _linesWidth, this ) );
    addChild( new SkyDome( this ) );
}
