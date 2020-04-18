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
#ifndef SIM_SCENERY_H
#define SIM_SCENERY_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Switch>

#ifdef SIM_DESKTOP
#   include <osg/MatrixTransform>
#   include <osgSim/OverlayNode>
#endif

#include <sim/cgi/sim_Module.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Scenery module class. */
class Scenery : public Module
{
public:

    typedef std::vector< std::string > ObjectFiles;

    static const float m_size_2;        ///< [m]
    static const float m_size;          ///< [m]
    static const float m_limit;         ///< [m] minimum distance in main directions from scenery center when generic terrain tiles are visible

    static std::string m_terrainFile;   ///< terrain file
    static std::string m_genericFile;   ///< generic terrain file
    static ObjectFiles m_objectFiles;   ///< object files

    /** */
    inline static osg::Node* getTerrainNode()
    {
        return m_terrainNode.get();
    }

    /** Constructor. */
    Scenery( Module *parent = 0 );

    /** Destructor. */
    virtual ~Scenery();

    /** Loads terrain. */
    void load();

    /** Updates scenery. */
    void update();

private:

#   ifdef SIM_DESKTOP
    osg::ref_ptr<osgSim::OverlayNode> m_overlayNode;        ///< aircraft shadow
    osg::ref_ptr<osg::MatrixTransform> m_mt;                ///<
#   endif

    static osg::ref_ptr<osg::Node> m_terrainNode;           ///< static terrain node for world view camera manipulator

    osg::ref_ptr<osg::Switch> m_switchGeneric;              ///< generic terrain tile switch

    osg::ref_ptr<osg::PositionAttitudeTransform> m_pat_0;   ///< center
    osg::ref_ptr<osg::PositionAttitudeTransform> m_pat_x;   ///< EW (x) horizontal
    osg::ref_ptr<osg::PositionAttitudeTransform> m_pat_y;   ///< NS (y) vertical
    osg::ref_ptr<osg::PositionAttitudeTransform> m_pat_c;   ///< NE/NW/SE/SW (xy) corner

    UInt16 m_counter;                                       ///< generic terrain update counter

    void createGeneric();
    void createObjects();
#   ifdef SIM_DESKTOP
    void createShadow();
#   endif

    void loadGeneric();
    void loadTerrain();

    void setPositionC( float xc, float yc, short ix, short iy, short dix, short diy );
    void setPositionX( float xc, float yc, short ix, short iy, short dix );
    void setPositionY( float xc, float yc, short ix, short iy, short diy );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_SCENERY_H
