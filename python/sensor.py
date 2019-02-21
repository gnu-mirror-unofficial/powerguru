#!/usr/bin/python3

"""
   Copyright (C) 2019 Free Software Foundation, Inc.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

import sys
import epdb
import logging
import psycopg2
from datetime import datetime
from enum import Enum
from postgresql import Postgresql


# Types of sensors. These enums and their string values must match
# the database schema's enum. See powerguru.sql for details.
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
sensorStrings= ('UNKNOWN', 'ACVOLTAGE', 'DCVOLTAGE', 'AUTH', 'BATTERY', 'POWER', 'CLOCK', 'TEMPERATURE', 'MOISTURE', 'UNSUPPORTED' )

# Types of devices. These enums and their string values must match
# the database schema's enum. See powerguru.sql for details.
class DeviceType(Enum):
    UNKNOWN = 0
    ONEWIRE = 1
    OWNET = 2
    RTL433 = 3
    RTLSDR = 4
    USB = 5
    SERIAL = 6
    GPIO = 7

deviceStrings = ('UNNOWN', 'ONEWIRE', 'OWNET', 'RTL433', 'RTLSDR', 'USB', 'SERIAL', 'GPIO')

class Sensors(object):
    """Data about all the sensors"""
    def __init__(self, data=dict()):
        self.active = False
        self.sensors = dict()
        self.db = Postgresql()

        # Get any existing sensor data
        query = """SELECT * FROM sensors;"""
        logging.debug(query)
        result = self.db.query(query)
        logging.debug("Got %r sensor records: %r" % (self.db.rowcount, len(result)))
        for id,alias,location,device,sense,channel in result:
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

    def makeXML(self, xml=""):
        """Create an XML string of all the sensor data"""
        for id,sensor in self.sensors.items():
            xml += sensor.makeXML()
        return xml

    def dump(self, result=""):
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

    def makeXML(self, xml=""):
        """Convert data about each sensor into XML for the remote API"""
        xml="""<ID>%r</ID><ALIAS>%r</ALIAS><LOCATION>%r</LOCATION><CHANNEL>%r</CHANNEL><DEVICE>%r</DEVICE><SENSOR>%r</SENSOR>""" % (self.data['id'], self.data['alias'], self.data['location'], self.data['channel'], self.data['device'], self.data['sensor'])
        return xml

    def dump(self):
        """ Dump the data about this sensor"""
        print("ID: %r" % self.data['id'])
        print("\tAlias: %r" % self.data['alias'])
        print("\tLocation: %r" % self.data['location'])
        print("\tChannel: %r" % self.data['channel'])
        print("\tDevice: %r" % self.data['device'])
        print("\tSensor: %r" % self.data['sensor'])

    def MakeSQL(self):
        """ Format the SQL query to add this sensor"""
        device = deviceStrings[self.data['device']]
        sense = sensorStrings[self.data['sensor']]

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

        query = """INSERT INTO sensors VALUES (%r, %r, %r, %r, %r, %r) ON CONFLICT DO NOTHING;""" % (id, alias, location, device, sense, channel)

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
