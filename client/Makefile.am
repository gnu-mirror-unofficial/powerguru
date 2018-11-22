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

## Process this file with automake to generate Makefile.in
AUTOMAKE_OPTIONS = 

bin_PROGRAMS = pguru 

pguru_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/devices

pguru_SOURCES = cmd.cc
pguru_LDADD = ../lib/libpguru.la -ldl

if BUILD_LIBXML
pguru_CPPFLAGS += $(LIBXML_CPPFLAGS)
pguru_LDADD += $(LIBXML_LIBS)
endif

if BUILD_SNMP
pguru_LDADD += ../snmp/libpgsnmp.la $(SNMP_LIBS)
endif

if BUILD_OWNET
pguru_LDADD += $(OWNET_LIBS)
endif

if BUILD_POSTGRESQL
pguru_CPPFLAGS += $(PQ_CPPFLAGS)
pguru_LDADD +=  $(PQ_LIBS)
endif


# pguru_DEPENDENCIES = ../lib/libpguru.la

