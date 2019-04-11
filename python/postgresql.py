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
## \file postgresql.py Wrapper for the psycopg2 module

import epdb
import glob
import psycopg2
import subprocess
import logging
from options import CmdOptions


class Postgresql(object):
    """A class to work with a postgresql database"""

    def rowcount(self):
        return self.dbcursor.rowcount

    def fetchResult(self):
        #return self.dbcursor.fetchall()
        return self.result

    def __init__(self):
        self.options = CmdOptions()
        self.connect()
        self.result = ""

    def connect(self):
        """Connect to a local or remote postgresql server"""

        # Supported parameters for connect are: 
        # *database*: the database name (only as keyword argument)
        # *user*: user name used to authenticate
        # *password*: password used to authenticate
        # *host*: database host address (defaults to UNIX socket if not provided)
        # *port*: connection port number (defaults to 5432 if not provided)
        dbserver = self.options.get('dbserver')
        database = self.options.get('database')
        connect = ""
        if dbserver is not "localhost":
            connect += "host='" + dbserver + "'"
        connect += " dbname='" + database + "'"

        logging.debug("postgresql.connect(%r)" % connect)
        self.dbshell = psycopg2.connect(connect)
        if self.dbshell.closed == 0:
            self.dbshell.autocommit = True
            logging.info("Opened connection to %r %r" % (database, self.dbshell))

            self.dbcursor = self.dbshell.cursor()
            if self.dbcursor.closed == 0:
                logging.info("Opened cursor in %r %r" % (database, self.dbcursor))

    def query(self, query=""):
        """Query a local or remote postgresql database"""

        logging.debug("postgresql.query(" + query + ")")
        #epdb.set_trace()
        if self.dbshell.closed != 0:
            logging.error("Database %r is not connected!" % self.options.get('database'))
            return self.result

        self.result = list()
        try:
            self.dbcursor.execute(query)
            self.result = self.dbcursor.fetchall()
        except psycopg2.ProgrammingError as e:
            if e.pgcode != None:
                logging.error("Query failed to fetch! %r" % e.pgcode)

        #epdb.set_trace()
        #logging.debug("Returned %r rows from query: %r" % (self.dbcursor.rowcount, self.dbcursor.statusmessage))
        #if self.dbcursor.rowcount is None:
        parsed = query.split(' ')
        table = ""
        if parsed[0] == "INSERT":
            table = parsed[2]
        if self.dbcursor.statusmessage == "INSERT 0 1":
            logging.debug("Inserted into %r" % (table))
            self.result = self.dbcursor.statusmessage
#        elif self.dbcursor.statusmessage == "INSERT 0 0":
#            logging.debug("Already exists in database %r" % (table))
        return self.result

    def isConnected(self):
        """Test to see if there is a working database connection"""
        if self.dbshell.closed == 0:
            return True
        else:
            return False

    def dump(self):
        """Display all internal data"""
        print("Dumping data from postgresql class")
        print("\tDB server: %r" % self.options.get('dbserver'))
        print("\tDatabase: %r" % self.options.get('database'))
        #self.list_functions()
        if self.dbshell.closed == 0:
            status = "Open"
        else:
            status = "Closed"
        print("\tConnection: " + status)
        print("\tDSN: " + self.dbshell.dsn)
        print("\tAutoCommit: " + str(self.dbshell.autocommit))
        for i in self.dbshell.notices:
            print("\tHistory: " + i.replace('\n', ''))
