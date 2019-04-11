#!/usr/bin/python3

#
#   Copyright (C) 2018,2019 Free Software Foundation, Inc.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

## \copyright GNU Public License.
## \file onewire.py Setup data array  to convert sensor ID to the type
##                  of sensor and other configuration data,

def onewire_handler(arg):
    print("Start onewire")

_family = dict();
#_family['A2'] = {"description"="AC Voltage", "chips"="mCM001", "type"="ACVOLTAGE"};

_family["A2"] = {"description":"AC Voltage", "chips":"mCM001", "type":"ACVOLTAGE"};
_family["82"] = {"description":"Authorization", "chips":"DS1425", "type":"AUTH"};
_family["30"] = {"description":"Battery", "chips":"DS2760", "type":"POWER"};
_family["32"] = {"description":"Battery", "chips":"DS2780", "type":"POWER"};
_family["35"] = {"description":"Battery", "chips":"DS2755", "type":"POWER"};
_family["2E"] = {"description":"Battery", "chips":"DS2770", "type":"POWER"};
_family["3D"] = {"description":"Battery", "chips":"DS2781", "type":"POWER"};
_family["31"] = {"description":"Battery ID", "chips":"DS2720", "type":"POWER"};
_family["26"] = {"description":"Battery monitor", "chips":"DS2438", "type":"POWER"};
_family["51"] = {"description":"Battery monitor", "chips":"DS2751", "type":"POWER"};
_family["1B"] = {"description":"Battery monitor", "chips":"DS2436", "type":"POWER"};
_family["1E"] = {"description":"Battery monitor", "chips":"DS2437", "type":"POWER"};
_family["24"] = {"description":"Clock", "chips":"DS2415", "type":"CLOCK"};
_family["27"] = {"description":"Clock + interrupt", "chips":"DS2417", "type":"CLOCK"};
_family["36"] = {"description":"Coulomb counter", "chips":"DS2740", "type":"UNSUPPORTED"};
_family["1D"] = {"description":"Counter", "chips":"DS2423", "type":"UNSUPPORTED"};
_family["16"] = {"description":"crypto-ibutton", "chips":"DS1954 DS1957", "type":"UNSUPPORTED"};
_family["B2"] = {"description":"DC Current or Voltage", "chips":"mAM001", "type":"POWER"};
_family["04"] = {"description":"EconoRam Time chi", "chips":"DS2404", "type":"UNSUPPORTED"};
_family["7E"] = {"description":"Envoronmental Monitors", "chips":"EDS00xx", "type":"UNSUPPORTED"};
_family["41"] = {"description":"Hygrocron", "chips":"DS1923", "type":"UNSUPPORTED"};
_family["81"] = {"description":"ID found in DS2490R and DS2490B USB adapters", "chips":"USB id", "type":"UNSUPPORTED"};
_family["01"] = {"description":"ID-only", "chips":"DS2401 DS2411 DS1990R DS2490A", "type":"UNSUPPORTED"};
_family["A6"] = {"description":"IR Temperature", "chips":"mTS017", "type":"UNSUPPORTED"};
_family["06"] = {"description":"Memory", "chips":"DS1993", "type":"UNSUPPORTED"};
_family["08"] = {"description":"Memory", "chips":"DS1992", "type":"UNSUPPORTED"};
_family["09"] = {"description":"Memory", "chips":"DS2502 DS2703 DS2704", "type":"UNSUPPORTED"};
_family["14"] = {"description":"Memory", "chips":"DS2430A", "type":"UNSUPPORTED"};
_family["23"] = {"description":"Memory", "chips":"DS2433 DS1973", "type":"UNSUPPORTED"};
_family["43"] = {"description":"Memory", "chips":"DS28EC20", "type":"UNSUPPORTED"};
_family["0B"] = {"description":"Memory", "chips":"DS2505", "type":"UNSUPPORTED"};
_family["0F"] = {"description":"Memory", "chips":"DS2506", "type":"UNSUPPORTED"};
_family["2D"] = {"description":"Memory", "chips":"DS2431 DS1972", "type":"UNSUPPORTED"};
_family["1F"] = {"description":"Microhub", "chips":"DS2409", "type":"UNSUPPORTED"};
_family["EF"] = {"description":"Moisture meter.4 Channel Hub 1A", "chips":"DS1963L Monetary iButton", "type":"UNSUPPORTED"};
_family["02"] = {"description":"Multikey", "chips":"DS1991", "type":"UNSUPPORTED"};
_family["37"] = {"description":"password EEPROM", "chips":"DS1977", "type":"UNSUPPORTED"};
_family["FC"] = {"description":"Moisture Hub", "chips":"BAE0910 BAE0911", "type":"UNSUPPORTED"};
_family["00"] = {"description":"Provide location information", "chips":"Link locator", "type":"UNSUPPORTED"};
_family["A0"] = {"description":"Rotation Sensor", "chips":"mRS001", "type":"UNSUPPORTED"};
_family["18"] = {"description":"SHA iButton", "chips":"DS1963S DS1962", "type":"UNSUPPORTED"};
_family["44"] = {"description":"SHA-1 Authenticator", "chips":"DS28E10", "type":"UNSUPPORTED"};
_family["34"] = {"description":"SHA-1 Battery", "chips":"DS2703", "type":"UNSUPPORTED"};
_family["33"] = {"description":"SHA-1 ibutton", "chips":"DS1961s DS2432", "type":"UNSUPPORTED"};
_family["FF"] = {"description":"Swart LCD", "chips":"LCD", "type":"UNSUPPORTED"};
_family["05"] = {"description":"Switch", "chips":"Ds2405", "type":"UNSUPPORTED"};
_family["12"] = {"description":"Switch", "chips":"DS2406", "type":"UNSUPPORTED"};
_family["29"] = {"description":"Switch", "chips":"DS2408", "type":"UNSUPPORTED"};
_family["1C"] = {"description":"Switch", "chips":"DS28E04-100", "type":"UNSUPPORTED"};
_family["3A"] = {"description":"Switch", "chips":"DS2413", "type":"UNSUPPORTED"};
_family["10"] = {"description":"Temperature", "chips":"DS18S20", "type":"TEMPERATURE"};
_family["22"] = {"description":"Temperature", "chips":"DS1922", "type":"TEMPERATURE"};
_family["28"] = {"description":"Temperature", "chips":"DS18B20", "type":"TEMPERATURE"};
_family["3B"] = {"description":"Temperature/memory", "chips":"DS1825 X31826", "type":"UNSUPPORTED"};
_family["42"] = {"description":"Temperature/IO", "chips":"DS28EA00", "type":"UNSUPPORTED"};
_family["B1"] = {"description":"Thermocouple Converter", "chips":"mTC001", "type":"UNSUPPORTED"};  
_family["B3"] = {"description":"Thermocouple Converter", "chips":"mTC002", "type":"UNSUPPORTED"};
_family["21"] = {"description":"Thermocron", "chips":"DS1921", "type":"UNSUPPORTED"};
_family["EE"] = {"description":"Ultra Violet Index", "chips":"UVI", "type":"UNSUPPORTED"};
_family["89"] = {"description":"Uniqueware", "chips":"DS1982U", "type":"UNSUPPORTED"};
_family["8B"] = {"description":"Uniqueware", "chips":"DS1985U", "type":"UNSUPPORTED"};
_family["8F"] = {"description":"Uniqueware", "chips":"DS1986U", "type":"UNSUPPORTED"};
_family["2C"] = {"description":"Varible Resitor", "chips":"DS2890", "type":"UNSUPPORTED"};
_family["A1"] = {"description":"Vibratio", "chips":"mVM001", "type":"UNSUPPORTED"};
_family["20"] = {"description":"Voltage", "chips":"DS2450", "type":"UNSUPPORTED"};

