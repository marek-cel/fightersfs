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

#include <sim/entities/sim_Flak.h>

#include <osgParticle/ModularEmitter>
#include <osgParticle/ParticleSystemUpdater>

#include <sim/cgi/sim_Effects.h>
#include <sim/cgi/sim_Geometry.h>
#include <sim/cgi/sim_Models.h>
#include <sim/cgi/sim_Textures.h>

#include <sim/entities/sim_Entities.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const float Flak::_r_limit = 30.0f;
const float Flak::_r_limit_2 = Flak::_r_limit * Flak::_r_limit;

////////////////////////////////////////////////////////////////////////////////

Flak::Flak( UInt32 shooterId, float fuse_time ) :
    Bullet( 10, shooterId, 15.0f, 0 ),

    _fuse_time ( fuse_time ),
    _exploded ( false )
{}

////////////////////////////////////////////////////////////////////////////////

Flak::~Flak() {}

////////////////////////////////////////////////////////////////////////////////

void Flak::update( double timeStep )
{
    ///////////////////////////
    Bullet::update( timeStep );
    ///////////////////////////

    if ( isActive() )
    {
        if ( isTopLevel() )
        {
            if ( !_exploded )
            {
                if ( _life_time > _fuse_time )
                {
                    rangeDamage();
                    resetLifeTime();
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Flak::hit( Unit* )
{
    rangeDamage();
}

////////////////////////////////////////////////////////////////////////////////

void Flak::rangeDamage()
{
    if ( !_exploded )
    {
        _exploded = true;

        _vel.x() = 0.0;

        List entities = *Entities::instance()->getEntities();
        List::iterator it = entities.begin();

        while ( it != entities.end() )
        {
            Unit *target = dynamic_cast< Unit* >(*it);

            if ( target )
            {
                if ( target->isActive() )
                {
                    float r2 = ( target->getPos() - _pos ).length2() - target->getRadius2();

                    if ( r2 < _r_limit_2 )
                    {
                        UInt16 dp = (float)_dp * ( 1.0f - r2 / _r_limit_2 );

                        target->hit( dp, this );
                        reportTargetHit( target );
                    }
                }
            }

            ++it;
        }

        createBurst();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Flak::createBurst()
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    _switch->addChild( group.get() );

    osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
    ps->getDefaultParticleTemplate().setLifeTime( _life_span - 3.0f );
    ps->getDefaultParticleTemplate().setShape( osgParticle::Particle::QUAD );
    ps->getDefaultParticleTemplate().setSizeRange( osgParticle::rangef(7.0f, 10.0f) );
    ps->getDefaultParticleTemplate().setAlphaRange( osgParticle::rangef(0.9f, 0.0f) );
    ps->getDefaultParticleTemplate().setColorRange(
        osgParticle::rangev4(osg::Vec4(1.0f,1.0f,1.0f,1.0f), osg::Vec4(1.0f,1.0f,0.0f,1.0f)) );
    ps->setDefaultAttributes( getPath( "textures/flak.rgb" ), false, false );

    osg::ref_ptr<osgParticle::RandomRateCounter> rrc = new osgParticle::RandomRateCounter();
    rrc->setRateRange( 10.0f, 10.0f );

    osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
    shooter->setThetaRange( -M_PI, M_PI );
    shooter->setPhiRange( -M_PI, M_PI );
    shooter->setInitialSpeedRange( 0.0f, 0.0f );

    osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
    emitter->setParticleSystem( ps.get() );
    emitter->setCounter( rrc.get() );
    emitter->setShooter( shooter.get() );
    emitter->setEndless( false );
    emitter->setLifeTime( 0.1f );

    group->addChild( emitter.get() );

    osg::ref_ptr<osgParticle::ParticleSystemUpdater> updater = new osgParticle::ParticleSystemUpdater();
    updater->addParticleSystem( ps.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable( ps.get() );

    group->addChild( updater.get() );
    group->addChild( geode.get() );
}
