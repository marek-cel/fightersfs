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

#include <sim/cgi/sim_Gates.h>

#include <osg/Billboard>
#include <osg/Geode>
#include <osg/LineWidth>

#include <sim/cgi/sim_Color.h>
#include <sim/utils/sim_Convert.h>
#include <sim/sim_Ownship.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const float Gates::_distMax = 0.9 * SIM_SKYDOME_RAD;
const float Gates::_distScale = 2000.0f;
const float Gates::_size = 2.0f * Gates::_distScale * tan( Convert::deg2rad( 10.0f * 90.0f / 1200.0f ) );


////////////////////////////////////////////////////////////////////////////////

Gates::Gates( float linesWidth, Module *parent ) :
    Module( new osg::Switch(), parent ),

    _linesWidth ( linesWidth )
{
    _switch = dynamic_cast<osg::Switch*>( _root.get() );

    _pat0 = new osg::PositionAttitudeTransform();
    _pat1 = new osg::PositionAttitudeTransform();
    _pat2 = new osg::PositionAttitudeTransform();

    _switch->addChild( _pat0.get() );
    _switch->addChild( _pat1.get() );
    _switch->addChild( _pat2.get() );

    _geom0 = createGate( _pat0.get() );
    _geom1 = createGate( _pat1.get() );
    _geom2 = createGate( _pat2.get() );
}

////////////////////////////////////////////////////////////////////////////////

Gates::~Gates() {}

////////////////////////////////////////////////////////////////////////////////

void Gates::update()
{
    /////////////////
    Module::update();
    /////////////////

    if ( Data::get()->mission.status == Pending && !Data::get()->paused
      && ( Data::get()->camera.type == ViewChase || Data::get()->camera.type == ViewPilot ) )
    {
        if ( Data::get()->ownship.waypoint )
        {
            if ( Data::get()->ownship.waypoint_dist < _distMax )
            {
                // current gate
                _switch->setValue( 0, true );

                _pat0->setPosition( Vec3( Data::get()->ownship.waypoint_x,
                                          Data::get()->ownship.waypoint_y,
                                          Data::get()->ownship.waypoint_z ) );

                float coef = Data::get()->ownship.waypoint_dist / _distScale;

                if ( coef > 1.0f )
                {
                    _pat0->setScale( Vec3( coef, coef, coef ) );
                }
                else
                {
                    _pat0->setScale( Vec3( 1.0f, 1.0f, 1.0f ) );
                }

                // following gates
                Aircraft *ownship = Ownship::instance()->getAircraft();

                if ( ownship )
                {
                    coef = updateGate( _geom1, _pat1, ownship, 1, coef );
                    coef = updateGate( _geom2, _pat2, ownship, 2, coef );
                }
                else
                {
                    _switch->setValue( 1, false );
                    _switch->setValue( 2, false );
                }
            }
            else
            {
                _switch->setAllChildrenOff();
            }
        }
        else
        {
            _switch->setAllChildrenOff();
        }
    }
    else
    {
        _switch->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

osg::Geometry* Gates::createGate( osg::Group *parent )
{
    const float w_2 = _size / 2.0f;
    const float h_2 = _size / 2.0f;

    osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard();
    parent->addChild( billboard.get() );

    billboard->setMode( osg::Billboard::AXIAL_ROT );
    billboard->setAxis( osg::Vec3( 0.0f, 0.0f, 1.0f ) );
    billboard->setNormal( osg::Vec3f( 0.0f, 1.0f, 0.0f ) );

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    billboard->addDrawable( geometry.get(), osg::Vec3( 0.0, 0.0, 0.0 ) );

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    v->push_back( Vec3( -w_2, 0.0f,  h_2 ) );
    v->push_back( Vec3( -w_2, 0.0f, -h_2 ) );
    v->push_back( Vec3(  w_2, 0.0f, -h_2 ) );
    v->push_back( Vec3(  w_2, 0.0f,  h_2 ) );

    n->push_back( osg::Vec3( 0.0f, 1.0f, 0.0f ) );

    c->push_back( osg::Vec4( Color::lime, 0.8f ) );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_LOOP, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::StateSet> stateSet = billboard->getOrCreateStateSet();

    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth();
    lineWidth->setWidth( _linesWidth );

    stateSet->setAttributeAndModes( lineWidth, osg::StateAttribute::ON );

    stateSet->setMode( GL_LIGHTING , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
    stateSet->setMode( GL_LIGHT0   , osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
    stateSet->setMode( GL_BLEND    , osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON  );

    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    return geometry.get();
}

////////////////////////////////////////////////////////////////////////////////

float Gates::updateGate( osg::Geometry *geom, osg::PositionAttitudeTransform *pat,
                         Aircraft *ownship, int number, float coef_prev )
{
    float coef = 0.0f;

    UInt32 index = ownship->getWaypointIndex() + number;

    // next waypoint
    if ( ownship->getRoute().size() > index )
    {
        Vec3 pos = ownship->getRoute()[ index ].first;
        coef = ( ownship->getPos() - pos ).length() / _distScale;

        if ( coef_prev <= 1.0f )
        {
            _switch->setValue( number, true );

            pat->setPosition( pos );

            // scaling
            if ( coef > 1.0f )
            {
                pat->setScale( Vec3( coef, coef, coef ) );
            }
            else
            {
                pat->setScale( Vec3( 1.0f, 1.0f, 1.0f ) );
            }

            // fade in
            osg::ref_ptr<osg::Vec4Array> c = dynamic_cast< osg::Vec4Array* >( geom->getColorArray() );

            if ( c.valid() )
            {
                c->clear();
                c->push_back( osg::Vec4( Color::grey, 1.0f * ( 1.0f - coef_prev ) ) );

                geom->setColorArray( c.get() );
            }
        }
        else
        {
            _switch->setValue( number, false );
        }
    }
    else
    {
        _switch->setValue( number, false );
    }

    return coef;
}
