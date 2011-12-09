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

AC_PREREQ(2.59c)
AC_INIT(snmp/POWERGURU-MIB.txt)
dnl AC_CANONICAL_SYSTEM
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(powerguru, 0.3dev)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([macros])

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
dnl Use the POSIX Threads library, if specified.
AC_ARG_ENABLE(pthreads, [  --enable-pthreads       Enable support for pthreads],[case "${enableval}" in
  yes) pthreads=yes ;;
  no)  pthreads=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-pthreads option]) ;;
esac])
 
if test x$pthreads = xyes; then
    AC_DEFINE([HAVE_PTHREADS], [], [Use POSIX Threads])
fi
                                                                                
dnl Add XML support, if specified.
AC_ARG_ENABLE(libxml, [  --disable-xml           Disable support for xml tcpip],
[case "${enableval}" in
  yes) disablexml=yes ;;
  no)  disablexml=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for disable-xml option]) ;;
esac])
if test x$disable_xml != xno; then
    AC_DEFINE([HAVE_LIBXML], [], [Use XML])
fi
    AM_PATH_XML2

dnl Use the Dom parser from libxml2, not the xmlReader one
dnl The default is to use the Dom parser
dom=yes
AC_ARG_ENABLE(dom, [  --enable-dom            Enable support for Dom parser],
[case "${enableval}" in
  yes) dom=yes ;;
  no)  dom=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac])
 
if test x$dom = xyes; then
    AC_DEFINE([USE_DOM], [], [Use Dom parser])
fi
 
dnl Use an xmlReader parser, instead of the DOM one
AC_ARG_ENABLE(reader, [  --enable-xmlreader      Enable support for xmlReader parser],
[case "${enableval}" in
  yes) reader=yes ;;
  no)  reader=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac])

if test x$reader = xyes; then
    AC_DEFINE([USE_XMLREADER], [], [Use xmlReader parser])
fi
                                                                                
if test x$reader = xyes -a x$dom = xyes; then
   AC_ERROR([Can't specify a DOM and an xmlReader parser])
fi

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

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC
dnl AC_PROG_AR
AC_CHECK_TOOL(AR, ar)

dnl These are for MinGW or Cygwin support.
AC_EXEEXT
AC_CHECK_TOOL(DLLTOOL, dlltool)
AC_CHECK_TOOL(WINDRES, windres)

AC_CHECK_HEADERS([stdarg.h])
AC_CHECK_HEADERS([syslog])

AC_PROG_LIBTOOL

dnl We only use these definitions if we want to dynamically load
dnl shared libraries from our application code.

AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

AM_PATH_MYSQL
AM_PATH_SNMP
AM_CONDITIONAL(BUILD_SNMP, test x$snmp = xtrue)

sim=false
AC_ARG_ENABLE(sim, [  --enable-sim           enable support for Sim (default=yes)],
[case "${enableval}" in
  yes) sim=true ;;
  no)  sim=false ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac])

AM_CONDITIONAL(BUILD_SIM, test x${sim} = xtrue)

dnl link to the images so we can browse the HTML output before
dnl installing it.
AC_LINK_FILES(doc/C/images,doc/C/images)

AC_OUTPUT([
Makefile
lib/Makefile
daemon/Makefile
sim/Makefile
snmp/Makefile
client/Makefile
po/Makefile.in
doc/Makefile
doc/C/Makefile
testsuite/Makefile
testsuite/libtests/Makefile
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

if test x$sim = xtrue; then
  echo "        Using the simulator"
else
  echo "        Not using the simulator"
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
if test x"$pthreads" = x"yes"; then
  echo "        Using POSIX Threads"
else
  echo "        Not using POSIX Threads"
fi
if test x"$disablexml" = x"yes"; then
  echo "        Not using XML"
else
  echo "        Using XML"
fi
if test x"$dom" != x"yes"; then
  echo "        Using DOM parser"
fi
if test x"$reader" != x"yes"; then
  echo "        Using XmlReader (SAX) parser"
fi
