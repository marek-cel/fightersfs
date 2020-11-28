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

#include <sim/entities/sim_Unit.h>

#include <sim/cgi/sim_Models.h>

#include <sim/entities/sim_Entities.h>
#include <sim/entities/sim_Gunner.h>
#include <sim/entities/sim_Munition.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlUtils.h>

#include <sim/sim_Ownship.h>
#include <sim/sim_Statistics.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Unit::Unit( Affiliation affiliation ) :
    Entity(),

    m_affiliation ( affiliation ),

    m_ap ( 0 ),
    m_hp ( 100 ),

    m_radius  ( 0.0 ),
    m_radius2 ( 0.0 ),

    m_ownship ( false )
{}

////////////////////////////////////////////////////////////////////////////////

Unit::~Unit()
{
    if ( m_ownship )
    {
        Ownship::instance()->reportDestroyed();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::activate()
{
    if ( _state == Standby )
    {
        Statistics::instance()->reportActivated( m_affiliation );

        setState( Active );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::collide( Unit *unit )
{
    unit->damage( 180 * m_ap );
}

////////////////////////////////////////////////////////////////////////////////

void Unit::damage( UInt16 dp )
{
    if ( m_ap > 0 )
    {
        dp = (float)dp / (float)m_ap;
    }
    else
    {
        dp = m_hp;
    }

    if ( dp < m_hp )
    {
        setHP( m_hp - dp );
    }
    else
    {
        setHP( 0 );
    }

    if ( m_hp == 0 )
    {
        if ( isActive() ) destroy();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::destroy()
{
    if ( _state == Active )
    {
        Statistics::instance()->reportDestroyed( m_affiliation );

        m_hp = 0;
        setState( Inactive );

        if ( m_ownship )
        {
            Ownship::instance()->reportDestroyed();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::hit( UInt16 dp, const Munition *munition )
{
    damage( dp );

    if ( m_ownship )
    {
        Ownship::instance()->reportHit( munition );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::load()
{
    ///////////////
    Entity::load();
    ///////////////

    if ( m_model.valid() )
    {
        _switch->removeChild( m_model.get() );
    }

    m_model = Models::get( getPath( m_modelFile ) );

    if ( m_model.valid() )
    {
        _switch->addChild( m_model.get() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::makeAutomatic()
{
    _switch->setName( _name.c_str() );
    m_ownship = false;
}

////////////////////////////////////////////////////////////////////////////////

void Unit::makeOwnship()
{
    Unit *ownship = Entities::instance()->getOwnship();

    if ( ownship )
    {
        ownship->makeAutomatic();
    }

    activate();

    _switch->setName( "ownship" );

    /////////////////
    m_ownship = true;
    /////////////////
}

////////////////////////////////////////////////////////////////////////////////

int Unit::read( const XmlNode &node )
{
    if ( node.isValid() )
    {
        if ( 0 == String::icompare( node.getName(), "unit" ) )
        {
            XmlNode nodeModel  = node.getFirstChildElement( "model" );
            XmlNode nodeAp     = node.getFirstChildElement( "ap" );
            XmlNode nodeRadius = node.getFirstChildElement( "radius" );

            if ( nodeModel.isValid() && nodeAp.isValid() && nodeRadius.isValid() )
            {
                int result = SIM_SUCCESS;

                std::string model = "";
                int ap = 0;
                float radius = 0.0;

                if ( result == SIM_SUCCESS ) result = XmlUtils::read( nodeModel, model );
                if ( result == SIM_SUCCESS ) result = XmlUtils::read( nodeAp, ap );
                if ( result == SIM_SUCCESS ) result = XmlUtils::read( nodeRadius, radius );

                if ( result == SIM_SUCCESS )
                {
                    m_modelFile = model;
                    m_ap = (UInt16)ap;
                    m_radius = radius;
                    m_radius2 = m_radius * m_radius;

                    readGunners( node );
                }

                return result;
            }
        }
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void Unit::reportTargetHit( Unit *target )
{
    if ( m_ownship )
    {
        if ( target->getHP() > 0 )
            Ownship::instance()->reportTargetHit( target );
        else
            Ownship::instance()->reportTargetKill( target );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::update( double timeStep )
{
    ///////////////////////////
    Entity::update( timeStep );
    ///////////////////////////

    if ( isActive() )
    {
        if ( m_ownship && Data::get()->camera.type == ViewPilot )
        {
            _switch->setAllChildrenOff();
        }
        else
        {
            _switch->setAllChildrenOn();
        }

        updateCollisions();
        updateWeapons();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::setAP( UInt16 ap )
{
    m_ap = ap;
}

////////////////////////////////////////////////////////////////////////////////

void Unit::setHP( UInt16 hp )
{
    m_hp = hp;

    if ( m_hp > 100 ) m_hp = 100;
}

////////////////////////////////////////////////////////////////////////////////

void Unit::setRadius( float radius )
{
    m_radius = radius;
    m_radius2 = m_radius * m_radius;
}

////////////////////////////////////////////////////////////////////////////////

void Unit::readGunners( const XmlNode &node )
{
    XmlNode nodeGunners = node.getFirstChildElement( "gunners" );

    if ( nodeGunners.isValid() )
    {
        XmlNode nodeGunner = nodeGunners.getFirstChildElement();

        Vec3 position;

        while ( nodeGunner.isValid() )
        {
            if ( SIM_SUCCESS == XmlUtils::read( nodeGunner, position ) )
            {
                Gunner *gunner = 0;

                std::string name = nodeGunner.getName();

                if ( 0 == String::icompare( name, "gunner" ) )
                {
                    gunner = new Gunner( m_affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_flak" ) )
                {
                    gunner = new GunnerFlak( m_affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_back" ) )
                {
                    gunner = new GunnerBack( m_affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_front" ) )
                {
                    gunner = new GunnerFront( m_affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_left" ) )
                {
                    gunner = new GunnerLeft( m_affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_rear" ) )
                {
                    gunner = new GunnerRear( m_affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_right" ) )
                {
                    gunner = new GunnerRight( m_affiliation, this );
                }

                if ( gunner )
                {
                    gunner->setPos( position );
                }
            }

            nodeGunner = nodeGunner.getNextSiblingElement();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::updateCollisions()
{
    List *entities = Entities::instance()->getEntities();
    List::iterator it = entities->begin();

    Vec3 r;

    while ( it != entities->end() )
    {
        Unit *unit = dynamic_cast< Unit* >(*it);

        if ( unit )
        {
            if ( _id != unit->getId() && unit->isActive() )
            {
                r = unit->getPos() - _pos;

                if ( r.length2() < m_radius2 + unit->getRadius2() )
                {
                    unit->collide( this );
                    collide( unit );
                    break;
                }
            }
        }

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::updateWeapons() {}
