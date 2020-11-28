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
#ifndef SIM_FONTS_H
#define SIM_FONTS_H

////////////////////////////////////////////////////////////////////////////////

#include <osgText/Font>

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Fonts container class. */
class Fonts : public Singleton< Fonts >
{
    friend class Singleton< Fonts >;

public:

    typedef std::vector< osg::ref_ptr<osgText::Font> > List;

    /** */
    static osgText::Font* get( const std::string &fontFile );

    /** Resets fonts list. */
    static void reset();

private:

    /**
     * You should use static function getInstance() due to get refernce
     * to Fonts class instance.
     */
    Fonts();

    /** Using this constructor is forbidden. */
    Fonts( const Fonts & ) : Singleton< Fonts >() {}

public:

    /** Destructor. */
    virtual ~Fonts();

private:

    List _fonts;                            ///<
    std::vector< std::string > _fileNames;  ///<
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_FONTS_H
