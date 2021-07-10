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

#include <sim/cgi/sim_Models.h>

#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/LineWidth>

#include <osgDB/ReadFile>

#include <sim/sim_Log.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

osg::ref_ptr<osg::LOD> Models::_tracer;

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

    _tracer = 0;

    _tracer = new osg::LOD();

    _tracer->setCenterMode( osg::LOD::USER_DEFINED_CENTER );
    _tracer->setCenter( osg::Vec3( 0.0f, 0.0f, 0.0f ) );
    _tracer->setRadius( 5.0f );
    _tracer->setRangeMode( osg::LOD::DISTANCE_FROM_EYE_POINT );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    _tracer->addChild( geode.get(), 0.0f, range );
    _tracer->setRange( 0, 0.0f, range );

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

osg::Node* Models::get( const std::string &objectFile, bool straight,
                        bool no_storage )
{
    for ( unsigned int i = 0; i < instance()->_fileNames.size(); i++ )
    {
        if ( objectFile == instance()->_fileNames.at( i ) )
        {
            return instance()->_objects.at( i );
        }
    }

    osg::ref_ptr<osg::Node> object = ( straight ) ? osgDB::readNodeFile( objectFile ) : readNodeFile( objectFile );

    if ( object.valid() )
    {
        if ( !no_storage )
        {
            instance()->_objects.push_back( object.get() );
            instance()->_fileNames.push_back( objectFile );

            return object.get();
        }
        else
        {
            return object.release();
        }
    }
    else
    {
        Log::e() << "Cannot open object file: " << objectFile << std::endl;
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
        Log::e() << "Cannot read object file: " << objectFile << std::endl;
    }

    return object.release();
}

////////////////////////////////////////////////////////////////////////////////

void Models::reset()
{
    instance()->_fileNames .clear();
    instance()->_objects   .clear();
}

////////////////////////////////////////////////////////////////////////////////

Models::Models()
{
    _fileNames .clear();
    _objects   .clear();
}

////////////////////////////////////////////////////////////////////////////////

Models::~Models() {}
