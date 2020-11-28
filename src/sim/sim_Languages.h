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
#ifndef SIM_LANGUAGES_H
#define SIM_LANGUAGES_H

////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#include <sim/sim_Types.h>

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** */
class Languages : public sim::Singleton< Languages >
{
    friend class sim::Singleton< Languages >;

public:

    typedef std::vector< std::string > List;

private:

    /** Default constructor. */
    Languages();

    /** Copy constructor. */
    Languages( const Languages & ) : sim::Singleton< Languages >() {}

public:

    /** Destructor. */
    virtual ~Languages();

    /** */
    inline UInt8 getCurrent() const { return _current; }

    /** */
    inline UInt8 getDefault() const { return _default; }

    /** */
    std::string getCodeByIndex( UInt8 index ) const;

    /** */
    int getIndexByCode( const std::string &code ) const;

    /** */
    inline unsigned int getCount() const { return _list.size(); }

    /** */
    bool isCodeValid( const std::string &code ) const;

    /** */
    void setCurrent( UInt8 current );

private:

    List _list;         ///< supported languages list

    UInt8 _current;     ///< current language index
    UInt8 _default;     ///< default language index

    void readLanguages();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_LANGUAGES_H
