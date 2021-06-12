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

#include <sim/cgi/sim_Effects.h>

#include <osg/PositionAttitudeTransform>

#include <osgParticle/ModularEmitter>
#include <osgParticle/ParticleSystemUpdater>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

osg::Group* Effects::createExplosion( float scale )
{
    osg::ref_ptr<osg::Group> group = new osg::Group();

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    group->addChild( pat.get() );

    pat->setAttitude( osg::Quat( 0.0, osg::Z_AXIS,
                          -osg::PI_2, osg::Y_AXIS,
                          -osg::PI_2, osg::X_AXIS ) );

    osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
    ps->getDefaultParticleTemplate().setLifeTime( 0.75f );
    ps->getDefaultParticleTemplate().setShape( osgParticle::Particle::QUAD );
    ps->getDefaultParticleTemplate().setSizeRange( osgParticle::rangef(1.0f, scale) );
    ps->getDefaultParticleTemplate().setAlphaRange( osgParticle::rangef(1.0f, 0.0f) );
    ps->getDefaultParticleTemplate().setColorRange(
        osgParticle::rangev4(osg::Vec4(1.0f,1.0f,0.5f,1.0f), osg::Vec4(1.0f,0.5f,0.0f,1.0f)) );
    ps->setDefaultAttributes( getPath( "textures/explosion.rgb" ), true, false );

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
    emitter->setLifeTime( 0.75f );

    pat->addChild( emitter.get() );

    osg::ref_ptr<osgParticle::ParticleSystemUpdater> updater = new osgParticle::ParticleSystemUpdater();
    updater->addParticleSystem( ps.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable( ps.get() );

    group->addChild( updater.get() );
    group->addChild( geode.get() );

    return group.release();
}

////////////////////////////////////////////////////////////////////////////////

osg::Group* Effects::createFlames( const char *texture )
{
    osg::ref_ptr<osg::Group> group = new osg::Group();

    osg::ref_ptr<osgParticle::ParticleSystemUpdater> updater = new osgParticle::ParticleSystemUpdater();
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
    ps->getDefaultParticleTemplate().setLifeTime( 1.5f );
    ps->getDefaultParticleTemplate().setShape( osgParticle::Particle::QUAD );
    ps->getDefaultParticleTemplate().setSizeRange( osgParticle::rangef(1.0f, 0.4f) );
    ps->getDefaultParticleTemplate().setAlphaRange( osgParticle::rangef(1.0f, 0.0f) );
    ps->getDefaultParticleTemplate().setColorRange(
        osgParticle::rangev4(osg::Vec4(1.0f,1.0f,0.5f,1.0f), osg::Vec4(1.0f,0.5f,0.0f,1.0f)) );
    ps->setDefaultAttributes( texture, true, false );

    osg::ref_ptr<osgParticle::RandomRateCounter> rrc = new osgParticle::RandomRateCounter();
    rrc->setRateRange( 7, 15 );

    osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
    shooter->setThetaRange( -osg::PI_4*0.05f, osg::PI_4*0.05f );
    shooter->setPhiRange( -osg::PI_4, osg::PI_4 );
    shooter->setInitialSpeedRange( 7.5f, 15.0f );
    shooter->setInitialRotationalSpeedRange( osg::Vec3( 0.0f, 0.0, 5.0 ),
                                             osg::Vec3( 0.0f, 0.0, 5.0 ) );

    osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
    emitter->setParticleSystem( ps.get() );
    emitter->setCounter( rrc.get() );
    emitter->setShooter( shooter.get() );

    group->addChild( emitter.get() );

    updater->addParticleSystem( ps.get() );
    geode->addDrawable( ps.get() );

    group->addChild( updater.get() );
    group->addChild( geode.get() );

    return group.release();
}

////////////////////////////////////////////////////////////////////////////////

 osg::Group* Effects::createSmoke( float lifeTime,
                                   float size0, float size1,
                                   float spread,
                                   float intensity,
                                   float speed )
{
    if ( spread    > 1.0f ) spread    = 1.0f;
    if ( intensity > 1.0f ) intensity = 1.0f;
    if ( speed     > 1.0f ) speed     = 1.0f;

    osg::ref_ptr<osg::Group> group = new osg::Group();

    osg::ref_ptr<osgParticle::ParticleSystemUpdater> updater = new osgParticle::ParticleSystemUpdater();
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
    ps->getDefaultParticleTemplate().setLifeTime( lifeTime );
    ps->getDefaultParticleTemplate().setShape( osgParticle::Particle::QUAD );
    ps->getDefaultParticleTemplate().setSizeRange( osgParticle::rangef(size0, size1) );
    ps->getDefaultParticleTemplate().setAlphaRange( osgParticle::rangef(1.0f, 0.0f) );
    ps->getDefaultParticleTemplate().setColorRange(
        osgParticle::rangev4(osg::Vec4(1.0f,1.0f,1.0f,1.0f), osg::Vec4(1.0f,1.0f,1.0f,0.0f)) );
    ps->setDefaultAttributes( getPath( "textures/smoke_dark.rgb" ), false, false );

    osg::ref_ptr<osgParticle::RandomRateCounter> rrc = new osgParticle::RandomRateCounter();
    rrc->setRateRange( 20.0f * intensity, 30.0f * intensity );

    osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
    shooter->setThetaRange( -osg::PI_4*0.5*spread, osg::PI_4*0.5*spread );
    shooter->setPhiRange( -osg::PI_4*0.5*spread, osg::PI_4*0.5*spread );
    shooter->setInitialSpeedRange( 7.5f*speed, 15.0f*speed );

    osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
    emitter->setParticleSystem( ps.get() );
    emitter->setCounter( rrc.get() );
    emitter->setShooter( shooter.get() );

    group->addChild( emitter.get() );

    updater->addParticleSystem( ps.get() );
    geode->addDrawable( ps.get() );

    group->addChild( updater.get() );
    group->addChild( geode.get() );

    return group.release();
}

////////////////////////////////////////////////////////////////////////////////

Effects::Smoke* Effects::createSmokeTrail()
{
    Smoke *smokeTrail = new Smoke();
    smokeTrail->setTextureFileName( getPath( "textures/smoke_light.rgb" ) );
    smokeTrail->setIntensity( 10.0f );
    smokeTrail->setEmitterDuration( 1000.0 );

    return smokeTrail;
}
