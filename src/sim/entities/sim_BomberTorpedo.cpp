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

#include <sim/entities/sim_BomberTorpedo.h>

#include <sim/entities/sim_UnitMarine.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const std::string BomberTorpedo::_tagName = "bomber_torpedo";

const float BomberTorpedo::_dropAltMin  = 40.0f;
const float BomberTorpedo::_dropAltMax  = 60.0f;
const float BomberTorpedo::_dropDistMax = 600.0f;
const float BomberTorpedo::_tanMaxTht   = tan( SIM_AIRCRAFT_MAX_THT );

////////////////////////////////////////////////////////////////////////////////

BomberTorpedo::BomberTorpedo( Affiliation affiliation ) :
    Bomber( affiliation ),

    _attack ( false )
{}

////////////////////////////////////////////////////////////////////////////////

BomberTorpedo::~BomberTorpedo() {}

////////////////////////////////////////////////////////////////////////////////

int BomberTorpedo::read( const XmlNode &node )
{
    //////////////////////////////////
    int result = Bomber::read( node );
    //////////////////////////////////

    if ( result == SIM_SUCCESS )
    {
        XmlNode nodeData = node.getFirstChildElement( "bomber_torpedo" );
        readOrdnance( nodeData );

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void BomberTorpedo::limitTht( float &tht )
{
    if ( _attack )
    {
        if ( _altitude_agl < _dropAltMin )
        {
            tht += ( 1.0 - _altitude_agl / _dropAltMin ) * SIM_AIRCRAFT_MAX_THT;
        }

        if ( tht < -SIM_AIRCRAFT_MAX_THT ) tht = -SIM_AIRCRAFT_MAX_THT;
        if ( tht >  SIM_AIRCRAFT_MAX_THT ) tht =  SIM_AIRCRAFT_MAX_THT;
    }
    else
    {
        ////////////////////////
        Bomber::limitTht( tht );
        ////////////////////////
    }
}

////////////////////////////////////////////////////////////////////////////////

void BomberTorpedo::updateTarget()
{
    ///////////////////////
    Bomber::updateTarget();
    ///////////////////////

    _attack = false;

    if ( _engaged )
    {
        float dist_attack = _target_dist - _dropDistMax;
        float dist_descent = _target_alt / _tanMaxTht;

        if ( dist_attack < 2.0f * dist_descent )
        {
            UnitMarine *target = dynamic_cast< UnitMarine* >( _target->getTarget() );

            if ( target )
            {
                _attack = true;

                Vec3 target_dir = _target_pos - _pos;
                target_dir.z() = 0.0f;
                target_dir *= 1.0/target_dir.length();

                Vec3 drop_pos = _pos + target_dir * dist_attack;

                _destination.first  = Vec3( drop_pos.x(), drop_pos.y(), _dropAltMin );
                _destination.second = _speed_max;
            }
            else
            {
                _target->setTarget( 0 );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void BomberTorpedo::updateWeapons()
{
    ////////////////////////
    Bomber::updateWeapons();
    ////////////////////////

    if ( _engaged )
    {
        if ( _target_alt < _dropAltMax
         && _target_dist < _dropDistMax
         && fabs( _target_psi ) < 0.05f )
        {
            _trigger = true;
        }
    }
    else
    {
        _trigger = false;
    }

    if ( _trigger && _ordnanceIndex < _ordnance.size() )
    {
        if ( _time_drop > 1.0f )
        {
            releaseWeapon();
        }
    }
}
