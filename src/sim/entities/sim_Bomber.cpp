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

#include <sim/entities/sim_Bomber.h>

#include <sim/entities/sim_Bomb.h>
#include <sim/entities/sim_Torpedo.h>

#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Bomber::Bomber( Affiliation affiliation ) :
    Aircraft( affiliation ),

    _target ( 0 ),

    _ordnanceIndex ( 0 ),

    _target_dist ( 0.0f ),
    _target_alt  ( 0.0f ),
    _target_tht  ( 0.0f ),
    _target_psi  ( 0.0f ),

    _engaged ( false ),
    _trigger ( false )
{
    _target = new Target< UnitSurface >( this, ( _affiliation == Hostile ) ? Friend : Hostile );
}

////////////////////////////////////////////////////////////////////////////////

Bomber::~Bomber()
{
    DELPTR( _target );
}

////////////////////////////////////////////////////////////////////////////////

void Bomber::update( double timeStep )
{
    if ( isActive() ) _target->update();

    /////////////////////////////
    Aircraft::update( timeStep );
    /////////////////////////////

    if ( isActive() )
    {
        if ( !_target->getTarget() )
        {
            _target->findForward( M_PI_2 );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

UnitSurface* Bomber::getTarget() const
{
    return _target->getTarget();
}

////////////////////////////////////////////////////////////////////////////////

void Bomber::readOrdnance( const XmlNode &node )
{
    if ( node.isValid() )
    {
        XmlNode nodeOrdnance = node.getFirstChildElement( "bomb" );

        while ( nodeOrdnance.isValid() )
        {
            Bomb *bomb = new Bomb( _id, this );

            readOrdnance( nodeOrdnance, bomb );
            _ordnance.push_back( bomb );

            nodeOrdnance = nodeOrdnance.getNextSiblingElement( "bomb" );
        }

        nodeOrdnance = node.getFirstChildElement( "torpedo" );

        while ( nodeOrdnance.isValid() )
        {
            Torpedo *torpedo = new Torpedo( _id, this );

            readOrdnance( nodeOrdnance, torpedo );
            _ordnance.push_back( torpedo );

            nodeOrdnance = nodeOrdnance.getNextSiblingElement( "torpedo" );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Bomber::readOrdnance( const XmlNode &node, Munition *ordnance )
{
    if ( node.isValid() && ordnance )
    {
        if ( String::toBool( node.getAttribute( "hidden" ) ) )
        {
            ordnance->makeTopLevel();
            ordnance->setState( Standby );
        }

        ordnance->setModelFile( getPath( node.getAttribute( "model" ) ) );

        XmlNode nodePosition = node.getFirstChildElement( "position" );
        XmlNode nodeAttitude = node.getFirstChildElement( "attitude" );

        if ( nodePosition.isValid() )
        {
            Vec3 position;

            if ( SIM_SUCCESS == XmlUtils::read( nodePosition, position ) )
            {
                ordnance->setPos( position );
            }
        }

        if ( nodeAttitude.isValid() )
        {
            Vec3 attitude;

            if ( SIM_SUCCESS == XmlUtils::read( nodeAttitude, attitude ) )
            {
                Quat quat( Convert::deg2rad( attitude.x() ), osg::X_AXIS,
                           Convert::deg2rad( attitude.y() ), osg::Y_AXIS,
                           Convert::deg2rad( attitude.z() ), osg::Z_AXIS );

                ordnance->setAtt( quat );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Bomber::releaseWeapon()
{
    _time_drop = 0.0f;

    _ordnance[ _ordnanceIndex ]->setParent( this );
    _ordnance[ _ordnanceIndex ]->setState( Active );
    _ordnance[ _ordnanceIndex ]->makeTopLevel();

    _ordnanceIndex++;
}

////////////////////////////////////////////////////////////////////////////////

void Bomber::updateDestination()
{
    updateTarget();

    if ( !_engaged )
    {
        if ( _route.size() > 0 )
        {
            if ( !_enroute || _engaged )
            {
                _destination = _route[ _waypointIndex ];
                _destValid = true;
            }
        }
        else
        {
            _destValid = false;
        }

        _enroute = true;
        _wingman = _leaderValid;
        _engaged = false;
    }

    //////////////////////////////
    Aircraft::updateDestination();
    //////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void Bomber::updateTarget()
{
    UnitSurface *target = _target->getTarget();

    if ( target && _ordnanceIndex < _ordnance.size() )
    {
        _enroute = false;
        _wingman = false;
        _engaged = true;

        _destValid = true;

        _target_pos = target->getPos();

        Vec3 dist_enu = _target_pos - _pos;
        _target_dist = sqrt( dist_enu.x()*dist_enu.x() + dist_enu.y()*dist_enu.y() );
        _target_alt  = -dist_enu.z();

        Vec3 dir_bas = _att.inverse() * ( dist_enu * ( 1.0/dist_enu.length() ) );
        dir_bas *= 1.0/dir_bas.length();

        _target_psi = atan2( -dir_bas.y(), -dir_bas.x() );
        _target_tht = atan2( dir_bas.z(), sqrt( dir_bas.x()*dir_bas.x() + dir_bas.y()*dir_bas.y() ) );

        // mentain level flight
        _destination.first  = Vec3( _target_pos.x(), _target_pos.y(), _altitude_asl );
        _destination.second = _speed_max;
    }
    else
    {
        _engaged = false;
    }
}
