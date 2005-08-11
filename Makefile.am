# Copyright (C) 2005 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = dejagnu

STD_DIRS = lib daemon doc sim client # testsuite
if SNMP
SUBDIRS = snmp $(STD_DIRS) #src
else
SUBDIRS = $(STD_DIRS) #src
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

install-doc:
	@cd doc/C ; $(MAKE) install-doc

doc:
	@cd doc/C ; $(MAKE) alldocs
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
