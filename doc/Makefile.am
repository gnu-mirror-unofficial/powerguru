# Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013
#               2014, 2015, 2016, 2017, 2018, 2019
#     Free Software Foundation, Inc.
#
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

SUBDIRS = C

XTRA_DIST = Doxyfile.in

if ENABLE_DOXYGEN

.PHONY: apidoc

apidoc: 
	$(DOXYGEN) $<

clean-generic:
	rm -Rf apidoc

install-apidoc:

endif
