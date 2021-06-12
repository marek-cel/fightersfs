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
#ifndef SIM_LISTUNITS_H
#define SIM_LISTUNITS_H

////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#include <sim/sim_Types.h>

#include <sim/utils/sim_Singleton.h>
#include <sim/utils/sim_XmlNode.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** */
class ListUnits : public sim::Singleton< ListUnits >
{
    friend class sim::Singleton< ListUnits >;

public:

    /** Unit types. */
    enum UnitType
    {
        Aerial = 0,             ///< aerial unit
        Marine,                 ///< marine vessel
        Ground                  ///< ground vehicle
    };

    /** Unit data. */
    struct UnitData
    {
        UnitType type;          ///< unit type

        bool fighter;           ///<

        std::string file;       ///< unit data file
        std::string model;      ///< unit model file
        std::string texture;    ///< unit texture file (aircrafts only)

        std::vector< std::string > liveries;    ///<

        float distance;         ///< [m] camera distance
        float heading;          ///< [deg] camera heading
        float offset_x;         ///< [m] camera offset along x-axis
    };

    typedef std::vector< UnitData > UnitsList;  ///<

private:

    ListUnits();

public:

    virtual ~ListUnits();

    inline UInt32 getCount() const { return _data.size(); }

    UnitData getData( UInt32 index );

private:

    UnitsList _data;        ///< list of all units

    void readUnits();
    void readUnits( UnitType type, const XmlNode &node );
    void readUnit( UnitType type, const XmlNode &node );

};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_LISTUNITS_H
