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
#ifndef SIM_LISTSCENERY_H
#define SIM_LISTSCENERY_H

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
class ListScenery : public sim::Singleton< ListScenery >
{
    friend class sim::Singleton< ListScenery >;

public:

    typedef std::vector< std::string > ObjectFiles;

    /** Unit data. */
    struct SceneryData
    {
        std::string elevationFile;  ///<
        std::string terrainFile;    ///<
        std::string genericFile;    ///<
        ObjectFiles objectFiles;    ///<

        Vec3 initialPosition;       ///< [m]
    };

    typedef std::vector< SceneryData > SceneryList; ///<

private:

    ListScenery();

public:

    virtual ~ListScenery();

    inline UInt32 getCount() const { return _data.size(); }

    SceneryData getData( UInt32 index );

private:

    SceneryList _data;      ///< list of scenery data

    void readScenery();
    void readScenery( const XmlNode &node );

};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_LISTSCENERY_H
