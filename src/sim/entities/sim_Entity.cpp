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

#include <sim/entities/sim_Entity.h>

#include <sim/entities/sim_Unit.h>
#include <sim/entities/sim_Entities.h>

#include <sim/utils/sim_Misc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

std::vector< UInt32 > Entity::_ids;

////////////////////////////////////////////////////////////////////////////////

UInt32 Entity::createId()
{
    if ( _ids.size() > 0 )
    {
        for ( UInt32 id = 0; id < SIM_ENTITIES_MAX; id++ )
        {
            bool exists = false;

            std::vector< UInt32 >::iterator it = _ids.begin();

            while ( it != _ids.end() )
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
                _ids.push_back( id );
                return id;
            }
        }
    }
    else
    {
        _ids.push_back( 0 );
        return 0;
    }

    return std::numeric_limits< UInt32 >::quiet_NaN();
}

////////////////////////////////////////////////////////////////////////////////

void Entity::removeId( UInt32 id )
{
    std::vector< UInt32 >::iterator it = _ids.begin();

    while ( it != _ids.end() )
    {
        if ( id == (*it) )
        {
            _ids.erase( it );
            return;
        }

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

Entity::Entity( Group *parent, State state, float life_span ) :
    Group( parent ),

    _id ( createId() ),

    _state ( state ),
    _active ( true ),

    _life_time ( 0.0f ),
    _life_span ( life_span )
{
    _pat = new osg::PositionAttitudeTransform();
    _root->addChild( _pat.get() );

    _switch = new osg::Switch();
    _pat->addChild( _switch.get() );

    _switch->setName( "entity" );

    _att.zeroRotation();

    if ( _parent == 0 )
    {
        _parent = Entities::instance();
    }

    setParent( _parent );
}

////////////////////////////////////////////////////////////////////////////////

Entity::~Entity()
{
    if ( _parentGroup.valid() )
    {
        _parentGroup->removeChild( _root.get() );
    }

    ////////////////
    removeId( _id );
    ////////////////
}

////////////////////////////////////////////////////////////////////////////////

void Entity::makeTopLevel()
{
    if ( _parent )
    {
        Entity *entity = dynamic_cast< Entity* >( _parent );

        _pos = getAbsPos();
        _att = getAbsAtt();

        if ( entity )
        {
            _vel = entity->getVel();
        }

        setParent( Entities::instance() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Entity::resetLifeTime()
{
    _life_time = 0.0f;
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

    _timeStep = timeStep;

    if ( !Misc::isValid( _id ) || _life_time > _life_span )
    {
        setState( Inactive );
    }

    if ( isActive() )
    {
        _life_time += _timeStep;

        updateElevation();
        updateVelocity();
        timeIntegration();
        updateVariables();

        _pat->setPosition( _pos );
        _pat->setAttitude( _att );
    }
}

////////////////////////////////////////////////////////////////////////////////

Vec3 Entity::getAbsPos() const
{
    if ( !isTopLevel() )
    {
        Entity *entity = dynamic_cast< Entity* >( _parent );

        if ( entity )
        {
            return entity->getAbsPos() + entity->getAbsAtt() * _pos;
        }
    }

    return _pos;
}

////////////////////////////////////////////////////////////////////////////////

Quat Entity::getAbsAtt() const
{
    if ( !isTopLevel() )
    {
        Entity *entity = dynamic_cast< Entity* >( _parent );

        if ( entity )
        {
            Quat att = _att * entity->getAbsAtt();
            att *= 1.0 / att.length();
            return att;
        }
    }

    return _att;
}

////////////////////////////////////////////////////////////////////////////////

bool Entity::isTopLevel() const
{
    return ( _parent == Entities::instance() );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setPos( const Vec3 &pos )
{
    _pos = pos;
    updateVariables();
    _pat->setPosition( _pos );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setAtt( const Quat &att )
{
    _att = att;
    _att *= 1.0/_att.length();

    updateVariables();
    _pat->setAttitude( _att );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setVel( const Vec3 &vel )
{
    _vel = vel;
    updateVariables();
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setOmg( const Vec3 &omg )
{
    _omg = omg;
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
    _name = name;
    _switch->setName( _name.c_str() );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setParent( Group *parent )
{
    if ( _parentGroup.valid() )
    {
        _parentGroup->removeChild( _root.get() );
        _parentGroup = 0;
    }

    if ( _parent )
    {
        _parent->dettachEntity( this );
        _parent = 0;
    }

    if ( parent )
    {
        _parent = parent;

        parent->attachEntity( this );
        _parent = parent;

        Entity *entity = dynamic_cast< Entity* >( _parent );

        if ( entity )
        {
            _parentGroup = entity->_switch;
        }
        else
        {
            _parentGroup = _parent->getNode();
        }

        if ( _parentGroup.valid() ) _parentGroup->addChild( _root.get() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Entity::setState( State state )
{
    _state = state;

    if ( _state == Active )
    {
        _active = true;
        _switch->setAllChildrenOn();
    }
    else
    {
        _active = false;
        _switch->setAllChildrenOff();
    }

    List::iterator it = _children.begin();

    while ( it != _children.end() )
    {
        (*it)->setState( _state );
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
    _pos += derivPos( _att, _vel ) * _timeStep;
    _att = _att + derivAtt( _att, _omg ) * _timeStep;

    // normalize
    _att *= 1.0 / _att.length();
}

////////////////////////////////////////////////////////////////////////////////

void Entity::updateElevation() {}

////////////////////////////////////////////////////////////////////////////////

void Entity::updateVariables()
{
    _angles.set( _att );
}

////////////////////////////////////////////////////////////////////////////////

void Entity::updateVelocity() {}
