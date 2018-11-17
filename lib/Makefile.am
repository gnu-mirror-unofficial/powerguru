# 
# Copyright (C) 2005, 2006, 2007, 2008,	2009, 2010, 2011
#	Free Software Foundation, Inc.
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
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

AUTOMAKE_OPTIONS = 

#DL_LIB = @DL_LIB@
#LIBLTDL = @LIBLTDL@
# __USLC__ makes MySQL++ use the std namespace
AM_CXXFLAGS = -g -D__USLC__

lib_LTLIBRARIES = libpguru.la

# The list of tty's to use by default
TTYS=@TTYS@

# These headers get installed
# include_HEADERS = log.h err.h proc.h serial.h xantrex.h outback.h

libpguru_la_CPPFLAGS =	-I$(top_srcdir)
libpguru_la_LIBDADD =

if BUILD_LIBXML
XML_FILES = xml.cc xml.h
libpguru_la_CPPFLAGS += $(LIBXML_CPPFLAGS)
libpguru_la_LIBDADD += $(LIBXML_LIBS)
endif
# libpguru_la_CPPFLAGS = $(LIBXML_CPPFLAGS)

if BUILD_SNMP
SNMP_FILES = snmp.cc snmp.h
libpguru_la_CPPFLAGS += -I$(top_srcdir)/snmp
libpguru_la_LIBDADD += $(SNMP_LIBS) ../snmp/libpgsnmp.la
endif

if BUILD_MARIADB
MARIADB_FILES = mariadb.cc
libpguru_la_LIBDADD += $(MARIADB_LIBS)
endif

if BUILD_POSTGRESQL
LIBPQ_FILES = postgresql.cc
libpguru_la_CPPFLAGS += ${LIBPQ_CPPFLAGS}
libpguru_la_LIBDADD +=  $(LIBPQ_LIBS)
endif

libpguru_la_SOURCES = \
	proc.cc proc.h \
	err.cc err.h \
	log.cc log.h \
	sem.cc sem.h \
	rc.cc rc.h \
	serial.cc serial.h \
	console.cc console.h \
	menuitem.cc menuitem.h \
	database.h \
	$(SNMP_FILES) \
	$(LIBPQ_FILES) \
	$(MARIADB_FILES) \
	$(XML_FILES) \
	tcputil.cc tcputil.h \
	tcpip.cc tcpip.h \
	msgs.cc msgs.h

include_HEADERS = \
	proc.h \
	serial.h \
	menuitem.h \
	database.h \
	console.h \
	err.h \
	log.h \
	tcputil.h \
	tcpip.h \
	xml.h

#libpguru_la_LDFLAGS = -module -avoid-version -no-undefined

clean-hook:
	-rm -f core.*

