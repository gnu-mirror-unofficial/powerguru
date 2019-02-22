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
import sensor
import mpld3
from mpld3 import plugins
from options import CmdOptions
from postgresql import Postgresql

# Setup a disk space log filemode. By default, everything
# gets logged to the disk file
logging.basicConfig(
    filename='chart.log',
    filemode='w',
    level=logging.DEBUG,
    format= '[%(asctime)s] {%(filename)s:%(lineno)d} %(levelname)s - %(message)s',
     datefmt='%Y-%m-%d %H:%M:%S'
)

#
# Create local options class
#
class ChartOptions(CmdOptions):
    """Command line options for this program"""

    def __init__(self):
        """Initialize additional command line options"""
        super(ChartOptions, self).__init__()
        self.options['starttime'] = ""
        self.options['endtime'] = ""
        self.options['web'] = False

    def usage(self):
        """Append additional help messages"""
        print(argv[0] + ": options: ")
        help = """
        \t--starttime(-t)   Use timestanps after this, default begining of data
        \t--endtime(-e)     Use timestanps after this, default end of data
        \t--webpage(-w)     Enable HTML output for browser
        """
        super(ChartOptions, self).usage(help)

opts = ChartOptions()

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
ch.setLevel(opts.get('verbosity'))

#opts.dump()
#epdb.set_trace()

# delta = 0
# dbname = ""
# connect = ""
# if options['dbserver'] is "localhost":
#     connect += " dbname='" + options['dbname'] + "'"
# else:
#     connect += "host='" + options['dbserver'] + "'"
#     connect += " dbname='" + options['dbname'] + "'"

# logging.debug(connect)
# dbshell = psycopg2.connect(connect)
# if dbshell.closed != 0:
#     logging.error("Couldn't connect with %r" % connect)
#     quit();

# dbshell.autocommit = True
# logging.info("Opened connection to %r" % options['dbserver'])

# dbcursor = dbshell.cursor()
# if dbcursor.closed != 0:
#     logging.error("Couldn't get a cursor from %r" % options['dbname'])
#     quit();

# logging.info("Opened cursor in %r" % options['dbserver'])

# Get the data on each sensor
sensors = sensor.Sensors()

db = Postgresql()

# Setup optional timestamp filter
start = ""
end = ""
if opts.get('starttime') != "":
    start = "AND timestamp>=%r" % opts.get('starttime')
elif opts.get('endtime') != "" and opts.get('starttime') != "":
    end = " AND timestamp<=%r" % opts.get('endtime')
    if opts.get('endtime') != "" and opts.get('starttime') == "":
        end = " AND timestamp<=%r" % opts.get('endtime')

# https://matplotlib.org/gallery/color/named_colors.html
colors = list()
colors.append("red")
colors.append("green")
colors.append("blue")
colors.append("orange")
colors.append("cyan")
colors.append("yellow")
colors.append("black")
colors.append("violet")
colors.append("silver")
colors.append("purple")
colors.append("grey")
colors.append("navy")

# Create the subslots
fig, (temp) = plt.subplots(1, 1, sharex=True)
#fig, (temp, humidity) = plt.subplots(2, 1, sharex=True)
#fig, (temp, dcvolts, amps) = plt.subplots(3, 1, sharex=True)
#plt.subplots_adjust(top=0.88, bottom=0.20, left=0.10, right=0.95, hspace=0.58,wspace=0.35)

#fig, (temp) = plt.subplots(1, 1, sharex=True)
fig.suptitle('PowerGuru')
fig.set_figwidth(18)

class plotTemps(object):
    """Class to create a plot of temperatures"""

    def __init__(self):
        cur = 0
        temp = plt.subplot()
        for id in sensors.getIDs(sensor.SensorType.TEMPERATURE):
            x = list()
            y = list()
            query = "SELECT id,temperature,humidity,timestamp FROM weather WHERE (id='%s' %s %s) ORDER BY timestamp;" % (id, start, end)
            logging.debug(query)
            db.query(query)
            logging.debug("Query returned %r records" % db.rowcount())

            for model,temperature,humidity,timestamp in db.fetchResult():
                #print("TEMP: %r, %r" % (temperature,timestamp))
                x.append(timestamp)
                y.append(temperature)

            #sensors.dump()
            temp.set_ylabel("Temperature in F")
            temp.set_title("Temperature")
            temp.grid(which='major', color='red')
            temp.grid(which='minor', color='blue', linestyle='dashed')
            temp.minorticks_on()
            legend = temp.legend(loc='upper left', shadow=True)
            sense = sensors.get(id)
            if sense != None:
                location = sense.get('location')
            else:
                location = id
            temp.plot(x, y, color=colors[cur], label=location)
            cur += 1


def animate(i):
    logging.debug("Refreshing data...")
    tplot = plotTemps()
    # xx = list()
    # yy = list()
    # zz = list()
    # query = "SELECT DISTINCT id FROM power"
    # logging.debug(query)
    # dbcursor.execute(query)
    # logging.debug("Query returned %r records" % dbcursor.rowcount)
    # if  dbcursor.rowcount > 0:
    #     for id in dbcursor:
    #         ids.append(id)
    #         query = "SELECT id,current,volts,timestamp FROM power WHERE (id='%s' %s %s) ORDER BY timestamp " % (id[0], start, end)
    #         logging.debug(query)
    #         dbcursor.execute(query)
    #         logging.debug("Query returned %r records" % dbcursor.rowcount)
    #         cur = 0
    #         for id,current,voltage,timestamp in dbcursor:
    #             #print("BATTERY: %r, %r, %r, %r" % (id, current, voltage, timestamp))
    #             xx.append(timestamp)
    #             yy.append(voltage)
    #             zz.append(current)

    #     dcvolts.set_title("DC Voltage")
    #     dcvolts.plot(xx, yy, color="purple")
    #     dcvolts.legend([id])
    #     dcvolts.set_ylabel("DC Volts")
    #     #dcvolts.set_xlabel("Time (hourly)")
    #     dcvolts.grid(which='major', color='red')
    #     dcvolts.grid(which='minor', color='blue', linestyle='dashed')
    #     dcvolts.minorticks_on()
    
    #     amps.set_title("DC Current")
    #     amps.plot(xx, zz, color=colors[cur])
    #     amps.legend([id])
    #     cur += 1
    #     amps.set_ylabel("Amps")
    #     amps.set_xlabel("Time (hourly)")
    #     amps.grid(which='major', color='red')
    #     amps.grid(which='minor', color='blue', linestyle='dashed')
    #     amps.minorticks_on()
    #     plt.setp(amps.xaxis.get_majorticklabels(), rotation=90)
    #     amps.xaxis.set_major_formatter(mdates.DateFormatter('%m-%d %H'))
    #     amps.xaxis.set_major_locator(mdates.HourLocator(byhour=range(0,24,6)))
    #     amps.xaxis.set_minor_locator(mdates.HourLocator())

    # Get the time delta between data samples, as it's not worth updating there
    # the display till their in fresh data. Sample may be minutes or hours apart,
    # so o need to waste cpu cycles
    #query = "SELECT AGE(%r::timestamp, %r::timestamp);" % (x[1].strftime("%Y-%m-%d %H:%M:%S"), x[0].strftime("%Y-%m-%d %H:%M:%S"))
    #logging.debug(query)
    #db.query(query)
    #delta = (db.fetchResult())[0][0].total_seconds()
    #logging.debug("Query returned %r" % delta)

# The timeout is in miliseconds
#seconds = 1000 * delta
seconds = 1000 * opts.get('interval')
ani = animation.FuncAnimation(fig, animate, interval=seconds)
if opts.get('web') is True:
    mpld3.show(port=9999, open_browser=False)
else:
    plt.show()
