# 
#  Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
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
SUBDIRS = lib # doc

# this is an optional build directory
if BUILD_SNMP
SUBDIRS += snmp $(STD_DIRS) #src
endif

# These are built now, as they depend on the the libraries
SUBDIRS += daemon client # testsuite

# this is an optional build directory
if BUILD_SIM
SUBDIRS += sim
endif

EXTRA_DIST = \
	autogen.sh \
	powerguru.sql \
	powerguru.glade2 \
	powerguru.glade2p \
	doc/C/topic.dat \
	doc/C/xantrex.xml \
	doc/C/outback.xml \
	doc/C/pgd-man.xml \
	po/ChangeLog \
	po/Makefile.in.in \
	po/POTFILES.in \
	web

#install-data-local:
# 	@$(NORMAL_INSTALL)
# 	if test -d $(srcdir)/pixmaps; then \
# 	  $(mkinstalldirs) $(datadir)/pixmaps/$(PACKAGE); \
# 	  for pixmap in $(srcdir)/pixmaps/*; do \
# 	    if test -f $$pixmap; then \
# 	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(datadir)/pixmaps/$(PACKAGE); \
# 	    fi \
# 	  done \
# 	fi

#install-doc:
#	@cd doc/C ; $(MAKE) install-doc

#doc:
#	@cd doc/C ; $(MAKE) alldocs

#dist-hook:
# 	if test -d pixmaps; then \
# 	  mkdir $(distdir)/pixmaps; \
# 	  for pixmap in pixmaps/*; do \
# 	    if test -f $$pixmap; then \
# 	      cp -p $$pixmap $(distdir)/pixmaps; \
# 	    fi \
# 	  done \
# 	fi

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

mudflap:
	@echo "Rebuilding with GCC Mudflap support"
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -fmudflap" LDFLAGS="$(LDFLAGS) -lmudflap"
