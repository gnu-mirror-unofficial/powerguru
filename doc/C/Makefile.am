## Process this file with automake to produce Makefile.in

# Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
# 2003, 2004  Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

include $(top_srcdir)/doc/xmldocs.make

STYLESDIR = @docbook_styles@
EXTRA_DIST += \
	powerguru.html \
	powerguru.pdf \
	pgd.1

figdir = images
docname = powerguru
lang = C
omffile = powerguru.omf
entities = legal.xml xantrex.xml outback.xml
# powerguru_helpdir = $(datadir)/gnome/help/$(docname)/C
# xml_files = $(entities) $(docname).xml
# omf_dir=$(top_srcdir)/omf-install
# EXTRA_DIST = $(xml_files) $(omffile)

# powerguru_help_DATA =    \
#         topic.dat        \
# 	powerguru.xml    \
# 	xantrex.xml      \
# 	outback.xml

all:
	@echo "Nothing to do in doc/C for target all"

alldocs: html pdf man

html: powerguru.html
pdf:  powerguru.pdf
man:  pgd.1
powerguru.pdf powerguru.html: powerguru.xml outback.xml xantrex.xml

.xml.html:
	@echo "Making HTML from XML source..."
	@xsltproc --output $@ --nonet $(STYLESDIR)/html/docbook.xsl $<
#xsltproc --output powerguru.fo --stringparam  paper.type  A4 --nonet $(STYLESDIR)/html/docbook.xsl $(srcdir)/powerguru/doc/C/powerguru.xml
.xml.pdf:
	@echo "Making PDF from XML source..."
	@fop.sh -xsl $(STYLESDIR)/fo/docbook.xsl -xml $< -pdf $@
#	fop.sh -fo powerguru.fo -pdf powerguru.pdf

dist-hook: app-dist-hook

lint:
	@xmllint $(srcdir)/powerguru.xml

CLEANFILES += \
	powerguru.pdf \
	powerguru.rtf \
	powerguru.html \
	powerguru.omf.out \
	manpage.links  \
	manpage.refs  \
	powerguru.tex \
	powerguru.log \
	pgd.1 \
	omf_timestamp

pgd.1: pgd-man.xml
	@echo "Making man pages from XML source..."
	@xsltproc --output pgd.1 --nonet $(STYLESDIR)/manpages/docbook.xsl $(srcdir)/pgd-man.xml

install-data-hook: install-data-hook-omf powerguru.html powerguru.pdf
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(datadir)/doc/$(PACKAGE)-$(VERSION)/images;
	$(INSTALL_DATA) powerguru.html $(DESTDIR)$(datadir)/doc/$(PACKAGE)-$(VERSION);
	$(INSTALL_DATA) powerguru.pdf  $(DESTDIR)$(datadir)/doc/$(PACKAGE)-$(VERSION);
	for png in $(srcdir)/images/*.png; do \
	    if test -f $$png; then \
	      $(INSTALL_DATA) $$png $(DESTDIR)$(datadir)/doc/$(PACKAGE)-$(VERSION)/images; \
	    fi \
         done

uninstall-hook: 
	-rm -fr $(DESTDIR)$(datadir)/doc/$(PACKAGE)-$(VERSION)

# install GNOME help files, which are basically the html output
# install-ghelp: powerguru.html
# 	$(mkinstalldirs) $(powerguru_helpdir)/images
# 	$(INSTALL_DATA) powerguru.html $(powerguru_helpdir)
# 	$(INSTALL_DATA) images/*.png $(powerguru_helpdir)/images

#	$(INSTALL_DATA) $(srcdir)/images/*.png $(powerguru_helpdir)/images

install-doc:
