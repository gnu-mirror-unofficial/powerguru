#!/usr/bin/python3

# 
#   Copyright (C) 2017, 2018, 2019   Free Software Foundation, Inc.
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

import pdb
import glob
import psycopg2
import subprocess
import config
import logging

## \class pgdb
# A class to work with a postgresql database
class pgdb(object):
    """PostgreSQL database class."""
    def __init__(self, config):
        self.config = config
        self.dbname = ""
        self.result = ""
        # self.config.dump()

    def connect(self, dbname):
        '''Connect to a postgresql server'''
        logging.debug("plotcalls.connect(" + dbname + ")")
        self.config.set('dbname', dbname)
        connect = " dbname=" + dbname
        
        try:
            self.dbshell = psycopg2.connect(connect)
            if self.dbshell.closed == 0:
                self.dbshell.autocommit = True
                logging.info("Opened connection to %r %r" % (dbname, self.dbshell))
                
                self.dbcursor = self.dbshell.cursor()
                if self.dbcursor.closed == 0:
                    logging.info("Opened cursor in %r %r" % (dbname, self.dbcursor))

        except Exception as e:
            print("Couldn't connect to database: %r" % e)

    def query(self, query, nores=""):
        '''Query a postgresql database'''
        logging.debug("plotcalls.query(" + query + ")")
        try:
            self.dbcursor.execute(query)
            self.result = self.dbcursor.fetchall()
        except:
            self.result = list()
        #logging.debug("FIXME: query(%r)" % len(self.result))
        nores = self.result
        return self.result

    def shell(self, cmd):
        cmdline = list()
        cmdline.append("psql")
        if len(self.config.get('dbname')) > 0:
            cmdline.append("--dbname=" + str(self.config.get('dbname')))
        cmdline.append("--command=" + cmd)
        self.verbose.log(cmdline)
        try:
            subprocess.call(cmdline, stdout=subprocess.DEVNULL)
            out = True
        except:
            self.verbose.warning("Couldn't list function: %r" % func)
            out = False
            
        return out

    def dump(self):
        print("Dumping data from pgdb class")
        self.list_functions()
        if self.dbshell.closed == 0:
            status = "Open"
        else:
            status = "Closed"
        print("Connection: " + status)
        print("DSN: " + self.dbshell.dsn)
        print("AutoCommit: " + str(self.dbshell.autocommit))
        for i in self.dbshell.notices:
            print("History: " + i.replace('\n', ''))
