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

#include <sim/sim_Log.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

osgText::Font* Fonts::get( const std::string &fontFile )
{
    for ( unsigned int i = 0; i < instance()->_fileNames.size(); i++ )
    {
        if ( fontFile == instance()->_fileNames.at( i ) )
        {
            return instance()->_fonts.at( i );
        }
    }

    osg::ref_ptr<osgText::Font> font = osgText::readFontFile( fontFile );

    if ( font.valid() )
    {
        instance()->_fonts.push_back( font.get() );
        instance()->_fileNames.push_back( fontFile );

        return font.get();
    }
    else
    {
        Log::e() << "Cannot open font file: " << fontFile << std::endl;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void Fonts::reset()
{
    instance()->_fileNames.clear();
    instance()->_fonts.clear();
}

////////////////////////////////////////////////////////////////////////////////

Fonts::Fonts()
{
    _fileNames.clear();
    _fonts.clear();
}

////////////////////////////////////////////////////////////////////////////////

Fonts::~Fonts()
{
    _fileNames.clear();
    _fonts.clear();
}
