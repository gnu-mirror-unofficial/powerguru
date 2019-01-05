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

#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/regex.hpp>
#include <string>
#include <string>
#include <string>
#include <map>
#include "onewire.h"
#include "ownet.h"
#include "log.h"

const int OWPORT = 4304;        ///< TCP/IP port for owserver
const char *OWHOST = "localhost"; ///< Remote hostname for owserver

/// \class Ownet
/// Consruct a class for OWNet support
Ownet::Ownet(void)
    : _poll_sleep(300),
      _scale('F'),
      _owserver(false)
{
//    DEBUGLOG_REPORT_FUNCTION;
    
    // Initialize the table of family types
    initTable(_family);
}

///
/// Consruct a class for OWNet support connected to a remote host
/// @param host Hostname of the remote owserver
Ownet::Ownet(std::string &host)
    : Ownet()
{
//    DEBUGLOG_REPORT_FUNCTION;
    int retries = 2;
    if (host.find(':') == std::string::npos) {
        host += ":" + std::to_string(OWPORT);
    }
    BOOST_LOG(lg) << "Trying to connect to the owserver on " << host;

    // OW_init() takes what looks like a standard command line
    std::string argv = "-s " + host;

    // On my machine, it never seems to connect on the first attempt,
    // but always does on the second.
    while (retries-- > 0) {
        if (OW_init(argv.c_str()) < 0) {
            BOOST_LOG_SEV(lg, severity_level::warning)
                << "Couldn't connect to owserver with " << argv;
            //return;
        } else {
            BOOST_LOG(lg) << "Connected to owserver on host " << host;
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

    // Iterate through all the directories in the root dir
    int i = 0;
    std::vector<std::string>::iterator it;
    for(it = results.begin(); it != results.end(); it++,i++ ) {
        boost::shared_ptr<onewire_t> data(new onewire_t);
        std::string result;
        data->family = getValue(it->c_str(), "family", result);
        if (data->family.empty()) { // not a sensor
            continue;
        }
        data->id = getValue(it->c_str(), "id", result);
        data->alias = getValue(it->c_str(), "alias", result);
        data->device = data->family + "." + data->id;
        data->type = _family[data->family].type;
        std::lock_guard<std::mutex> guard(_mutex);
        _sensors[*it] = data;
        std::string device = *it;
        switch (data->type) {
          case TEMPERATURE:
          {
              BOOST_LOG(lg) << device << " is a thermometer";
              boost::shared_ptr<temperature_t> temp(getTemperature(device));
              if (temp) {
                  try {
                      temp->id = data->id;
                      temp->temp = std::stof(getValue(device, "temperature", result));
                      temp->lowtemp =std::stof(getValue(device, "templow", result));
                      temp->hightemp = std::stof(getValue(device, "temphigh", result));
                  } catch (const std::exception& e) {
                      temp->temp = 0;
                      temp->lowtemp = 0;
                      temp->hightemp = 0;
                  }
              }
              break;
          }
          case BATTERY:
          {
              BOOST_LOG(lg) << device << " is a battery";
              boost::shared_ptr<battery_t> power(getBattery(device));
              power->id = data->id;
              try {
                  power->volts = std::stof(getValue(device, "volts", result));
                  power->current = std::stof(getValue(device, "current", result));
                  power->DC = true; // FIXME: this shouldn't be hardcoded
              } catch (const std::exception& e) {
                  power->volts = 0;
                  power->current = 0;
              }
              break;
          }
        };

    }

    dump();
}

const boost::shared_ptr<battery_t>
Ownet::getBattery(const std::string &device)
{
    //DEBUGLOG_REPORT_FUNCTION;
    // boost::shared_ptr<temperature_t> temp = boost::make_shared<temperature_t>(1);
    std::string result;
    if (_sensors[device]->type == BATTERY) {
        boost::shared_ptr<battery_t> batt(new battery_t);
        batt->id = _sensors[device]->id;
        batt->current = std::stof(getValue(device, "current", result));
        batt->volts = std::stof(getValue(device, "volts", result));
        batt->DC = true;
        return batt;
    } else {
        BOOST_LOG_SEV(lg, severity_level::warning) << device << " is not a battery!";
    }

    boost::shared_ptr<battery_t> batt;
    return batt;
}

/// Get the temperature(s) via ownet from a temperature sensor
/// @param device The full device name including the family, ie... "28.021316A4D6AA"
/// @return The temerature(s) from a temperature sensor
const boost::shared_ptr<temperature_t>
Ownet::getTemperature(const std::string &device)
{
    //DEBUGLOG_REPORT_FUNCTION;
    
    std::string result;
    if (_sensors[device]->type == TEMPERATURE) {
        boost::shared_ptr<temperature_t> temp(new temperature_t);
        try {
            std::string result;
            temp->id = _sensors[device]->id;
            temp->temp = std::stof(getValue(device, "temperature", result));
            temp->lowtemp =std::stof(getValue(device, "templow", result));
            temp->hightemp = std::stof(getValue(device, "temphigh", result));
        } catch (const std::exception& e) {
            temp->temp = 0;
            temp->lowtemp = 0;
            temp->hightemp = 0;
        }

        char *buffer;
        size_t blen;
        OW_get("/settings/units/temperature_scale", &buffer, &blen);
        temp->scale = buffer[0];
        return temp;
    } else {
        BOOST_LOG_SEV(lg, severity_level::warning) << device << " is not a thermometer";
    }
    boost::shared_ptr<temperature_t> temp;
    return temp;
}

///
/// Get a list of all connected sensor devices
/// @param an array to hold the device list
/// @return a list of all sensors 
const std::vector<std::string>
Ownet::listDevices(std::vector<std::string> &list)
{
//    DEBUGLOG_REPORT_FUNCTION;
    
    std::map<std::string, boost::shared_ptr<onewire_t>>::iterator sit;
    for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
        std::string dev = sit->first.substr(sit->first.size()-1);
        list.push_back(sit->first);
    }
    return list;
}

///
/// Get the value from a 1wire file
/// @param device The full device name including the family, ie... "28.021316A4D6AA"
/// @param file The base name of the file to read
/// @param result String to hold the returned value
/// @return the value from the file
///
std::string &
Ownet::getValue(const std::string &device, std::string file, std::string &result)
{
//    DEBUGLOG_REPORT_FUNCTION;

    char * buf;
    size_t s  = 0;
    
    std::string data = device + file;
    //std::cout << "Looking for: " << data;
    int ret = OW_get(data.c_str(), &buf, &s);
    if (ret <= 0) {
        return result;
        //} else {
        //std::cout << ", Got(" << s << "): " <<  buf;
    }
    
    result = (buf);
    free(buf);
    
    return result;
}

///
/// Dump data about the sensors
///
void
Ownet::dump(void)
{
//    DEBUGLOG_REPORT_FUNCTION;

    std::map<family_e, std::string> table;
    table[ACVOLTAGE] = "ACVOLTAGE";
    table[DCVOLTAGE] = "DCVOLTAGE";
    table[AUTH] = "AUTHENTICATIN";
    table[BATTERY] = "BATTERY";
    table[CLOCK] = "CLOCK";
    table[TEMPERATURE] = "TEMPERATURE";
    table[THERMCOUPLE] = "THERMOCOUPLE";
    table[MOISTURE] = "MOISTURE";
    table[UNSUPPORTED] = "UNSUPORTED";

    std::map<std::string, boost::shared_ptr<onewire_t>>::iterator sit;
    for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
        std::cout << "Data for device: " << sit->first << std::endl;
        std::cout << "\tfamily: " << sit->second->family << std::endl;
        std::cout << "\tid: " << sit->second->id << std::endl;
        std::cout << "\ttype: " << table[sit->second->type] << std::endl;
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
