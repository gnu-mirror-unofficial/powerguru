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

def follow(thefile):
    thefile.seek(0,2)
    while True:
        line = thefile.readline()
        if not line:
            time.sleep(0.1)
            continue
        yield line

#def gpio_ts_handler(sensors):
def gpio_ts_handler():
    """This is a handler for sensors using wireless at 433Mhz,
    commonly used by many wireless weather sensors. This requires
    a working gpio-ts driver module and the f007th utiity installed
    from https://github.com/alex-konshin/f007th-rpi.git

    Typical output looks like this:
    2019-02-25 09:24:55 MST
      type              = AcuRite 00592TXR
      channel           = B
      rolling code      = 75
      temperature       = 43.1F
      humidity          = 32%
      battery           = OK

    The thermometers send data on an interval, if nothing has changed it
    displays: "Data is not changed."

    """
    
    logging.debug("Start f007th-rpi_send...")

    options = CmdOptions()
    #db = Postgresql()

    cmd = [ 'f007th-send', '-v', '-l', '/tmp/xxx', '-g', '27']
    #cmd = [ 'f007th-send', '-v', '-V', '-g', '27']
    #ppp = Popen(cmd, stdout=PIPE, stderr=STDOUT, bufsize=0, close_fds=ON_POSIX)
    #epdb.set_trace()
    ppp = Popen(cmd, stdout=PIPE, bufsize=0, close_fds=ON_POSIX)
    foo = open('/tmp/xxx', 'r')
    while True:
        #out, err = ppp.communicate()
        out = follow(foo)
        print("FIXME0: %r" % out)
        for line in out:
            print("FIXME1: %r" % line)
        #mapper = map
        #for line in ppp.readline():
        temp = dict()
        #temp['timestamp'] = tokens[0]
        #temp['model'] = tokens[3]
        #temp['id'] = tokens[4]
        #temp['channel'] = tokens[6]
        #temp['temperature'] = tokens[7]
        #temp['humidity'] = tokens[8]
        #if sensors.get(temp['id']) is None:
        #    print("New sensor %r found!" % temp['id'])
        #    sense = sensor.SensorDevice()
        #    sense.set('id', temp['id'])
        #    sense.set('alias', temp['model'])
        #    sense.set('device', sensor.DeviceType.RTL433)
        #    sense.set('sensor', sensor.SensorType.TEMPERATURE)
        #    sense.set('channel', temp['channel'])
        #    sensors.add(sense)
        #if (options.get('scale') == 'F'):
        #    temp['temperature'] = (float(temp['temperature']) * 1.8) + 32.0;
            #temp['lowtemp'] =  (float(temp['lowtemp']) * 1.8) + 32.0;
            #temp['hightemp'] =  (float(temp['hightemp']) * 1.8) + 32.0;
        #query = """INSERT INTO weather VALUES( '%s', %s, %s, %s, %s, '%s', '%s' )  ON CONFLICT DO NOTHING;; """ % (temp['id'], temp['temperature'], "0", "0",  temp['humidity'], options.get('scale'), temp['timestamp'])
        #logging.debug(query)
        #db.query(query)
        #time.sleep(options.get('interval'))
 
    # _sensors = list()


gpio_ts_handler()
