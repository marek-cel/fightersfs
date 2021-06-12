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
