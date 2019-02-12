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

## \file mergedb.py
# Simple script to merge logged data from a remote database to
# local one.

import epdb
import logging
import time
import psycopg2
import getopt
import sys
from sys import argv

options = dict()
options['dbname'] = "powerguru"  # hostname of the database
options['source'] = "pi"  # hostname of the source database server
options['dest'] = "localhost"  # hostname of the destination server
options['starttime'] = ""  # hostname of the destination server
options['endtime'] = ""  # hostname of the destination server

# menu for --help
def usage(argv):
    print(argv[0] + ": options: ")
    print("""
    \t--help(-h)        Help
    \t--source(-s)      Source Database server [host]:port]], default '%s'
    \t--dest(-d)        Destination Database server [host]:port]], default '%s'
    \t--database(-n)    Database name, default for now '%s'
    \t--starttime(-t)   Use timestanps after this, default begining of data
    \t--endtime(-e)     Use timestanps after this, default end of data
    \t--verbose(-v)     Enable verbosity

    The starting and ending timestamps need to be in a normal timestamp
    format for now. ie... '2019-01-10 12:18:48'
    """ % (options['source'], options['dest'], options['dbname'])
    )
    quit()

# Check command line arguments
try:
    (opts, val) = getopt.getopt(argv[1:], "h,s:,d:,n:,t:,e:,v,",
           ["help", "source", "dest", "database", "starttime", "endtime", "verbose"])
except getopt.GetoptError as e:
    logging.error('%r' % e)
    usage(argv)
    quit()

# Setup a disk space log filemode. By default, everything
# gets logged to the disk file
logging.basicConfig(
    filename='mergedb.log',
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
    elif opt == "--source" or opt == '-s':
        options['source'] = val
    elif opt == "--dest" or opt == '-d':
        options['dest'] = val
    elif opt == "--starttime" or opt == '-t':
        options['starttime'] = val
    elif opt == "--endtime" or opt == '-e':
        options['endtime'] = val
    elif opt == "--verbose" or opt == '-v':
        if verbosity == logging.INFO:
            verbosity = logging.DEBUG
            formatter = logging.Formatter('{%(filename)s:%(lineno)d} %(levelname)s - %(message)s')
            ch.setFormatter(formatter)
        if verbosity == logging.CRITICAL:
            verbosity = logging.INFO

ch.setLevel(verbosity)

#
# Open the two data base connections, if not localhost
#

connect = ""
#if options['source'] != "localhost":
connect = "host='" + options['source'] + "'"
connect += " dbname='" + options['dbname'] + "'"

logging.debug("Source connect: %r" % connect)
source = psycopg2.connect(connect)
if source.closed == 0:
    source.autocommit = True
    logging.info("Opened connection using %r" % source.dsn)
    srccursor = source.cursor()
    #if srccursor.closed == 0:
    logging.debug("Opened src cursor using %r" % source.dsn)
    #else:
    #    logging.error("Couldn't open src cursor using %r" % source.dsn)
else:
    logging.error("Couldn't open connection using %r" % source.dsn)
    
connect = ""
if options['dest'] != "localhost":
    connect = "host='" + options['dest'] + "'"
connect += " dbname='" + options['dbname'] + "'"
logging.debug("Dest connect: %r" % connect)
dest = psycopg2.connect(connect)
if dest.closed == 0:
    dest.autocommit = True
    logging.info("Opened connection to using %r" % dest.dsn)
    # FIXME: for now this is limited to the powerguru database
    destcursor = dest.cursor()
    #if destcursor.closed == True:
    logging.debug("Opened dest cursor on %r" % options['dest'])
    #else:
    #    logging.error("Couldn't open dest cursor on %r" % options['dest'])
else:
    logging.error("Couldn't open connection using %r" % dest.dsn)

start = ""
if options['starttime'] != "":
    start = "WHERE timestamp>=%r" % options['starttime']
else:
    # Get the last entry
    query = "SELECT timestamp FROM weather ORDER BY timestamp DESC LIMIT 1;"
    logging.debug(query)
    destcursor.execute(query)
    last = destcursor.fetchone()
    if destcursor.rowcount != 0:
        start = "WHERE timestamp>%r" % last[0].strftime("%Y-%m-%d %H:%M:%S")
        logging.info("Starting from: %r" % last[0].strftime("%Y-%m-%d %H:%M:%S"))
    else:
        start = ""
if options['endtime'] != "" and options['starttime'] != "":
    end = " AND timestamp<=%r" % options['endtime']
if options['endtime'] != "" and options['starttime'] == "":
    end = " WHERE timestamp<=%r" % options['endtime']
else:
    end = ""

# FIXME: Add LIMIT if we need to transfer data by blocks
query = """SELECT * FROM weather %s %s ORDER BY timestamp;""" % (start, end)
logging.debug(query)
srccursor.execute(query)
temps = dict()
data = list()
# Store the returned data
logging.debug("Got %r records" % srccursor.rowcount)
for id,temperature,temphigh,templow,humidity,scale,timestamp in srccursor:
    temps['id'] = id
    temps['temperature'] = temperature
    temps['temphigh'] = temphigh
    temps['templow'] = templow
    temps['humidity'] = humidity
    temps['scale'] = scale
    temps['timestamp'] = timestamp
    #print("%r" % temps)
    data.append(temps)
    #INSERT INTO datas () VALUES ('$data','$notes','$sortname','$listname','$url')";

    query = """INSERT INTO weather VALUES (%r, '%r', '%r', '%r', %r, %r, %r) ON CONFLICT DO NOTHING;""" % (id, temperature, temphigh, templow, humidity, scale,
                             timestamp.strftime("%Y-%m-%d %H:%M:%S"))
    logging.debug("Dest query: %r" % query)
    logging.debug("Dest status: %r" % destcursor.statusmessage)
    destcursor.execute(query)
