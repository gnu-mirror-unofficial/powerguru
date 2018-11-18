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

noinst_LTLIBRARIES = libpdev.la

# The list of tty's to use by default
TTYS=@TTYS@

# These headers get installed
# include_HEADERS = log.h err.h proc.h serial.h xantrex.h outback.h

libpdev_la_CPPFLAGS = -I$(top_srcdir)/lib -I..

if BUILD_SNMP
SNMP_FILES = snmp.cc snmp.h
libpdev_la_CPPFLAGS += -I$(top_srcdir)/snmp
libpdev_la_LIBDADD =  $(SNMP_LIBS) ../snmp/libpgsnmp.la
endif

libpdev_la_SOURCES = \
	xantrex-trace.cc xantrex-trace.h \
	xanbus.cc xanbus.h \
	outbackpower.cc outbackpower.h

include_HEADERS = \
	xantrex-trace.h \
	outbackpower.h \
	xanbus.h

#libpdev_la_LDFLAGS = -module -avoid-version -no-undefined

clean-hook:
	-rm -f core.*

