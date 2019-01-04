# 
#   Copyright (C) 2005, 2006-2018
#   Free Software Foundation, Inc.
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

AUTOMAKE_OPTIONS = dejagnu subdir-objects

AM_CPPFLAGS = -I$(top_srcdir) \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/devices \
	$(LIBXML_CPPFLAGS) \
	$(PQ_CPPFLAGS) \
	-DSRCDIR=\"$(srcdir)\"

check_PROGRAMS =  tcpip tutil childtcpip db
TESTS = $(check_PROGRAMS)

if BUILD_LIBXML
check_PROGRAMS +=  xml-test cmd-test
endif

LOG = ../../lib/log.lo

if BUILD_LIBXML
cmd_test_SOURCES = cmd-test.cc ../../lib/commands.cc ../../lib/xml.cc
cmd_test_LDADD = $(LIBXML_LIBS) $(LOG)
xml_test_SOURCES = xml-test.cc ../../lib/xml.cc
xml_test_LDADD = $(LIBXML_LIBS) $(LOG)
endif

tcpip_SOURCES = tcpip-test.cc
tcpip_LDADD =  ../../lib/tcpip.lo ../../lib/tcputil.lo $(LOG)
tcpip_DEPENDENCIES = ../../lib/tcpip.lo ../../lib/tcputil.lo $(LOG)

tutil_SOURCES = tcputil-test.cc
tutil_LDADD = ../../lib/tcputil.lo  $(LOG)
tutil_DEPENDENCIES = ../../lib/tcputil.lo $(LOG)

db_SOURCES = db-test.cc
db_LDADD =  $(LOG) -lpthread
db_DEPENDENCIES = $(LOG)

childtcpip_SOURCES = childtcpip.cc
childtcpip_LDADD = ../../lib/tcpip.lo ../../lib/tcputil.lo $(LOG)
childtcpip_DEPENDENCIES = ../../lib/tcpip.lo ../../lib/tcputil.lo $(LOG)
