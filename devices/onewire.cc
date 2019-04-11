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

/// \copyright GNU Public License.
/// \file onewire.cc Class for 1wire sensors

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/endian/conversion.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <streambuf>
#include <sstream>
#include "onewire.h"
#include "log.h"

//std::map<const std::string, family_t> initTable(
//    std::map<const std::string, family_t> &result);

///
/// \class Onewire
/// Construct a class for 1wire sensors
///
Onewire::Onewire(void)
    : _scale('F'),
      _poll_sleep(300),
    _mounted(false),
    _rootdir("/mnt/1wire")
{
//    DEBUGLOG_REPORT_FUNCTION;

    // Initialize the table of family types
    initTable(_family);

    boost::filesystem::path p(_rootdir);
    try {
        if (boost::filesystem::exists(p) & boost::filesystem::is_directory(p)) {
            for (boost::filesystem::directory_entry & x : boost::filesystem::directory_iterator(p)) {
                if (boost::regex_match(x.path().string(), boost::regex(_rootdir + "/[A-Z0-9.]*"))) {
                    boost::shared_ptr<onewire_t> owire(new onewire_t);
                    owire->device = x.path().string();
                    std::string result;
                    owire->family = getValue(owire->device, "family", result);
                    if (owire->family.empty()) { // not a sensor
                        continue;
                    }
                    owire->id = getValue(owire->id, "id", result);
                    owire->id = getValue(owire->alias, "alias", result);
                    owire->type = _family[owire->family].type;
                    owire->device = owire->family + "." + owire->id;
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
    
    if (_sensors.size() == 0) {
        _rootdir  = "/sys/bus/w1/devices";
        boost::filesystem::path p(_rootdir);
        if (boost::filesystem::exists(p) & boost::filesystem::is_directory(p)) {
            for (boost::filesystem::directory_entry & x : boost::filesystem::directory_iterator(p)) {
                if (boost::regex_match(x.path().string(), boost::regex(_rootdir + "/[a-z0-9-]*"))) {
                    _mounted = true;
                    boost::shared_ptr<onewire_t> owire(new onewire_t);
                    std::cerr << x.path().string().size() << std::endl;
                    std::string result;
                    owire->device = x.path().filename().string();
                    getValue(owire->device, "name", result);
                    owire->family = owire->device.substr(0,2);
                    owire->id = owire->device.substr(3, owire->device.size());
                    owire->bus = true;
                    owire->type = _family[owire->family].type;
                    _sensors[x.path().string()] = owire;
                }
            }  
        }
    }
    
    setValue("", "/settings/units/temperature_scale", "F");
    dump();
}

Onewire::~Onewire(void)
{
//    DEBUGLOG_REPORT_FUNCTION;
};

///
/// Set the value in a 1wire file
/// @param device The full device name including the family, ie... "28.021316A4D6AA"
/// @param file The base name of the file to read
/// @param value The value to set the file to
///
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

///
/// Get the value from a 1wire file
/// @param device The full device name including the family, ie... "28.021316A4D6AA"
/// @param file The base name of the file to read
/// @param result String to hold the returned value
/// @return the value from the file
///
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
        std::ifstream entry(filespec);
        entry.rdbuf()->pubsetbuf(0, 0);
        std::stringstream buffer;
        buffer << entry.rdbuf();
        result = buffer.str();
        entry.close();
    } catch (const std::exception& e) {
        BOOST_LOG_SEV(lg, severity_level::warning) << "Warning: iostream failure! "
                                                 << e.what();
    }
    //std::cerr << "Getting " << filespec << ", value: " << result<< std::endl;
    return result;
}

///
/// Get all the temperatures from all the temperture sensors
/// @return The temperatures from all temperature sensors
///

const boost::shared_ptr<battery_t>
Onewire::getBattery(const std::string &device)
{
    //DEBUGLOG_REPORT_FUNCTION;
    // boost::shared_ptr<temperature_t> temp = boost::make_shared<temperature_t>(1);
    std::string family = _sensors[device]->family;
    if (_sensors[device]->type == BATTERY) {
        boost::shared_ptr<battery_t> batt(new battery_t);
        std::string result;
        batt->id = getValue(device, "id", result);
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

const boost::shared_ptr<temperature_t>
Onewire::getTemperature(const std::string &device)
{
    //DEBUGLOG_REPORT_FUNCTION;

    std::string family = _sensors[device]->family;
    bool bus = _sensors[device]->bus;
    boost::shared_ptr<temperature_t> temp(new temperature_t);
    // Data stored
    int64_t idnum = std::stoll(_sensors[device]->id, 0, 16);
    std::stringstream id;
    id << std::hex << std::setw(12) << std::setfill('0') << boost::endian::endian_reverse(idnum << 16);
    temp->id = boost::algorithm::to_upper_copy(id.str());
    std::string result;
    if (bus) {
        getValue(device, "w1_slave", result);
        // FIXME: calculate the position value
        std::string value = result.substr(69, result.size());
        temp->scale = _scale;
        temp->temp = std::stof(value)/1000;
        temp->lowtemp = 0;
        temp->hightemp = 0;
        if (_scale == 'F') {
            temp->temp = convertScale(temp->temp);
        }
    } else {
        getValue(device, "temperature", result);
        if (result.size() == 0) {
            temp->temp = 0;
        } else {
            temp->temp = std::stof(result);
        }
        getValue(device, "temphigh", result);
        if (result.size() == 0) {
            temp->hightemp = 0;
        } else {
            temp->hightemp = std::stof(result);
        }
        getValue(device, "templow", result);
        if (result.size() == 0) {
            temp->lowtemp = 0;
        } else {
            temp->lowtemp = std::stof(result);
        }

        getValue("", "/settings/units/temperature_scale", result);
        temp->scale = result[0];
    }

    return temp;
}

///
/// Dump data about the sensors
///
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

///
/// Initialize the table of 1wire sensor data
///
void
initTable(std::map<std::string, family_t> &result)
{
//    DEBUGLOG_REPORT_FUNCTION;

    // This is a complete chart of all supported 1 wire sensors from
    // http://owfs.org/index.php?page=family-code-list. This is
    // mostly used for identifying the type of sensor, and display
    // purposes.
    result["A2"] = {"AC Voltage", "mCM001", ACVOLTAGE};
    result["82"] = {"Authorization", "DS1425", AUTH};
    result["30"] = {"Battery", "DS2760", BATTERY};
    result["32"] = {"Battery", "DS2780", BATTERY};
    result["35"] = {"Battery", "DS2755", BATTERY};
    result["2E"] = {"Battery", "DS2770", BATTERY};
    result["3D"] = {"Battery", "DS2781", BATTERY};
    result["31"] = {"Battery ID", "DS2720", BATTERY};
    result["26"] = {"Battery monitor", "DS2438", BATTERY};
    result["51"] = {"Battery monitor", "DS2751", BATTERY};
    result["1B"] = {"Battery monitor", "DS2436", BATTERY};
    result["1E"] = {"Battery monitor", "DS2437", BATTERY};
    result["24"] = {"Clock", "DS2415", CLOCK};
    result["27"] = {"Clock + interrupt", "DS2417", CLOCK};
    result["36"] = {"Coulomb counter", "DS2740", UNSUPPORTED};
    result["1D"] = {"Counter", "DS2423", UNSUPPORTED};
    result["16"] = {"crypto-ibutton", "DS1954 DS1957", UNSUPPORTED};
    result["B2"] = {"DC Current or Voltage", "mAM001", BATTERY};
    result["04"] = {"EconoRam Time chi", "DS2404", UNSUPPORTED};
    result["7E"] = {"Envoronmental Monitors", "EDS00xx", UNSUPPORTED};
    result["41"] = {"Hygrocron", "DS1923", UNSUPPORTED};
    result["81"] = {"ID found in DS2490R and DS2490B USB adapters", "USB id", UNSUPPORTED};
    result["01"] = {"ID-only", "DS2401 DS2411 DS1990R DS2490A", UNSUPPORTED};
    result["A6"] = {"IR Temperature", "mTS017", UNSUPPORTED};
    result["06"] = {"Memory", "DS1993", UNSUPPORTED};
    result["08"] = {"Memory", "DS1992", UNSUPPORTED};
    result["09"] = {"Memory", "DS2502 DS2703 DS2704", UNSUPPORTED};
    result["14"] = {"Memory", "DS2430A", UNSUPPORTED};
    result["23"] = {"Memory", "DS2433 DS1973", UNSUPPORTED};
    result["43"] = {"Memory", "DS28EC20", UNSUPPORTED};
    result["0B"] = {"Memory", "DS2505", UNSUPPORTED};
    result["0F"] = {"Memory", "DS2506", UNSUPPORTED};
    result["2D"] = {"Memory", "DS2431 DS1972", UNSUPPORTED};
    result["1F"] = {"Microhub", "DS2409", UNSUPPORTED};
    result["EF"] = {"Moisture meter.4 Channel Hub 1A", "DS1963L Monetary iButton", UNSUPPORTED};
    result["02"] = {"Multikey", "DS1991", UNSUPPORTED};
    result["37"] = {"password EEPROM", "DS1977", UNSUPPORTED};
    result["FC"] = {"Moisture Hub", "BAE0910 BAE0911", UNSUPPORTED};
    result["00"] = {"Provide location information", "Link locator", UNSUPPORTED};
    result["A0"] = {"Rotation Sensor", "mRS001", UNSUPPORTED};
    result["18"] = {"SHA iButton", "DS1963S DS1962", UNSUPPORTED};
    result["44"] = {"SHA-1 Authenticator", "DS28E10", UNSUPPORTED};
    result["34"] = {"SHA-1 Battery", "DS2703", UNSUPPORTED};
    result["33"] = {"SHA-1 ibutton", "DS1961s DS2432", UNSUPPORTED};
    result["FF"] = {"Swart LCD", "LCD", UNSUPPORTED};
    result["05"] = {"Switch", "Ds2405", UNSUPPORTED};
    result["12"] = {"Switch", "DS2406", UNSUPPORTED};
    result["29"] = {"Switch", "DS2408", UNSUPPORTED};
    result["1C"] = {"Switch", "DS28E04-100", UNSUPPORTED};
    result["3A"] = {"Switch", "DS2413", UNSUPPORTED};
    result["10"] = {"Temperature", "DS18S20", TEMPERATURE};
    result["22"] = {"Temperature", "DS1922", TEMPERATURE};
    result["28"] = {"Temperature", "DS18B20", TEMPERATURE};
    result["3B"] = {"Temperature/memory", "DS1825 X31826", UNSUPPORTED};
    result["42"] = {"Temperature/IO", "DS28EA00", UNSUPPORTED};
    result["B1"] = {"Thermocouple Converter", "mTC001", UNSUPPORTED};  
    result["B3"] = {"Thermocouple Converter", "mTC002", UNSUPPORTED};
    result["21"] = {"Thermocron", "DS1921", UNSUPPORTED};
    result["EE"] = {"Ultra Violet Index", "UVI", UNSUPPORTED};
    result["89"] = {"Uniqueware", "DS1982U", UNSUPPORTED};
    result["8B"] = {"Uniqueware", "DS1985U", UNSUPPORTED};
    result["8F"] = {"Uniqueware", "DS1986U", UNSUPPORTED};
    result["2C"] = {"Varible Resitor", "DS2890", UNSUPPORTED};
    result["A1"] = {"Vibratio", "mVM001", UNSUPPORTED};
    result["20"] = {"Voltage", "DS2450", UNSUPPORTED};
}
    
// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
