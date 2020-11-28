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
#ifndef SIM_FINDNODE_H
#define SIM_FINDNODE_H

////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#include <osg/NodeVisitor>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Finding node visitor class. */
class FindNode : public osg::NodeVisitor
{
public:

    typedef std::vector< osg::ref_ptr<osg::Node> > Nodes;

    /** */
    static osg::Node* findFirst( osg::Node* node, const std::string &name );

    /** */
    static Nodes findNodes( osg::Node* node, const std::string &name );

    /** Constructor. */
    FindNode();

    /** Constructor. */
    FindNode( const std::string &name, bool findAll = false );

    /** */
    virtual void apply( osg::Node &searchNode );

    /** Returns first found node. */
    osg::ref_ptr<osg::Node> getFirst();

    /** Returns list of found nodes. */
    Nodes getNodes();

private:

    bool _findAll;      ///< specifies if search should be continued after finding first matching node
    std::string _name;  ///< name of the nodes to be looking for
    Nodes _nodes;       ///< found nodes list
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_FINDNODE_H
