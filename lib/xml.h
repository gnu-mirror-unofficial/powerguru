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

#ifndef __XML_H__
#define __XML_H__

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstring>
#include <vector>
#include <map>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>
#include "xml.h"

// This holds the data (name & value) for an XML node.
class XMLNode
{
public:
    XMLNode() {};
    ~XMLNode() {};

    XMLNode *childGet(int x) { return _children[x]; }
    XMLNode *operator [] (int x) {
        return (x < _children.size())? _children[x]: 0;
    }
    
    void operator = (XMLNode &node);

    const std::string &nameGet(void) { return _name; }
    void nameSet(const std::string &name) { _name = name; }
    const std::string &valueGet(void) { return _value; }
    void valueSet(const std::string &val) { _value = val; }
    void childAdd(XMLNode *node) { _children.push_back(node);} ;
    std::string &attribGet(const std::string &name) { return _attributes[name]; };
    void attribAdd(const std::string &name, const std::string &value) {
        _attributes[name] = value;
    }

    int childrenSize(void) { return _children.size(); };
    bool hasContent(void);
    bool hasChildren(void) { return (_children.size() > 0)? true : false; };
    bool hasAttributes(void) { return (_attributes.size() > 0)? true : false;};
private:
    std::string _name;
    std::string _value;
    std::vector<XMLNode *> _children;
    std::map<std::string, std::string> _attributes;
};

// This is the top level for parsing an XML network message or file.
class XML {
public:
    XML() {} ;
    XML(const std::string &xml_in) {};
    //XML(struct node * childNode) {};
    ~XML() {};

    // bool parseRoot(xmlDocPtr document);
    bool parseMem(const std::string &ml_in);
    bool parseFile(const std::string &filespec);

    const std::string &nameGet(void) { return _nodes->nameGet(); }
    const std::string &valueGet(void) { return _nodes->valueGet(); }
    void nameSet(const std::string &name) { _nodes->nameSet(name); }
    bool hasChildren(void) {  return _nodes->hasChildren(); }
    bool hasAttributes(void) { return _nodes->hasAttributes(); }
    //const std::string &nodeNameGet(void) { return nameGet(); }

    XMLNode *operator [] (int x) {
        // range check the index for better error handling
        return (x < _nodes->childrenSize())? _nodes->childGet(x): 0;
    };
    //const XML &operator = (const XMLNode &node);
private:
    XMLNode *extractNode(xmlNodePtr node);
    xmlDocPtr     _doc;
    XMLNode       *_nodes;
};

int memadjust(int x);

#endif	// __XML_H__

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
