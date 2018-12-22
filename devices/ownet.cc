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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "log.h"
#include "ownet.h"
#include <string>
#include <boost/algorithm/string.hpp>

extern LogFile dbglogfile;

//const char DEFAULT_ARGV[] = "--fake 28 --fake 10";
const char DEFAULT_ARGV[] = "-s 192.168.0.50:4304";

Ownet::Ownet(const std::string &host)
{
//    DEBUGLOG_REPORT_FUNCTION;

    dbglogfile << "Trying to connect to the owserver" << std::endl;
    char *buf = 0;
    size_t s  = 0;

    //OW_init("/dev/ttyS0");
    int count = 5;
    std::string hostname = "-s " + host;
    const char *argv = hostname.c_str();

    while (count-- > 0) {
        if (OW_init(argv) < 0) {
            dbglogfile << "WARNING: Couldn't connect to owserver!" << std::endl;
            //return;
        } else {
            dbglogfile << "Connected to owserver." << std::endl;
            _owserver = true;
            break;
        }
    }
    // 0=mixed  output,  1=syslog, 2=console.
    OW_set_error_print("2");
    // (0=default, 1=err_connect, 2=err_call, 3=err_data, 4=err_detail,
    // 5=err_debug, 6=err_beyond)
    OW_set_error_level("0");
    OW_get("/", &buf, &s);
    // buf looks like:
    // 10.67C6697351FF/,05.4AEC29CDBAAB/,bus.0/,uncached/,settings/,system/,statistics/,structure/,simultaneous/,alarm/
    //
    //if (s <= 0) {
    //    dbglogfile << "S: " << (int)s << std::endl;
    // return;
    //}

    OW_put("/settings/units/temperature_scale", &_scale, 1);

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
        ownet_t *data = new ownet_t[1];
        data->family = getValue(it->c_str(), "family");
        data->type = getValue(it->c_str(), "type");
        data->id = getValue(it->c_str(), "id");
        if (data->type.length() == 0 || data->id.length() == 0) {
            break;
        }
        std::string dev = *it + "temperature";
        if (OW_present(dev.c_str()) == 0) {
            dbglogfile << "Temperature sensor found: " << *it << std::endl;
        } else {
            dbglogfile << "Temperature sensor not found!" << std::endl;
        }
        std::lock_guard<std::mutex> guard(_mutex);
        _sensors[*it] = data;
    }
}

void Ownet::dump(void)
{
//    DEBUGLOG_REPORT_FUNCTION;

    std::map<std::string, ownet_t *>::iterator sit;
    for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
        std::cout << "Data for device: " << sit->first << std::endl;
        std::cout << "\tfamily: " << sit->second->family << std::endl;
        std::cout << "\ttype: " << sit->second->type << std::endl;
        std::cout << "\tid: " << sit->second->id << std::endl;
    }
    // std::map<std::string, temperature_t *>::iterator tit;
    // for (tit = _temperatures.begin(); tit != _temperatures.end(); tit++) {
    //     std::cout << "\tCurrent temperature: " << tit->second->temp << std::endl;
    //     std::cout << "\tLow temperture: " << tit->second->lowtemp << std::endl;
    //     std::cout << "\tHigh Temperature: " << tit->second->hightemp << std::endl;
    // }
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
