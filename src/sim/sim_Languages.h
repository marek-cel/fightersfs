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

    /** @brief Default constructor. */
    Languages();

    /** @brief Copy constructor. */
    Languages( const Languages & ) : sim::Singleton< Languages >() {}

public:

    /** @brief Destructor. */
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

    /** */
    void setCurrentByCode( const std::string &code );

private:

    List _list;         ///< supported languages list

    UInt8 _current;     ///< current language index
    UInt8 _default;     ///< default language index

    void readLanguages();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_LANGUAGES_H
