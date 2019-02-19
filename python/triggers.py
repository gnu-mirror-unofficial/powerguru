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

import epdb
import logging
from lxml import etree
from lxml.etree import tostring
from enum import Enum
from datetime import datetime
import psycopg2


# Types of events. These enums and their string values must match
# the database schema's enum. See powerguru.sql for details.
class triggerType(Enum):
    FIRE_STARTED = 0
    FIRE_OUT = 1
    DOOR_OPEN = 2
    DOOR_CLOSED = 3
    WINDOW_OPEN = 4
    WINDOW_CLOSED = 5
    GENERIC = 6
    DEVICE_ON = 7
    DEVICE_OFF = 8
triggerStrings = ('FIRE_STARTED', 'FIRE_OUT', 'DOOR_OPEN', 'DOOR_CLOSED', 'WINDOW_OPEN', 'WINDOW_CLOSED', 'GENERIC', 'DEVICE_ON', 'DEVICE_OFF' )


class Triggers(object):
    """Handle events"""

    def __init__(self):
        self.data = dict()
        self.data['id'] = ""
        self.data['name'] = ""
        self.data['trigger'] = triggerType.GENERIC
        # Note tha this uses the timestamp from when this class
        # is instantiated, and should letter be overwritten
        # when the instantiated object is upated with a new event
        self.data['timestamp'] = datetime.now()

        dbname = ""
        connect = ""
        if options['dbserver'] is "localhost":
            connect += " dbname='" + options['dbname'] + "'"
        else:
            connect += "host='" + options['dbserver'] + "'"
            connect += " dbname='" + options['dbname'] + "'"
            
            logging.debug(connect)
            shelf.dbshell = psycopg2.connect(connect)
            if self.dbshell.closed != 0:
                logging.error("Couldn't connect with %r" % connect)
                quit();
                
                self.dbshell.autocommit = True
                logging.info("Opened connection to %r" % options['dbserver'])

                self.dbcursor = self.dbshell.cursor()
                if self.dbcursor.closed != 0:
                    logging.error("Couldn't get a cursor from %r" % options['dbname'])
                    quit();

                logging.info("Opened cursor in %r" % options['dbserver'])

    def set(self, key, value=""):
        self.data[key] = value

    def get(self, key):
        return self.data[key]

    def writeSQL(self, sql=""):
        if sql is None:
            querying = self.makeSQL(sql)
        logging.debug(query)
        self.dbcursor.execute(query)

    def makeSQL(self, sql=""):
        timestamp = datetime.now()
        sql = """INSERT INTO events VALUES (%r, %r, %r, %r)""" % (self.data['id'], self.data['name'], self.data['trigger'], self.data['timestamp'].strftime("%Y-%m-%d %H:%M:%S"))
        return sql

    def fromXML(self, xml):
        logging.debug("fromXML: %r" % xml)
        if len(xml) > 0:
            logging.debug("Has chidren")
            for child in xml:
                self.data[child.tag.lower()] = child.text

    def dump(self):
        print("ID: %r" % self.data['id'])
        print("Name: %r" % self.data['name'])
        print("Trigger: %r" % self.data['trigger'])
        print("Timestamp: %r" % self.data['timestamp'].strftime("%Y-%m-%d %H:%M:%S"))

