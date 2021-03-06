// 
//   Copyright (C) 2018 Free Software Foundation, Inc.
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <unistd.h>
#include <vector>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>

#include "dejagnu.h"
#include "xml.h"

int verbosity;
static void usage (void);
bool waitforgdb = false;

TestState runtest;

void xmlnode_tests(void);
void xml_tests(void);

int
main(int argc, char *argv[])
{
    int c;
    bool dump = false;
    char buffer[300];
    std::string filespec;
    
    int retries = 3;

    memset(buffer, 0, 300);
    
    while ((c = getopt (argc, argv, "hdvsm:")) != -1) {
        switch (c) {
          case 'h':
            usage ();
            break;
            
          case 'd':
            dump = true;
            break;
            
          case 's':
            waitforgdb = true;
            break;
                                                                                
          case 'v':
            verbosity++;
            break;
            
          default:
            usage ();
            break;
        }
    }
    
    // get the file name from the command line
    if (optind < argc) {
        filespec = argv[optind];
        std::cout << "Will use \"" << filespec << "\" for test " << std::endl;
    }

    xml_tests();
}

void
xml_tests(void) {

    XML xml;

    std::string xmlfile = SRCDIR;
    xmlfile += "/test.xml";
    
    std:: cout << "Testing file parsing" << std::endl;
    if (xml.parseFile(xmlfile)) {
        runtest.pass ("XML::parseFile()");
    } else {
        runtest.fail ("XML::parseFile()");
    }
    
    std::string name = xml.nameGet();
    std::string value = xml.valueGet();
    if (name == "one" && value == "One") {
        runtest.pass ("XML::parseFile(name, value)");
    } else {
        runtest.fail ("XML::parseFile(name, value)");
    }

    if (xml.hasChildren()) {
        runtest.pass ("XML::hasChildren(file)");
    } else {
        runtest.fail ("XML::hasChildren(file)");
    }

    if (!xml.hasAttributes()) {
        runtest.pass ("XML::hasAttributes(file)");
    } else {
        runtest.fail ("XML::hasAttributes(file)");
    }

    // Get first child element
    XMLNode *child = xml[0];
    name = child->nameGet();
    value = child->valueGet();
    if (name == "two" && value == "Two") {
        runtest.pass ("XML[0]");
    } else {
        runtest.fail ("XML]0]");
    }
    
    if (child->hasAttributes() && (child->attribGet("foo") == "Bar")) {
        runtest.pass ("XML::attribGet(child)");
    } else {
        runtest.fail ("XML::attribGet(child)");
    }

    // there should only be one child, so this should return an error.
    if (xml[1] == 0) {
        runtest.pass ("XM[1]: out of range");
    } else {
        runtest.fail ("XML[1]: out of range");
    }
        
    child = child->childGet(0);
    if (child->nameGet() == "three2" && child->valueGet() == "Three2") {
        runtest.xpass ("XML[0][0]");
    } else {
        runtest.xfail ("XML[0][0]");
    }

    // Memory parsing
    std:: cout << "Testing memory parsing" << std::endl;
    std::string testxml = "<one2>One2<two2>Two2<three2 bar='foo'>Three2</three2></two2></one2>";
    if (xml.parseMem(testxml)) {
        runtest.pass ("XML::parseMem()");
    } else {
        runtest.fail ("XML::parseMem()");
    }
    if (xml.hasChildren()) {
        runtest.pass ("XML::hasChildren(mem)");
    } else {
        runtest.fail ("XML::hasChildren(mem)");
    }

    if (!xml.hasAttributes()) {
        runtest.pass ("XML::hasAttributes(mem)");
    } else {
        runtest.fail ("XML::hasAttributes(mem)");
    }
    // 
    name = xml.nameGet();
    value = xml.valueGet();
    if (name == "one2" && value == "One2") {
        runtest.pass ("XML::nameGet(mem)");
    } else {
        runtest.fail ("XML::nameGet(mem)");
        std::cerr << "FIXME2: " << name << " | " << value << std::endl;
    }

    // Get first child element
    child = xml[0];
    name = child->nameGet();
    value = child->valueGet();
    if (name == "two2" && value == "Two2") {
        runtest.pass ("XML::nameGet(child)");
    } else {
        runtest.fail ("XML::nameGet(child)");
    }
    
    if (!child->hasAttributes()) {
        runtest.pass ("XML::attribGet()");
    } else {
        runtest.fail ("XML::attribGet()");
    }

    // Get the child of the child
    child = child->childGet(0);
    if (child->nameGet() == "three2" && child->valueGet() == "Three2") {
        runtest.pass ("XML::nameGet(child)");
    } else {
        runtest.fail ("XML::nameGet(child)");
    }    
    if (child->hasAttributes() && (child->attribGet("bar") == "foo")) {
        runtest.pass ("XML::attribGet()");
    } else {
        runtest.fail ("XML::attribGet()");
    }
}

void
xmlnode_tests(void) {

    XMLNode node;
}

void
cntrlc_handler (int sig)
{
    std::cerr << "Got a ^C !" << std::endl;
}

void
alarm_handler (int sig)
{
    std::cerr << "Got an alarm signal !" << std::endl;
    std::cerr << "This is OK, we use it to end this test case." << std::endl;
}

static void
usage (void)
{
    std::vector<XMLNode> child;
    XMLNode foo;
    child.push_back(foo);
    const XMLNode &bar = child[0];
          
    std::cerr << "This program tests the XML processing code." << std::endl;
    std::cerr << "Usage: ./xml [h]" << std::endl;
    std::cerr << "-h\tHelp" << std::endl;
    std::cerr << "-d\tDump parsed data" << std::endl;
    exit (-1);
}

