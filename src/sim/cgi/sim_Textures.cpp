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

#include <sim/cgi/sim_Textures.h>

#include <osgDB/ReadFile>

#include <sim/sim_Log.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

osg::Texture2D* Textures::get( const std::string &textureFile, float maxAnisotropy,
                               osg::Texture::WrapMode mode )
{
    for ( unsigned int i = 0; i < instance()->m_fileNames.size(); i++ )
    {
        if ( textureFile == instance()->m_fileNames.at( i ) )
        {
            instance()->m_textures.at( i )->setMaxAnisotropy( maxAnisotropy );
            return instance()->m_textures.at( i );
        }
    }

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile( textureFile );

    if ( image.valid() )
    {
        osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
        texture->setImage( image.get() );

        texture->setWrap( osg::Texture2D::WRAP_S, mode );
        texture->setWrap( osg::Texture2D::WRAP_T, mode );

        texture->setNumMipmapLevels( 4 );
        texture->setMaxAnisotropy( maxAnisotropy );

        texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_NEAREST );
        texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );

        texture->setUnRefImageDataAfterApply( false );

        instance()->m_textures.push_back( texture.get() );
        instance()->m_fileNames.push_back( textureFile );

        return texture.get();
    }
    else
    {
        Log::e() << "Cannot open texture file: " << textureFile << std::endl;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

Textures::Textures()
{
    m_fileNames.clear();
    m_textures.clear();
}

////////////////////////////////////////////////////////////////////////////////

Textures::~Textures() {}
