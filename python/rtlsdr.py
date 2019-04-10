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

## \file rtlsdr.py Read data from an RTL-SDR USB dongle

#import epdb
import logging
#import time
#import psycopg2
#from rtlsdr import RtlSdr
from options import CmdOptions
from postgresql import Postgresql


def rtlsdr_handler(sensors):
    logging.debug("Start rtl_sdr...")
    #self.sdr = RtlSdr(1, True,"00000001")

    options = CmdOptions()
    #db = Postgresql()
    #db.dump()
