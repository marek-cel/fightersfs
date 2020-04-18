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

const float Gates::m_distMax = 0.9 * SIM_SKYDOME_RAD;
const float Gates::m_distScale = 2000.0f;
const float Gates::m_size = 2.0f * Gates::m_distScale * tan( Convert::deg2rad( 10.0f * 90.0f / 1200.0f ) );


////////////////////////////////////////////////////////////////////////////////

Gates::Gates( float linesWidth, Module *parent ) :
    Module( new osg::Switch(), parent ),

    m_linesWidth ( linesWidth )
{
    m_switch = dynamic_cast<osg::Switch*>( m_root.get() );

    m_pat0 = new osg::PositionAttitudeTransform();
    m_pat1 = new osg::PositionAttitudeTransform();
    m_pat2 = new osg::PositionAttitudeTransform();

    m_switch->addChild( m_pat0.get() );
    m_switch->addChild( m_pat1.get() );
    m_switch->addChild( m_pat2.get() );

    m_geom0 = createGate( m_pat0.get() );
    m_geom1 = createGate( m_pat1.get() );
    m_geom2 = createGate( m_pat2.get() );
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
            if ( Data::get()->ownship.waypoint_dist < m_distMax )
            {
                // current gate
                m_switch->setValue( 0, true );

                m_pat0->setPosition( Vec3( Data::get()->ownship.waypoint_x,
                                           Data::get()->ownship.waypoint_y,
                                           Data::get()->ownship.waypoint_z ) );

                float coef = Data::get()->ownship.waypoint_dist / m_distScale;

                if ( coef > 1.0f )
                {
                    m_pat0->setScale( Vec3( coef, coef, coef ) );
                }
                else
                {
                    m_pat0->setScale( Vec3( 1.0f, 1.0f, 1.0f ) );
                }

                // following gates
                Aircraft *ownship = Ownship::instance()->getAircraft();

                if ( ownship )
                {
                    coef = updateGate( m_geom1, m_pat1, ownship, 1, coef );
                    coef = updateGate( m_geom2, m_pat2, ownship, 2, coef );
                }
                else
                {
                    m_switch->setValue( 1, false );
                    m_switch->setValue( 2, false );
                }
            }
            else
            {
                m_switch->setAllChildrenOff();
            }
        }
        else
        {
            m_switch->setAllChildrenOff();
        }
    }
    else
    {
        m_switch->setAllChildrenOff();
    }
}

////////////////////////////////////////////////////////////////////////////////

osg::Geometry* Gates::createGate( osg::Group *parent )
{
    const float w_2 = m_size / 2.0f;
    const float h_2 = m_size / 2.0f;

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
    lineWidth->setWidth( m_linesWidth );

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
        coef = ( ownship->getPos() - pos ).length() / m_distScale;

        if ( coef_prev <= 1.0f )
        {
            m_switch->setValue( number, true );

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
            m_switch->setValue( number, false );
        }
    }
    else
    {
        m_switch->setValue( number, false );
    }

    return coef;
}
