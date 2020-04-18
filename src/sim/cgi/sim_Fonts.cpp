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

#include <sim/cgi/sim_Fonts.h>

#include <osgDB/ReadFile>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

osgText::Font* Fonts::get( const std::string &fontFile )
{
    for ( unsigned int i = 0; i < instance()->m_fileNames.size(); i++ )
    {
        if ( fontFile == instance()->m_fileNames.at( i ) )
        {
            return instance()->m_fonts.at( i );
        }
    }

    osg::ref_ptr<osgText::Font> font = osgText::readFontFile( fontFile );

    if ( font.valid() )
    {
        instance()->m_fonts.push_back( font.get() );
        instance()->m_fileNames.push_back( fontFile );

        return font.get();
    }
    else
    {
        osg::notify(osg::ALWAYS) << "ERROR! Cannot open font file: " << fontFile << std::endl;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void Fonts::reset()
{
    instance()->m_fileNames.clear();
    instance()->m_fonts.clear();
}

////////////////////////////////////////////////////////////////////////////////

Fonts::Fonts()
{
    m_fileNames.clear();
    m_fonts.clear();
}

////////////////////////////////////////////////////////////////////////////////

Fonts::~Fonts()
{
    m_fileNames.clear();
    m_fonts.clear();
}
