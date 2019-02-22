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
from subprocess import PIPE, Popen, STDOUT
from threading  import Thread
import epdb
import logging
import time
import os
import psycopg2
import sensor
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
    cmd = [ 'rtl_433', '-F', 'csv', '-R', '40', '-T', '60']
    while True:
        ppp = Popen(cmd, stdout=PIPE, bufsize=0, close_fds=ON_POSIX)
        out, err = ppp.communicate()
        #out, err = ppp.communicate(timeout=0.5)
        #print("FIXME0: %r" % out)
        for line in out.splitlines():
            #mapper = map
            #for line in ppp.readline():
            #print("FIXME: %r" % line)
            str = line.decode('utf8')
            tokens = str.split(',')
            # this is just the csv header fields
            if tokens[0] == 'time':
                continue
            temp = dict()
            temp['timestamp'] = tokens[0]
            temp['model'] = tokens[3]
            temp['id'] = tokens[4]
            temp['channel'] = tokens[6]
            temp['temperature'] = tokens[7]
            temp['humidity'] = tokens[8]
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
            if (options.get('scale') == 'F'):
                temp['temperature'] = (float(temp['temperature']) * 1.8) + 32.0;
                #temp['lowtemp'] =  (float(temp['lowtemp']) * 1.8) + 32.0;
                #temp['hightemp'] =  (float(temp['hightemp']) * 1.8) + 32.0;
            query = """INSERT INTO weather VALUES( '%s', %s, %s, %s, %s, '%s', '%s' )  ON CONFLICT DO NOTHING;; """ % (temp['id'], temp['temperature'], "0", "0",  temp['humidity'], options.get('scale'), temp['timestamp'])
            logging.debug(query)
            db.query(query)
        time.sleep(options.get('interval'))
 
    # _sensors = list()

