# 
#   Copyright (C) 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
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

## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = dejagnu

# __USLC__ makes MySQL++ use the std namespace
AM_CXXFLAGS = -g -D__USLC__

AM_INCLUDES =  $(MYSQLH) # $(INCLTDL)

noinst_PROGRAMS = tcpip tutil childtcpip

INCLUDES = -I$(top_srcdir)/lib

LOG = ../../lib/log.lo ../../lib/err.lo
# Tcpip.C test case
tcpip_SOURCES = tcpip-test.cc
tcpip_LDADD =  ../../lib/tcpip.lo ../../lib/tcputil.lo $(LOG) # ../../lib/libpguru.la
tcpip_DEPENDENCIES = ../../lib/tcpip.lo ../../lib/tcputil.lo $(LOG) # ../../lib/libpguru.la

#
tutil_SOURCES = tcputil-test.cc
tutil_LDADD = ../../lib/tcputil.lo  $(LOG) # ../../lib/libpguru.la
tutil_DEPENDENCIES = ../../lib/tcputil.lo $(LOG) # ../../lib/libpguru.la

#
childtcpip_SOURCES = childtcpip.cc
childtcpip_LDADD = ../../lib/tcpip.lo ../../lib/tcputil.lo $(LOG) # ../../lib/libpguru.la
childtcpip_DEPENDENCIES = ../../lib/tcpip.lo ../../lib/tcputil.lo $(LOG) # ../../lib/libpguru.la
