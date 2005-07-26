# 
#   Copyright (C) 2005 Free Software Foundation, Inc.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = # dejagnu

if SNMP
  SNMP_LA = ../snmp/libpgsnmp.la
  SNMP_FILES = $(SNMP_LA) $(SNMP_LIBS)
  SNMP_INCLUDES =  -I$(SNMP_CFLAGS)
  SNMP_DIR = -L../snmp
else
  SNMP_LA = 
  SNMP_FILES = 
  SNMP_INCLUDES = 
  SNMP_DIR = 
endif

INCLUDES = -I$(top_srcdir)/lib $(MYSQL_CFLAGS) $(SNMP_INCLUDES)

bin_PROGRAMS = pgd

pgd_SOURCES = main.cc
pgd_LDFLAGS = -export-dynamic -static -L../lib $(SNMP_DIR)
pgd_LDADD =  -lpguru $(MYSQL_LIBS)  $(SNMP_FILES) #-lpgsnmp
pgd_DEPENDENCIES = ../lib/libpguru.la $(SNMP_LA)

