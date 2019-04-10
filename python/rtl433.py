#!/usr/bin/python3

#
#   Copyright (C) 2019 Free Software Foundation, Inc.
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


## \file rtl433.py This file use the rtl_433 utility to collect data
##                 from wireless sensors using an RTL-SDR USB dongle.

import sys
from subprocess import PIPE, Popen, STDOUT
from threading  import Thread
import epdb
import logging
import time
import os
import psycopg2
import sensor
from datetime import datetime
import itertools, operator
from options import CmdOptions
from postgresql import Postgresql
ON_POSIX = 'posix' in sys.builtin_module_names


def rtl433_handler(sensors):
    """This is a handler for sensors using wireless at 433Mhz,
    commonly used by many wireless weather sensors. This requires
    a working RTL-SDR radio dongle, and the rtl_433 utiity installed"""
    logging.debug("Start rtl_433...")

    options = CmdOptions()
    db = Postgresql()
    #db.dump()

    #ppp = Popen(cmd, stdout=PIPE, stderr=STDOUT, bufsize=1, close_fds=ON_POSIX)
    #cmd = [ 'rtl_433', '-F', 'csv:/tmp/rtl433.csv', '-s', time.sleep(options.get('interval'))]
    cmd = [ 'rtl_433', '-F', 'csv:/tmp/rtl433.csv']
    ppp = Popen(cmd, stdout=PIPE, bufsize=0, close_fds=ON_POSIX)
    foo = open('/tmp/rtl433.csv', 'r+')
    previous = dict()
    foo.truncate()
    while True:
        out = sensor.follow(foo)
        temp = dict()
        #print("FIXME0: %r" % out)
        for line in out:
            #print("FIXME1: %r" % line)
            tokens = line.split(',')
            # Ignore the header in the csv file, which is the first line
            if tokens[0] == 'time':
                continue
            temp = dict()
            temp['timestamp'] = tokens[0]
            #epdb.set_trace()
            # Different parsing for different devices
            temp['model'] = tokens[3]
            if temp['model'] == 'WT0124 Pool Thermometer':
                # FIXME: use an array to convert a numberr to the file
                # channel alpha numeric
                temp['channel'] ='A'
                temp['temperature'] = str(tokens[8])
                temp['humidity'] = "0"
                temp['id'] = tokens[10]
            elif temp['model'] == 'Acurite tower sensor':
                temp['id'] = tokens[5]
                temp['channel'] = tokens[6]
                temp['temperature'] = tokens[8]
                temp['humidity'] = tokens[11]

            # We don't want to record more samples than the specified interval,
            FMT = "%Y-%m-%d %H:%M:%S"

            try:
                if ('id' in temp) is True:
                    if (temp['id'] in previous) is False:
                        previous[temp['id']] = datetime.now().strftime(FMT)
                tdelta = datetime.strptime(temp['timestamp'], FMT) - datetime.strptime(previous[temp['id']], FMT)
            #logging.debug("TDELTA: %r: %r == %r" % (tdelta.seconds, temp['timestamp'], previous[temp['id']]))
            #epdb.set_trace()
                if tdelta.seconds <= options.get('interval') and tdelta.days != -1:
                #logging.debug("Not doing anything for %r!" % temp['id'])
                    continue
            except:
                continue
            previous[temp['id']] = temp['timestamp']
            if sensors.get(temp['id']) is None:
                print("New sensor %r found!" % temp['id'])
                sense = sensor.SensorDevice()
                sense.set('id', temp['id'])
                sense.set('alias', temp['model'])
                sense.set('device', sensor.DeviceType.RTL433)
                sense.set('sensor', sensor.SensorType.TEMPERATURE)
                sense.set('channel', temp['channel'])
                sensors.add(sense)
            #else:
                #sensor.sensors[temp['id']]['channel'] = temp['channel']
                #sensor.sensors[temp['id']]['device'] = DeviceType.RTL433
            #sensors.dump()
            # Convert from Celcius if needed
            #epdb.set_trace()
            if (options.get('scale') == 'F'):
                temp['temperature'] = (float(temp['temperature']) * 1.8) + 32.0;
                #temp['lowtemp'] =  (float(temp['lowtemp']) * 1.8) + 32.0;
                #temp['hightemp'] =  (float(temp['hightemp']) * 1.8) + 32.0;
            query = """INSERT INTO weather VALUES( '%s', %s, %s, %s, %s, '%s', '%s' )  ON CONFLICT DO NOTHING;; """ % (temp['id'], temp['temperature'], "0", "0",  temp['humidity'], options.get('scale'), temp['timestamp'])
            logging.debug(query)
            db.query(query)
        ppp.truncate()
 
    # _sensors = list()

