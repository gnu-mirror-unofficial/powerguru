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

#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <cstdio>
#include "onewire.h"

extern LogFile dbglogfile;

Onewire::Onewire(void)
    : _scale('F'),
      _poll_sleep(300),
    _mounted(false),
    _rootdir("/mnt/1wire")
{
    DEBUGLOG_REPORT_FUNCTION;

    boost::filesystem::path p(_rootdir);
    try {
        if (boost::filesystem::exists(p) & boost::filesystem::is_directory(p)) {
            for (boost::filesystem::directory_entry & x : boost::filesystem::directory_iterator(p)) {
                if (boost::regex_match(x.path().string(), boost::regex(_rootdir + "/[A-Z0-9.]*"))) {
                    std::cout << x.path() << " is a device" << std::endl;
                    boost::shared_ptr<onewire_t> owire(new onewire_t);
                    owire->device = x.path().string();
                    std::string result;
                    owire->family = getValue(owire->device, "family", result);
                    owire->type = getValue(owire->device, "type", result);
                    owire->id = getValue(owire->device, "id", result);
                    _sensors[x.path().string()] = owire;
                }
            }
        } else {
            std::cerr << "ERROR: " << _rootdir << " doesn't exist" << std::endl;
            _mounted = false;
        }
    } catch (const boost::filesystem::filesystem_error& ex) {
        std::cout << ex.what() << std::endl;
    }

    setValue("", "/settings/units/temperature_scale", "F");
    dump();
}

void
Onewire::setValue(const std::string &device, const std::string &file,
                  const std::string &value)
{
    DEBUGLOG_REPORT_FUNCTION;

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
    std::cerr << "Setting " << filespec << " to value: " << value <<  std::endl;
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
        std::ifstream entry(filespec);
        entry.rdbuf()->pubsetbuf(0, 0);
        entry >> result;
        entry.close();
    } catch (const std::exception& e) {
        dbglogfile << "Warning: iostream failure! " << e.what() << std::endl;
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
        getValue(sit->first, "family", temp->family);
        getValue(sit->first, "id", temp->id);
        getValue(sit->first, "type", temp->type);
        std::string result;
        temp->temp = std::stof(getValue(sit->first, "temperature", result));
        temp->hightemp = std::stof(getValue(sit->first, "temphigh", result));
        temp->lowtemp = std::stof(getValue(sit->first, "templow", result));
        getValue("", "/settings/units/temperature_scale", result);
        temp->scale = result[0];
        _temps[sit->first] = temp;
    }
    
    return _temps;
}

void
Onewire::dump(void)
{
    DEBUGLOG_REPORT_FUNCTION;

    std::map<std::string, boost::shared_ptr<onewire_t>>::iterator sit;
    for (sit = _sensors.begin(); sit != _sensors.end(); sit++) {
        std::cerr << "Device: " << sit->first << std::endl;
        std::cerr << "\tFamily: " << sit->second->family << std::endl;
        std::cerr << "\tID: " << sit->second->id << std::endl;
        std::cerr << "\tType: " << sit->second->type << std::endl;
    }
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
