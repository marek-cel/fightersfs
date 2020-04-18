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

#include <sim/cgi/sim_Models.h>

#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/LineWidth>

#include <osgDB/ReadFile>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

osg::ref_ptr<osg::LOD> Models::m_tracer;

////////////////////////////////////////////////////////////////////////////////

Models::VisitorLOD::VisitorLOD() :
    osg::NodeVisitor( TRAVERSE_ALL_CHILDREN )
{}

////////////////////////////////////////////////////////////////////////////////

void Models::VisitorLOD::apply( osg::Node &searchNode )
{
    osg::ref_ptr<osg::LOD> lod = dynamic_cast<osg::LOD*>( &searchNode );

    if ( lod.valid() )
    {
        osg::ref_ptr<osg::Node> node = 0;

        if ( lod->getNumChildren() > 0 )
        {
            node = lod->getChild( 0 );
            lod->removeChildren( 0, lod->getNumChildren() );
        }

        if ( node.valid() )
        {
            lod->addChild( node.get(), 0.0f, 1.0e8 );
        }
    }

    traverse( searchNode );
}

////////////////////////////////////////////////////////////////////////////////

void Models::createTracer( float linesWidth )
{
    const float range = 500.0f;

    m_tracer = 0;

    m_tracer = new osg::LOD();

    m_tracer->setCenterMode( osg::LOD::USER_DEFINED_CENTER );
    m_tracer->setCenter( osg::Vec3( 0.0f, 0.0f, 0.0f ) );
    m_tracer->setRadius( 5.0f );
    m_tracer->setRangeMode( osg::LOD::DISTANCE_FROM_EYE_POINT );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    m_tracer->addChild( geode.get(), 0.0f, range );
    m_tracer->setRange( 0, 0.0f, range );

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  // vertices
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();  // normals
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();  // colors

    v->push_back( osg::Vec3( -3.0f, 0.0f, 0.0f ) );
    v->push_back( osg::Vec3(  3.0f, 0.0f, 0.0f ) );

    n->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );

    c->push_back( osg::Vec4( 1.0f, 0.9f, 0.5f, 1.0f ) );
    c->push_back( osg::Vec4( 1.0f, 0.9f, 0.5f, 0.1f ) );

    geometry->setVertexArray( v.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, v->size() ) );

    geometry->setNormalArray( n.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geometry->setColorArray( c.get() );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth();
    lineWidth->setWidth( linesWidth );

    stateSet->setAttributeAndModes( lineWidth, osg::StateAttribute::ON );

    stateSet->setMode( GL_LIGHTING , osg::StateAttribute::OFF );
    stateSet->setMode( GL_BLEND    , osg::StateAttribute::ON  );
    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
}

////////////////////////////////////////////////////////////////////////////////

osg::Node* Models::get( const std::string &objectFile, bool straight )
{
    for ( unsigned int i = 0; i < instance()->m_fileNames.size(); i++ )
    {
        if ( objectFile == instance()->m_fileNames.at( i ) )
        {
            return instance()->m_objects.at( i );
        }
    }

    osg::ref_ptr<osg::Node> object = ( straight ) ? osgDB::readNodeFile( objectFile ) : readNodeFile( objectFile );

    if ( object.valid() )
    {
        instance()->m_objects.push_back( object.get() );
        instance()->m_fileNames.push_back( objectFile );

        return object.get();
    }
    else
    {
        osg::notify(osg::ALWAYS) << "ERROR! Cannot open object file: " << objectFile << std::endl;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

osg::Node* Models::readNodeFile( std::string objectFile )
{
    osg::ref_ptr<osg::Node> object = osgDB::readNodeFile( objectFile );

    if ( object.valid() )
    {
#       ifdef SIM_TEST
        VisitorLOD visitorLOD;
        object->accept( visitorLOD );
#       endif
    }
    else
    {
        osg::notify(osg::ALWAYS) << "ERROR! Cannot read object file: " << objectFile << std::endl;
    }

    return object.release();
}

////////////////////////////////////////////////////////////////////////////////

void Models::reset()
{
    instance()->m_fileNames.clear();
    instance()->m_objects.clear();
}

////////////////////////////////////////////////////////////////////////////////

Models::Models()
{
    m_fileNames.clear();
    m_objects.clear();
}

////////////////////////////////////////////////////////////////////////////////

Models::~Models() {}
