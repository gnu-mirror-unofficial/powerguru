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
from datetime import timedelta
import numpy as np
import getopt
import sys
from sys import argv

# http://initd.org/psycopg/docs/

options = dict()
options['dbserver'] = "pi"  # hostname of the database
options['dbname'] = "powerguru"  # hostname of the database
options['interval'] = 100        # interval in seconds between data updates
options['starttime'] = ""
options['endtime'] = ""

#import matplotlib
#matplotlib.use('agg')

# menu for --help
def usage(argv):
    print(argv[0] + ": options: ")
    print("""\t--help(-h)   Help
    \t--dbserver(-s)    Database server [host]:port]], default '%s'
    \t--database(-d)    Database on server, default '%s'
    \t--interval(-i)    Interval for data updates, default '%s'
    \t--starttime(-t)   Use timestanps after this, default begining of data
    \t--endtime(-e)     Use timestanps after this, default end of data
    \t--verbose(-v)     Enable verbosity
    """ %  (options['dbserver'], options['dbname'],  options['interval'])
    )
    quit()

# Check command line arguments
try:
    (opts, val) = getopt.getopt(argv[1:], "h,d:,s;v,i:,t:,e:",
           ["help", "database", "dbserver", "verbose", "interval", "starttime", "endtime"])
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
    elif opt == "--starttime" or opt == '-t':
        options['starttime'] = val
    elif opt == "--endtime" or opt == '-e':
        options['endtime'] = val
    elif opt == "--dbserver" or opt == '-s':
        options['dbserver'] = val
    elif opt == "--interval" or opt == '-i':
        options['interval'] = val
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

delta = 0
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

# Setup optional timestamp filter
start = ""
end = ""
if options['starttime'] != "":
    start = "AND timestamp>=%r" % options['starttime']
elif options['endtime'] != "" and options['starttime'] != "":
    end = " AND timestamp<=%r" % options['endtime']
    if options['endtime'] != "" and options['starttime'] == "":
        end = " AND timestamp<=%r" % options['endtime']

# Create the two subslots
fig, (temp, dcvolts, amps) = plt.subplots(3, 1, sharex=True)
plt.subplots_adjust(top=0.88, bottom=0.20, left=0.10, right=0.95, hspace=0.58,
                    wspace=0.35)

colors = list()
colors.append("red")
colors.append("green")
colors.append("blue")
colors.append("black")
def animate(i):
    logging.debug("Refreshing data...")
    ids = list()
    query = "SELECT DISTINCT id FROM weather"
    logging.debug(query)
    dbcursor.execute(query)
    logging.debug("Query returned %r records" % dbcursor.rowcount)
    for id in dbcursor:
        print("ID: %r" % id)
        ids.append(id)

    cur = 0
    for id in ids:
        query = "SELECT id,temperature,humidity,timestamp FROM weather WHERE (id='%s' %s %s) ORDER BY timestamp;" % (id[0], start, end)
        logging.debug(query)
        dbcursor.execute(query)
        logging.debug("Query returned %r records" % dbcursor.rowcount)
        x = list()
        y = list()
        for id,temperature,humidity,timestamp in dbcursor:
            #print("TEMP: %r, %r" % (temperature,timestamp))
            x.append(timestamp)
            y.append(temperature)

        fig.suptitle('PowerGuru')
        temp.set_ylabel("Temperature in F")
        temp.set_title("Temperature")
        temp.grid(which='major', color='red')
        temp.grid(which='minor', color='blue', linestyle='dashed')
        temp.minorticks_on()
        temp.plot(x, y, color=colors[cur])
        cur += 1
    
    xx = list()
    yy = list()
    zz = list()
    query = "SELECT DISTINCT id FROM power"
    logging.debug(query)
    dbcursor.execute(query)
    logging.debug("Query returned %r records" % dbcursor.rowcount)
    if  dbcursor.rowcount > 0:
        for id in dbcursor:
            ids.append(id)
            query = "SELECT id,current,volts,timestamp FROM power WHERE (id='%s' %s %s) ORDER BY timestamp " % (id[0], start, end)
            logging.debug(query)
            dbcursor.execute(query)
            logging.debug("Query returned %r records" % dbcursor.rowcount)
            cur = 0
            for id,current,voltage,timestamp in dbcursor:
                print("BATTERY: %r, %r, %r, %r" % (id, current, voltage, timestamp))
                xx.append(timestamp)
                yy.append(voltage)
                zz.append(current)

        dcvolts.set_title("DC Voltage")
        dcvolts.plot(xx, yy, color="purple")
        dcvolts.set_ylabel("DC Volts")
        #dcvolts.set_xlabel("Time (hourly)")
        dcvolts.grid(which='major', color='red')
        dcvolts.grid(which='minor', color='blue', linestyle='dashed')
        dcvolts.minorticks_on()
    
        amps.set_title("DC Current")
        amps.plot(xx, zz, color=colors[cur])
        cur += 1
        amps.set_ylabel("Amps")
        amps.set_xlabel("Time (hourly)")
        amps.grid(which='major', color='red')
        amps.grid(which='minor', color='blue', linestyle='dashed')
        amps.minorticks_on()
        plt.setp(amps.xaxis.get_majorticklabels(), rotation=90)
        amps.xaxis.set_major_formatter(mdates.DateFormatter('%m-%d %H'))
        amps.xaxis.set_major_locator(mdates.HourLocator(byhour=range(0,24,6)))
        amps.xaxis.set_minor_locator(mdates.HourLocator())

    # Get the time delta between data samples, as it's not worth updating there
    # the display till their in fresh data. Sample may be minutes or hours apart,
    # so o need to waste cpu cycles
    query = "SELECT AGE(%r::timestamp, %r::timestamp);" % (x[1].strftime("%Y-%m-%d %H:%M:%S"), x[0].strftime("%Y-%m-%d %H:%M:%S"))
    logging.debug(query)
    dbcursor.execute(query)
    delta = (dbcursor.fetchall())[0][0].total_seconds()
    logging.debug("Query returned %r" % delta)
    
# The timeout is in miliseconds
#seconds = 1000 * delta
seconds = 1000 * options['interval']
ani = animation.FuncAnimation(fig, animate, interval=seconds)
plt.show()
