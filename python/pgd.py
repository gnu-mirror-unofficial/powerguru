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

# Setup default command line options
options = dict()
options['interval'] = "300"  # time interval to delay when polling for data
options['owserver'] = "localhost"  # hostname of the owserver
options['dbserver'] = "localhost"  # hostname of the database
options['scale'] = "F"             # The scale, 'C' or 'F'

# menu for --help
def usage(argv):
    print(argv[0] + ": options: ")
    print("""\t--help(-h)   Help
    \t--owserver(-w)    OW server [host[:port]], default '%s'
    \t--dbserver(-d)    Database server [host]:port]], default '%s'
    \t--interval(-i)    Set the time interval for device polling, default '%s'
    \t--verbose(-v)     Enable verbosity
    \t--scale(-s)       Set the temperature scale, 'F' or 'C', default '%s'
    """ % (options['owserver'],
           options['dbserver'],
           options['interval'],
           options['scale']))
    quit()

# Check command line arguments
try:
    (opts, val) = getopt.getopt(argv[1:], "h,w:,d:,s:,i:v,",
           ["help", "owserver", "dbserver", "scale", "interval", "verbose"])
except getopt.GetoptError as e:
    logging.error('%r' % e)
    usage(argv)
    quit()

# Setup a disk space log filemode. By default, everything
# gets logged to the disk file
logging.basicConfig(
    filename='pgdpy.log',
    filemode='w',
    level=logging.DEBUG,
    format= '[%(asctime)s] {%(filename)s:%(lineno)d} %(levelname)s - %(message)s',
     datefmt='%Y-%m-%d %H:%M:%S'
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

# process command line arguments, will override the defaults
for (opt, val) in opts:
    if opt == '--help' or opt == '-h':
        usage(argv)
    elif opt == "--owserver" or opt == '-w':
        options['owserver'] = val
    elif opt == "--interval" or opt == '-i':
        options['interval'] = val
    elif opt == "--dbserver" or opt == '-d':
        options['dbserver'] = val
    elif opt == "--verbose" or opt == '-v':
        if verbosity == logging.INFO:
            verbosity = logging.DEBUG
            formatter = logging.Formatter('{%(filename)s:%(lineno)d} %(levelname)s - %(message)s')
            ch.setFormatter(formatter)
        if verbosity == logging.CRITICAL:
            verbosity = logging.INFO

ch.setLevel(verbosity)

sensors = sensor.Sensors()
sensors.dump()

#
# Start the I/O threads
#

# OWFS network protocol
ownet_thread = Thread(target=ownet.ownet_handler, args=(options, ))
ownet_thread.start()

# OWFS filesystem
onewire_thread = Thread(target = onewire.onewire_handler, args = (10, ))
onewire_thread.start()

# rtl_433 filesystem
rtl433_thread = Thread(target = rtl433.rtl433_handler, args = (options, sensors,))
rtl433_thread.start()

# rtl_sdr filesystem
rtlsdr_thread = Thread(target = rtlsdr.rtlsdr_handler, args = (options, ))
rtlsdr_thread.start()

#
# Join the I/O threads as we're done.
#
ownet_thread.join()
print("ownet_thread finished...exiting")

onewire_thread.join()
print("onewire_thread finished...exiting")

rtl433_thread.join()
print("rtl433_thread finished...exiting")

rtlsdr_thread.join()
print("rtlsdr_thread finished...exiting")

