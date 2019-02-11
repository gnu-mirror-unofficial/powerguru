#!/usr/bin/python3

# 
#   Copyright (C) 2019 Free Software Foundation, Inc.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
# 

# API documentation at: https://pyownet.readthedocs.io/en/latest/

import sys
import epdb
import logging
import psycopg2
from datetime import datetime
from enum import Enum


class SensorType(Enum):
    UNKNOWN = 0
    ACVOLTAGE = 1
    DCVOLTAGE = 2
    AUTH = 3
    BATTERY = 4
    POWER = 5
    CLOCK = 6
    TEMPERATURE = 7
    MOISTURE = 8
    UNSUPPORTED = 9

class DeviceType(Enum):
    UNKNOWN = 0
    ONEWIRE = 1
    OWNET = 2
    RTL433 = 3
    RTLSDR = 4
    USB = 5
    SERIAL = 6
    GPIO = 7

class Sensor(object):
    def __init__(self, data=dict()):
        """A class to hold weather sensor data"""
        self.data = dict()
        if data['id'] is not None and data['id'] is not "":
            self.data['id'] = data['id']
        else:
            self.data['id'] = None
        if data['alias'] is not None and data['alias'] is not "":
            self.data['alias'] = data['alias']
        else:
            self.data['alias'] = None
        if data['location'] is not None and data['location'] is not "":
            self.data['location'] = data['location']
        else:
            self.data['location'] = None
        if data['device'] is not None and data['device'] is not "":
            self.data['device'] = data['device']
        else:
            self.data['device'] = DeviceType.UNKNOWN
        if data['sensor'] is not None and data['sensor'] is not "":
            self.data['sensor'] = SensorType.UNKNOWN
        else:
            self.data['sensor'] = SensorType.UNKNOWN
        if data['channel'] is not None and data['channel'] is not "":
            self.data['channel'] = data['channel']
        else:
            self.data['channel'] = None

    def set(self, index, value):
        """Set an internal value for this sensor"""
        if (index in self.data) == True:
            self.data[index] = value
        else:
            logging.warning("Key %r doesn't exist in data structure" % index)

    def get(self, index):
        """Get an internal value for this sensor"""
        if (index in self.data) == True:
            return self.data[index]
        else:
            logging.warning("Key %r doesn't exist in data structure" % index)

    def dump(self):
        """ Dump the data about this sensor"""
        print("ID: %r" % self.data['id'])
        print("Alias: %r" % self.data['alias'])
        print("Location: %r" % self.data['location'])
        print("Channel: %r" % self.data['channel'])
        if self.data['device'] ==  DeviceType.UNKNOWN:
            print("Device: UNKNOWN")
        elif self.data['device'] ==  DeviceType.ONEWIRE:
            print("Device: ONEWIRE")
        elif self.data['device'] ==  DeviceType.OWNET:
            print("Device: OWNET")
        elif self.data['device'] ==  DeviceType.RTL433:
            print("Device: RTL433")
        elif self.data['device'] ==  DeviceType.RTLSDR:
            print("Device: RTLSDR")
        elif self.data['device'] ==  DeviceType.USB:
            print("Device: USB")
        elif self.data['device'] ==  DeviceType.SERIAL:
            print("Device: SERIAL")
        elif self.data['device'] ==  DeviceType.GPIO:
            print("Device: GPIO")

        if self.data['sensor'] ==  SensorType.UNKNOWN:
            print("Sensor: UNKNOWN")
        elif self.data['sensor'] ==  SensorType.ACVOLTAGE:
            print("Sensor: ACVOLTAGE")
        elif self.data['sensor'] ==  SensorType.DCVOLTAGE:
            print("Sensor: DCVOLTAGE")
        elif self.data['sensor'] ==  SensorType.AUTH:
            print("Sensor: AUTH")
        elif self.data['sensor'] ==  SensorType.BATTERY:
            print("Sensor: BATTERY")
        elif self.data['sensor'] ==  SensorType.POWER:
            print("Sensor: POWER")
        elif self.data['sensor'] ==  SensorType.CLOCK:
            print("Sensor: CLOCK")
        elif self.data['sensor'] ==  SensorType.TEMPERATURE:
            print("Sensor: TEMPERATURE")
        elif self.data['sensor'] ==  SensorType.MOISTURE:
            print("Sensor: MOISTURE")
        elif self.data['sensor'] ==  SensorType.UNSUPPORTED:
            print("Sensor: UNSUPPORTED")

    def MakeSQL(self):
        """ Format the SQL query to add this sensor"""
        if self.data['device'] ==  DeviceType.UNKNOWN:
            device = "UNKNOWN"
        elif self.data['device'] ==  DeviceType.ONEWIRE:
            device = "ONEWIRE"
        elif self.data['device'] ==  DeviceType.OWNET:
            device = "OWNET"
        elif self.data['device'] ==  DeviceType.RTL433:
            device = "RTL433"
        elif self.data['device'] ==  DeviceType.RTLSDR:
            device = "RTLSDR"
        elif self.data['device'] ==  DeviceType.USB:
            device = "USB"
        elif self.data['device'] ==  DeviceType.SERIAL:
            device = "SERIAL"
        elif self.data['device'] ==  DeviceType.GPIO:
            device = "GPIO"

        if self.data['sensor'] ==  SensorType.UNKNOWN:
            sensor = "UNKNOWN"
        elif self.data['sensor'] ==  SensorType.ACVOLTAGE:
            sensor = "ACVOLTAGE"
        elif self.data['sensor'] ==  SensorType.DCVOLTAGE:
            sensor = "DCVOLTAGE"
        elif self.data['sensor'] ==  SensorType.AUTH:
            sensor = "AUTH"
        elif self.data['sensor'] ==  SensorType.BATTERY:
            sensor = "BATTERY"
        elif self.data['sensor'] ==  SensorType.POWER:
            sensor = "POWER"
        elif self.data['sensor'] ==  SensorType.CLOCK:
            sensor = "CLOCK"
        elif self.data['sensor'] ==  SensorType.TEMPERATURE:
            sensor = "TEMPERATURE"
        elif self.data['sensor'] ==  SensorType.MOISTURE:
            sensor = "MOISTURE"
        elif self.data['sensor'] ==  SensorType.UNSUPPORTED:
            sensor = "UNSUPPORTED"

        if self.data['channel'] is not None:
            channel = self.data['channel']
        else:
            channel = ''

        query = """INSERT INTO sensors VALUES (%r, %r, %r, %r, %r, %r) ON CONFLICT DO NOTHING;""" % (self.data['id'], self.data['alias'], self.data['location'], device, sensor, channel)

        logging.debug(query)
        return (query)
