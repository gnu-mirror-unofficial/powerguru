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


class Sensors(object):
    """Data about all the sensors"""
    def __init__(self, data=dict()):
        self.sensors = dict()
        # Connect to a postgresql database
        try:
            dbname = "powerguru"
            connect = "dbname=" + dbname
            self.dbshell = psycopg2.connect(connect)
            if self.dbshell.closed == 0:
                self.dbshell.autocommit = True
                logging.info("Opened connection to %r" % dbname)

            self.dbcursor = self.dbshell.cursor()
            if self.dbcursor.closed == 0:
                logging.info("Opened cursor in %r" % dbname)

        except Exception as e:
            logging.warning("Couldn't connect to database: %r" % e)

        # Get any existing sensor data
        query = """SELECT * FROM sensors;"""
        logging.debug(query)
        self.dbcursor.execute(query)
        logging.debug("Got %r sensor records" % self.dbcursor.rowcount)
        for id,alias,location,device,sense,channel in self.dbcursor:
            data = dict()
            if id is not None:
                data['id'] = id
            if alias is not None:
                data['alias'] = alias
            if location is not None:
                data['location'] = location
            if device is not None:
                data['device'] = device
            if sense is not None:
                data['sensor'] = sense
            if channel is not None:
                data['channel'] = channel
            self.sensors[id] = SensorDevice(data)

    def dump(self):
        logging.debug("Sensor.dump(%r entries)" % len(self.sensors))
        for id,sensor in self.sensors.items():
            sensor.dump()

    def add(self, sensor):
        id = sensor.get('id')
        self.sensors[id] = sensor
        self.dbcursor.execute(sensor.MakeSQL())

    def get(self, id):
        try:
            sensor = self.sensors[id]
        except:
            sensor = None
        return sensor

class SensorDevice(object):
    """A class to hold sensor data"""
    def __init__(self, data=dict()):
        self.data = dict()
        if ('id' in data) is True:
            self.data['id'] = data['id']
        else:
            self.data['id'] = None
        if ('alias' in data) == True:
            self.data['alias'] = data['alias']
        else:
            self.data['alias'] = None
        if ('location' in data) is True:
            self.data['location'] = data['location']
        else:
            self.data['location'] = None
        if ('device' in data) is True:
            self.data['device'] = data['device']
        else:
            self.data['device'] = DeviceType.UNKNOWN
        if ('sensor' in data) is True:
            self.data['sensor'] = data['sensor']
        else:
            self.data['sensor'] = SensorType.UNKNOWN
        if ('channel' in data) is True:
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
        print("\tAlias: %r" % self.data['alias'])
        print("\tLocation: %r" % self.data['location'])
        print("\tChannel: %r" % self.data['channel'])
        if self.data['device'] ==  DeviceType.UNKNOWN:
            print("\tDevice: UNKNOWN")
        elif self.data['device'] ==  DeviceType.ONEWIRE:
            print("\tDevice: ONEWIRE")
        elif self.data['device'] ==  DeviceType.OWNET:
            print("\tDevice: OWNET")
        elif self.data['device'] ==  DeviceType.RTL433:
            print("\tDevice: RTL433")
        elif self.data['device'] ==  DeviceType.RTLSDR:
            print("\tDevice: RTLSDR")
        elif self.data['device'] ==  DeviceType.USB:
            print("\tDtevice: USB")
        elif self.data['device'] ==  DeviceType.SERIAL:
            print("\tDevice: SERIAL")
        elif self.data['device'] ==  DeviceType.GPIO:
            print("\tDevice: GPIO")

        if self.data['sensor'] ==  SensorType.UNKNOWN:
            print("\tSensor: UNKNOWN")
        elif self.data['sensor'] ==  SensorType.ACVOLTAGE:
            print("\tSensor: ACVOLTAGE")
        elif self.data['sensor'] ==  SensorType.DCVOLTAGE:
            print("\tSensor: DCVOLTAGE")
        elif self.data['sensor'] ==  SensorType.AUTH:
            print("\tSensor: AUTH")
        elif self.data['sensor'] ==  SensorType.BATTERY:
            print("\tSensor: BATTERY")
        elif self.data['sensor'] ==  SensorType.POWER:
            print("\tSensor: POWER")
        elif self.data['sensor'] ==  SensorType.CLOCK:
            print("\tSensor: CLOCK")
        elif self.data['sensor'] ==  SensorType.TEMPERATURE:
            print("\tSensor: TEMPERATURE")
        elif self.data['sensor'] ==  SensorType.MOISTURE:
            print("\tSensor: MOISTURE")
        elif self.data['sensor'] ==  SensorType.UNSUPPORTED:
            print("\tSensor: UNSUPPORTED")

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

        if self.data['id'] is not None:
            id = self.data['id']
        else:
            id = ""

        if self.data['alias'] is not None:
            alias = self.data['alias']
        else:
            alias = ""

        if self.data['location'] is not None:
            location = self.data['location']
        else:
            location = ""

        query = """INSERT INTO sensors VALUES (%r, %r, %r, %r, %r, %r) ON CONFLICT DO NOTHING;""" % (id, alias, location, device, sensor, channel)

        logging.debug(query)
        return (query)

    def populate(self, result=""):
        """Populate the internal data from an SQL query """

        self.data['id'] = data['id']
        self.data['alias'] = data['alias']
        self.data['location'] = data['location']
        self.data['device'] = data['device']
        self.data['sensor'] = data['sensor']
        self.data['channel'] = data['channel']
