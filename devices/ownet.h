// 
// Copyright (C) 2018, 2019 Free Software Foundation, Inc.
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

#ifndef __OWNET_H__
#define __OWNET_H__

#include <owcapi.h>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include "onewire.h"
#include "log.h"

// Default host and port for the owserver
extern const int OWPORT;
extern const char *OWHOST;

/// \typedef ownet_t
/// Holds data for an ownet connection
typedef struct ownet {
    std::string family;
    std::string id;
    std::string type;
    std::string device;
} ownet_t;

void initTable(std::map<std::string, family_t> &result);

///
/// \class Ownet
/// Construct a class for the ownet protocol
///
class Ownet
{
private:
    std::map<std::string, boost::shared_ptr<ownet_t>> _sensors;
    bool        _owserver;
    std::mutex  _mutex;
    int         _poll_sleep;
    char        _scale;
    std::map<std::string, family_t> _family;
public:
    Ownet(void);
    Ownet(std::string &host);
    ~Ownet(void) {
        DEBUGLOG_REPORT_FUNCTION;
        if (_owserver) {
            OW_finish();
        }
    };

    void setScale(char scale) {
        OW_put("/settings/units/temperature_scale", &_scale, 1);
    };

    // Thread have a polling frequency to avoid eating up all the cpu cycles
    // by polling to quickly.
    int getPollSleep(void) { return _poll_sleep; };
    void setPollSleep(int x) {_poll_sleep = x; };

    // See if we're connected to the owserver
    bool isConnected(void) { return _owserver; };

    // See if any 1 wire sensors were found during scanning
    bool hasSensors(void) { return (_sensors.size() > 0) ? true : false; };

    // extract a value from an owfs file
    std::string &getValue(const std::string &device, std::string file, std::string &result);

    // return a handle to all the sensors
    const boost::shared_ptr<ownet_t> &getSensor(const std::string &device) {
        return _sensors[device];
    };

    const std::map<std::string, boost::shared_ptr<ownet_t>> getSensors(void) {
        return _sensors;
    };
    
    std::map<std::string, boost::shared_ptr<battery_t>> &getBatteries(void);

    const boost::shared_ptr<temperature_t> getTemperature(const std::string &device);
    
    void dump(void);
    
    const std::vector<std::string>
    listDevices(std::vector<std::string> &list);
};

// __OWNET_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
