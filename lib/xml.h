// 
// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011
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
#ifdef HAVE_LIBXML
# include <libxml/xmlmemory.h>
# include <libxml/parser.h>
# include <libxml/xmlreader.h>
#endif

#include "log.h"
#include "err.h"

// This holds the data (name & value) for an XML node's attributes.
class XMLAttr {
public:
  XMLAttr();
  ~XMLAttr();
  const xmlChar *nameGet(void);
  void nameSet(const xmlChar *name);
  const xmlChar *valueGet(void);
  void valueSet(const xmlChar *val);
 private:
  const xmlChar *_name;
  const xmlChar *_value;
};

// This holds the data (name & value) for an XML node.
class XMLNode
{
public:
  XMLNode();
  ~XMLNode();

  int size(void) { return _children.size(); }
  XMLNode *operator [] (int x);
  XMLNode *operator = (XMLNode &node);
  XMLNode *operator = (XMLNode *node);

  const xmlChar *nameGet(void);
  //  void nameSet(std::string name);
  void nameSet(const xmlChar *name);
  const xmlChar *valueGet(void);
  //  void valueSet(std::string val);
  void valueSet(const xmlChar *val);
  
  std::vector<XMLNode *> childrenGet(void);
  XMLNode *childGet(int x);
  void childAdd(XMLNode *node);
  
  std::vector<XMLAttr *> attributesGet(void);
  XMLAttr *attribGet(int x);
  void attribAdd(XMLAttr *attr);
  int childrenSize(void);
  int attributesSize(void);
  bool hasContent(void);
  bool hasChildren(void);
  bool hasAttributes(void);
 private:
  const xmlChar         *_name;
  const xmlChar         *_value;
  std::vector<XMLNode *> _children;
  std::vector<XMLAttr *> _attributes;
};

// This is the top level for parsing an XML network message or file.
class XML {
 public:
  XML();
  XML(std::string xml_in);
  XML(struct node * childNode);
  ~XML();

  bool parseDoc(xmlDocPtr document, bool mem);
  bool parseXML(std::string xml_in);
  bool load(const char *filespec);
  bool hasChildren(void);
  XMLNode *extractNode(xmlNodePtr node, bool mem);
  XMLNode *processNode(xmlTextReaderPtr reader, XMLNode *node);
  const xmlChar *nodeNameGet(void);
  int size(void);  
  XMLNode *operator [] (int x);
  XML *operator = (XMLNode *node);
private:
  xmlDocPtr     _doc;
  XMLNode       _nodes;
};

int memadjust(int x);

#endif	// __XML_H__

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
