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

const float Flak::m_r_limit = 30.0f;
const float Flak::m_r_limit_2 = Flak::m_r_limit * Flak::m_r_limit;

////////////////////////////////////////////////////////////////////////////////

Flak::Flak( UInt32 shooterId, float fuse_time ) :
    Bullet( 10, shooterId, 15.0f, 0 ),

    m_fuse_time ( fuse_time ),
    m_exploded ( false )
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
            if ( !m_exploded )
            {
                if ( _life_time > m_fuse_time )
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
    if ( !m_exploded )
    {
        m_exploded = true;

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

                    if ( r2 < m_r_limit_2 )
                    {
                        UInt16 dp = (float)m_dp * ( 1.0f - r2 / m_r_limit_2 );

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
