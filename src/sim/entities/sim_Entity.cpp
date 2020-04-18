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

#include <sim/entities/sim_Entity.h>

#include <sim/entities/sim_Unit.h>
#include <sim/entities/sim_Entities.h>

#include <sim/utils/sim_Misc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

std::vector< UInt32 > Entity::m_ids;

////////////////////////////////////////////////////////////////////////////////

UInt32 Entity::createId()
{
    if ( m_ids.size() > 0 )
    {
        for ( UInt32 id = 0; id < SIM_ENTITIES_MAX; id++ )
        {
            bool exists = false;

            std::vector< UInt32 >::iterator it = m_ids.begin();

            while ( it != m_ids.end() )
            {
                if ( id == (*it) )
                {
                    exists = true;
                    break;
                }

                ++it;
            }

            if ( !exists )
            {
                m_ids.push_back( id );
                return id;
            }
        }
    }
    else
    {
        m_ids.push_back( 0 );
        return 0;
    }

    return std::numeric_limits< UInt32 >::quiet_NaN();
}

////////////////////////////////////////////////////////////////////////////////

void Entity::removeId( UInt32 id )
{
    std::vector< UInt32 >::iterator it = m_ids.begin();

    while ( it != m_ids.end() )
    {
        if ( id == (*it) )
        {
            m_ids.erase( it );
            return;
        }

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

Entity::Entity( Group *parent, State state, float life_span ) :
    Group( parent ),

    m_id ( createId() ),

    m_state ( state ),
    m_active ( true ),

    m_life_time ( 0.0f ),
    m_life_span ( life_span )
{
    m_pat = new osg::PositionAttitudeTransform();
    m_root->addChild( m_pat.get() );

    m_switch = new osg::Switch();
    m_pat->addChild( m_switch.get() );

    m_switch->setName( "entity" );

    m_att.zeroRotation();

    if ( m_parent == 0 )
    {
        m_parent = Entities::instance();
    }

    setParent( m_parent );
}

////////////////////////////////////////////////////////////////////////////////

Entity::~Entity()
{
    if ( m_parentGroup.valid() )
    {
        m_parentGroup->removeChild( m_root.get() );
    }

    /////////////////
    removeId( m_id );
    /////////////////
}

////////////////////////////////////////////////////////////////////////////////

void Entity::makeTopLevel()
{
    if ( m_parent )
    {
        Entity *entity = dynamic_cast< Entity* >( m_parent );

        m_pos = getAbsPos();
        m_att = getAbsAtt();

        if ( entity )
        {
            m_vel = entity->getVel();
        }

        setParent( Entities::instance() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Entity::resetLifeTime()
{
    m_life_time = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

void Entity::load()
{
    //////////////
    Group::load();
    //////////////
}

////////////////////////////////////////////////////////////////////////////////

void Entity::update( double timeStep )
{
    //////////////////////////
    Group::update( timeStep );
    //////////////////////////

    m_timeStep = timeStep;

    if ( !Misc::isValid( m_id ) || m_life_time > m_life_span )
    {
        setState( Inactive );
    }

    if ( isActive() )
    {
        m_life_time += m_timeStep;

        updateElevation();
        updateVelocity();
        timeIntegration();
        updateVariables();

        m_pat->setPosition( m_pos );
        m_pat->setAttitude( m_att );
    }
}

////////////////////////////////////////////////////////////////////////////////

Vec3 Entity::getAbsPos() const
{
    if ( !isTopLevel() )
    {
        Entity *entity = dynamic_cast< Entity* >( m_parent );

        if ( entity )
        {
            return entity->getAbsPos() + entity->getAbsAtt() * m_pos;
        }
    }

    return m_pos;
}

////////////////////////////////////////////////////////////////////////////////

Quat Entity::getAbsAtt() const
{
    if ( !isTopLevel() )
    {
        Entity *entity = dynamic_cast< Entity* >( m_parent );

        if ( entity )
        {
            Quat att = m_att * entity->getAbsAtt();
            att *= 1.0 / att.length();
            return att;
        }
    }

    return m_att;
}

////////////////////////////////////////////////////////////////////////////////

bool Entity::isTopLevel() const
{
    return ( m_parent == Entities::instance() );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setPos( const Vec3 &pos )
{
    m_pos = pos;
    updateVariables();
    m_pat->setPosition( m_pos );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setAtt( const Quat &att )
{
    m_att = att;
    m_att *= 1.0/m_att.length();

    updateVariables();
    m_pat->setAttitude( m_att );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setVel( const Vec3 &vel )
{
    m_vel = vel;
    updateVariables();
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setOmg( const Vec3 &omg )
{
    m_omg = omg;
    updateVariables();
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setHeading( float heading )
{
    float psi = -M_PI_2 - heading;

    setAtt( Quat( psi, osg::Z_AXIS ) );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setName( const std::string &name )
{
    m_name = name;
    m_switch->setName( m_name.c_str() );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setParent( Group *parent )
{
    if ( m_parentGroup.valid() )
    {
        m_parentGroup->removeChild( m_root.get() );
        m_parentGroup = 0;
    }

    if ( m_parent )
    {
        m_parent->dettachEntity( this );
        m_parent = 0;
    }

    if ( parent )
    {
        m_parent = parent;

        parent->attachEntity( this );
        m_parent = parent;

        Entity *entity = dynamic_cast< Entity* >( m_parent );

        if ( entity )
        {
            m_parentGroup = entity->m_switch;
        }
        else
        {
            m_parentGroup = m_parent->getNode();
        }

        if ( m_parentGroup.valid() ) m_parentGroup->addChild( m_root.get() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setState( State state )
{
    m_state = state;

    if ( m_state == Active )
    {
        m_active = true;
        m_switch->setAllChildrenOn();
    }
    else
    {
        m_active = false;
        m_switch->setAllChildrenOff();
    }

    List::iterator it = m_children.begin();

    while ( it != m_children.end() )
    {
        (*it)->setState( m_state );
        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setVelocity( float velocity )
{
    setVel( Vec3( -velocity, 0.0, 0.0 ) );
}

////////////////////////////////////////////////////////////////////////////////

Vec3 Entity::derivPos( const Quat &att, const Vec3 &vel )
{
    return att * vel;
}

////////////////////////////////////////////////////////////////////////////////

Quat Entity::derivAtt( const Quat &att, const Vec3 &omg )
{
    osg::Quat deriv;

    deriv.w() = -0.5 * ( att.z()*omg.z() + att.y()*omg.y() + att.x()*omg.x() );
    deriv.x() = -0.5 * ( att.z()*omg.y() - att.y()*omg.z() - att.w()*omg.x() );
    deriv.y() = -0.5 * ( att.x()*omg.z() - att.w()*omg.y() - att.z()*omg.x() );
    deriv.z() = -0.5 * ( att.y()*omg.x() - att.w()*omg.z() - att.x()*omg.y() );

    return deriv;
}

////////////////////////////////////////////////////////////////////////////////

void Entity::timeIntegration()
{
    m_pos += derivPos( m_att, m_vel ) * m_timeStep;
    m_att = m_att + derivAtt( m_att, m_omg ) * m_timeStep;

    // normalize
    m_att *= 1.0 / m_att.length();
}

////////////////////////////////////////////////////////////////////////////////

void Entity::updateElevation() {}

////////////////////////////////////////////////////////////////////////////////

void Entity::updateVariables()
{
    m_angles.set( m_att );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::updateVelocity() {}
