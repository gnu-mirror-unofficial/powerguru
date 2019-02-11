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

import sys
from subprocess import PIPE, Popen, STDOUT
from threading  import Thread
import epdb
import logging
import time
import os
import psycopg2
ON_POSIX = 'posix' in sys.builtin_module_names

def rtl433_handler(args):
    logging.debug("Start rtl_433 %r" % args)
    cmd = [ 'rtl_433', '-F', 'csv', '-R', '40', '-T', '15']

    #ppp = Popen(cmd, stdout=PIPE, stderr=STDOUT, bufsize=1, close_fds=ON_POSIX)
    retries = 10
    while retries > 0:
        ppp = Popen(cmd, stdout=PIPE, bufsize=0, close_fds=ON_POSIX)
        try:
            out, err = ppp.communicate()
            #out, err = ppp.communicate(timeout=0.2)
        except subprocess.TimeoutExpired:
            logging.warning('subprocess did not terminate in time')
        #print("FIXME0: %r" % retries)
        #epdb.set_trace()
        for line in out.splitlines():
            #for line in ppp.readline():
            print("FIXME: %r" % line)
            str = line.decode('utf8')
            tokens = str.split(',')
            # this is just the csv header fields
            if tokens[0] == 'time':
                continue
            sensors = dict()
            sensors['model'] = tokens[3]
            sensors['id'] = tokens[5]
            sensors['channel'] = tokens[6]
            sensors['temperature'] = tokens[7]
            sensors['humidity'] = tokens[8]
            # Dump data
            print("MODEL: %r" % sensors['model'])
            print("\tID: %r" % sensors['id'])
            print("\tCHANNEL: %r" % sensors['channel'])
            print("\tTEMPERATURE: %rC" % sensors['temperature'])
            print("\tHUMIDITY: %r" % sensors['humidity'])
            print("")
            time.sleep(1)
        retries -= 1    
        epdb.set_trace()
 
    # # Connect to a postgresql database
    # try:
    #     dbname = "powerguru"
    #     connect = "dbname=" + dbname
    #     dbshell = psycopg2.connect(connect)
    #     if dbshell.closed == 0:
    #         dbshell.autocommit = True
    #         logging.info("Opened connection to %r" % dbname)
            
    #         dbcursor = dbshell.cursor()
    #         if dbcursor.closed == 0:
    #             logging.info("Opened cursor in %r" % dbname)
                
    # except Exception as e:
    #     logging.warning("Couldn't connect to database: %r" % e)
        

    # _sensors = list()

