// 
// Copyright (C) 2005, 2006 - 2018
//      Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <cstring>
#include <vector>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>

#include "xml.h"
#include "log.h"

extern LogFile dbglogfile;

// XMLNode methods. An XMLNode holds all the info for an XML node,
// including it's children and attributes.
// XMLNode::XMLNode()
// {
//     // DEBUGLOG_REPORT_FUNCTION;
// }

// XMLNode::~XMLNode()
// {
//     // DEBUGLOG_REPORT_FUNCTION;
//     unsigned int i;
// }

// Convert a raw XML node to a C++ representation
XMLNode *
XML::extractNode(xmlNodePtr node)
{
    DEBUGLOG_REPORT_FUNCTION;
    xmlAttrPtr attr;
    xmlNodePtr childnode;
    xmlChar *ptr = NULL;
    XMLNode *child;
    int len;

    // dbglogfile << "\rCreated new element for " << (const char *)node->name << " at " << element << std::endl;

    dbglogfile << "extracting node " << node->name << std::endl;

    XMLNode *xml = new XMLNode;
    std::string name;
    std::string value;

    // See if we have any Attributes (properties)
    if (attr = node->properties) {
        // extract all the attributes for this node
        while (attr != NULL) {
            name = reinterpret_cast<const char *>(attr->name);
            value = reinterpret_cast<const char *>(attr->children->content);
            xml->attribAdd(name, value);
            attr = attr->next;
#if 0
            dbglogfile << "FIXME: attribute " << node->name
                       << " has property "
                       << name << " value is "
                       << value << std::endl;
#endif
        }
    }

    if (node->name != 0) {
        name = reinterpret_cast<const char *>(node->name);
        xml->nameSet(name);
    }
    if (node->children) {
        // Sometimes the content is at the end of a line, so remove the
        // carriage return and trailing garbage.
        if (node->children->content != 0) {
            value = reinterpret_cast<char *>(node->children->content);
            xml->valueSet(value.substr(0, value.find('\n')));
        }
        
        ptr = xmlNodeGetContent(node->children);        
        if (ptr != NULL) {
            //value = reinterpret_cast<const char *>(node->children->content);
            value = reinterpret_cast<const char *>(ptr);
#if 1
            dbglogfile << "\tChild node: " << name
                       << " has contents " << value << std::endl;
#endif
            xml->valueSet(value.substr(0, value.find('\n')));
            xmlFree(ptr);
        }
    }

    // See if we have any data (content)
    childnode = node->children;

    while (childnode != NULL) {
        if (childnode->type == XML_ELEMENT_NODE) {
            dbglogfile << "\tfound node " << (const char *)childnode->name << std::endl;
            XMLNode *child = extractNode(childnode);
            //if (child->_value.get_type() != as_value::UNDEFINED) {
#if 1
            dbglogfile << "\tPushing child Node " << child->nameGet()
                       << " value " << child->valueGet()
                       <<  " on element "
                       <<  xml->nameGet() << std::endl;
#endif
            xml->childAdd(child);
        }
        childnode = childnode->next;
    }

    return xml;
}

// This reads in an XML file from disk and parses into into a memory resident
// tree which can be walked through later.
bool
XML::parseMem(const std::string &xml_in)
{
    DEBUGLOG_REPORT_FUNCTION;
    bool          ret = true;

    dbglogfile << "Parse XML from memory: " << xml_in.c_str() << std::endl;

    if (xml_in.size() == 0) {
        dbglogfile << "ERROR: XML data is empty!" << std::endl;
        return false;
    }

    xmlInitParser();
  
    _doc = xmlParseMemory(xml_in.c_str(), xml_in.size());
    if (_doc == 0) {
        dbglogfile << "ERROR: Can't parse XML data!" << std::endl;
        return false;
    }
    _nodes = extractNode(xmlDocGetRootElement(_doc));
    xmlCleanupParser();
    xmlFreeDoc(_doc);
    xmlMemoryDump();

    return ret;  
}

//     XML_READER_TYPE_NONE = 0
//     XML_READER_TYPE_ELEMENT = 1,
//     XML_READER_TYPE_ATTRIBUTE = 2,
//     XML_READER_TYPE_TEXT = 3,
//     XML_READER_TYPE_COMMENT = 8,
//     XML_READER_TYPE_SIGNIFICANT_WHITESPACE = 14,
//     XML_READER_TYPE_END_ELEMENT = 15,
//
// processNode:
// 2 1 IP 0
// processNode:
// 3 3 #text 0 192.168.2.50
// processNode:
// 2 15 IP 0
// processNode:
// 2 14 #text 0
const char *tabs[] = {
    "",
    "\t",
    "\t\t",
    "\t\t\t",
    "\t\t\t",
    "\t\t\t\t",
};

// This reads in an XML file from disk and parses into into a memory resident
// tree which can be walked through later.
bool
XML::parseFile(const std::string &filespec)
{
    DEBUGLOG_REPORT_FUNCTION;
    dbglogfile << "Load disk XML file: " << filespec << std::endl;
  
    //dbglogfile << %s: mem is %d\n", __FUNCTION__, mem);

    xmlInitParser();
    _doc = xmlParseFile(filespec.c_str());
    if (_doc == 0) {
        dbglogfile << "ERROR: Can't load XML file: " << filespec << std::endl;
        return false;
    }
    //_nodes = extractNode(xmlDocGetRootElement(_doc));
    _nodes = extractNode(xmlDocGetRootElement(_doc));
    xmlCleanupParser();
    xmlFreeDoc(_doc);
    xmlMemoryDump();

    return true;
}

// const XMLNode &
// XML::operator [] (int x) {
//     // DEBUGLOG_REPORT_FUNCTION;
//     return _nodes->childGet(x);
// }

// bool
// XML::hasChildren(void)
// {
//     return (_nodes->childrenSize() > 0)? true : false;
// }

// const std::string &
// XML::nodeNameGet(void)
// {
//     return _nodes->nameGet();
// }

void
XMLNode::operator = (XMLNode &node)
{
    _name = node.nameGet();
}

int
memadjust(int x)
{
    // DEBUGLOG_REPORT_FUNCTION;
    return (x + (4 - x % 4));
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
