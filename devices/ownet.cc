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

#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "ownet.h"

//extern LogFile dbglogfile;
#define dbglogfile std::cerr

// Default host and port for the owserver
const int OWPORT = 4304;
const char *OWHOST = "localhost";

Ownet::Ownet(void)
    : _poll_sleep(60),
      _scale('F'),
      _owserver(false)
{
    DEBUGLOG_REPORT_FUNCTION;
}

Ownet::Ownet(std::string &host)
{
    DEBUGLOG_REPORT_FUNCTION;

    int retries = 2;
    if (host.find(':') == std::string::npos) {
        host += ":" + std::to_string(OWPORT);
    }
    dbglogfile << "Trying to connect to the owserver on " << host << std::endl;

    // OW_init() takes what looks like a stadard command line
    std::string argv = "-s " + host;

    // On my machine, it never seems to connect on the first attempt,
    // but always does on the second.
    while (retries-- > 0) {
        if (OW_init(argv.c_str()) < 0) {
            dbglogfile << "WARNING: Couldn't connect to owserver with " << argv << std::endl;
            //return;
        } else {
            dbglogfile << "Connected to owserver on host " << host << std::endl;
            _owserver = true;
            break;
        }
    }

    // Setup ownet
    OW_set_error_print("2"); // 0=mixed  output,  1=syslog, 2=console.
    OW_set_error_level("0"); // (0=default, 1=err_connect, 2=err_call,
                             // 3=err_data, 4=err_detail,
                             // 5=err_debug, 6=err_beyond)
    // Set the default temperature scale, 'F' or 'C'
    OW_put("/settings/units/temperature_scale", &_scale, 1);

    // Get a directory listing of the rootdir
    char *buf = 0;
    size_t s  = 0;
    OW_get("/", &buf, &s);
    std::vector<std::string> results;
    if (buf != 0) {
        boost::split(results, buf, boost::is_any_of(","));
        free(buf);
        s = 0;
        if( results.size() <= 0) {
            return;
        }
    }

    // Iterate through all the directorys in the root dir
    int i = 0;
    std::vector<std::string>::iterator it;
    for(it = results.begin(); it != results.end(); it++,i++ ) {
        boost::shared_ptr<ownet_t> data(new ownet_t);
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

boost::shared_ptr<temperature_t> &
Ownet::getTemperature(const std::string &device)
{
    // DEBUGLOG_REPORT_FUNCTION;
    
    std::string family = getValue(device, "family");
    std::string id = getValue(device, "id");
    std::string type = getValue(device, "type");
    
    if (family == "10" | family == "28") {
        // dbglogfile << device << " is a thermometer" << std::endl;
        // boost::shared_ptr<temperature_t> temp = boost::make_shared<temperature_t>(1);
        boost::shared_ptr<temperature_t> temp(new temperature_t);
        temp->family = getValue(device, "family");
        temp->id = getValue(device, "id");
        temp->type = getValue(device, "type");
        temp->temp = std::stof(getValue(device, "temperature"));
        temp->lowtemp =std::stof(getValue(device, "templow"));
        temp->hightemp = std::stof(getValue(device, "temphigh"));
        char *buffer;
        size_t blen;
        OW_get("/settings/units/temperature_scale", &buffer, &blen);
        temp->scale = buffer[0];
        return temp;
    } else {
        dbglogfile << device << " is not a thermometer" << std::endl;
    }
    boost::shared_ptr<temperature_t> temp;
    return temp;
}

std::vector<std::string> &
Ownet::listDevices(std::vector<std::string> &list)
{
    DEBUGLOG_REPORT_FUNCTION;
    
    std::map<std::string, boost::shared_ptr<ownet_t>>::iterator sit;
    for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
        std::string dev = sit->first.substr(sit->first.size()-1);
        list.push_back(sit->first);
    }
    return list;
}

std::string
Ownet::getValue(const std::string &device, std::string file)
{
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
    
    std::string value = (buf);
    free(buf);
    
    return value;
}

void
Ownet::dump(void)
{
//    DEBUGLOG_REPORT_FUNCTION;

    std::map<std::string, boost::shared_ptr<ownet_t>>::iterator sit;
    for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
        std::cout << "Data for device: " << sit->first << std::endl;
        std::cout << "\tfamily: " << sit->second->family << std::endl;
        std::cout << "\ttype: " << sit->second->type << std::endl;
        std::cout << "\tid: " << sit->second->id << std::endl;
    }
    // std::map<std::string, boost::shared_ptr<temperature_t>>::iterator tit;
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
