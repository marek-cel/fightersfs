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
#ifndef SIM_TEXTURES_H
#define SIM_TEXTURES_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Texture2D>

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Textures container class. */
class Textures : public Singleton< Textures >
{
    friend class Singleton< Textures >;

public:

    typedef std::vector< osg::ref_ptr<osg::Texture2D> > List;

    /** */
    static osg::Texture2D* get( const std::string &textureFile, float maxAnisotropy = 1.0f,
                                osg::Texture::WrapMode mode = osg::Texture::MIRROR );

private:

    /**
     * You should use static function getInstance() due to get refernce
     * to Textures class instance.
     */
    Textures();

    /** Using this constructor is forbidden. */
    Textures( const Textures & ) : Singleton< Textures >() {}

public:

    /** Destructor. */
    virtual ~Textures();

private:

    List m_textures;                        ///<
    std::vector< std::string > m_fileNames; ///<
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_TEXTURES_H
