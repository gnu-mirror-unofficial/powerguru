// 
//   Copyright (C) 2005 Free Software Foundation, Inc.
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

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <map>
#ifdef __STDC_HOSTED__
#include <sstream>
#else
#include <strstream>
#endif
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/debugXML.h>
#include "database.h"
#include "log.h"
#include "err.h"
#include "xml.h"
#include "msgs.h"
#include "tcputil.h"
#include "tcpip.h"

using namespace std;
Msgs::net_mode_e  Msgs::_net_mode;
std::map<const xmlChar *, Msgs::methodPtr_t> Msgs::_methods;
std::map<const xmlChar *, std::string> Msgs::_cache;

#define MY_ENCODING "ISO-8859-1"

Msgs::Msgs()
{
  // DEBUGLOG_REPORT_FUNCTION;
  Tcputil tu;
  tu.hostDataGet();
  
  _thisip = tu.hostIPNameGet();
  _thishost = tu.hostNameGet();

  _version = atof(VERSION);
}

Msgs::Msgs(std::string host, std::string ip) // : _net_mode(NONET)
{
  // DEBUGLOG_REPORT_FUNCTION;
  Tcputil tu;
  tu.hostDataGet();
  
  _thisip = tu.hostIPNameGet();
  _thishost = tu.hostNameGet();
  
  _remoteip = ip;
  _remotehost = host;

  _version = atof(VERSION);
}

Msgs::Msgs(Tcpip *tcpip)//  : _net_mode(NONET)
{
  // DEBUGLOG_REPORT_FUNCTION;
  Tcputil tu;
  tu.hostDataGet();
  
  _thisip = tu.hostIPNameGet();
  _thishost = tu.hostNameGet();
  
  //  _remoteip = ip;
  //  _remotehost = host;
  
  _version = atof(VERSION);
}

retcode_t
Msgs::init(net_mode_e mode)
{
  DEBUGLOG_REPORT_FUNCTION;
  string hostname = "localhost";
  bool block = true;
  
  if (mode == CLIENT) {
    _net_mode = CLIENT;
    if (createNetClient(hostname)) {
      dbglogfile << "Connected to server at " << hostname.c_str() << endl;
      init();                     // initialize the table of pointers
      return SUCCESS;
    } else {
      dbglogfile << "ERROR: Couldn't create connection to server" << hostname.c_str()  << endl;
    }
  } else if (mode == DAEMON) {
    _net_mode = DAEMON;
    init(DAEMON, block);
    dbglogfile << "WARNING: ignoring hostname, starting as daemon" << endl;
  } else {
    dbglogfile << "ERROR: no mode specified! " << endl;
    _net_mode = NONET;
  }
  return ERROR;  
}

// If a hostname is specifed, we force client mode.
retcode_t
Msgs::init(net_mode_e mode, std::string hostname)
{
  DEBUGLOG_REPORT_FUNCTION;
  return init(hostname);
}

// By default, if just a hostname is supplied, we assume it's to establish
// a network connection to the specified host.
retcode_t
Msgs::init(std::string hostname)
{
  DEBUGLOG_REPORT_FUNCTION;
  
  if (createNetClient(hostname)) {
    dbglogfile << "Connected to server at " << hostname.c_str() << endl;
    init();                     // initialize the table of pointers
    _net_mode = CLIENT;
    writeNet(heloCreate(_version));
    return SUCCESS;
  } else {
    dbglogfile << "ERROR: Couldn't create connection to server" << hostname  << endl;
  }
  return ERROR;
}

retcode_t
Msgs::init(net_mode_e mode, bool block)
{
  DEBUGLOG_REPORT_FUNCTION;
  string hostname = "localhost";
  if (mode == CLIENT) {
    _net_mode = CLIENT;
    if (createNetClient(hostname)) {
      dbglogfile << "Connected to server at " << hostname.c_str() << endl;
      init();                     // initialize the table of pointers
      return SUCCESS;
    } else {
      dbglogfile << "ERROR: Couldn't create connection to server" << hostname.c_str()  << endl;
    }
  } else if (mode == DAEMON) {
    _net_mode = DAEMON;
    init(true);
    dbglogfile << "WARNING: ignoring hostname, starting as daemon" << endl;
  } else {
    dbglogfile << "ERROR: no mode specified! " << endl;
    _net_mode = NONET;
  }
  return ERROR;
}
               
retcode_t
Msgs::init(bool block)
{
  DEBUGLOG_REPORT_FUNCTION;
  _net_mode = DAEMON;
  
  if (createNetServer()) {
    dbglogfile << "New server started for remote client." << endl;
    init();                     // initialize the table of pointers
  } else {
    dbglogfile << "ERROR: Couldn't create a new server" << endl;
  }      
  
  if (newNetConnection(block)) {
    dbglogfile << "New connection started for remote client." << endl;
    _net_mode = DAEMON;
    writeNet(heloCreate(_version));
    return SUCCESS;
  } else {
    dbglogfile << "ERROR: Couldn't create a new connection!" << endl;
  }
  return ERROR;
}

  
retcode_t
Msgs::init(void)
{
  DEBUGLOG_REPORT_FUNCTION;

  // Top level node of the message
  _methods[BAD_CAST "powerguru"] =      &Msgs::powerguruProcess;
  
    // initialization message
  _methods[BAD_CAST "helo"] =           &Msgs::heloProcess;
  _methods[BAD_CAST "client"] =         &Msgs::clientProcess;
  _methods[BAD_CAST "server"] =         &Msgs::serverProcess;
  
  // Meter readings
  _methods[BAD_CAST "meters"] =         &Msgs::metersProcess;
  _methods[BAD_CAST "charge-mps"] =     &Msgs::chargeAmpsProcess;
  _methods[BAD_CAST "load-amps"] =      &Msgs::loadAmpsProcess;
  _methods[BAD_CAST "pv-amps"] =        &Msgs::pvAmpsProcess;
  _methods[BAD_CAST "pv-volts"] =       &Msgs::pvVoltsProcess;
  _methods[BAD_CAST "daily-kwh"] =      &Msgs::dailyKwhProcess;
  _methods[BAD_CAST "hertz"] =          &Msgs::hertzProcess;
  _methods[BAD_CAST "battery-volts"] =  &Msgs::batteryVoltsProcess;
  _methods[BAD_CAST "buy-amps"] =       &Msgs::buyAmpsProcess;
  _methods[BAD_CAST "sell-amps"] =      &Msgs::sellAmpsProcess;
  _methods[BAD_CAST "ac-volts-out"] =   &Msgs::acVoltsOutProcess;
  _methods[BAD_CAST "ac1-volts-in"] =   &Msgs::ac1InProcess;
  _methods[BAD_CAST "ac2-volts-in"] =   &Msgs::ac2InProcess;
  
  // Status messages on the system
  _methods[BAD_CAST "status"] = &Msgs::statusProcess;
  _methods[BAD_CAST "version"] = &Msgs::statusProcess;
  _methods[BAD_CAST "revision"] = &Msgs::statusProcess;
  _methods[BAD_CAST "opmode"] = &Msgs::statusProcess;
  _methods[BAD_CAST "errormode"] = &Msgs::statusProcess;
  _methods[BAD_CAST "warningmode"] = &Msgs::statusProcess;

  // Configuration settings
  _methods[BAD_CAST "config"] = &Msgs::configProcess;
  //  _methods[BAD_CAST "generator"] = &Msgs::config;
  //  _methods[BAD_CAST "grid"] = &Msgs::config;
  //  _methods[BAD_CAST "buy"] = &Msgs::config;
  //  _methods[BAD_CAST "sell"] = &Msgs::config;
  //  _methods[BAD_CAST "start"] = &Msgs::config;
  //  _methods[BAD_CAST "end"] = &Msgs::config;

  // Command messages
  _methods[BAD_CAST "command"] = &Msgs::commandProcess;
//   _methods[BAD_CAST "auxilary"] = &Msgs::commandAuxilaryProcess;
//   _methods[BAD_CAST "relay"] = &Msgs::commandRelayProcess;
//   _methods[BAD_CAST "poll"] = &Msgs::commandPollProcess;;
//   _methods[BAD_CAST "inverter"] = &Msgs::commandInverterProcess;;
//   _methods[BAD_CAST "charger"] = &Msgs::commandChargerProcess;;
  // _methods[BAD_CAST "restart"] = &Msgs::commandRestartProcess;;

  //  _methods[""] = &Msgs::heloProcess;

  // preload a few values
  if (_net_mode == DAEMON) {
    _cache[BAD_CAST "version"] = VERSION;
    _cache[BAD_CAST "revision"] = VERSION;
//     _cache[BAD_CAST "opmode"] = "unknown";
//     _cache[BAD_CAST "errormode"] = "unknown";
//     _cache[BAD_CAST "warningmode"] = "unknown";

    // FIXME: fake meter values!
    // Meters
    _cache[BAD_CAST "battery-volts"] = "88888888";
    _cache[BAD_CAST "charge-amps"] = "777777777";
    _cache[BAD_CAST "load-amps"] = "66666666";
    _cache[BAD_CAST "pv-amps"] = "55555555";
    _cache[BAD_CAST "sell-amps"] = "444444444444";
  }
  
  return SUCCESS;
}

std::string
Msgs::cacheGet(const xmlChar * name) {
  // DEBUGLOG_REPORT_FUNCTION;
#if 1
  const xmlChar         *tag;
  string                str;
  
  _body.str("");
  std::map<const xmlChar *, string>::const_iterator it;
  for (it = _cache.begin(); it != _cache.end(); it++) {
    //entry = it->second;
    tag = it->first;
    str  = it->second;
    _body.str("");
    if (xmlStrcmp(tag, name) == 0) {
      if (str.size() != 0) {
        _body << " Has data: " << str;
      } else {
        dbglogfile << " doesn't have data";
        return _body.str();
      }
      return str;
    }
    dbglogfile << "Looking for cache value for XML Tag \"" << tag
               << "\" has " << _body.str().c_str() << endl;
  }
  return _body.str();
#else
  return _cache[name];
#endif
}

retcode_t
Msgs::cacheAdd(const xmlChar * name, string str)
{
  // DEBUGLOG_REPORT_FUNCTION;
  _cache[name] = str;

  // FIXME: we should make sure this actually worked.
  return SUCCESS;
}


// Add a function for handling an XML tag to the list.
void
Msgs::methodSet(const xmlChar * name, methodPtr_t func)
{
  // DEBUGLOG_REPORT_FUNCTION;
  _methods[name] = func;
}

// Get the function for an XML tag from the list.
Msgs::methodPtr_t
Msgs::methodGet(const xmlChar * name)
{
  //DEBUGLOG_REPORT_FUNCTION;
#if 1
  const xmlChar         *str;
  Msgs::methodPtr_t     ptr;
  
  std::map<const xmlChar *, Msgs::methodPtr_t>::const_iterator it;
  for (it = _methods.begin(); it != _methods.end(); it++) {
    //entry = it->second;
    str = it->first;
    ptr  = it->second;
    _body.str("");
    if (ptr != 0) {
      _body << " a function pointer";
    } else {
      _body << " doesn't have a function pointer";
    }
    //     dbglogfile << "Looking for method for XML Tag \"" << name.c_str()
    //                << "\" has " << _body.str().c_str() << endl;
    if (xmlStrcmp(str, name) == 0) {
      return ptr;
    }
  }
#else
    dbglogfile << "\"" << name << "\" method we want" << endl;
    return _methods[name];
#endif
}

// Call the function to process an XML node
retcode_t
Msgs::methodProcess(const xmlChar *name, XMLNode *node)
{
  // DEBUGLOG_REPORT_FUNCTION;
  //(this->*_methods.find(name)(node); 
  return (this->*_methods[name])(node);
}

// Dump all the pointer to methods for each XML Tag
void
Msgs::methodsDump(void)
{
  DEBUGLOG_REPORT_FUNCTION;
  const xmlChar         *name;
  Msgs::methodPtr_t     ptr;
  
  dbglogfile << "We have " << _methods.size() << " in function table" << endl;
  
  std::map<const xmlChar *, Msgs::methodPtr_t>::const_iterator it;
  for (it = _methods.begin(); it != _methods.end(); it++) {
    name = it->first;
    ptr  = it->second;
    _body.str("");
    if (ptr != 0) {
      _body << "a method pointer";
    } else {
      _body << "no pointer to method";
    }
    dbglogfile << "XML Tag \"" << name
               << "\" has " << _body.str().c_str() << endl;
  }
}

// Dump all the pointer to methods for each XML Tag
void
Msgs::cacheDump(void)
{
  DEBUGLOG_REPORT_FUNCTION;
  const xmlChar         *name;
  string                data;
  
  dbglogfile << "We have " << _cache.size() << " items in the cache" << endl;
  
  std::map<const xmlChar *, string>::const_iterator it;
  for (it = _cache.begin(); it != _cache.end(); it++) {
    name = it->first;
    data  = it->second;
    _body.str("");
    if (data.size() != 0) {
      _body << "data is " << data;
    } else {
      _body << "no data";
    }
    dbglogfile << "XML Tag \"" << name
               << "\" has " << _body.str().c_str() << endl;
  }
}

Msgs::~Msgs()
{
  // DEBUGLOG_REPORT_FUNCTION;
  
}

void
Msgs::process(XMLNode *xml)
{
  DEBUGLOG_REPORT_FUNCTION;
  methodPtr_t   fptr;
  const xmlChar *str;
  retcode_t     ret;
  int           i;

  str = xml->nameGet();
  fptr = methodGet(str);
  //methodsDump();
  
  if (fptr != 0) {
    dbglogfile << "XML tag \"" << str << "\" has method pointer" << endl;
    ret = (this->*fptr)(xml);
  } else {
    dbglogfile << "WARNING: XML tag \"" << str << "\" doesn't have a method pointer" << endl;
  }

  if (ret < 0) {
    dbglogfile << "ERROR: Got an error from executing function pointer!" << endl;
  } else {    
    dbglogfile << "executed function pointer successfully!" << endl;
  }
  
  // Process the children too
  for (i=0; i<xml->childrenSize(); i++) {
    process(xml->childGet(i));
  }  
}

void
Msgs::dump(XMLNode *xml)
{
  // DEBUGLOG_REPORT_FUNCTION;
  int           child,  i;
  XMLNode       *childnode;

  dbglogfile << "processing node " << xml->nameGet() << endl;

  // Process the node's value, if it has one.
  if (xml->valueGet() == 0) {
    dbglogfile << "No content for node " << xml->nameGet() << endl;
  } else {
    dbglogfile << "Content for node " << xml->nameGet()
               << " is " << xml->valueGet() << endl;
  }
  
  // Process the attributes, if any
  if (xml->attributesSize() == 0) {
    dbglogfile << "\tNo attributes for node " << xml->nameGet() << endl;
  } else {
    for (i=0; i<xml->attributesSize(); i++) {
      dbglogfile << "\tAttribute is " << xml->attribGet(i)->nameGet()
                 << " who's value is " << xml->attribGet(i)->valueGet() << endl;
    }
  }

  // Process the children, if there are any
  if (xml->childrenSize()) {
    dbglogfile << "\tProcessing " << xml->childrenSize() << " children nodes for "
               << xml->nameGet() << endl;
    for (child=0; child<xml->childrenSize(); child++) {
      childnode = xml->childGet(child);
      dump(childnode); // setup child node
    }
  } else {
    dbglogfile << "Node " << xml->nameGet() << " has no children" << endl;
  }
}

// These format client side messages to the daemon
string
Msgs::statusCreate(meter_data_t *md)
{
  DEBUGLOG_REPORT_FUNCTION;
#if 1
  _body.str("");                // erase the current string
  _body << "<powerguru version=\"";
  _body << _version << "\">";
  _body << "<status>" << "</status>"; 
  _body << "</powerguru>";
  _body << ends;
  
  return _body.str();
#else
  int rc;
  xmlTextWriterPtr writer;
  xmlBufferPtr buf;
  xmlChar *tmp;
  
  if ((buf = xmlBufferCreate()) == NULL) {
    printf("testXmlwriterMemory: Error creating the xml buffer\n");
    return "";
  }
  
  if ((writer = xmlNewTextWriterMemory(buf, 0)) == NULL) {
    printf("testXmlwriterMemory: Error creating the xml writer\n");
    return "";
  }
  
  /* Start the document with the xml default for the version,
   * encoding ISO 8859-1 and the default for the standalone
   * declaration. */
  if ((rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL)) < 0) {
    printf("testXmlwriterMemory: Error at xmlTextWriterStartDocument\n");
    return "";
  }
  
  if ((rc = xmlTextWriterStartElement(writer, BAD_CAST "powerguru")) < 0) {
    printf
      ("testXmlwriterMemory: Error at xmlTextWriterStartElement\n");
    return "";
  }
  
  if ((rc = xmlTextWriterStartElement(writer, BAD_CAST "status")) < 0) {
    printf
      ("testXmlwriterMemory: Error at xmlTextWriterStartElement\n");
    return "";
  }
    
  /* Add an attribute with name "version" and value "1.0" to status. */
  if ((rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "version",
                                          BAD_CAST "1.0")) < 0) {
    printf
      ("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
    return "";
  }
  
  // end status tag
  if ((rc = xmlTextWriterEndElement(writer)) < 0) {
    printf("testXmlwriterMemory: Error at xmlTextWriterEndElement\n");
    return "";
  }
  
  /* Close the element named FOOTER. */
  if ((rc = xmlTextWriterEndElement(writer)) < 0) {
    printf("testXmlwriterMemory: Error at xmlTextWriterEndElement\n");
    return "";
  }
  
  if ((rc = xmlTextWriterEndDocument(writer)) < 0) {
    printf("testXmlwriterMemory: Error at xmlTextWriterEndDocument\n");
    return "";
  }
  
  xmlFreeTextWriter(writer);
  
  //    cerr << "Buffer says: " << (const char *) buf->content << endl;
  string str = (const char *) buf->content;
  xmlBufferFree(buf);
  return str;
#endif  
}

// Say "helo" to the connecting program to establish the messaging
// system. Both ends of the connection use this string to make sure
// they are speaking the same version of the protocol.
string
Msgs::heloCreate(float version)
{
  DEBUGLOG_REPORT_FUNCTION;
  _body.str("");                // erase the current string
  _body << "<powerguru version=\"";
  _body << _version << "\">";
  _body << "<client ip=\"" << _remoteip << "\">" << _remotehost << "</client>"; 
  _body << "<server ip=\"" << _thisip   << "\">" << _thishost   << "</server>"; 
  _body << "</powerguru>";
  _body << ends;

  return _body.str();
}

string
Msgs::metersRequestCreate(std::string str) {
  DEBUGLOG_REPORT_FUNCTION;
  _body.str("");                // erase the current string
  _body << "<powerguru version=\"";
  _body << _version << "\">";
  _body << "<meters>" << str.c_str() << "</meters>"; 
  _body << "</powerguru>";
  _body << ends;

  return _body.str();  
}

string
Msgs::metersRequestCreate(xml_meters_e val) {
  DEBUGLOG_REPORT_FUNCTION;
  string str;

  switch (val) {
  case CHARGE_AMPS:
    str = "charge-amps";
    break;
  case AC_LOAD_AMPS:
    str = "load-amps";
    break;
  case BATTERY_VOLTS:
    str = "battery-volts";
    break;
  case AC_VOLTS_OUT:
    str = "ac-volts-out";
    break;
  case AC1_VOLTS_IN:
    str = "ac1-volts-in";
    break;
  case AC2_VOLTS_IN:
    str = "ac2-volts-in";
    break;
  case PV_AMPS_IN:
    str = "pv-amps-in";
    break;
  case PV_VOLTS_IN:
    str = "pv-volts-in";
    break;
  case BUY_AMPS:
    str = "buy-amps";
    break;
  case SELL_AMPS:
    str = "sell-amps";
    break;
  case DAILY_KWH:
    str = "daily-kwh";
    break;
  case HERTZ:
    str = "hertz";
    break;
  case TEMPCOMP_VOLTS:
    str = "tempcomp";
    break;
  default:
    break;                      // you should never be
  };

  return metersRequestCreate(str);
}

std::string
Msgs::metersResponseCreate(const xmlChar * type, int val) {
  DEBUGLOG_REPORT_FUNCTION;
  _body.str("");                // erase the current string
  _body << "<powerguru version=\"";
  _body << _version << "\">";
  _body << "<meters><" << (const char *)type << ">";
  _body << val;
  _body << "</" << (const char *)type << ">" << "</meters>"; 
  _body << "</powerguru>";
  _body << ends;

  return _body.str();  
}

std::string
Msgs::metersResponseCreate(const xmlChar *type, float val) {
  DEBUGLOG_REPORT_FUNCTION;
  _body.str("");                // erase the current string
  _body << "<powerguru version=\"";
  _body << _version << "\">";
  _body << "<meters><" << (const char *)type << ">";
  _body << val;
  _body << "</" << (const char *)type << ">" << "</meters>"; 
  _body << "</powerguru>";
  _body << ends;

  return _body.str();  
}

std::string
Msgs::metersResponseCreate(const xmlChar *type, string val) {
  DEBUGLOG_REPORT_FUNCTION;
  _body.str("");                // erase the current string
  _body << "<powerguru version=\"";
  _body << _version << "\">";
  _body << "<meters><" << (const char *)type << ">";
  _body << val;
  _body << "</" << (const char *)type << ">" << "</meters>"; 
  _body << "</powerguru>";
  _body << ends;

  return _body.str();  
}

std::string
Msgs::requestCreate(xml_meters_e tag)
{
  DEBUGLOG_REPORT_FUNCTION;
  _body.str("");                // erase the current string
  _body << "<powerguru version=\"" << _version << "\"><meters>";
  switch (tag) {
  case CHARGE_AMPS:
    _body << "charge-amps";
    break;
  case AC_LOAD_AMPS:
    _body << "load-amps";
    break;
  case BATTERY_VOLTS:
    _body << "battery-volts";
    break;
  case AC_VOLTS_OUT:
    _body << "ac-volts-out";
    break;
  case AC1_VOLTS_IN:
    _body << "ac1-volts-in";
    break;
  case AC2_VOLTS_IN:
    _body << "ac2-volts-in";
    break;
  case PV_AMPS_IN:
    _body << "pv-amps-in";
    break;
  case PV_VOLTS_IN:
    _body << "pv-volts-in";
    break;
  case BUY_AMPS:
    _body << "buy-amps";
    break;
  case SELL_AMPS:
    _body << "sell-amps";
    break;
  case DAILY_KWH:
    _body << "daily-kwh";
    break;
  case HERTZ:
    _body << "hertz";
    break;
  case TEMPCOMP_VOLTS:
    _body << "tempcomp";
    break;
  default:
    break;                      // you should never be
  };
  _body << "</meters></powerguru>";
  _body << ends;

  return _body.str();  
}

string
Msgs::requestCreate(xml_status_e tag)
{
  DEBUGLOG_REPORT_FUNCTION;

  _body.str("");                // erase the current string
  _body << "<powerguru version=\"" << _version << "\"><status>";
  switch (tag) {
  case SYSVERSION:
    _body << "sysversion";
    break;
  case REVISION:
    _body << "revision";
    break;
  case ERRORMODE:
    _body << "errormode";
    break;
  case WARNINGMODE:
    _body << "warningmode";
    break;
  case OPMODE:
    _body << "opmode";
    break;
  default:
    break;
  };
  _body << "</status></powerguru>";

  _body << ends;  

  return _body.str();  
}

string
Msgs::requestCreate(xml_config_e tag)
{
  DEBUGLOG_REPORT_FUNCTION;  

  _body.str("");                // erase the current string
  _body << "<powerguru version=\"" << _version << "\"><config>";
  switch (tag) {
  case GENCONFIG:
    _body << "generator";
    break;
  case GRIDCONFIG:
    _body << "grid";
    break;
  case BUY:
    _body << "buy";
    break;
  case SELL:
    _body << "sell";
    break;
  case CHARGE:
    _body << "charge";
    break;
  default:
    break;
  };
  _body << "</config></powerguru>";

  _body << ends;  

  return _body.str();  
}

string
Msgs::requestCreate(xml_command_e tag)
{
  DEBUGLOG_REPORT_FUNCTION;

  _body.str("");                // erase the current string
  _body << "<powerguru version=\"" << _version << "\"><status>";
  switch (tag) {
  case GENERATOR:
    _body << "generator";
    break;
  case GRID:
    _body << "grid";
    break;
  case RELAY:
    _body << "relay";
    break;
  case AUXILARY:
    _body << "auxilary";
    break;
  case POLL:
    _body << "poll";
    break;
  case INVERTER:
    _body << "inverter";
    break;
  case CHARGER:
    _body << "charger";
    break;
  default:
    break;
  };
  _body << "</status></powerguru>";

  _body << ends;  

  return _body.str();  
}


std::string
Msgs::responseCreate(xml_msg_e type, const xmlChar *tag, string val)
{
  DEBUGLOG_REPORT_FUNCTION;
  string str;
  
  switch (type) {
  case HEARTBEAT:
    str = "heartbeat";
    break;
  case STATUS:
    str = "status";
    break;
  case METER:
    str = "meters";
    break;
  case CONFIG:
    str = "meters";
    break;
  case COMMAND:
    str = "meters";
    break;
  };
  
  _body.str("");                // erase the current string
  _body << "<powerguru version=\"";
  _body << _version << "\"><" << str;
  _body << "><" << (const char *)tag << ">";
  _body << val;
  _body << "</" << (const char *)tag << "></" << str; 
  _body << "></powerguru>";
  _body << ends;

  return _body.str();  
}

string
Msgs::configCreate(string tag, int value)
{
  DEBUGLOG_REPORT_FUNCTION;
  _body.str("");                // erase the current string
  _body << "<powerguru>";
  _body << "<" << tag << ">" << value << "</" << tag << ">";
  _body << "</powerguru>";
  _body << ends;
  
  return _body.str();
}

void
Msgs::print_msg(std::string msg)
{
  // DEBUGLOG_REPORT_FUNCTION;
  int level = 1;
  int start = 0;
  size_t pos;
  string str, prefix, newmsg;
  const char *tabs[] = {
    "\r\n\t",
    "\r\n\t\t",
    "\r\n\t\t\t",
    "\r\n\t\t\t\t",
    "\r\n\t\t\t\t",
    "\r\n\t\t\t\t\t",
  };
  
  xmlDebugDumpString(stderr, (const xmlChar *)msg.c_str());
  //  cerr << "++++++++++++++++++++++" << endl;
  
  // Strip off the DTD header, as we're not bothering to validate
  // our XML.
  if ((pos = msg.find("?>\n", start)) != string::npos) {
    newmsg = msg.substr(pos+3, msg.size());
  } else {
    newmsg = msg;
  }  

  start = 0;
  while ((pos = newmsg.find('\n', start)) != string::npos) {
    newmsg.erase(pos, 1);
  }  

//   while ((pos = newmsg.find("><", start)) != string::npos) {
//     newmsg.insert(pos+1, "GGGGGG");
//   }  

  start = 0;
  while ((pos = newmsg.find('>', start)) != string::npos) {
    str = newmsg.substr(start, newmsg.find('>', start) - start + 1);
    start += str.size();
    if ((pos = str.rfind(">", start)) != string::npos) {
      //      str.insert(pos+1, "FFFF");
      str.insert(pos+1, tabs[level]);
      level++;
    }
    if ((pos = str.rfind("</", start)) != string::npos) {
      //      cerr << "EEEE";
      //      str.insert(pos, "EEEE");
      level--;
      str.insert(pos, tabs[level]);
    }
      
    cerr << str;
    //    cerr << prefix << str;
  }
  //cerr << newmsg.substr(start, newmsg.size()) << endl;
}

// These parse incoming messages for the daemon
retcode_t
Msgs::statusProcess(XMLNode *node)
{
  DEBUGLOG_REPORT_FUNCTION;
  XMLNode *child;
  string str;
  unsigned int i;

  // dbglogfile << BAD_CAST node->valueGet() << endl;
  
  if (_net_mode == DAEMON) {
    if (xmlStrcmp(node->valueGet(), BAD_CAST "sysversion") == 0) {
      str = responseCreate(STATUS, node->valueGet(), "0.0");
    } else if (xmlStrcmp(node->valueGet(), BAD_CAST "revision") == 0) {
      str = responseCreate(STATUS, node->valueGet(), "0.0");
    } else if (xmlStrcmp(node->valueGet(), BAD_CAST "opmode") == 0) {
      str = responseCreate(STATUS, node->valueGet(), "none");
    } else if (xmlStrcmp(node->valueGet(), BAD_CAST "warningmode") == 0) {
      str = responseCreate(STATUS, node->valueGet(), "none");    
    } else if (xmlStrcmp(node->valueGet(), BAD_CAST "errormode") == 0) {
      str = responseCreate(STATUS, node->valueGet(), "none");
    }
    
    if (writeNet(str)) {
      return ERROR;
    } else {
      return SUCCESS;
    }
  }

  if (_net_mode == CLIENT) {
    if (xmlStrcmp(node->nameGet(), BAD_CAST "status") == 0) {
      return SUCCESS;
    }

#if 0
    if        (xmlStrcmp(node->nameGet(), BAD_CAST "revision") == 0) {
      //cerr << "VER" << endl;
    } else if (xmlStrcmp(node->nameGet(), BAD_CAST "sysversion") == 0) {
      //cerr << "REV" << endl;
    } else if (xmlStrcmp(node->nameGet(), BAD_CAST "opmode") == 0) {
      //cerr << "OP" << endl;
    } else if (xmlStrcmp(node->nameGet(), BAD_CAST "warningmode") == 0) {
      //cerr << "WARN" << endl;
    } else if (xmlStrcmp(node->nameGet(), BAD_CAST "errormode") == 0) {
      //cerr << "ERROR" << endl;
    }
#endif
    cacheAdd(node->nameGet(), (const char*)node->valueGet());
    dbglogfile << "tag \"" << node->nameGet() << "\" has a value of: " << node->valueGet() << endl;
    
    return SUCCESS;
  }
  
  return ERROR;                 // FIXME: implement this method
}

retcode_t
Msgs::heloProcess(XMLNode *node)
{
  DEBUGLOG_REPORT_FUNCTION;
  dbglogfile << "WARNING: unimplemented method" << endl;
  
  return ERROR;                 // FIXME: implement this method
}

retcode_t
Msgs::configProcess(XMLNode *node)
{
   DEBUGLOG_REPORT_FUNCTION;
  dbglogfile << "WARNING: unimplemented method" << endl;
  return ERROR;                 // FIXME: implement this method
}

retcode_t
Msgs::metersProcess(XMLNode *node)
{
  DEBUGLOG_REPORT_FUNCTION;

  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet()
             << " and mode is: " << (int)_net_mode << endl;

  if(_net_mode == DAEMON) {
    //cacheDump();
    const xmlChar *xxx = node->nameGet();
    string value = cacheGet(node->valueGet());
    dbglogfile << "value for \"" << node->valueGet() << "\" is " << value.c_str() << endl;
    if (value.size() == 0) {
    } else {
      
    }
  
    string str = metersResponseCreate(node->valueGet(), value);
    if (writeNet(str)) {
      return ERROR;
    } else {
      return SUCCESS;
    }
  }

  if (_net_mode == CLIENT) {
    //dbglogfile << node->valueGet()
    // Process the result
  }

  return ERROR;
}

retcode_t
Msgs::serverProcess(XMLNode *node)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  XMLAttr *attr;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->hasAttributes()) {
    if ((attr = node->attribGet(0))) {
      dbglogfile << "\tAttribute is \"" << attr->nameGet()
                 << "\" with a value of " << attr->valueGet() << endl;
      if (xmlStrcmp(attr->valueGet(), (const xmlChar *)_thisip.c_str()) != 0) {
        dbglogfile << "WARNING: IP's don't match!!!!" << endl;
        return ERROR;
      }
    }
  }

  if (xmlStrcmp(node->valueGet(), (const xmlChar *)_thishost.c_str()) != 0) {
    dbglogfile << "WARNING: Host's don't match!!!!" << endl;
    return ERROR;
  }

  dbglogfile << "Host and IP data match" << endl;
  return SUCCESS;
}

retcode_t
Msgs::clientProcess(XMLNode *node)
{
  // DEBUGLOG_REPORT_FUNCTION;
  XMLAttr *attr;

  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (_remoteip.size() > 0) {
    if (node->hasAttributes()) {
      if ((attr = node->attribGet(0))) {
        dbglogfile << "\tAttribute is \"" << attr->nameGet()
                   << "\" with a value of " << attr->valueGet() << endl;
        if (xmlStrcmp(attr->valueGet(), (const xmlChar *)_remoteip.c_str()) != 0) {
          dbglogfile << "WARNING: IP's don't match!!!!" << endl;
          return ERROR;
        }
      }
    }
  }

  if (_remotehost.size() != 0) {
    if (xmlStrcmp(node->valueGet(), (const xmlChar *)_remotehost.c_str()) != 0) {
      dbglogfile << "WARNING: Host's don't match!!!!" << endl;
      return ERROR;
    }
  }

  return SUCCESS;
}

// Process the top level header tag.
retcode_t
Msgs::powerguruProcess(XMLNode *node)
{
  DEBUGLOG_REPORT_FUNCTION;
  XMLAttr *attr;

  _body.str("");
  _body << _version;
  
//   dbglogfile << "Node is \"" << node->nameGet()
//              << "\" with a value of " << node->valueGet() << endl;
      
  if (node->hasAttributes()) {
    if ((attr = node->attribGet(0))) {
//       dbglogfile << "\tAttribute is \"" << attr->nameGet().c_str()
//                  << "\" with a value of " << attr->valueGet().c_str() << endl;
      if (xmlStrcmp((const xmlChar *)_body.str().c_str(), attr->valueGet()) != 0) {
        dbglogfile << "Versions in header don't match!" << endl;
      } else {
        dbglogfile << "Versions in header match" << endl;        
      }
    }
  }

  return SUCCESS;
}

retcode_t
Msgs::chargeAmpsProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}

retcode_t
Msgs::loadAmpsProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    //    return ERROR;
  }
  
  if (_net_mode == CLIENT) {
    dbglogfile << "Battery voltage is: " << node->valueGet() << endl;
    
    return SUCCESS;
  }
  
  if (_net_mode == DAEMON) {
    string str = metersResponseCreate(node->valueGet(), _cache[node->nameGet()]);
    
    return SUCCESS;
  }

  return SUCCESS;
}

retcode_t
Msgs::pvAmpsProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}

retcode_t
Msgs::pvVoltsProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}

retcode_t
Msgs::dailyKwhProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}

retcode_t
Msgs::hertzProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}

retcode_t
Msgs::batteryVoltsProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
//   dbglogfile << "Node is \"" << node->nameGet()
//              << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    //    return ERROR;
  }

  if (_net_mode == CLIENT) {
    dbglogfile << "Battery voltage is: " << node->valueGet() << endl;
    
    return SUCCESS;
  }
  
  if (_net_mode == DAEMON) {
    string str = metersResponseCreate(node->valueGet(), _cache[node->nameGet()]);
    
    return SUCCESS;
  }
  
  return ERROR;
}

retcode_t
Msgs::buyAmpsProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}

retcode_t
Msgs::sellAmpsProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}

retcode_t
Msgs::acVoltsOutProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}
  
retcode_t
Msgs::ac1InProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}

retcode_t
Msgs::ac2InProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}

retcode_t
Msgs::commandProcess(XMLNode *node) {
  DEBUGLOG_REPORT_FUNCTION;
  
  dbglogfile << "Node is \"" << node->nameGet()
             << "\" with a value of " << node->valueGet() << endl;
      
  if (node->valueGet() <= 0) {
    dbglogfile << "ERROR: no value in messages!" << endl;
    return ERROR;
  }
      
  return SUCCESS;
}
