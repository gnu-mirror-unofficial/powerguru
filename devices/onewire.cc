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
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <cstdio>
#include <streambuf>
#include "onewire.h"
#include "log.h"

Onewire::Onewire(void)
    : _scale('F'),
      _poll_sleep(300),
    _mounted(false),
    _rootdir("/mnt/1wire")
{
//    DEBUGLOG_REPORT_FUNCTION;

    // Initialize the table of family types
    initTable();

#if 0
    boost::filesystem::path p(_rootdir);
    try {
        if (boost::filesystem::exists(p) & boost::filesystem::is_directory(p)) {
            for (boost::filesystem::directory_entry & x : boost::filesystem::directory_iterator(p)) {
                if (boost::regex_match(x.path().string(), boost::regex(_rootdir + "/[A-Z0-9.]*"))) {
                    boost::shared_ptr<onewire_t> owire(new onewire_t);
                    owire->device = x.path().string();
                    std::string result;
                    owire->family = getValue(owire->device, "family", result);
                    owire->type = getValue(owire->device, "type", result);
                    owire->id = getValue(owire->device, "id", result);
                    owire->bus = false;
                    _sensors[x.path().string()] = owire;
                }
            }
        } else {
            BOOST_LOG_SEV(lg, severity_level::error) << _rootdir << " doesn't exist!";
            _mounted = false;
        }
    } catch (const boost::filesystem::filesystem_error& ex) {
        std::cout << ex.what() << std::endl;
    }
#endif
    
    if (_sensors.size() == 0) {
        _rootdir  = "/sys/bus/w1/devices";
        boost::filesystem::path p(_rootdir);
        if (boost::filesystem::exists(p) & boost::filesystem::is_directory(p)) {
            for (boost::filesystem::directory_entry & x : boost::filesystem::directory_iterator(p)) {
                if (boost::regex_match(x.path().string(), boost::regex(_rootdir + "/[a-z0-9-]*"))) {
                    _mounted = true;
                    boost::shared_ptr<onewire_t> owire(new onewire_t);
                    owire->device = x.path().string();
                    std::string result;
                    getValue(owire->device, "name", result);
                    owire->family = result.substr(0,2);
                    owire->id = result.substr(3,result.size());
                    owire->bus = true;
                    owire->type =  _family[owire->family].chips;
                    std::cerr << "BUS path found: " << result <<  std::endl;
                    _sensors[x.path().string()] = owire;
                }
                //readBus()
            }  
        }
    }
    
    setValue("", "/settings/units/temperature_scale", "F");
    dump();
}

void
Onewire::setValue(const std::string &device, const std::string &file,
                  const std::string &value)
{
//    DEBUGLOG_REPORT_FUNCTION;

    // Don't try to do anything if the owfs isn't mounted and we somehow got here anyway.
    if (!_mounted) {
        return;
    }

    std::string filespec;
    if (!device.empty()) {
        filespec = device +"/";
    } else {
        filespec = _rootdir; 
    }
    filespec += file;
    std::ofstream entry(filespec);
    entry << value;

    entry.close();
}

// extract a value from an owfs file
std::string &
Onewire::getValue(const std::string &device, std::string file, std::string &result)
{
//    DEBUGLOG_REPORT_FUNCTION;

    // Don't try to do anything if the owfs isn't mounted and we somehow got here anyway.
    if (!_mounted) {
        return result;
    }

    std::string filespec;
    if (!device.empty()) {
        filespec = device +"/";
    } else {
        filespec = _rootdir; 
    }
    filespec += file;
    try {
        BOOST_LOG_SEV(lg, severity_level::debug) << "Opening " << filespec;
        std::ifstream entry(filespec);
        entry.rdbuf()->pubsetbuf(0, 0);
        std::stringstream buffer;
        buffer << entry.rdbuf();
        //entry >> result;
        result = buffer.str();
        entry.close();
    } catch (const std::exception& e) {
        BOOST_LOG_SEV(lg, severity_level::warning) << "Warning: iostream failure! "
                                                 << e.what();
    }
    //std::cerr << "Getting " << filespec << ", value: " << result<< std::endl;
    return result;
}

std::map<std::string, boost::shared_ptr<temperature_t>> &
Onewire::getTemperatures(void)
{
    //DEBUGLOG_REPORT_FUNCTION;

    _temps.clear();

    std::map<std::string, boost::shared_ptr<onewire_t>>::iterator sit;
    for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
        boost::shared_ptr<temperature_t> temp(new temperature_t);
        temp->family = sit->second->family;
        temp->id = sit->second->id;
        temp->type = sit->second->type;
        std::string result;
        if (sit->second->bus == true) {
            getValue(sit->first, "w1_slave", result);
            std::cerr << "POS: " << result.find('t=') << std::endl;
            std::string value = result.substr(69, result.size());
            //std::string value = result.substr(result.find('t='), result.size());
            temp->scale = _scale;
            temp->temp = std::stof(value)/1000;
            if (_scale == 'F') {
                temp->temp = convertScale(temp->temp);
            }
            
            _temps[sit->first] = temp;
        } else {
            getValue(sit->first, "temperature", result);
            if (result.size() == 0) {
                temp->temp = 0;
            } else {
                temp->temp = std::stof(result);
            }
            getValue(sit->first, "temphigh", result);
            if (result.size() == 0) {
                temp->hightemp = 0;
            } else {
                temp->hightemp = std::stof(result);
            }
            getValue(sit->first, "templow", result);
            if (result.size() == 0) {
                temp->lowtemp = 0;
            } else {
                temp->lowtemp = std::stof(result);
            }
            
            getValue("", "/settings/units/temperature_scale", result);
            temp->scale = result[0];
            _temps[sit->first] = temp;
        }
    }
    
    return _temps;
}

void
Onewire::initTable(void)
{
//    DEBUGLOG_REPORT_FUNCTION;

    // This is a complete chart of all supported 1 wire sensors from
    // http://owfs.org/index.php?page=family-code-list. This is
    // mostly used for identifying the type of sensor, and display
    // purposes.
    _family["A2"] = {"AC Voltage", "mCM001", ACVOLTAGE};
    _family["82"] = {"Authorization", "DS1425", AUTH};
    _family["30"] = {"Battery", "DS2760", BATTERY};
    _family["32"] = {"Battery", "DS2780", BATTERY};
    _family["35"] = {"Battery", "DS2755", BATTERY};
    _family["2E"] = {"Battery", "DS2770", BATTERY};
    _family["3D"] = {"Battery", "DS2781", BATTERY};
    _family["31"] = {"Battery ID", "DS2720", BATTERY};
    _family["26"] = {"Battery monitor", "DS2438", BATTERY};
    _family["51"] = {"Battery monitor", "DS2751", BATTERY};
    _family["1B"] = {"Battery monitor", "DS2436", BATTERY};
    _family["1E"] = {"Battery monitor", "DS2437", BATTERY};
    _family["24"] = {"Clock", "DS2415", CLOCK};
    _family["27"] = {"Clock + interrupt", "DS2417", CLOCK};
    _family["36"] = {"Coulomb counter", "DS2740", UNSUPPORTED};
    _family["1D"] = {"Counter", "DS2423", UNSUPPORTED};
    _family["16"] = {"crypto-ibutton", "DS1954 DS1957", UNSUPPORTED};
    _family["B2"] = {"DC Current or Voltage", "mAM001", DCVOLTAGE};
    _family["04"] = {"EconoRam Time chi", "DS2404", UNSUPPORTED};
    _family["7E"] = {"Envoronmental Monitors", "EDS00xx", UNSUPPORTED};
    _family["41"] = {"Hygrocron", "DS1923", UNSUPPORTED};
    _family["81"] = {"ID found in DS2490R and DS2490B USB adapters", "USB id", UNSUPPORTED};
    _family["01"] = {"ID-only", "DS2401 DS2411 DS1990R DS2490A", UNSUPPORTED};
    _family["A6"] = {"IR Temperature", "mTS017", UNSUPPORTED};
    _family["06"] = {"Memory", "DS1993", UNSUPPORTED};
    _family["08"] = {"Memory", "DS1992", UNSUPPORTED};
    _family["09"] = {"Memory", "DS2502 DS2703 DS2704", UNSUPPORTED};
    _family["14"] = {"Memory", "DS2430A", UNSUPPORTED};
    _family["23"] = {"Memory", "DS2433 DS1973", UNSUPPORTED};
    _family["43"] = {"Memory", "DS28EC20", UNSUPPORTED};
    _family["0B"] = {"Memory", "DS2505", UNSUPPORTED};
    _family["0F"] = {"Memory", "DS2506", UNSUPPORTED};
    _family["2D"] = {"Memory", "DS2431 DS1972", UNSUPPORTED};
    _family["1F"] = {"Microhub", "DS2409", UNSUPPORTED};
    _family["EF"] = {"Moisture meter.4 Channel Hub 1A", "DS1963L Monetary iButton", UNSUPPORTED};
    _family["02"] = {"Multikey", "DS1991", UNSUPPORTED};
    _family["37"] = {"password EEPROM", "DS1977", UNSUPPORTED};
    _family["FC"] = {"Moisture Hub", "BAE0910 BAE0911", UNSUPPORTED};
    _family["00"] = {"Provide location information", "Link locator", UNSUPPORTED};
    _family["A0"] = {"Rotation Sensor", "mRS001", UNSUPPORTED};
    _family["18"] = {"SHA iButton", "DS1963S DS1962", UNSUPPORTED};
    _family["44"] = {"SHA-1 Authenticator", "DS28E10", UNSUPPORTED};
    _family["34"] = {"SHA-1 Battery", "DS2703", UNSUPPORTED};
    _family["33"] = {"SHA-1 ibutton", "DS1961s DS2432", UNSUPPORTED};
    _family["FF"] = {"Swart LCD", "LCD", UNSUPPORTED};
    _family["05"] = {"Switch", "Ds2405", UNSUPPORTED};
    _family["12"] = {"Switch", "DS2406", UNSUPPORTED};
    _family["29"] = {"Switch", "DS2408", UNSUPPORTED};
    _family["1C"] = {"Switch", "DS28E04-100", UNSUPPORTED};
    _family["3A"] = {"Switch", "DS2413", UNSUPPORTED};
    _family["10"] = {"Temperature", "DS18S20", TEMPERATURE};
    _family["22"] = {"Temperature", "DS1922", TEMPERATURE};
    _family["28"] = {"Temperature", "DS18B20", TEMPERATURE};
    _family["3B"] = {"Temperature/memory", "DS1825 X31826", UNSUPPORTED};
    _family["42"] = {"Temperature/IO", "DS28EA00", UNSUPPORTED};
    _family["B1"] = {"Thermocouple Converter", "mTC001", UNSUPPORTED};  
    _family["B3"] = {"Thermocouple Converter", "mTC002", UNSUPPORTED};
    _family["21"] = {"Thermocron", "DS1921", UNSUPPORTED};
    _family["EE"] = {"Ultra Violet Index", "UVI", UNSUPPORTED};
    _family["89"] = {"Uniqueware", "DS1982U", UNSUPPORTED};
    _family["8B"] = {"Uniqueware", "DS1985U", UNSUPPORTED};
    _family["8F"] = {"Uniqueware", "DS1986U", UNSUPPORTED};
    _family["2C"] = {"Varible Resitor", "DS2890", UNSUPPORTED};
    _family["A1"] = {"Vibratio", "mVM001", UNSUPPORTED};
    _family["20"] = {"Voltage", "DS2450", UNSUPPORTED};
}

std::string &
Onewire::readBus(const std::string &device, std::string &data)
{
    DEBUGLOG_REPORT_FUNCTION;

    ///sys/bus/w1/devices/28-021316a4d6aa/w1_slave

    return data;
}
               

void
Onewire::dump(void)
{
//    DEBUGLOG_REPORT_FUNCTION;

    if (_sensors.size() == 0) {
        BOOST_LOG(lg) << "No sensors found";
    }
    std::cerr << "Rootdir for sensors is: " << _rootdir << std::endl;
    
    std::map<std::string, boost::shared_ptr<onewire_t>>::iterator sit;
    for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
        std::cerr << "Device: " << sit->first << std::endl;
        std::cerr << "\tFamily: " << sit->second->family << std::endl;
        std::cerr << "\tID: " << sit->second->id << std::endl;
        std::cerr << "\tType: " << sit->second->type << std::endl;
        std::cerr << "\tBus: " << (sit->second->bus ? "true" : "false") << std::endl;
    }
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
