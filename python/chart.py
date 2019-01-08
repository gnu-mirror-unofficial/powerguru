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

import epdb
import logging
import time
import psycopg2
import matplotlib.pyplot as plt
import numpy as np

dbname = "";
connect = " dbname=" + dbname

try:
        dbname = "powerguru"
        connect = "dbname=" + dbname
        dbshell = psycopg2.connect(connect)
        if dbshell.closed == 0:
            dbshell.autocommit = True
            logging.info("Opened connection to %r" % dbname)
            
            dbcursor = dbshell.cursor()
            if dbcursor.closed == 0:
                logging.info("Opened cursor in %r" % dbname)
except Exception as e:
    print("Couldn't connect to database: %r" % e)

fig = plt.figure()

x = list()
y = list()
query = "SELECT temperature,timestamp  FROM temperature ORDER BY timestamp"
dbcursor.execute(query)
for temperature,timestamp in dbcursor:
    # print("%r, %r" % (temperature,timestamp))
    x.append(timestamp)
    y.append(temperature)

plt.plot(x, y, label="foo")
plt.xlabel("Timestamps")
plt.xticks(rotation='vertical')

#plt.yticks(np.arange(1, step=0.5))
plt.ylabel("Temperature in F")
#plt.xticks(np.arange(min(y), max(y)+1, 1.0))

plt.title("Test")
plt.show()
