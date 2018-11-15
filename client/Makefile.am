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

PGURU = ../lib/libpguru.la
bin_PROGRAMS = pguru 

AM_CPPFLAGS = -I$(top_srcdir)/lib $(XML_CPPFLAGS)

pguru_SOURCES = cmd.cc
pguru_LDADD = $(PGURU) $(XML_LIBS) $(MYSQL_LIBS) -lpthread $(SNMP_LIBS)
if BUILD_SNMP
pguru_LDADD += ../snmp/libpgsnmp.la
endif

pguru_DEPENDENCIES = $(PGURU)


