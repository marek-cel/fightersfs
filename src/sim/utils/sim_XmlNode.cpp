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

#include <sim/utils/sim_XmlNode.h>

#include <sim/utils/sim_String.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

XmlNode::XmlNode() :
    _node ( 0 )
{}

////////////////////////////////////////////////////////////////////////////////

XmlNode::XmlNode( const XmlNode &node ) :
    _file ( node._file ),
    _node ( node._node )
{}

////////////////////////////////////////////////////////////////////////////////

XmlNode::XmlNode( xmlNodePtr node, const std::string &file ) :
    _file ( file ),
    _node ( node )
{}

////////////////////////////////////////////////////////////////////////////////

std::string XmlNode::getErrorInfo() const
{
    std::string info;

    info.clear();

    if ( isValid() )
    {
        info += "Reading file: \"";
        info += getFile();
        info += "\" near line: ";
        info += String::toString( (int)getLine() );
        info += " failed.";
    }
    else
    {
        info += "Invalid node.";
    }

    return info;
}

////////////////////////////////////////////////////////////////////////////////

std::string XmlNode::getAttribute( const std::string &name ) const
{
    if ( hasAttributes() )
    {
        xmlAttrPtr attr = _node->properties;

        while ( attr != 0 )
        {
            if ( 0 == xmlStrcmp( attr->name, (const xmlChar*)name.c_str() ) )
            {
                if ( attr->children != 0 )
                {
                    xmlChar* value = xmlNodeListGetString( attr->children->doc, attr->children, 1);

                    std::string result( (const char*)value );

                    xmlFree( value );

                    return result;
                }
                else
                {
                    return std::string();
                }
            }

            attr = attr->next;
        }
    }

    return std::string();
}

////////////////////////////////////////////////////////////////////////////////

XmlNode::Attributes XmlNode::getAttributes() const
{
    Attributes attributes;
    attributes.clear();

    if ( hasAttributes() )
    {
        xmlAttrPtr attr = _node->properties;

        while ( attr != 0 )
        {
            if ( attr->children != 0 )
            {
                xmlChar* value = xmlNodeListGetString( attr->children->doc, attr->children, 1);

                std::string strName( (const char*)attr->name );
                std::string strValue( (const char*)value );

                attributes.insert( std::pair<std::string,std::string>( strName, strValue ) );

                xmlFree( value );
            }

            attr = attr->next;
        }
    }

    return attributes;
}

////////////////////////////////////////////////////////////////////////////////

XmlNode XmlNode::getFirstChild() const
{
    XmlNode result;

    if ( isValid() )
    {
        if ( _node->children != 0 )
        {
            result._node = _node->children;
            result._file = _file;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

XmlNode XmlNode::getFirstChildElement( const std::string &name ) const
{
    XmlNode result;

    if ( isValid() )
    {
        xmlNodePtr child = _node->children;

        while ( child != 0 )
        {
            if ( child->type == XML_ELEMENT_NODE )
            {
                if ( 0 == xmlStrcmp( child->name, (const xmlChar*)name.c_str() )
                  || name.length() == 0 )
                {
                    result._node = child;
                    result._file = _file;
                    return result;
                }
            }

            child = child->next;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::string XmlNode::getFileAndLine() const
{
    std::string result;

    result.clear();

    result += _file;
    result += "(";
    result += String::toString( (int)_node->line );
    result += ")";

    return result;
}

////////////////////////////////////////////////////////////////////////////////

XmlNode XmlNode::getNextSibling() const
{
    XmlNode result;

    if ( isValid() )
    {
        if ( _node->next != 0 )
        {
            result._node = _node->next;
            result._file = _file;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

XmlNode XmlNode::getNextSiblingElement( const std::string &name ) const
{
    XmlNode result;

    if ( isValid() )
    {
        xmlNodePtr next = _node->next;

        while ( next != 0 )
        {
            if ( next->type == XML_ELEMENT_NODE )
            {
                if ( 0 == xmlStrcmp( next->name, (const xmlChar*)name.c_str() )
                  || name.length() == 0 )
                {
                    result._node = next;
                    result._file = _file;
                    return result;
                }
            }

            next = next->next;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::string XmlNode::getText() const
{
    switch ( _node->type )
    {
    case XML_TEXT_NODE:
        return std::string( (const char*)_node->content );
        break;

    default:
        return std::string();
        break;
    }

    return std::string();
}
