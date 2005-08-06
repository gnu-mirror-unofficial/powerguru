dnl Process this file with autoconf to produce a configure script.
dnl
dnl  Copyright (C) 2005 Free Software Foundation, Inc.
dnl
dnl  This program is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU General Public License as published by
dnl  the Free Software Foundation; either version 2 of the License, or
dnl  (at your option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with this program; if not, write to the Free Software
dnl  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(powerguru, 0.2)
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(config.h)

dnl Use the DMalloc library, if specified.
AC_ARG_ENABLE(dmalloc, [  --enable-dmalloc        enable support for DMalloc],
[case "${enableval}" in
  yes) dmalloc=yes ;;
  no)  dmalloc=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac])

if test x$dmalloc = xyes; then
    AC_DEFINE([USE_DMALLOC], [], [Use DMalloc])
    DMALLOC_LIB="-ldmallocxx"
else
    DMALLOC_LIB=""
fi
AC_SUBST(DMALLOC_LIB)

dnl Where are the Docbook stylesheets ?
AC_ARG_WITH(docbook, [  --with-docbook          specify the path to the Docbook stylesheets],
[case "${withval}" in
  yes) docbook=yes ;;
  no)  docbook=no ;;
  *)   AC_MSG_ERROR([bad value ${withval} for with-docbook option]) ;;
esac])

if test x$docbook = xyes; then
    DOCBOOK_STYLES="${withval}"
else
   AC_DOCBOOK_STYLES
fi

AC_CHECK_HEADERS([stdarg.h])
AC_CHECK_HEADERS([syslog])

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC

AC_PROG_LIBTOOL
dnl We only use these defnitions if we want to dynamically load
dnl shared libraries from our application code.

dnl Look for libltdl, which we use for dealing with shared libraries
dnl AC_LIBTOOL_DLOPEN
dnl AC_LIBLTDL_CONVENIENCE
dnl AC_LIB_LTDL
dnl DLOPEN="-dlopen"
dnl DLPREOPEN="-dlpreopen"
dnl AC_SUBST(DLOPEN)
dnl AC_SUBST(DLPREOPEN)

dnl Substitute INCLTDL and LIBLTDL in the Makefiles
dnl AC_SUBST(INCLTDL)
dnl AC_SUBST(LIBLTDL)
dnl AC_SUBST(LIBTOOL_DEPS)

AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"powerguru", [Gettext package.])

AM_PATH_MYSQL
AM_PATH_SNMP

dnl link to the images so we can browse the HTML output before
dnl installing it.
AC_LINK_FILES(doc/C/images,doc/C/images)

AC_OUTPUT([
Makefile
lib/Makefile
daemon/Makefile
sim/Makefile
snmp/Makefile
po/Makefile.in
doc/Makefile
doc/C/Makefile
])

dnl we arent building this for now
dnl src/Makefile

echo ""
echo "Configurable options are:"
dnl SNMP
if test x$snmp = xtrue; then
  echo "        Using SNMP"
else
  echo "        Not using SNMP"
fi

dnl Dmalloc
if test x$dmalloc = xyes; then
  echo "        Using DMALLOC"
else
  echo "        Not using DMalloc"
fi

dnl Docbook
if test x$docbook = xyes; then
  echo "        Docbook styles in $DOCBOOK_STYLES"
fi
