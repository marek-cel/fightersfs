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

#include <sim/cgi/sim_Scenery.h>

#ifdef SIM_DESKTOP
#   include <osg/Geode>
#   include <osg/Geometry>
#   include <osg/Material>
#   include <osg/TexEnv>
#endif

#include <sim/cgi/sim_Models.h>
#include <sim/cgi/sim_Textures.h>

#include <sim/utils/sim_Misc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const float Scenery::_size_2 = 65536.0f;
const float Scenery::_size   = 2.0f * Scenery::_size_2;
const float Scenery::_limit  = Scenery::_size_2 - SIM_SKYDOME_RAD;

std::string Scenery::_terrainFile = "scenery/ocean.osgb";
std::string Scenery::_genericFile = "scenery/ocean.osgb";

Scenery::ObjectFiles Scenery::_objectFiles;

osg::ref_ptr<osg::Node> Scenery::_terrainNode = 0;

////////////////////////////////////////////////////////////////////////////////

Scenery::Scenery( Module *parent ) :
    Module( new osg::Group(), parent ),

    _counter ( 100 )
{
#   ifdef SIM_DESKTOP
    osgSim::OverlayNode::OverlayTechnique technique = osgSim::OverlayNode::OBJECT_DEPENDENT_WITH_ORTHOGRAPHIC_OVERLAY;
    _root = _overlayNode = new osgSim::OverlayNode( technique );

    _overlayNode->getOrCreateStateSet()->setTextureAttribute( 1, new osg::TexEnv( osg::TexEnv::DECAL ) );
    _overlayNode->setContinuousUpdate( true );
#   endif

    _root->setName( "scenery" );

    loadTerrain();
    createObjects();
    createGeneric();

#   ifdef SIM_DESKTOP
    createShadow();
#   endif
}

////////////////////////////////////////////////////////////////////////////////

Scenery::~Scenery()
{
    _terrainNode = 0;
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::load()
{
    if ( _root->getNumChildren() > 0 )
    {
        _root->removeChildren( 0, _root->getNumChildren() );
    }

    loadTerrain();
    createObjects();
    createGeneric();
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::update()
{
    /////////////////
    Module::update();
    /////////////////

#   ifdef SIM_DESKTOP
    _mt->setMatrix( osg::Matrixd::rotate( osg::Quat( -Data::get()->ownship.heading, osg::Z_AXIS ) )
                  * osg::Matrixd::translate( osg::Vec3( Data::get()->ownship.pos_x,
                                                        Data::get()->ownship.pos_y,
                                                        0.0f ) ) );
#   endif

    if ( _counter == 100 )
    {
        _counter = 0;

        if ( fabs( Data::get()->camera.pos_x ) > _limit
          || fabs( Data::get()->camera.pos_y ) > _limit )
        {
            _switchGeneric->setValue( 1, true );
            _switchGeneric->setValue( 2, true );
            _switchGeneric->setValue( 3, true );

            short ix = 0;
            short iy = 0;

            if ( fabs( Data::get()->camera.pos_x ) > _size_2 )
            {
                if ( Data::get()->camera.pos_x > 0.0 )
                {
                    ix = ( Data::get()->camera.pos_x - _size_2 ) / _size + 1;
                }
                else
                {
                    ix = ( Data::get()->camera.pos_x + _size_2 ) / _size - 1;
                }
            }

            if ( fabs( Data::get()->camera.pos_y ) > _size_2 )
            {
                if ( Data::get()->camera.pos_y > 0.0 )
                {
                    iy = ( Data::get()->camera.pos_y - _size_2 ) / _size + 1;
                }
                else
                {
                    iy = ( Data::get()->camera.pos_y + _size_2 ) / _size - 1;
                }
            }

            float xc = ix * _size;
            float yc = iy * _size;

            if ( ix == 0 && iy == 0 )
            {
                _switchGeneric->setValue( 0, false );
            }
            else
            {
                _switchGeneric->setValue( 0, true );

                _pat_0->setPosition( osg::Vec3d( xc, yc, 0.0 ) );
            }

            short dix = Misc::sign( Data::get()->camera.pos_x - xc );
            short diy = Misc::sign( Data::get()->camera.pos_y - yc );

            setPositionC( xc, yc, ix, iy, dix, diy );
            setPositionX( xc, yc, ix, iy, dix );
            setPositionY( xc, yc, ix, iy, diy );
        }
        else
        {
            _switchGeneric->setAllChildrenOff();
        }
    }

    _counter++;
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::createGeneric()
{
    _switchGeneric = new osg::Switch();
    _root->addChild( _switchGeneric.get() );

    _pat_0 = new osg::PositionAttitudeTransform();
    _pat_x = new osg::PositionAttitudeTransform();
    _pat_y = new osg::PositionAttitudeTransform();
    _pat_c = new osg::PositionAttitudeTransform();

    _switchGeneric->addChild( _pat_0 );
    _switchGeneric->addChild( _pat_x );
    _switchGeneric->addChild( _pat_y );
    _switchGeneric->addChild( _pat_c );

    loadGeneric();
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::createObjects()
{
    for ( ObjectFiles::iterator it = _objectFiles.begin(); it != _objectFiles.end(); ++it )
    {
        osg::ref_ptr<osg::Node> object = Models::get( getPath( *it ) );

        if ( object.valid() )
        {
            _root->addChild( object.get() );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_DESKTOP
void Scenery::createShadow()
{
    const double w_2 = 8.0;
    const double l_2 = 8.0;

    _mt = new osg::MatrixTransform();
    _overlayNode->setOverlaySubgraph( _mt.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    _mt->addChild( geode.get() );

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    geode->addDrawable( geom.get() );

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    osg::ref_ptr<osg::Vec2Array> t = new osg::Vec2Array();
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array();

    v->push_back( osg::Vec3d( -w_2, -l_2, 0.0 ) );
    v->push_back( osg::Vec3d(  w_2, -l_2, 0.0 ) );
    v->push_back( osg::Vec3d(  w_2,  l_2, 0.0 ) );
    v->push_back( osg::Vec3d( -w_2,  l_2, 0.0 ) );

    c->push_back( osg::Vec4( 0.8, 0.8, 0.8, 1.0 ) );

    t->push_back( osg::Vec2( 0, 0 ) );
    t->push_back( osg::Vec2( 1, 0 ) );
    t->push_back( osg::Vec2( 1, 1 ) );
    t->push_back( osg::Vec2( 0, 1 ) );

    n->push_back( osg::Vec3d( 0.0, 0.0, 1.0 ) );

    geom->setNormalArray( n.get() );
    geom->setNormalBinding( osg::Geometry::BIND_OVERALL );

    geom->setColorArray( c.get() );
    geom->setColorBinding( osg::Geometry::BIND_OVERALL );

    geom->setVertexArray( v.get() );
    geom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, v->size() ) );

    geom->setTexCoordArray( 0, t.get() );

    // material
    osg::ref_ptr<osg::Material> material = new osg::Material();
    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
    material->setAmbient( osg::Material::FRONT, osg::Vec4f( 0.8f, 0.8f, 0.8f, 1.0f ) );
    material->setDiffuse( osg::Material::FRONT, osg::Vec4f( 0.5f, 0.5f, 0.5f, 1.0f ) );

    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

    stateSet->setAttribute( material.get() );

    osg::ref_ptr<osg::Texture2D> texture = Textures::get( getPath( "textures/shadow.png" ) );
    stateSet->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON );
}
#endif

////////////////////////////////////////////////////////////////////////////////

void Scenery::loadGeneric()
{
    osg::ref_ptr<osg::Node> ocean = Models::get( getPath( "scenery/ocean.osgb" ), true );

    if ( ocean.valid() )
    {
        _pat_0->addChild( ocean.get() );
        _pat_x->addChild( ocean.get() );
        _pat_y->addChild( ocean.get() );
        _pat_c->addChild( ocean.get() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::loadTerrain()
{
    osg::ref_ptr<osg::Node> terrain = Models::get( getPath( _terrainFile ), true );

    if ( terrain.valid() )
    {
        _terrainNode = terrain.get();

        _root->addChild( terrain.get() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::setPositionC( float xc, float yc, short ix, short iy, short dix, short diy )
{
    if ( ix + dix == 0 && iy + diy == 0 )
    {
        if ( dix < 0 ) dix -= 2;
        else           dix += 2;

        if ( diy < 0 ) diy -= 2;
        else           diy += 2;
    }

    _pat_c->setPosition( osg::Vec3d( xc + dix * _size, yc + diy * _size, 0.0 ) );
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::setPositionX( float xc, float yc, short ix, short iy, short dix )
{
    if ( ix + dix == 0 && iy == 0 )
    {
        if ( dix < 0 ) dix -= 2;
        else           dix += 2;
    }

    _pat_x->setPosition( osg::Vec3d( xc + dix * _size, yc, 0.0 ) );
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::setPositionY( float xc, float yc, short ix, short iy, short diy )
{
    if ( iy + diy == 0 && ix == 0 )
    {
        if ( diy < 0 ) diy -= 2;
        else           diy += 2;
    }

    _pat_y->setPosition( osg::Vec3d( xc, yc + diy * _size, 0.0 ) );
}

