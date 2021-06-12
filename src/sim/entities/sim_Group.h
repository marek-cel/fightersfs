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
#ifndef SIM_GROUP_H
#define SIM_GROUP_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Group>

#include <sim/sim_Base.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{
class Entity;

/** @brief Entities group class. */
class Group : public Base
{
public:

    typedef std::vector< Entity* > List;

    /** Constructor */
    Group( Group *parent = 0 );

    /** Destructor */
    virtual ~Group();

    /** Attaches child entity. */
    virtual void attachEntity( Entity *entity );

    /** Deletes all children entities. */
    virtual void deleteAllEntities();

    /** Deletes entity. */
    virtual void deleteEntity( Entity *entity );

    /** Detaches entity. */
    virtual void dettachEntity( Entity *entity );

    /** Returns all children entities list. */
    virtual List* getEntities();

    /**
     * Returns entity of a given ID if exists, otherwise returns 0.
     * @param id entity unique ID
     */
    virtual Entity* getEntityById( UInt32 id );

    /** Returns parent node. */
    inline Group* getParent() { return _parent; }

    /** Returns OSG node.  */
    inline osg::Group* getNode() { return _root.get(); }

    /** Loads entities. */
    virtual void load();

    /** Updates all children entities. */
    virtual void update( double timeStep );

protected:

    Group *_parent;     ///< parent node
    List _children;     ///< children nodes

    osg::ref_ptr<osg::Group> _root;     ///< root group node
};

}

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_GROUP_H
