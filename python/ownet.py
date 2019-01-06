#!/usr/bin/python3

# 
#   Copyright (C) 2019   Free Software Foundation, Inc.
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

# https://pyownet.readthedocs.io/en/latest/

import epdb
import logging
import time
from pyownet import protocol
import onewire
import psycopg2

def ownet_handler(args):
    logging.debug("Start ownet %r" % args)

    try:
        dbname = "powerguru"
        connect = "dbname=" + dbname
        dbshell = psycopg2.connect(connect)
        if dbshell.closed == 0:
            dbshell.autocommit = True
            logging.info("Opened connection to %r" % dbname)
            
            dbcursor = dbshell.cursor()
            if dbcursor.closed == 0:
                logging.info("Opened cursor in %r" % dbname)
                
    except Exception as e:
        logging.error("Couldn't connect to database: %r" % e)
        

    _sensors = list()
    try:
        owproxy = protocol.proxy(host="pi", port=4304)
        owproxy.dir()
    except Exception as e:
        logging.error("Couldn't connect to OW server: %r" % e)
        
    for dir in owproxy.dir():
        logging.info("Checking directory: " + dir)
        sensor = dict()
        family = owproxy.read(dir + 'family').decode("utf-8")
        sensor['family'] = family
        id = owproxy.read(dir + 'id').decode("utf-8")
        sensor['id'] = id;
        sensor['alias'] = owproxy.read(dir + 'alias')
        sensor['type'] = onewire._family[family]['type']
        sensor['chips'] = owproxy.read(dir + 'type')
        #logging.debug("%r" % sensor)
        _sensors.append(sensor)
        # family | id | alias | type | timestamp

        if sensor['type'] == 'TEMPERATURE':
            logging.info("Found a temperature sensor: " + family + '.' + id)
            temp = dict()
            temp['temperature'] = owproxy.read(dir + 'temperature').lstrip().decode("utf-8")
            temp['lowtemp'] = owproxy.read(dir + 'templow').lstrip().decode("utf-8")
            temp['hightemp'] = owproxy.read(dir + 'temphigh').lstrip().decode("utf-8")
            logging.debug("Temperature data: %r" % temp)
            query = "INSERT INTO temperature VALUES("
            query += "'" + id + "'"
            query += ", " + temp['temperature']
            query += ", " + temp['lowtemp']
            query += ", " + temp['hightemp']
            query += ", " + "'F'"
            query += ", '" + time.strftime("%Y-%m-%d %H:%M:%S") + "'"
            query += ");"
            logging.debug(query)
            dbcursor.execute(query)
            # id | temperature | temphigh | templow | scale | timestamp
 
        if sensor['type'] == 'BATTERY':
            logging.info("Found a power monitor sensor: " + family + '.' + id)
            batt = dict()
            batt['current'] = owproxy.read(dir + 'current').lstrip().decode("utf-8")
            batt['voltage'] = owproxy.read(dir + 'volts').lstrip().decode("utf-8")
            logging.debug("Battery data: %r" % batt)
            query = "INSERT INTO battery VALUES("
            query += "'" + id + "'"
            query += ", " + batt['current']
            query += ", " + batt['voltage']
            query += ", 'DC'"
            query += ", '" + time.strftime("%Y-%m-%d %H:%M:%S") + "'"
            query += ");"
            logging.debug(query)
            dbcursor.execute(query)
            # id | current | volts | type | timestamp
