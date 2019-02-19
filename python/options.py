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
import os
from proc.core import Process


class CmdOptions(object):
    """This class handles command line options between threads in
    a thread-safe way by using the /proc filesystem read-only."""
    def __init__(self):
        """Read the command line option from /proc/PID/cmdline"""
        proc = Process.from_path('/proc/self')
        self.options = dict()
        # Setup default command line options
        self.options = dict()
        self.options['interval'] = 300  # time interval to delay when polling for data
        self.options['owserver'] = "localhost"  # hostname of the owserver
        self.options['dbserver'] = "localhost"  # hostname of the database
        self.options['scale'] = "F"             # The scale, 'C' or 'F'
        self.options['database'] = "powerguru"  # The database name
        self.options['verbosity'] = logging.ERROR
        self.processArgs(proc.cmdline[2:])

        # menu for --help
    def usage(self):
        """Display command line option usage"""
        #print(argv[0] + ": self.options: ")
        print("""\t--help(-h)   Help
        \t--owserver(-o)    OW server [host[:port]], default '%s'
        \t--dbserver(-p)    Database server [host]:port]], default '%s'
        \t--database(-d)    Database name, default '%s'
        \t--interval(-i)    Set the time interval for device polling, default '%s'
        \t--verbose(-v)     Enable verbosity
        \t--scale(-s)       Set the temperature scale, 'F' or 'C', default '%s'
        """ % (self.options['owserver'],
               self.options['dbserver'],
               self.options['database'],
               self.options['interval'],
               self.options['scale']))
        quit()

    # Check command line arguments
    def processArgs(self, args):
        """Process the list of command line arguments into
        internal data."""
        logging.debug("options.processArgs: %r" % args)

        #epdb.set_trace()
        #if len(args) == 0:
        #    self.usage()
        
        try:
            (opts, val) = getopt.getopt(args, "h,o:,i:,p:,d:,s:,v,",
                        ["help", "owserver", "dbserver", "database", "scale", "interval", "verbosity"])
        except getopt.GetoptError as e:
            logging.error('%r' % e)
            self.usage()

        # process command line arguments, will override the defaults
        i = 0
        while i < len(args):
            opt = args[i]
            try:
                val = args[i + 1]
            except:
                val = ""
            #print("%r: %r = %r" % (i, opt, val))
            if opt == '--help' or opt == '-h':
                self.usage()
            elif opt == "--owserver" or opt == '-o':
                self.options['owserver'] = val
            elif opt == "--interval" or opt == '-i':
                self.options['interval'] = int(val)
            elif opt == "--dbserver" or opt == '-p':
                self.options['dbserver'] = val
            elif opt == "--database" or opt == '-d':
                self.options['database'] = val
            elif opt == "--verbose" or opt == '-v':
                #print("VERBOSITY: %r %r" % (self.options['verbosity'], val))
                #epdb.set_trace()
                if self.options['verbosity'] is logging.ERROR:
                    self.options['verbosity'] = logging.INFO
                elif self.options['verbosity'] is logging.INFO:
                    self.options['verbosity'] = logging.DEBUG
                i = i + 1
                continue
            i = i + 2

    def set(self, key, value=""):
        """Set the value of a command line option."""
        self.options[key] = value

    def get(self, key):
        """Get the value of a command line option."""
        return self.options[key]

    def dump(self):
        """Display all the internal option data"""
        for key,value in self.options.items():
            if key == 'verbosity':
                if value == logging.INFO:
                    print("%r = logging.INFO" % (key.capitalize()))
                elif value == logging.CRITICAL:
                    print("%r = logging.CRITICAL" % (key.capitalize()))
                elif value == logging.ERROR:
                    print("%r = logging.ERROR" % (key.capitalize()))
                elif value == logging.DEBUG:
                    print("%r = logging.DEBUG" % (key.capitalize()))
            else:
                print("%r = %r" % (key.capitalize(), value))
