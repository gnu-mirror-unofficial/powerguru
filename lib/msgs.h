// 
// Copyright (C) 2005, 2006-2018.
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

#ifndef __MSGS_H
#define __MSGS_H__

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <sstream>
#include <boost/system/error_code.hpp>

#include "database.h"
#include "log.h"
#ifdef HAVE_LIBXML
# include "xml.h"
#endif
#include "tcpip.h"

class Msgs : public Tcpip {
public:

    typedef enum {
        NONET,
        DAEMON,
        CLIENT
    } net_mode_e;

    typedef enum {
        CHARGE_AMPS,
        AC_LOAD_AMPS,
        BATTERY_VOLTS,
        AC_VOLTS_OUT,
        AC1_VOLTS_IN,
        AC2_VOLTS_IN,
        PV_AMPS_IN,
        PV_VOLTS_IN,
        BUY_AMPS,
        SELL_AMPS,
        DAILY_KWH,
        HERTZ,
        TEMPCOMP_VOLTS
    } xml_meters_e;

    typedef enum {
        SYSVERSION,
        REVISION,
        OPMODE,
        ERRORMODE,
        WARNINGMODE
    } xml_status_e;
  
    typedef enum {
        GENERATOR,
        GRID,
        RELAY,
        AUXILARY,
        POLL,
        INVERTER,
        CHARGER
    } xml_command_e;
  
    typedef enum {
        GENCONFIG,
        GRIDCONFIG,
        BUY,
        SELL,
        CHARGE
    } xml_config_e;

    typedef enum {
        METER,
        STATUS,
        COMMAND,
        CONFIG,
        HEARTBEAT,
        RESPONSE
    } xml_msg_e;
  
    typedef boost::system::error_code (Msgs::*methodPtr_t)(XMLNode *node);

//   struct msg_data 
//   {
//     FPtr ptr;
//     std::string name;
//   };

    Msgs();
    Msgs(const std::string &, const std::string &);
    Msgs(Tcpip *tcpip);
    ~Msgs();

    boost::system::error_code init(void);
    boost::system::error_code init(net_mode_e);
    boost::system::error_code init(std::string &);
    boost::system::error_code init(net_mode_e, const std::string &);
    boost::system::error_code init(bool);
    boost::system::error_code init(net_mode_e, bool);
  
    void dump(XMLNode &);
    void process(const XMLNode &node);
    
    // These parse incoming messages for the daemon
    boost::system::error_code statusProcess(const XMLNode &node);
    boost::system::error_code powerguruProcess(const XMLNode &node);
  
    boost::system::error_code heloProcess(const XMLNode &node);
    boost::system::error_code serverProcess(const XMLNode &node);
    boost::system::error_code clientProcess(const XMLNode &node);
  
    boost::system::error_code configProcess(const XMLNode &node);

    // These are all the tags associated with meters
    boost::system::error_code metersProcess(const XMLNode &node);
    boost::system::error_code chargeAmpsProcess(const XMLNode &node);
    boost::system::error_code loadAmpsProcess(const XMLNode &node);
    boost::system::error_code pvAmpsProcess(const XMLNode &node);
    boost::system::error_code pvVoltsProcess(const XMLNode &node);
    boost::system::error_code dailyKwhProcess(const XMLNode &node);
    boost::system::error_code hertzProcess(const XMLNode &node);
    boost::system::error_code batteryVoltsProcess(const XMLNode &node);
    boost::system::error_code buyAmpsProcess(const XMLNode &node);
    boost::system::error_code sellAmpsProcess(const XMLNode &node);
    boost::system::error_code acVoltsOutProcess(const XMLNode &node);
    boost::system::error_code ac1InProcess(const XMLNode &node);
    boost::system::error_code ac2InProcess(const XMLNode &node);

    boost::system::error_code unimplementedProcess(const XMLNode &node);

    // These are all the system commands
    boost::system::error_code commandProcess(const XMLNode &node);

    // These format client side messages to the daemon
    std::string &statusCreate(meter_data_t *data);
  
    std::string &heloCreate(float version);
  
    std::string &configCreate(const std::string &, int value);
    std::string &configCreate(const std::string &tag, float value);
    std::string &metersRequestCreate(const std::string &str);
    std::string &metersRequestCreate(xml_meters_e type);

    // This formats a response from the daemon to the client
    std::string &metersResponseCreate(const std::string &tag, int val);
    std::string &metersResponseCreate(const std::string &tag, float val);
    std::string &metersResponseCreate(const std::string &tag, const std::string &);

    std::string &responseCreate(xml_msg_e type, const std::string &, const std::string &);
  
    std::string &requestCreate(xml_meters_e tag);
    std::string &requestCreate(xml_status_e tag);
    std::string &requestCreate(xml_config_e tag);
    std::string &requestCreate(xml_command_e tag);

    const std::string &packet(void) { return std::string(); /* _body.str(); */}
    void print_msg(std::string msg);

    void methodSet(const std::string &, methodPtr_t func);
    methodPtr_t methodGet(const std::string &name);
    boost::system::error_code methodProcess(const std::string &name, XMLNode &);
    void methodsDump(void);

    std::string cacheGet(const std::string &);
    boost::system::error_code cacheAdd(const std::string &name, const std::string &);
    void cacheDump(void);
    
    std::string &thisIPGet(void) { return _thisip; };
    std::string &remoteIPGet(void) { return _thishost; }; 
    std::string &thisHostnameGet(void) { return _remoteip; };
    std::string &remoteHostnameGet(void) { return _remotehost; };

    boost::system::error_code findTag(const std::string &);
  
private:
    float               _version;
    static std::map<const std::string, methodPtr_t> _methods;
    static std::map<const std::string, std::string> _cache;
    static net_mode_e   _net_mode;
    std::ostringstream  _body;
    std::string         _thisip;
    std::string         _thishost;
    std::string         _remoteip;
    std::string         _remotehost;
};

// end of __MSGS_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
