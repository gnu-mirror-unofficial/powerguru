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
## \file gpiots.py Wrapper to read wireless 433Mhz data when using circuit
##                 board dedicated 433Mhz transceiver instead of an RTL-SDR
##                 USB radio dongle.
 
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

#def gpio_ts_handler(sensors):
def gpiots_handler(sensors):
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

    As I had problems getting output from STDOUT, it was easier to just
    have the data get logged to a tect file, and parse that for each
    new entry.

    """
    
    logging.debug("Start f007th-rpi_send...")

    options = CmdOptions()
    db = Postgresql()

    cmd = [ 'f007th-send', '-v', '-l', '/tmp/gpiots.log', '-g', '27']
    #cmd = [ 'f007th-send', '-v', '-V', '-g', '27']
    #ppp = Popen(cmd, stdout=PIPE, stderr=STDOUT, bufsize=0, close_fds=ON_POSIX)
    ppp = Popen(cmd, stdout=PIPE, bufsize=0, close_fds=ON_POSIX)
    foo = open('/tmp/gpiots.log', 'r')
    while True:
        out = sensor.follow(foo)
        temp = dict()
        for line in out:
            if line.find('=') < 0:
                stamp = line.split(' ')
                temp['timestamp'] = stamp[0] + ' ' + stamp[1]
                continue
            tokens = line.replace(' ', '').rstrip().split('=')
            print("FIXME1: %r = %r" % (tokens[0], tokens[1]))
            if tokens[0] == "temperature":
                scale = tokens[1][len(tokens[1])-1]
                temp['scale'] = scale
                temp['temperature'] = tokens[1].rstrip(scale)
                continue
            elif tokens[0] == "humidity":
                temp['humidity'] = tokens[1].rstrip('%')
                continue
            elif tokens[0] == "rollingcode":
                temp['id'] = tokens[1]
                continue
            elif tokens[0] == "battery":            
                query = """INSERT INTO weather VALUES( '%s', %s, %s, %s, %s, '%s', '%s' )  ON CONFLICT DO NOTHING;; """ % (temp['id'], temp['temperature'], "0", "0",  temp['humidity'], options.get('scale'), temp['timestamp'])
                logging.debug(query)
                db.query(query)
        time.sleep(options.get('interval'))
 
