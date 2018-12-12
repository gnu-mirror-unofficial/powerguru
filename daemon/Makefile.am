# 
# Copyright (C) 2005, 2006-2018
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

AUTOMAKE_OPTIONS = # dejagnu

pgd_CPPFLAGS = -Wall -I$(top_srcdir)/lib -I$(top_srcdir)/devices
pgd_LDADD = ../devices/libpdev.la ../lib/libpguru.la -lpthread -lresolv

if BUILD_LIBXML
pgd_CPPFLAGS += $(LIBXML_CPPFLAGS)
pgd_LDADD += $(LIBXML_LIBS)
endif

if BUILD_SNMP
pgd_LDADD += ../snmp/libpgsnmp.la $(SNMP_LIBS)
endif

if BUILD_OWNET
pgd_LDADD += $(OWNET_LIBS)
endif

if BUILD_POSTGRESQL
pgd_CPPFLAGS += $(PQ_CPPFLAGS)
pgd_LDADD += $(PQ_LIBS)
endif

bin_PROGRAMS = pgd

pgd_SOURCES = main.cc threads.cc
pgd_LDFLAGS = -export-dynamic -ldl

# MUDFLAP_OPT = -fmudflap
# MUDFLAP_LIB =  /usr/local/lib/libmudflap.so

# Rebuild with GCC 4.x Mudflap support
# mudflap:
#	@echo "Rebuilding with GCC Mudflap support"
#	$(MAKE) CXXFLAGS="$(CXXFLAGS) $(MUDFLAP_OPT)" LIBS="$(LIBS) $(MUDFLAP_LIB)"

