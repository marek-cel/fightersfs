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

#include <sim/utils/sim_Angles.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Angles::Angles()
{
    _phi = 0.0f;
    _tht = 0.0f;
    _psi = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

Angles::Angles( const Angles &angl )
{
    set( angl._phi, angl._tht, angl._psi );
}

////////////////////////////////////////////////////////////////////////////////

Angles::Angles( double phi, double tht, double psi )
{
    set( phi, tht, psi );
}

////////////////////////////////////////////////////////////////////////////////

Angles::Angles( const osg::Quat &quat )
{
    set( quat );
}

////////////////////////////////////////////////////////////////////////////////

void Angles::normalize()
{
    while ( _tht >  M_PI_2 )
    {
        _phi += M_PI;
        _tht =  M_PI_2 - ( _tht - M_PI_2 );
        _psi += M_PI;
    }

    while ( _tht < -M_PI_2 )
    {
        _phi += M_PI;
        _tht = -M_PI_2 - ( _tht + M_PI_2 );
        _psi += M_PI;
    }

    while ( _phi >  M_PI ) _phi -= 2.0 * M_PI;
    while ( _phi < -M_PI ) _phi += 2.0 * M_PI;

    while ( _psi > 2.0 * M_PI ) _psi -= 2.0 * M_PI;
    while ( _psi < 0.0        ) _psi += 2.0 * M_PI;
}

////////////////////////////////////////////////////////////////////////////////

osg::Quat Angles::getRotate() const
{
    osg::Quat result;

    double phi2 = _phi / 2.0f;
    double tht2 = _tht / 2.0f;
    double psi2 = _psi / 2.0f;

    double sinPhi2 = sin( phi2 );
    double cosPhi2 = cos( phi2 );

    double sinTht2 = sin( tht2 );
    double cosTht2 = cos( tht2 );

    double sinPsi2 = sin( psi2 );
    double cosPsi2 = cos( psi2 );

    double cosPhi2CosPsi2 = cosPhi2 * cosPsi2;
    double cosPhi2SinPsi2 = cosPhi2 * sinPsi2;
    double sinPhi2SinPsi2 = sinPhi2 * sinPsi2;
    double sinPhi2CosPsi2 = sinPhi2 * cosPsi2;

    result.w() = ( cosPhi2CosPsi2 * cosTht2 ) + ( sinPhi2SinPsi2 * sinTht2 );
    result.x() = ( sinPhi2CosPsi2 * cosTht2 ) - ( cosPhi2SinPsi2 * sinTht2 );
    result.y() = ( cosPhi2CosPsi2 * sinTht2 ) + ( sinPhi2SinPsi2 * cosTht2 );
    result.z() = ( cosPhi2SinPsi2 * cosTht2 ) - ( sinPhi2CosPsi2 * sinTht2 );

    result *= 1.0/result.length();

    return result;
}

////////////////////////////////////////////////////////////////////////////////

void Angles::set( double phi, double tht, double psi )
{
    _phi = phi;
    _tht = tht;
    _psi = psi;
}

////////////////////////////////////////////////////////////////////////////////

void Angles::set( const osg::Quat &quat )
{
    double sinTht2 = quat.w()*quat.y() - quat.x()*quat.z();

    if( sinTht2 >= 0.5f )
    {
        _phi =  2.0f * asin( quat.x() / cos( M_PI_4 ) );
        _tht =  M_PI_2;
        _psi =  0.0f;
    }
    else if ( sinTht2 <= -0.5f )
    {
        _phi =  2.0f * asin( quat.x() / cos( M_PI_4 ) );
        _tht = -M_PI_2;
        _psi =  0.0f;
    }
    else
    {
        _phi = atan2( 2.0f*( quat.w()*quat.x() + quat.y()*quat.z() ),
               1.0f - 2.0f*( quat.x()*quat.x() + quat.y()*quat.y() ) );
        _tht =  asin( 2.0f*sinTht2 );
        _psi = atan2( 2.0f*( quat.w()*quat.z() + quat.x()*quat.y() ),
               1.0f - 2.0f*( quat.y()*quat.y() + quat.z()*quat.z() ) );
    }

    normalize();
}

////////////////////////////////////////////////////////////////////////////////

Angles& Angles::operator= ( const Angles &angl )
{
    set( angl._phi, angl._tht, angl._psi );

    return (*this);
}

////////////////////////////////////////////////////////////////////////////////

bool Angles::operator== ( const Angles &angl ) const
{
    return ( ( _phi == angl._phi )
          && ( _tht == angl._tht )
          && ( _psi == angl._psi ) );
}

////////////////////////////////////////////////////////////////////////////////

bool Angles::operator!= ( const Angles &angl ) const
{
    return !( (*this) == angl );
}
