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

#include <sim/entities/sim_Explosion.h>

#include <osgParticle/ModularEmitter>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/RadialShooter>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Explosion::Explosion( float scale, Group *parent ) :
    Entity( parent, Active, 10.5f )
{
    createExplosionFire( scale );
    createExplosionSmoke( scale );
}

////////////////////////////////////////////////////////////////////////////////

Explosion::~Explosion() {}

////////////////////////////////////////////////////////////////////////////////

void Explosion::createExplosionFire( float scale )
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    _switch->addChild( group.get() );

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    group->addChild( pat.get() );

    pat->setAttitude( osg::Quat( 0.0, osg::Z_AXIS,
                          -osg::PI_2, osg::Y_AXIS,
                          -osg::PI_2, osg::X_AXIS ) );

    osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
    ps->getDefaultParticleTemplate().setLifeTime( 0.5f );
    ps->getDefaultParticleTemplate().setShape( osgParticle::Particle::QUAD );
    ps->getDefaultParticleTemplate().setSizeRange( osgParticle::rangef(1.0f*scale, 2.0f*scale) );
    ps->getDefaultParticleTemplate().setAlphaRange( osgParticle::rangef(1.0f, 0.0f) );
    ps->getDefaultParticleTemplate().setColorRange(
        osgParticle::rangev4(osg::Vec4(1.0f,1.0f,0.5f,1.0f), osg::Vec4(1.0f,0.5f,0.0f,1.0f)) );
    ps->setDefaultAttributes( getPath( "textures/explosion_fire.rgb" ), true, false );

    osg::ref_ptr<osgParticle::RandomRateCounter> rrc = new osgParticle::RandomRateCounter();
    rrc->setRateRange( 10, 20 );

    osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
    shooter->setThetaRange( -osg::PI, osg::PI );
    shooter->setPhiRange( -osg::PI, osg::PI );
    shooter->setInitialSpeedRange( -10.0f, 10.0f );

    osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
    emitter->setParticleSystem( ps.get() );
    emitter->setCounter( rrc.get() );
    emitter->setShooter( shooter.get() );
    emitter->setEndless( false );
    emitter->setLifeTime( 0.25f );

    pat->addChild( emitter.get() );

    osg::ref_ptr<osgParticle::ParticleSystemUpdater> updater = new osgParticle::ParticleSystemUpdater();
    updater->addParticleSystem( ps.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable( ps.get() );

    group->addChild( updater.get() );
    group->addChild( geode.get() );
}

////////////////////////////////////////////////////////////////////////////////

void Explosion::createExplosionSmoke( float scale )
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    _switch->addChild( group.get() );

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    group->addChild( pat.get() );

    pat->setAttitude( osg::Quat( 0.0, osg::Z_AXIS,
                          -osg::PI_2, osg::Y_AXIS,
                          -osg::PI_2, osg::X_AXIS ) );

    osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
    ps->getDefaultParticleTemplate().setLifeTime( 10.0f );
    ps->getDefaultParticleTemplate().setShape( osgParticle::Particle::QUAD );
    ps->getDefaultParticleTemplate().setSizeRange( osgParticle::rangef(1.0f*scale, 8.0f*scale) );
    ps->getDefaultParticleTemplate().setAlphaRange( osgParticle::rangef(1.0f, 0.0f) );
    ps->getDefaultParticleTemplate().setColorRange(
        osgParticle::rangev4(osg::Vec4(0.0f,0.0f,0.0f,1.0f), osg::Vec4(0.5f,0.5f,0.5f,1.0f)) );
    ps->setDefaultAttributes( getPath( "textures/explosion_smoke.rgb" ), false, false );

    osg::ref_ptr<osgParticle::RandomRateCounter> rrc = new osgParticle::RandomRateCounter();
    rrc->setRateRange( 10, 10 );

    osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
    shooter->setThetaRange( -osg::PI, osg::PI );
    shooter->setPhiRange( -osg::PI, osg::PI );
    shooter->setInitialSpeedRange( -2.0f, 2.0f );

    osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
    emitter->setStartTime( 0.1f );
    emitter->setParticleSystem( ps.get() );
    emitter->setCounter( rrc.get() );
    emitter->setShooter( shooter.get() );
    emitter->setEndless( false );
    emitter->setLifeTime( 0.5f );

    pat->addChild( emitter.get() );

    osg::ref_ptr<osgParticle::ParticleSystemUpdater> updater = new osgParticle::ParticleSystemUpdater();
    updater->addParticleSystem( ps.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable( ps.get() );

    group->addChild( updater.get() );
    group->addChild( geode.get() );
}
