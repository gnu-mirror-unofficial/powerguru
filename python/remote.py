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

## \file remote.py Handler for remote commands to the pgd daemon.

import epdb
import logging
import socketserver
from lxml import etree
from lxml.etree import tostring
import sensor
from enum import Enum
import triggers
import psycopg2


class client_handler(socketserver.BaseRequestHandler):
    """Create Server for client requests"""

    def setup(self, options=dict()):
        print("SETUP"),

    def server_activate(self):
        print("ACTIVATE"),


    def handle(self):
        while True:
            # self.request is the TCP socket connected to the client
            self.data = self.request.recv(1024).strip()
            self.clientip = self.client_address[0]
            logging.info("{} wrote:".format(self.clientip))
            # logging.debug(self.data)
            # If no data, the client dropped the network connection.
            # Return and this handler will ge restarted for the next
            # incoming connection.
            if len(self.data) == 0:
                return
            
            # Data is returned as unicode bytes, so must be decoded
            # to do string compares.
            if self.data.decode('utf8')[0] != '<':
                logging.warning("Not XML data!")
                continue

            xml = etree.XML(self.data)
            logging.debug("Got XML: %r" % etree.tostring(xml))

            tag = xml.tag.upper()
            # These are the allowable top level XML tags
            if tag == "LIST":
                logging.debug("LIST: %r" % xml.text)
                # if no children, default to querying all sensors
                if len(xml) == 0:
                    self.list_command(xml.text)
                elif xml[0].tag.upper() == "SENSORS":
                        self.list_command(xml[0].text)
                # Don't do anything, just make  network connection
                # is working. Mostly use for development
            elif tag == "NOP":
                logging.debug("NOP: %r" % xml.text)
            elif tag == "HELO":
                logging.debug("HELO: %r" % xml.text)
            elif tag == "POLL":
                logging.debug("POLL: %r" % xml.text)
            elif tag == "TRIGGER":
                logging.debug("TRIGGER: %r" % xml.text)
                if len(xml) > 0:
                    trigger = triggers.Triggers()
                    trigger.fromXML(xml)
                    trigger.dump()
                    trigger.writeSQL()
                    #self.event_command(xml.text)
            else:
                logging.error("Tag %r not supported!" % tag)
                return

            if len(xml) > 0:
                logging.debug("Has chidren")
                for child in xml:
                    logging.debug("Child tag: %r: %r" % (child.tag, child.text))
                    if child.tag == "StopIteration":
                        logging.debug("All Done!")
                    if len(child) > 0:
                        logging.debug("Has chidren")
                        for subchild in child:
                            logging.debug("SubChild tag: %r: %r" % (subchild.tag, subchild.text))
                            if subchild.tag == "StopIteration":
                                logging.debug("All Done!")

    def list_command(self, text=""):
        """List all the sensors attached to this daemon"""
        logging.debug("LIST command: %r" % text)
        if text is None or text.upper() == "ALL":
            xmldata = sensor.Sensors().makeXML()
            #logging.debug("XMLDATA: %r" % xmldata)
        else:
            if sensor.Sensors().get(text) != None:
                xmldata = sensor.Sensors().get(text).makeXML()
            else:
                logging.error("Bad sensor ID %r!" % text)
        self.request.sendall(xmldata.encode('utf-8'))

    def event_command(self, event=""):
        """ Handle events. Events are actions that we want to have
        a timestamp for, so when looking at weather or power data,
        we can see triggers for changes in the data."""
        logging.debug("EVENT command")

