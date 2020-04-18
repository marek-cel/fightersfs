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

    inline UInt32 getCount() const { return m_data.size(); }

    SceneryData getData( UInt32 index );

private:

    SceneryList m_data;     ///< list of scenery data

    void readScenery();
    void readScenery( const XmlNode &node );

};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_LISTSCENERY_H
