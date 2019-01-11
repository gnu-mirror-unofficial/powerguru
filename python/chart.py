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
import psycopg2
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib.figure import Figure
from matplotlib.lines import Line2D
from matplotlib.dates import DateFormatter
import matplotlib.animation as animation
from datetime import datetime
import numpy as np
import getopt
import sys
from sys import argv

# http://initd.org/psycopg/docs/

options = dict()
options['dbserver'] = "pi"  # hostname of the database
options['dbname'] = "powerguru"  # hostname of the database

#import matplotlib
#matplotlib.use('agg')

# menu for --help
def usage(argv):
    print(argv[0] + ": options: ")
    print("""\t--help(-h)   Help
    \t--dbserver(-s)    Database server [host]:port]], default '%s'
    \t--database(-d)    Database on server, default '%s'
    \t--verbose(-v)     Enable verbosity
    """ %  (options['dbserver'], options['dbname'])
    )
    quit()

# Check command line arguments
try:
    (opts, val) = getopt.getopt(argv[1:], "h,d:,s;v,",
           ["help", "database", "dbserver", "verbose"])
except getopt.GetoptError as e:
    logging.error('%r' % e)
    usage(argv)
    quit()

# Setup a disk space log filemode. By default, everything
# gets logged to the disk file
logging.basicConfig(
    filename='chart.log',
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
    elif opt == "--dbserver" or opt == '-s':
        options['dbserver'] = val
    elif opt == "--database" or opt == '-d':
        options['dbname'] = val
    elif opt == "--verbose" or opt == '-v':
        if verbosity == logging.INFO:
            verbosity = logging.DEBUG
            formatter = logging.Formatter('{%(filename)s:%(lineno)d} %(levelname)s - %(message)s')
            ch.setFormatter(formatter)
        if verbosity == logging.CRITICAL:
            verbosity = logging.INFO

ch.setLevel(verbosity)

dbname = ""
connect = ""
if options['dbserver'] != "localhost":
    connect = "host='" + options['dbserver'] + "'"
connect += " dbname='" + options['dbname'] + "'"

logging.debug(connect)
dbshell = psycopg2.connect(connect)
if dbshell.closed != 0:
    logging.error("Couldn't connect with %r" % connect)
    quit();

dbshell.autocommit = True
logging.info("Opened connection to %r" % options['dbserver'])

dbcursor = dbshell.cursor()
if dbcursor.closed != 0:
    logging.error("Couldn't get a cursor from %r" % options['dbname'])
    quit();

logging.info("Opened cursor in %r" % options['dbserver'])

fig, (temp, power) = plt.subplots(2, 1, sharex=True)

def animate(i):
    logging.debug("Refreshing data...")
    x = list()
    y = list()
    xx = list()
    yy = list()
    query = "SELECT id,temperature,timestamp FROM temperature ORDER BY timestamp"
    logging.debug(query)
    dbcursor.execute(query)
    logging.debug("Query returned %r records" % dbcursor.rowcount)
    for id,temperature,timestamp in dbcursor:
        #print("%r, %r" % (temperature,timestamp))
        x.append(timestamp)
        y.append(temperature)

    query = "SELECT id,current,volts,timestamp FROM battery ORDER BY timestamp"
    logging.debug(query)
    dbcursor.execute(query)
    logging.debug("Query returned %r records" % dbcursor.rowcount)
    for id,current,volts,timestamp in dbcursor:
        #print("BATTERY: %r, %r, %r, %r" % id, current, volts, timestamp)
        xx.append(timestamp)
        yy.append(volts)

    #
    # There will be more plots in the future
    #
    fig.suptitle('PowerGuru')
    temp.set_ylabel("Temperature in F")
    temp.set_title("Temperature")
    temp.grid(which='major', color='red')
    temp.grid(which='minor', color='blue', linestyle='dashed')
    temp.minorticks_on()
    temp.plot(x, y, color="green")
    
    power.set_title("Battery")
    power.plot(xx, yy, color="purple")
    power.set_ylabel("DC Volts")
    power.set_xlabel("Time (hourly)")
    power.grid(which='major', color='red')
    power.grid(which='minor', color='blue', linestyle='dashed')
    power.minorticks_on()
    plt.setp(power.xaxis.get_majorticklabels(), rotation=90)
    power.xaxis.set_major_formatter(mdates.DateFormatter('%m-%d %H'))
    power.xaxis.set_major_locator(mdates.HourLocator(byhour=range(0,24,6)))
    power.xaxis.set_minor_locator(mdates.HourLocator())

# The timeout is in miliseconds
seconds = 1000 * 100
ani = animation.FuncAnimation(fig, animate, interval=seconds)
plt.show()
