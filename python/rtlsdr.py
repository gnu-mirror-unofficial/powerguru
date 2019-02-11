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

#import epdb
import logging
#import time
#import psycopg2
#from rtlsdr import RtlSdr


def rtlsdr_handler(args):
    logging.debug("Start rtl_sdr %r" % args)
    #self.sdr = RtlSdr(1, True,"00000001")

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
        

    _sensors = list()

