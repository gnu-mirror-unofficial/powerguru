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


import epdb
import logging
import getopt
import sys
import ownet
import onewire
from threading import Thread
from time import sleep
from sys import argv

# Get the path to this script
#import os
#import sys
#sys.path.append(os.path.dirname(argv[0]))


def usage(argv):
    print(argv[0] + ": options: ")
    print("""\t--help(-h)   Help
\t--owserver(-w)        OW server
\t--dbserver(-d)        Database server
\t--verbose(-v)         Enable verbosity
        """)
    quit()

try:
    (opts, val) = getopt.getopt(argv[1:], "h,w:,d:,v,",
           ["help", "owserver", "dbserver", "verbose"])
except getopt.GetoptError as e:
    logging.error('%r' % e)
    self.usage(argv)
    quit()

# Store command line options
options = dict()
options['owserver'] = val
options['dbserver'] = val
logging.basicConfig(
    filename='pgdpy.log',
    filemode='w',
    level=logging.DEBUG,
    format= '[%(asctime)s] {%(filename)s:%(lineno)d} %(levelname)s - %(message)s',
     datefmt='%Y-%m-%d %H:%M:%S'
)
# By default, print nothing to the console
root = logging.getLogger()
ch = logging.StreamHandler(sys.stdout)
formatter = logging.Formatter('%(message)s')
#formatter = logging.Formatter('{%(filename)s:%(lineno)d} - %(message)s')
ch.setLevel(logging.CRITICAL)
ch.setFormatter(formatter)
root.addHandler(ch)
verbosity = logging.CRITICAL
for (opt, val) in opts:
    if opt == '--help' or opt == '-h':
        usage(argv)
    elif opt == "--owserver" or opt == '-w':
        options['owserver'] = val
    elif opt == "--dbserver" or opt == '-d':
        options['dbserver'] = val
    elif opt == "--verbose" or opt == '-v':
        if verbosity == logging.INFO:
            verbosity = logging.DEBUG
        if verbosity == logging.CRITICAL:
            verbosity = logging.INFO

ch.setLevel(verbosity)
#
# Start the I/O threads
#
ownet_thread = Thread(target = ownet.ownet_handler, args = (options['owserver'], ))
ownet_thread.start()
onewire_thread = Thread(target = onewire.onewire_handler, args = (10, ))
onewire_thread.start()

#
# Join the I/O threads as we're done.
#
ownet_thread.join()
print("ownet_thread finished...exiting")

onewire_thread.join()
print("onewire_thread finished...exiting")

