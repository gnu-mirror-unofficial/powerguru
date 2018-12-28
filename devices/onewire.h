// 
// Copyright (C) 2018 Free Software Foundation, Inc.
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

#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include "log.h"

extern LogFile dbglogfile;

struct onewire {
    std::string family;
    std::string id;
    std::string type;
    std::string device;
} typedef onewire_t;

struct temperature {
    std::string family;
    std::string id;
    std::string type;
    float temp;
    float lowtemp;
    float hightemp;
    char scale;
} typedef temperature_t;

enum family_t {CONTROL = 05, THERMOMETER = 10, THERMOMETER2 = 28};

class Onewire {
private:
    std::map<std::string, boost::shared_ptr<onewire_t>> _sensors;
    std::mutex _mutex;
    int _poll_sleep;
    char _scale;
    std::string _rootdir;
    std::map<std::string, boost::shared_ptr<temperature_t>> _temps;
    bool _mounted = true;
public:
    Onewire(void);
    ~Onewire(void) {};

    char setScale(char scale);
    bool isMounted() { return _mounted; };

    // Thread have a polling frequency to avoid eating up all the cpu cycles
    // by polling to quickly.
    int getPollSleep(void) {
        return _poll_sleep;
    }

    void setPollSleep(int x) {
        _poll_sleep = x;
    }

    // see if any 1 wire sensors were found during scanning
    bool hasSensors(void) {
        if (_sensors.size() >0) {
            return true;
        } else {
            return false;
        }
    }

    // extract a value from an owfs file
    std::string &getValue(const std::string &device, std::string file,
                          std::string &result);

    void setValue(const std::string &device, const std::string &file,
                          const std::string &value);

    // get all the temperature fields for a device.
    std::map<std::string, boost::shared_ptr<temperature_t>> &getTemperatures(void);
    
    void dump(void);
    
    std::vector<std::string> &
    listDevices(std::vector<std::string> &list) {
        DEBUGLOG_REPORT_FUNCTION;

        std::map<std::string, boost::shared_ptr<onewire_t>>::iterator sit;
        for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
            std::string dev = sit->first.substr(sit->first.size()-1);
            list.push_back(sit->first);
        }
        return list;
    }

};

// __ONEWIRE_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
