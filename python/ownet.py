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

import epdb
import logging
import time
from pyownet import protocol
import onewire
import psycopg2
import sensor

def ownet_handler(args, sensors):
    logging.debug("Start ownet %r" % args)

    # Connect to a postgresql database
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
        logging.warning("Couldn't connect to database: %r" % e)
        

    # It turns out that the network connection occcsasionally times out
    # reading data, which is ok to ignore most of the time. However,
    # it's possible there is a server error, so kill the thread if
    # we exceed a threashold of pain.
    errors = 0
    error_threshold = 0
    
    # Connecting to the OW server over the network often fails due to a
    # short timeout, so try a few times before giving up
    retries = 3
    while retries > 0:
        retries -= 1
        # Get a list of all directories on the server
        try:
            owproxy = protocol.proxy(host=args['owserver'], port=4304, persistent=True)
            logging.info("Connected to OW server: %r" % args['owserver'])
            owproxy.dir()
            break
        except Exception as e:
            logging.error("Couldn't connect to OW server: %r" % e)

    interval = int(args['interval'])
    while interval > 0:
        for dir in owproxy.dir():
            logging.info("Reading data from: %s:%s"
                         % (args['owserver'], dir))
            sensor = dict()
            # Note that all the strings returned are wide strings, and
            # not ASCII. Because the 'b' qualifier for these strings
            # messes up comparisions with normal text strings. they
            # get decoded.
            try:
                family = owproxy.read(dir + 'family').decode("utf-8")
                sensor['family'] = family
                id = owproxy.read(dir + 'id').decode("utf-8")
                sensor['id'] = id;
                #sensor['id'] = id[10:12] + id[8:10] + id[6:8] + id[4:6] + id[2:4] + id[0:2]
                sensor['alias'] = owproxy.read(dir + 'alias').decode("utf-8")
                sensor['type'] = onewire._family[family]['type']
                sensor['chips'] = owproxy.read(dir + 'type').decode("utf-8")
            except Exception as e:
                logging.warning("Couldn't read from OW server: %r" % e)
                errors += 1
                time.sleep(1)   # give the server a second to recover
                continue

            #logging.debug("%r" % sensor)
            # FIXME: format query and write to database
            # dbcursor.execute(query)
            # family | id | alias | type | timestamp

            if sensor['type'] == 'TEMPERATURE':
                #logging.info("Found a temperature sensor: " + family + '.' + id)
                temp = dict()
                try:
                    temp['temperature'] = owproxy.read(dir + 'temperature').lstrip().decode("utf-8")
                    temp['lowtemp'] = owproxy.read(dir + 'templow').lstrip().decode("utf-8")
                    temp['hightemp'] = owproxy.read(dir + 'temphigh').lstrip().decode("utf-8")
                    logging.debug("Temperature data: %r" % temp)
                except Exception as e:
                    logging.warning("Couldn't read from OW server: %r" % e)
                    errors += 1
                    time.sleep(1)   # give the server a second to recover
                    continue
                # By default, all temperature readings are in 'C' (Celcius)
                # we convert to 'F' (Farenheit) if need be
                if (args['scale'] == 'F'):
                    temp['temperature'] = (float(temp['temperature']) * 1.8) + 32.0
                    if temp['lowtemp'] is not "0":
                        temp['lowtemp'] =  (float(temp['lowtemp']) * 1.8) + 32.0
                    if temp['hightemp'] is not "0":
                        temp['hightemp'] =  (float(temp['hightemp']) * 1.8) + 32.0;
                query = "INSERT INTO weather VALUES("
                query += "'" + id + "'"
                query += ", " + str(temp['temperature'])
                query += ", " + str(temp['lowtemp'])
                query += ", " + str(temp['hightemp'])
                query += ", 0"
                #query += ", " + str(temp['humidity'])
                query += ", " + "'" + args['scale'] + "'"
                query += ", '" + time.strftime("%Y-%m-%d %H:%M:%S") + "'"
                query += ");"
                #logging.debug(query)
                dbcursor.execute(query)
                # id | temperature | temphigh | templow | scale | timestamp
 
            if sensor['type'] == 'POWER':
                # logging.info("Found a power monitor sensor: " + family + '.' + id)
                batt = dict()
                try:
                    batt['current'] = owproxy.read(dir + 'current').lstrip().decode("utf-8")
                    batt['voltage'] = owproxy.read(dir + 'volts').lstrip().decode("utf-8")
                    logging.debug("Battery data: %r" % batt)
                except Exception as e:
                    logging.warning("Couldn't read from OW server: %r" % e)
                    errors += 1
                    time.sleep(1)   # give the server a second to recover
                    continue

                query = "INSERT INTO power VALUES("
                query += "'" + id + "'"
                query += ", " + batt['current']
                query += ", " + batt['voltage']
                query += ", 'DC'"
                query += ", '" + time.strftime("%Y-%m-%d %H:%M:%S") + "'"
                query += ");"
                #logging.debug(query)
                dbcursor.execute(query)
                # id | current | volts | type | timestamp

        # Don't eat up all the cpu cycles!
        time.sleep(interval)

        if errors > error_threshold:
            return
