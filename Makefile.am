# 
# Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013
#               2014, 2015, 2016, 2017, 2018, 2019
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

## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = dejagnu

ACLOCAL_AMFLAGS = -I macros

# these have to be built first
# FIXME: make python optional
SUBDIRS = lib testsuite doc python

# this is an optional build directory
if BUILD_SNMP
SUBDIRS += snmp $(STD_DIRS) #src
endif

# Build an RPM package
include $(srcdir)/rpm/rpm.am

# These are built now, as they depend on the the libraries
SUBDIRS += devices client daemon # testsuite

# this is an optional build directory
if BUILD_SIM
SUBDIRS += sim
endif

EXTRA_DIST = \
	autogen.sh \
	powerguru.sql \
	doc/C/topic.dat \
	doc/C/xantrex.xml \
	doc/C/outback.xml \
	doc/C/pgd-man.xml \
	po/ChangeLog \
	po/Makefile.in.in \
	po/POTFILES.in \
	web


# Install executables into qemu, which we do a lot when experimenting with
# toolchains and cross compiing something that actually works.
install-qemu:
	rsync -avrp -e "ssh -p 5022" client/pguru daemon/pgd localhost:/tmp/

# Note that this requires an entry in /etc/hostsfor 'pi'
install-pi: install
	rsync -avrp /tmp/foo pi:/tmp/

# Build an RPM package
rpm redhat: dist-gzip $(srcdir)/rpm/powerguru.spec
	cp -f $(srcdir)/rpm/powerguru.spec /usr/src/redhat/SPECS/
	cp -f powerguru-${VERSION}.tar.gz /usr/src/redhat/SOURCES/
	rpmbuild -ba /usr/src/redhat/SPECS/powerguru.spec

# Build a Debian GNU/Linux package
deb debian: dist-bzip2 $(srcdir)/debian/rules
	rm -rf powerguru-${VERSION}
	tar jxf powerguru-${VERSION}.tar.bz2
	cp -fr $(srcdir)/debian powerguru-${VERSION}/
	(cd powerguru-${VERSION}; dpkg-buildpackage)
#	(cd powerguru-${VERSION}; dpkg-buildpackage --host-arch armhf -us -uc -b )

mudflap:
	@echo "Rebuilding with GCC Mudflap support"
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -fmudflap" LDFLAGS="$(LDFLAGS) -lmudflap"

apidoc:
	$(MAKE) -C doc $@

pip-install:
	pip3 install --upgrade -e $(top_srcdir)/python

# Google's C++ utility https://github.com/myint/cppclean
cppclean:
	@-cppclean --include-path=. --include-path=$(top_srcdir)/lib $(top_srcdir)/devices >& /tmp/cppclean.tmp
	@echo ""
	@echo "Static data, be careful in threads"
	@grep "static data" /tmp/cppclean.tmp
	@echo ""
	@echo "Other issues found by cppclean"
	@grep -v "static data" /tmp/cppclean.tmp

dump:
	@echo "Dumping configure options"
if BUILD_SNMP
	@echo "	Building with net-snmp"
endif
if BUILD_LIBXML
	@echo "	Building with libxml2"
endif
if BUILD_MARIADB
	@echo "	Building with mariadb"
endif
if BUILD_POSTGRESQL
	@echo "	Building with Postgresql"
endif
if BUILD_OUTBACK
	@echo "	Building with Outback protocol"
endif

if BUILD_XANTREX
	@echo "	Building with Xantrex protocol"
endif
if BUILD_OWNET
	@echo "	Building with 1-Wire support"
endif
if BUILD_CROSS
	@echo "	Dumping compiler data for ${CXX}"
	@echo "	Default search dirs:"
	@${CXX} -print-search-dirs
	@echo "	Default sysroot: `${CXX} -print-sysroot`"
	@echo "	User supplied sysroot: $(sysroot)"
	@echo "	Libc.so is from here: `${CXX} -print-file-name=libc.so`"
endif

CLEANFILES = powerguru_* pypowerguru_* \
	powerguru-*dev powerguru*.bz2 *.sum *.log

# makefile for etags, used by emacs and vi
tags:
	etags `find -name \*.py -o -name \*.cc -o -name \*.h`
