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
import getopt
import sys
import rtlsdr
import rtl433
import ownet
import onewire
from threading import Thread
from time import sleep
from sys import argv
import psycopg2
#from sensor import Sensor
import sensor
import platform
import i2c
import remote
import socketserver
import os
from options import CmdOptions


# Setup a disk space log filemode. By default, everything
# gets logged to the disk file
logging.basicConfig(
    filename = 'pgdpy.log',
    filemode = 'w',
    level = logging.DEBUG,
    format = '[%(asctime)s] {%(filename)s:%(lineno)d} %(levelname)s - %(message)s',
     datefmt = '%Y-%m-%d %H:%M:%S'
)

# Setup console logging, useful for debugging
# By default, print nothing to the console. There
# re two versosity levels, the first just informational
# messages, the second all debugging messages as well.
root = logging.getLogger()
ch = logging.StreamHandler(sys.stdout)
ch.setLevel(logging.CRITICAL)
formatter = logging.Formatter('%(message)s')
#formatter = logging.Formatter('{%(filename)s:%(lineno)d} - %(message)s')
ch.setFormatter(formatter)
root.addHandler(ch)
terminator = ch.terminator
verbosity = logging.CRITICAL
formatter = logging.Formatter('{%(filename)s:%(lineno)d} %(levelname)s - %(message)s')
ch.setFormatter(formatter)

#
# See if we're running on a Raspberry PI, since other platforms
# don't have GPIO pins.
#
try:
    file = open("/etc/issue", "r")
except Exception as inst:
    logging.error("Couldn't open /etc/issue: %r" % inst)
issue = file.readlines()
on = issue[0].split(' ')
if on[0] == "Raspbian":
    pi = True
else:
    pi = False
logging.info("Running on %r" % on[0])

# process command line arguments. This uses /proc, so we don't
# have to pass anything in.
opts = CmdOptions()
opts.dump()

ch.setLevel(opts.get('verbosity'))

#
# Collect the data about the connected sensors
#
sensors = sensor.Sensors()
sensors.dump()

#
# Start the I/O threads
#

# OWFS network protocol
ownet_thread = Thread(target=ownet.ownet_handler, args=(sensors,))
ownet_thread.start()

# OWFS filesystem
#onewire_thread = Thread(target = onewire.onewire_handler, args = (options, ))
#onewire_thread.start()

# rtl_433 filesystem
rtl433_thread = Thread(target = rtl433.rtl433_handler, args = (sensors,))
rtl433_thread.start()

# rtl_sdr
rtlsdr_thread = Thread(target = rtlsdr.rtlsdr_handler, args = (sensors,))
rtlsdr_thread.start()

# GPIO only works on a Raspberry PI
if pi is True:
    import gpio433
    gpio433_thread = Thread(target = gpio433.gpio433_handler, args = (sensors,))
    gpio433_thread.start()

    i2c_thread = Thread(target = i2c.ina219_handler, args = (sensors,))
    i2c_thread.start()

try:
    server = socketserver.TCPServer(("0.0.0.0", 7654), remote.client_handler)
    server.allow_reuse_address = True
    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        server.shutdown()
        server.socket.close()
except:
    logging.debug("Another pgd is running...")
    

if pi is True:
    gpio433_thread.join()
    print("gpio433_thread finished...exiting")

    i2c_thread.join()
    print("i2c_thread finished...exiting")

#
# Join the I/O threads as we're done.
#
ownet_thread.join()
print("ownet_thread finished...exiting")

#onewire_thread.join()
#print("onewire_thread finished...exiting")

rtl433_thread.join()
print("rtl433_thread finished...exiting")

rtlsdr_thread.join()
print("rtlsdr_thread finished...exiting")

