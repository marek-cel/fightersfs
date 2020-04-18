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
#ifndef SIM_GROUP_H
#define SIM_GROUP_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Group>

#include <sim/sim_Base.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{
class Entity;

/** Entities group class. */
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
    inline Group* getParent() { return m_parent; }

    /** Returns OSG node.  */
    inline osg::Group* getNode() { return m_root.get(); }

    /** Loads entities. */
    virtual void load();

    /** Updates all children entities. */
    virtual void update( double timeStep );

protected:

    Group *m_parent;    ///< parent node
    List m_children;    ///< children nodes

    osg::ref_ptr<osg::Group> m_root;    ///< root group node
};

}

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_GROUP_H
