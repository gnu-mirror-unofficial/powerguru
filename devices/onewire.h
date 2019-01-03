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

struct onewire {
    std::string family;
    std::string id;
    std::string type;
    std::string device;
    bool bus;
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

enum { ACVOLTAGE,
       DCVOLTAGE,
       AUTH,
       BATTERY,
       CLOCK,
       TEMPERATURE,
       THERMCOUPLE,
       MOISTURE,
       UNSUPPORTED
} typedef family_e;

struct family {
    std::string desription;
    std::string chips;
    family_e    type;
} typedef family_t;

class Onewire {
private:
    std::mutex _mutex;
    // How long to delay between reading the sensor
    int _poll_sleep;
    char _scale;                // 'C' or 'F'
    std::map<std::string, boost::shared_ptr<temperature_t>> _temps;
    // Is _rootdir (usually /mnt/1wire) mounted ?
    std::string _rootdir;
    bool _mounted = true;
    // Table of family types of supported sensors
    std::map<const std::string, family_t> _family;    
    void initTable(void);
    // All the currently installed sensors
    std::map<std::string, boost::shared_ptr<onewire_t>> _sensors;
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

    float convertScale(float inc) {
        return (inc * 1.8) + 32.0;
    }
    
    // extract a value from an owfs file
    std::string &getValue(const std::string &device, std::string file,
                          std::string &result);

    void setValue(const std::string &device, const std::string &file,
                          const std::string &value);

    // get all the temperature fields for a device.
    std::map<std::string, boost::shared_ptr<temperature_t>> &getTemperatures(void);

    std::string &
    readBus(const std::string &device, std::string &data);
               
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
