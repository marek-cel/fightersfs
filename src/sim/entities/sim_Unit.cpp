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

    _affiliation ( affiliation ),

    _ap ( 0 ),
    _hp ( 100 ),

    _radius  ( 0.0 ),
    _radius2 ( 0.0 ),

    _ownship ( false )
{}

////////////////////////////////////////////////////////////////////////////////

Unit::~Unit()
{
    if ( _ownship )
    {
        Ownship::instance()->reportDestroyed();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::activate()
{
    if ( _state == Standby )
    {
        Statistics::instance()->reportActivated( _affiliation );

        setState( Active );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::collide( Unit *unit )
{
    unit->damage( 180 * _ap );
}

////////////////////////////////////////////////////////////////////////////////

void Unit::damage( UInt16 dp )
{
    if ( _ap > 0 )
    {
        dp = (float)dp / (float)_ap;
    }
    else
    {
        dp = _hp;
    }

    if ( dp < _hp )
    {
        setHP( _hp - dp );
    }
    else
    {
        setHP( 0 );
    }

    if ( _hp == 0 )
    {
        if ( isActive() ) destroy();
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::destroy()
{
    if ( _state == Active )
    {
        Statistics::instance()->reportDestroyed( _affiliation );

        _hp = 0;
        setState( Inactive );

        if ( _ownship )
        {
            Ownship::instance()->reportDestroyed();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::hit( UInt16 dp, const Munition *munition )
{
    damage( dp );

    if ( _ownship )
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

    if ( _model.valid() )
    {
        _switch->removeChild( _model.get() );
    }

    _model = Models::get( getPath( _modelFile ) );

    if ( _model.valid() )
    {
        _switch->addChild( _model.get() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Unit::makeAutomatic()
{
    _switch->setName( _name.c_str() );
    _ownship = false;
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

    ////////////////
    _ownship = true;
    ////////////////
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
                    _modelFile = model;
                    _ap = (UInt16)ap;
                    _radius = radius;
                    _radius2 = _radius * _radius;

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
    if ( _ownship )
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
        if ( _ownship && Data::get()->camera.type == ViewPilot )
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
    _ap = ap;
}

////////////////////////////////////////////////////////////////////////////////

void Unit::setHP( UInt16 hp )
{
    _hp = hp;

    if ( _hp > 100 ) _hp = 100;
}

////////////////////////////////////////////////////////////////////////////////

void Unit::setRadius( float radius )
{
    _radius = radius;
    _radius2 = _radius * _radius;
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

                if      ( 0 == String::icompare( name, "gunner"       ) )
                {
                    gunner = new Gunner      ( _affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_flak"  ) )
                {
                    gunner = new GunnerFlak  ( _affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_back"  ) )
                {
                    gunner = new GunnerBack  ( _affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_front" ) )
                {
                    gunner = new GunnerFront ( _affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_left"  ) )
                {
                    gunner = new GunnerLeft  ( _affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_rear"  ) )
                {
                    gunner = new GunnerRear  ( _affiliation, this );
                }
                else if ( 0 == String::icompare( name, "gunner_right"  ) )
                {
                    gunner = new GunnerRight ( _affiliation, this );
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

                if ( r.length2() < _radius2 + unit->getRadius2() )
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
