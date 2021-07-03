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

#include <sim/entities/sim_Tracer.h>

#include <sim/cgi/sim_Models.h>
#include <sim/entities/sim_Entities.h>
#include <sim/sim_Elevation.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

//UInt8 Tracer::m_counter_trail = 0;
//UInt8 Tracer::m_quantity      = 0;

////////////////////////////////////////////////////////////////////////////////

Tracer::Tracer( UInt32 shooterId, bool trail ) :
    Bullet( 25, shooterId, 5.0f, 0 ),
    _elevation ( 0.0f ),
    _trail ( trail )
//    _counter_update ( 0 )
{
    load();

//    if ( trail ) _counter_trail++;
}

////////////////////////////////////////////////////////////////////////////////

Tracer::~Tracer()
{
//    if ( _trail && _quantity > 0 )
//    {
//        _quantity--;
//    }
}

////////////////////////////////////////////////////////////////////////////////

void Tracer::load()
{
    if ( _switch->getNumChildren() > 0 )
    {
        _switch->removeChildren( 0, _switch->getNumChildren() );
    }

    _model = Models::getTracer();

    if ( _model.valid() )
    {
        _switch->addChild( _model.get() );
    }

//    if ( trail && _quantity < 50 && _counter_trail % 5 == 0 )
//    {
//        _trail = true;

//        _counter_trail = 0;

//        _smokeTrail = new osgParticle::SmokeTrailEffect();
//        _root->addChild( _smokeTrail.get() );

//        _smokeTrail->setTextureFileName( getPath( "textures/smoke_trail.rgb" ) );
//        _smokeTrail->setScale( 0.2f );
//        _smokeTrail->setEmitterDuration( 1.0f );
//        _smokeTrail->setIntensity( 0.01f );
//        _smokeTrail->setUseLocalParticleSystem( true );
//        _smokeTrail->setParticleDuration( 1.0f );

//        osg::ref_ptr<osg::StateSet> stateSet = _smokeTrail->getOrCreateStateSet();
//        stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
//        stateSet->setRenderBinDetails( SIM_DEPTH_SORTED_BIN_TRAIL, "DepthSortedBin" );

//        _quantity++;
//    }
//    else
//    {
//        _trail = false;
//    }
}

////////////////////////////////////////////////////////////////////////////////

void Tracer::update( double timeStep )
{
    /////////////////////////////
    Munition::update( timeStep );
    /////////////////////////////

    if ( _pos.z() < _elevation )
    {
        setState( Inactive );
    }

//    if ( _trail && _smokeTrail.valid() )
//    {
//        if ( _counter_update % ( _counter_update <= 5 ? 5 : 20 ) == 0 )
//        {
//            if ( _life_time < 1.5f )
//            {
//                _smokeTrail->setPosition( _pos );
//            }
//            else
//            {
//                _root->removeChild( _smokeTrail.get() );
//                _smokeTrail.release();
//            }
//        }

//        _counter_update++;
//    }
}

////////////////////////////////////////////////////////////////////////////////

void Tracer::updateElevation()
{
    if ( isTopLevel() )
    {
        ////////////////////////////
        Munition::updateElevation();
        ////////////////////////////

        _elevation = Elevation::instance()->getElevation( _pos.x(), _pos.y() );
    }
}
