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

#include <sim/entities/sim_BomberDive.h>

#include <sim/utils/sim_Convert.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const std::string BomberDive::_tagName = "bomber_dive";

const float BomberDive::_dropAltMin = 300.0f;
const float BomberDive::_dropAltMax = 400.0f;

////////////////////////////////////////////////////////////////////////////////

BomberDive::BomberDive( Affiliation affiliation ) :
    Bomber( affiliation ),

    _attack ( false )
{}

////////////////////////////////////////////////////////////////////////////////

BomberDive::~BomberDive() {}

////////////////////////////////////////////////////////////////////////////////

int BomberDive::read( const XmlNode &node )
{
    //////////////////////////////////
    int result = Bomber::read( node );
    //////////////////////////////////

    if ( result == SIM_SUCCESS )
    {
        XmlNode nodeData = node.getFirstChildElement( "bomber_dive" );
        readOrdnance( nodeData );

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void BomberDive::limitTht( float &tht )
{
    if ( _attack )
    {
        if ( tht < -_target_tht ) tht = -_target_tht;
        if ( tht >  _target_tht ) tht =  _target_tht;
    }
    else
    {
        ////////////////////////
        Bomber::limitTht( tht );
        ////////////////////////
    }
}

////////////////////////////////////////////////////////////////////////////////

void BomberDive::updateTarget()
{
    ///////////////////////
    Bomber::updateTarget();
    ///////////////////////

    _attack = false;

    if ( _engaged )
    {
        if ( _target_alt > _dropAltMin )
        {
            float max_range = ( _airspeed / SIM_GRAVITY_ACC ) * sqrt( 2.0f*SIM_GRAVITY_ACC*_target_alt );

            if ( _target_dist < 0.8f*max_range )
            {
                _attack = true;

                float v_2 = _airspeed * _airspeed;
                float num = v_2 - sqrt( v_2*v_2 - SIM_GRAVITY_ACC*(SIM_GRAVITY_ACC*_target_dist*_target_dist - 2.0f*_target_alt*v_2) );
                float den = SIM_GRAVITY_ACC * _target_dist;

                _target_tht = atan2( num, den );


                _destination.first  = _target_pos;
                _destination.second = _speed_min;
            }
        }
        else
        {
            _engaged = false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void BomberDive::updateWeapons()
{
    ////////////////////////
    Bomber::updateWeapons();
    ////////////////////////

    if ( _engaged )
    {
        if ( _target_alt < _dropAltMax  )
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
        if ( _time_drop > 0.5f )
        {
            releaseWeapon();
        }
    }
}
