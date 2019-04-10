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


## \file chart.py This file plots the data from a postgresql database.

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


## \class ChartOptions
class ChartOptions(CmdOptions):
    """A class to process command line options to this program"""
    
    def __init__(self):
        ## Initialize additional command line options
        """Initialize additional command line options"""
        super(ChartOptions, self).__init__()
        #self.options['starttime'] = None
        #self.options['endtime'] = None
        self.options['web'] = False

    def usage(self):
        """Append additional help messages to the base class's list"""
        print(argv[0] + ": options: ")
        help = """
        \t--starttime(-t)   Use timestanps after this, default begining of data
        \t--endtime(-e)     Use timestanps after this, default end of data
        \t--webpage(-w)     Enable HTML output for browser
        """
        super(ChartOptions, self).usage(help)
   
opts = ChartOptions()
opts.dump()

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

# Get the data on each sensor
sensors = sensor.Sensors()

db = Postgresql()

# Setup optional timestamp filter
## \var start 
start = ""

end = ""
if opts.get('starttime') != None:
    start = "AND timestamp>=%r" % opts.get('starttime')
else:
    start = ""
if opts.get('endtime') != None and opts.get('starttime') != "":
    end = " AND timestamp<=%r" % opts.get('endtime')
    if opts.get('endtime') != None and opts.get('starttime') == "":
        end = " AND timestamp<=%r" % opts.get('endtime')
else:
    end = ""

# Based on https://matplotlib.org/gallery/color/named_colors.html
## \var colors
##      Array to hold a indexed list of colors so each line
##      can be a different color.
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
fig, ax = plt.subplots()
#plotline = dict()
cur = 0
#for id in sensors.getIDs(sensor.SensorType.TEMPERATURE):
#    plotline[id] = ax.plot([], [], lw=2, color=colors[cur])[0]
#    cur += 1

plotline, = ax.plot([], [], lw=2, color=colors[cur])

#fig, (temp) = plt.subplots(1, 1, sharex=True)
#temp = plt.subplot(111)
#fig, (temp, humidity) = plt.subplots(2, 1, sharex=True)
#fig, (temp, dcvolts, amps) = plt.subplots(3, 1, sharex=True)
#plt.subplots_adjust(top=0.88, bottom=0.20, left=0.10, right=0.95, hspace=0.58,wspace=0.35)

fig.suptitle('PowerGuru')
fig.set_figwidth(18)

# temp = plt.subplot()
ax.yaxis.tick_right()
ax.yaxis.set_label_position("right")
ax.set_ylabel("Temperature in F")
ax.set_title("Temperature")
ax.grid(which='major', color='red')
ax.grid(which='minor', color='blue', linestyle='dashed')
ax.minorticks_on()
ax.xaxis.set_major_formatter(mdates.DateFormatter('%m-%d %H'))
ax.xaxis.set_major_locator(mdates.HourLocator(byhour=range(0,24,6)))
ax.xaxis.set_minor_locator(mdates.HourLocator())
id= "6202"
ids = list()
labels = dict()
for id in sensors.getIDs(sensor.SensorType.TEMPERATURE, ids):
    sense = sensors.get(id)
    if sense is not None:
        location = sense.get('location')
    else:
        location = "Unknown"
    labels[id] = location

## \class plotData
class plotData(object):
    """Base class to hold data for a line"""

    def __init__(self, id=0, color=3):
        logging.debug("TRACE: plotData")
        self.x = []
        self.y = []
        self.color = color
        self.id = id
        self.update()

    def setClor(self, col):
        self.color = colors[col]
        
    def getColor(self):
        return self.color
        
    def append(self, x, y):
        self.x.append(x)
        self.y.append(y)

    def getData(self):
        yield self.x, self.y


class plotPower(plotData):
    """Class to hold data for a Power line"""

    def __init__(self, id=0, color=3):
        logging.debug("FIXME: plotTemp")
        plotData.__init__(self, id, color)

    def update(self):
        logging.debug("TRACE: plotPower.update()")
        query = "SELECT id,current,volts,timestamp FROM power WHERE (id='%s' %s %s) ORDER BY timestamp " % (self.id, start, end)
        logging.debug(query)
        db.query(query)
        logging.debug("Query returned %r records" % db.rowcount())
        cur = 0
        for id,current,voltage,timestamp in dbcursor:
            print("BATTERY: %r, %r, %r, %r" % (id, current, voltage, timestamp))
            xx.append(timestamp)
            yy.append(voltage)
            zz.append(current)

class plotTemp(plotData):
    """Class to hold data for a Temperature line"""

    def __init__(self, id=0, color=3):
        logging.debug("TRACE: plotTemp")
        plotData.__init__(self, id, color)
        self.update()

    def update(self, frame=""):
        logging.debug("TRACE: plotTemp.update()")
        query = "SELECT id,temperature,humidity,timestamp FROM weather WHERE (id='%s' %s %s) ORDER BY timestamp;" % (self.id, start, end)
        logging.debug(query)
        db.query(query)
        logging.debug("Query returned %r records" % db.rowcount())

        for model,temperature,humidity,timestamp in db.fetchResult():
            #print("TEMP: %r, %r" % (temperature, timestamp))
            self.x.append(timestamp)
            self.y.append(temperature)
        plotline.set_data(self.x, self.y)
        return plotline,


seconds = 1000 * opts.get('interval')
plottemp = plotTemp("6202")
ani = animation.FuncAnimation(fig, plottemp.update, interval=seconds, blit=True)

#for id in sensors.getIDs(sensor.SensorType.TEMPERATURE):
#    plotdata[id] = plotTemp(id)

#ax.legend(loc='upper left', shadow=True)
handles, labels = ax.get_legend_handles_labels()
ax.legend(handles, labels, loc='upper left')

#plottemp2 = plotTemp("235")
#ani = animation.FuncAnimation(fig, plottemp2.update, interval=seconds, blit=True)
if opts.get('web') is True:
    mpld3.show(port=9999, open_browser=False)
else:
    plt.show()
