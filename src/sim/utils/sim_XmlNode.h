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
#ifndef SIM_XMLNODE_H
#define SIM_XMLNODE_H

////////////////////////////////////////////////////////////////////////////////

#include <limits>
#include <map>
#include <string>

#include <libxml/tree.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief XML node class. */
class XmlNode
{
public:

    typedef std::map<std::string,std::string> Attributes;

    /** @brief Constructor. */
    XmlNode();

    /** @brief Copy constructor. */
    XmlNode( const XmlNode &node );

    /** @brief Constructor. */
    XmlNode( xmlNodePtr node, const std::string &file );

    /** @brief Destrcutor. */
    virtual ~XmlNode() {}

    /** R@brief eturns error info dtring. */
    std::string getErrorInfo() const;

    /**
     * @brief Returns the value of the attribute.
     * Returns the value of the attribute or an empty string if the attribute
     * has not been specified.
     */
    std::string getAttribute( const std::string &name ) const;

    /**
     * @brief Returns element attributes list.
     * Returns element attributes list or empty list if the node is not
     * an element or element does not has any attributes.
     */
    Attributes getAttributes() const;

    /**
     * @brief Returns first child node.
     */
    XmlNode getFirstChild() const;

    /**
     * @brief Returns first child element node of the given name.
     * @param name element name
     */
    XmlNode getFirstChildElement( const std::string &name = std::string() ) const;

    /**
     * @brief Returns file name.
     */
    inline std::string getFile() const
    {
        return _file;
    }

    /**
     * @brief Returns file name and line number.
     */
    std::string getFileAndLine() const;

    /**
     * @brief Returns line number.
     */
    inline int getLine() const
    {
        if ( isValid() )
        {
            return (int)_node->line;
        }

        return std::numeric_limits< int >::quiet_NaN();
    }

    /**
     * @brief Returns node name.
     */
    std::string getName() const
    {
        if ( isValid() )
        {
            return std::string( (const char*)_node->name );
        }

        return std::string();
    }

    /**
     * @brief Returns next sibling node.
     */
    XmlNode getNextSibling() const;

    /**
     * @brief Returns next sibling element node of the given name.
     * @param name element name
     */
    XmlNode getNextSiblingElement( const std::string &name = std::string() ) const;

    /** */
    std::string getText() const;

    /**
     * @brief Returns true if node has attributes.
     */
    inline bool hasAttributes() const
    {
        if ( isValid() )
        {
            return ( _node->properties != 0 );
        }

        return false;
    }

    /**
     * @brief Returns true if node has children.
     */
    inline bool hasChildren() const
    {
        if ( isValid() )
        {
            return ( _node->children != 0 );
        }

        return false;
    }

    /**
     * @brief Returns true if node is attribute.
     */
    inline bool isAttribute() const
    {
        if ( isValid() )
        {
            return ( _node->type == XML_ATTRIBUTE_NODE );
        }

        return false;
    }

    /**
     * @brief Returns true if node is comment.
     */
    inline bool isComment() const
    {
        if ( isValid() )
        {
            return ( _node->type == XML_COMMENT_NODE );
        }

        return false;
    }

    /**
     * @brief Returns true if node is element.
     */
    inline bool isElement() const
    {
        if ( isValid() )
        {
            return ( _node->type == XML_ELEMENT_NODE );
        }

        return false;
    }

    /**
     * @brief Returns true if node is text.
     */
    inline bool isText() const
    {
        if ( isValid() )
        {
            return ( _node->type == XML_TEXT_NODE );
        }

        return false;
    }

    /**
     * @brief Returns true if node is valid.
     */
    inline bool isValid() const
    {
        return ( _node ) ? true : false;
    }

private:

    std::string _file;  ///< XML file name
    xmlNodePtr  _node;  ///< libxml XML node pointer
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_XMLNODE_H
