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

## \file i2c.py Read data from an i2c sensor,. Currently only the
##              ina219 current sensor is support.


import epdb
import logging
import psycopg2
from ina219 import INA219, DeviceRangeError
import time
from options import CmdOptions
from postgresql import Postgresql


def ina219_handler(sensors):
    logging.debug("Start ina219...")

    options = CmdOptions()
    db = Postgresql()
    #db.dump()

    SHUNT_OHMS = 0.1
    MAX_EXPECTED_AMPS = 2.0
    ina = INA219(SHUNT_OHMS, MAX_EXPECTED_AMPS)
    ina.configure(ina.RANGE_16V)

    while True:
        try:
            print('Bus Voltage: {0:0.2f}V'.format(ina.voltage()))
            print('Bus Current: {0:0.2f}mA'.format(ina.current()))
            print('Power: {0:0.2f}mW'.format(ina.power()))
            print('Shunt Voltage: {0:0.2f}mV\n'.format(ina.shunt_voltage()))

            id = "XXXXX"
            query = """INSERT INTO power VALUES( '%s', %s, %s, 'DC', '%s' )  ON CONFLICT DO NOTHING;; """ % (id, ina.current(), ina.voltage(), time.strftime("%Y-%m-%d %H:%M:%S"))
        except DeviceRangeError as e:
            # Current out of device range with specified shunt resister
            print(e)
        db.query(query)
        time.sleep(options.get('interval'))

