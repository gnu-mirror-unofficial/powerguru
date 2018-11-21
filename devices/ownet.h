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

#ifndef __OWNET_H__
#define __OWNET_H__

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <log.h>
#include <owcapi.h>
#include <boost/algorithm/string.hpp>

extern LogFile dbglogfile;

namespace pdev {

struct ownet {
    std::string family;
    std::string id;
    std::string type;
    std::string device;
} typedef ownet_t;

struct temperature {
    //ownet_t ownet;
    float temp;
    float lowtemp;
    float hightemp;
} typedef temperature_t;

class Ownet {
private:
    enum family_t {CONTROL = 05, THERMOMETER = 10, THERMOMETER2 = 28};
    std::map<std::string, ownet_t> _sensors;
    bool _owserver = false;
    std::map<std::string, temperature_t *> _temperatures;
    std::mutex _mutex;

public:
    bool isConnected(void) {
        return _owserver;
    }

    bool hasSensors(void) {
        if (_sensors.size() >0) {
            return true;
        } else {
            return false;
        }
    }

    std::string getValue(const std::string &device, std::string file) {
//        DEBUGLOG_REPORT_FUNCTION;
        char * buf;
        size_t s  = 0;

        std::string data = device + file;
        //std::cout << "Looking for: " << data;
        int ret = OW_get(data.c_str(), &buf, &s);
        if (ret <= 0) {
            return std::string();
            //} else {
            //std::cout << ", Got(" << s << "): " <<  buf << std::endl;
        }

        std::string bar(buf);
        free(buf);

        return bar;
    }

    std::map<std::string, ownet_t> &getSensors(void) {
        return _sensors;
    }

    temperature_t *getTemperature(const std::string &device) {
        DEBUGLOG_REPORT_FUNCTION;

        std::string family = getValue(device, "family");

        temperature_t *temp = 0;
        if (family == "10") {
            dbglogfile << device << " is a thermometer" << std::endl;
            temp = new temperature_t[1];
            temp->temp = std::stof(getValue(device, "temperature"));
            temp->lowtemp =std::stof(getValue(device, "templow"));
            temp->hightemp = std::stof(getValue(device, "temphigh"));
            std::lock_guard<std::mutex> guard(_mutex);
            _temperatures[device] = temp;
        } else {
            dbglogfile << device << " is not a thermometer" << std::endl;
        }
        return temp;
    }

#if 1
    void dump(void) {
        DEBUGLOG_REPORT_FUNCTION;

        std::map<std::string, ownet_t>::iterator sit;
        for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
            std::cout << "Data for device: " << sit->first << std::endl;
            std::cout << "\tfamily: " << sit->second.family << std::endl;
            std::cout << "\ttype: " << sit->second.type << std::endl;
            std::cout << "\tid: " << sit->second.id << std::endl;
        }
        std::map<std::string, temperature_t *>::iterator tit;
        for (tit = _temperatures.begin(); tit != _temperatures.end(); tit++) {
            std::cout << "Current temperature: " << tit->second->temp << std::endl;
            std::cout << "Low temperture: " << tit->second->lowtemp << std::endl;
            std::cout << "High Temperature: " << tit->second->hightemp << std::endl;
        }
    }
#else
    void dump(void);
#endif
    Ownet(void) {
        DEBUGLOG_REPORT_FUNCTION;

        dbglogfile << "Trying to connect to the owserver" << std::endl;
        char *buf = 0;
        size_t s  = 0;

        //OW_init("/dev/ttyS0");
        int count = 5;
        while (count-- > 0) {
            if (OW_init("localhost:4304") < 0) {
                dbglogfile << "WARNING: Couldn't connect to owserver!" << std::endl;
                //return;
            } else {
                dbglogfile << "Connected to owserver." << std::endl;
                _owserver = true;
                break;
            }
        }
        // 0=mixed  output,  1=syslog, 2=console.
        OW_set_error_print("1");
        // (0=default, 1=err_connect, 2=err_call, 3=err_data, 4=err_detail,
        // 5=err_debug, 6=err_beyond)
        OW_set_error_level("4");
        OW_get("/", &buf, &s);
        // buf looks like:
        // 10.67C6697351FF/,05.4AEC29CDBAAB/,bus.0/,uncached/,settings/,system/,statistics/,structure/,simultaneous/,alarm/
        //
        //if (s <= 0) {
        //    dbglogfile << "S: " << (int)s << std::endl;
            // return;
        //}

        std::vector<std::string> results;
        if (buf != 0) {
            boost::split(results, buf, boost::is_any_of(","));
            free(buf);
            s = 0;
            if( results.size() <= 0) {
                return;
            }
        }

        int i = 0;
        std::vector<std::string>::iterator it;
        for(it = results.begin(); it != results.end(); it++,i++ ) {
            ownet_t data;
            data.family = getValue(it->c_str(), "family");
            data.type = getValue(it->c_str(), "type");
            data.id = getValue(it->c_str(), "id");
            if (data.type.length() == 0 || data.id.length() == 0) {
                break;
            }
            std::string dev = *it + "temperature";
            if (OW_present(dev.c_str()) == 0) {
                dbglogfile << "Temperature sensor found: " << *it << std::endl;
                temperature_t *temp= new temperature_t[1];

                temp->temp = std::stof(getValue(*it, "temperature"));
                temp->lowtemp = std::stof(getValue(*it, "templow"));
                temp->hightemp = std::stof(getValue(*it, "temphigh"));
                _temperatures[*it] = temp;
            } else {
                dbglogfile << "Temperature sensor not found!" << std::endl;
            }
            _sensors[*it] = data;
        }

    }

//    ~Ownet(void);

    ~Ownet(void) {
        OW_finish();
    };

};

// end of namespace pdev
}

// __OWNET_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
