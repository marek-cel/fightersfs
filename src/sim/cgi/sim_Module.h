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
     * @brief Constructor.
     *
     * root parameter is intended to make possible using osg::Group derived
     * object type instead of pure osg::Group type (e.i. osg::LOD, osg::Switch,
     * osg::PositionAttitudeTransform, etc.) as root node without need
     * to create another scene graph level.
     *
     * @param root OSG module root node object
     * @param parent parent module
     */
    Module( osg::Group *root, Module *parent = NULLPTR );

    /** @brief Destructor. */
    virtual ~Module();

    /** @brief Adds child to the module. */
    virtual void addChild( Module *child );

    /** @brief Initializes module and all its children. */
    virtual void load();

    /** @brief Updates module and all its children. */
    virtual void update();

    /** @brief Returns module OSG node.  */
    inline osg::Group* getNode() { return _root.get(); }

protected:

    osg::ref_ptr<osg::Group> _root;     ///< OSG module root node

    List _children;                     ///< children nodes

    Module *_parent;                    ///< parent node

    /** @brief Removes all children. */
    virtual void removeAllChildren();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_MODULE_H
