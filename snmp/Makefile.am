# 
# Copyright (C) 2005, 2006, 2007, 2008,	2009, 2010 Free Software Foundation, Inc.
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

noinst_LTLIBRARIES = libpgsnmp.la

libpgsnmp_la_SOURCES = \
	powerguru.h powerguru.c

#	outback.cc outback.h \
#	xantrex.cc xantrex.h

#		ifTable.c \
#		ifTable_data_get.c \
#		ifTable_interface.c \
#		ifTable_data_access.c \
#		ifTable_data_set.c

AM_CPPFLAGS = -I$(srcdir) -I.. $(SNMP_CPPFLAGS)
libpgsnmp_la_LIBDADD =  $(SNMP_LIBS)

generate-templates:
	@echo "Generating templates from PowerGuru MIB for Xantrex"
	@mib2c xantrex
	@echo "Generating templates from PowerGuru MIB for Outback"
	@mib2c outback

# snmptranslate -M+. -mPOWERGURU-MIB -Tp -IR powerguru
# mib2c -c mib2c.scalar.conf powerGuru
# mib2c -c mib2c.column_defines.conf powerGuru
# mib2c -c mib2c.column_enums.conf powerGuru

#snmpwalk -Os -c public -v1 localhost system.sysDescr.0


