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
#ifndef SIM_MODULE_H
#define SIM_MODULE_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Group>

#include <sim/sim_Base.h>
#include <sim/sim_Data.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Simulation CGI module base class.
 */
class Module : public Base
{
public:

    typedef std::vector< Module* > List;

    /**
     * Constructor.
     * <p>root parameter is intended to make possible using osg::Group derived
     * object type instead of pure osg::Group type (e.i. osg::LOD, osg::Switch,
     * osg::PositionAttitudeTransform, etc.) as root node without need
     * to create another scene graph level.</p>
     * @param root OSG module root node object
     * @param parent parent module
     */
    Module( osg::Group *root, Module *parent = NULLPTR );

    /** Destructor. */
    virtual ~Module();

    /** Adds child to the module. */
    virtual void addChild( Module *child );

    /** Initializes module and all its children. */
    virtual void load();

    /** Updates module and all its children. */
    virtual void update();

    /** Returns module OSG node.  */
    inline osg::Group* getNode() { return _root.get(); }

protected:

    osg::ref_ptr<osg::Group> _root;     ///< OSG module root node

    List _children;                     ///< children nodes

    Module *_parent;                    ///< parent node

    /** Removes all children. */
    virtual void removeAllChildren();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_MODULE_H
