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

const float Scenery::m_size_2 = 65536.0f;
const float Scenery::m_size   = 2.0f * Scenery::m_size_2;
const float Scenery::m_limit  = Scenery::m_size_2 - SIM_SKYDOME_RAD;

std::string Scenery::m_terrainFile = "scenery/ocean.osgb";
std::string Scenery::m_genericFile = "scenery/ocean.osgb";

Scenery::ObjectFiles Scenery::m_objectFiles;

osg::ref_ptr<osg::Node> Scenery::m_terrainNode = 0;

////////////////////////////////////////////////////////////////////////////////

Scenery::Scenery( Module *parent ) :
    Module( new osg::Group(), parent ),

    m_counter ( 100 )
{
#   ifdef SIM_DESKTOP
    osgSim::OverlayNode::OverlayTechnique technique = osgSim::OverlayNode::OBJECT_DEPENDENT_WITH_ORTHOGRAPHIC_OVERLAY;
    m_root = m_overlayNode = new osgSim::OverlayNode( technique );

    m_overlayNode->getOrCreateStateSet()->setTextureAttribute( 1, new osg::TexEnv( osg::TexEnv::DECAL ) );
    m_overlayNode->setContinuousUpdate( true );
#   endif

    m_root->setName( "scenery" );

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
    m_terrainNode = 0;
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::load()
{
    if ( m_root->getNumChildren() > 0 )
    {
        m_root->removeChildren( 0, m_root->getNumChildren() );
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
    m_mt->setMatrix( osg::Matrixd::rotate( osg::Quat( -Data::get()->ownship.heading, osg::Z_AXIS ) )
                   * osg::Matrixd::translate( osg::Vec3( Data::get()->ownship.pos_x,
                                                         Data::get()->ownship.pos_y,
                                                         0.0f ) ) );
#   endif

    if ( m_counter == 100 )
    {
        m_counter = 0;

        if ( fabs( Data::get()->camera.pos_x ) > m_limit
          || fabs( Data::get()->camera.pos_y ) > m_limit )
        {
            m_switchGeneric->setValue( 1, true );
            m_switchGeneric->setValue( 2, true );
            m_switchGeneric->setValue( 3, true );

            short ix = 0;
            short iy = 0;

            if ( fabs( Data::get()->camera.pos_x ) > m_size_2 )
            {
                if ( Data::get()->camera.pos_x > 0.0 )
                {
                    ix = ( Data::get()->camera.pos_x - m_size_2 ) / m_size + 1;
                }
                else
                {
                    ix = ( Data::get()->camera.pos_x + m_size_2 ) / m_size - 1;
                }
            }

            if ( fabs( Data::get()->camera.pos_y ) > m_size_2 )
            {
                if ( Data::get()->camera.pos_y > 0.0 )
                {
                    iy = ( Data::get()->camera.pos_y - m_size_2 ) / m_size + 1;
                }
                else
                {
                    iy = ( Data::get()->camera.pos_y + m_size_2 ) / m_size - 1;
                }
            }

            float xc = ix * m_size;
            float yc = iy * m_size;

            if ( ix == 0 && iy == 0 )
            {
                m_switchGeneric->setValue( 0, false );
            }
            else
            {
                m_switchGeneric->setValue( 0, true );

                m_pat_0->setPosition( osg::Vec3d( xc, yc, 0.0 ) );
            }

            short dix = Misc::sign( Data::get()->camera.pos_x - xc );
            short diy = Misc::sign( Data::get()->camera.pos_y - yc );

            setPositionC( xc, yc, ix, iy, dix, diy );
            setPositionX( xc, yc, ix, iy, dix );
            setPositionY( xc, yc, ix, iy, diy );
        }
        else
        {
            m_switchGeneric->setAllChildrenOff();
        }
    }

    m_counter++;
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::createGeneric()
{
    m_switchGeneric = new osg::Switch();
    m_root->addChild( m_switchGeneric.get() );

    m_pat_0 = new osg::PositionAttitudeTransform();
    m_pat_x = new osg::PositionAttitudeTransform();
    m_pat_y = new osg::PositionAttitudeTransform();
    m_pat_c = new osg::PositionAttitudeTransform();

    m_switchGeneric->addChild( m_pat_0 );
    m_switchGeneric->addChild( m_pat_x );
    m_switchGeneric->addChild( m_pat_y );
    m_switchGeneric->addChild( m_pat_c );

    loadGeneric();
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::createObjects()
{
    for ( ObjectFiles::iterator it = m_objectFiles.begin(); it != m_objectFiles.end(); ++it )
    {
        osg::ref_ptr<osg::Node> object = Models::get( getPath( *it ) );

        if ( object.valid() )
        {
            m_root->addChild( object.get() );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_DESKTOP
void Scenery::createShadow()
{
    const double w_2 = 8.0;
    const double l_2 = 8.0;

    m_mt = new osg::MatrixTransform();
    m_overlayNode->setOverlaySubgraph( m_mt.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    m_mt->addChild( geode.get() );

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

    osg::ref_ptr<osg::Texture2D> texture = Textures::get( getPath( "shadow.png" ) );
    stateSet->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON );
}
#endif

////////////////////////////////////////////////////////////////////////////////

void Scenery::loadGeneric()
{
    osg::ref_ptr<osg::Node> ocean = Models::get( getPath( "scenery/ocean.osgb" ), true );

    if ( ocean.valid() )
    {
        m_pat_0->addChild( ocean.get() );
        m_pat_x->addChild( ocean.get() );
        m_pat_y->addChild( ocean.get() );
        m_pat_c->addChild( ocean.get() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::loadTerrain()
{
    osg::ref_ptr<osg::Node> terrain = Models::get( getPath( m_terrainFile ), true );

    if ( terrain.valid() )
    {
        m_terrainNode = terrain.get();

        m_root->addChild( terrain.get() );
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

    m_pat_c->setPosition( osg::Vec3d( xc + dix * m_size, yc + diy * m_size, 0.0 ) );
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::setPositionX( float xc, float yc, short ix, short iy, short dix )
{
    if ( ix + dix == 0 && iy == 0 )
    {
        if ( dix < 0 ) dix -= 2;
        else           dix += 2;
    }

    m_pat_x->setPosition( osg::Vec3d( xc + dix * m_size, yc, 0.0 ) );
}

////////////////////////////////////////////////////////////////////////////////

void Scenery::setPositionY( float xc, float yc, short ix, short iy, short diy )
{
    if ( iy + diy == 0 && ix == 0 )
    {
        if ( diy < 0 ) diy -= 2;
        else           diy += 2;
    }

    m_pat_y->setPosition( osg::Vec3d( xc, yc + diy * m_size, 0.0 ) );
}

