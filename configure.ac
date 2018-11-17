dnl Process this file with autoconf to produce a configure script.
dnl
dnl  Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011
dnl	Free Software Foundation, Inc.
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
AC_INIT([snmp/POWERGURU-MIB.txt], [0.4dev])
dnl AC_CANONICAL_SYSTEM
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([macros])

dnl Use the DMalloc library, if specified.
AM_WITH_DMALLOC

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

db="none"
AC_ARG_WITH(db, [  --with-db          database to use, mariadb or the default of postgresql],
[case "${withval}" in
  p*) db=postgresql ;;
  m*) db=mariadb ;;
  *)   AC_MSG_ERROR([bad value ${withval} for with-db option]) ;;
esac])
AM_CONDITIONAL(BUILD_MARIADB, [ test x${db} = xmariadb ])
AM_CONDITIONAL(BUILD_POSTGRESQL, [ test x${db} = xpostgresql ])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC
dnl AC_PROG_AR
AC_CHECK_TOOL(AR, ar)
AC_PATH_PROG(DEJAGNU, runtest)

sysroot="${sysroot:-`${CXX} -print-sysroot`}"
AC_ARG_WITH(sysroot,
  AC_HELP_STRING([--with-sysroot], [system root directory for cross compiling]),
  sysroot=${withval};
  cross_compiling=yes)

with_top_level=""
if test x"${cross_compiling}" = xyes; then
  AC_MSG_CHECKING([For a usable sysroot])
  dnl Check if /usr isn't part of the specified path
  if test -d ${sysroot}/usr; then
    sysroot=${withval}/usr
  else 
    dnl Check if the specified path supplies /usr
    if test -d ${sysroot}/usr/include; then
      sysroot=${sysroot}
    else
      AC_MSG_ERROR([Specified sysroot \"${sysroot}\" doesn't exist!])
    fi
    CXXFLAGS="--sysroot=${sysroot}"
    CFLAGS="--sysroot=${sysroot}"
    LDFLAGS="--sysroot=${sysroot}"
  fi
  AC_MSG_RESULT([${sysroot}])
  AC_SUBST(sysroot)
fi

dnl When cross compiling, limit the search directories cause otherwise
dnl we may get the host headers or libraries by accident. These values
dnl are exported, so all the other configure tests in macros/*.m4 use
dnl these same settings rather than duplicating them like we used to.
dnl To override thise, use the --with-*-incl= and --with-*-libs=
dnl options to configure.
if test x$cross_compiling = xyes; then
  AC_MSG_NOTICE([Configuring PowerGuru for cross compilation])
  export pkgroot="`$CXX -print-search-dirs | grep "install:" | sed -e 's/install: //' -e 's:/lib/gcc/.*::'`"
  dnl pkgroot only works correctly with builds of cross tools not in
  dnl /usr, ie... installed from the distribution packages, or just
  dnl plain installed in the system tools. This contaminates configure
  dnl when building for variations of the same basic architecture,
  dnl like i686-linux -> i586-mingw32.
  if test x"${pkgroot}" = x"/usr"; then
    export pkgroot=""
  fi
  export incllist="`eval echo ${sysroot}/include ${pkgroot}/${host_alias}/include ${pkgroot}/include`"
  export libslist="`eval echo ${sysroot}/lib ${pkgroot}/${host_alias}/lib ${pkgroot}/lib64 ${pkgroot}/lib32 ${pkgroot}/lib`"
  export pathlist="`eval echo ${pkgroot}/${host_alias}/bin:${pkgroot}/bin`"
  npapi=no
else
  AC_MSG_NOTICE([Configuring PowerGuru for native compilation])
  export incllist="`eval cat ${srcdir}/macros/incllist`"
  libslist="`cat ${srcdir}/macros/libslist`"
  if test -f /usr/bin/dpkg-architecture; then
    export DEB_HOST_MULTIARCH="`eval dpkg-architecture -qDEB_HOST_MULTIARCH`"
    export libslist="${libslist} /lib/${DEB_HOST_MULTIARCH} /usr/lib/${DEB_HOST_MULTIARCH}"
  fi
  export pathlist=$PATH
fi
AM_CONDITIONAL(CROSS_COMPILING, [ test x$cross_compiling = xyes ])

save_LDFLAGS="${LDFLAGS}"
save_CXXFLAGS="${CXXFLAGS}"
save_CPPFLAGS="${CPPFLAGS}"
save_CFLAGS="${CFLAGS}"

CFLAGS="${save_CFLAGS} -Wall -Werror"

# dnl libxml2's headers are sometimes in a libxml2 directory, but not always
CPPFLAGS="-I${sysroot}/usr/include/libxml2"
AC_MSG_CHECKING([For libxml2 development files])
AC_TRY_COMPILE([#include <libxml/xmlstring.h>], [
  xmlChar foo = 'x';
  foo += 'y';
  ],
  has_libxml=true,
  has_libxml=false
)
AC_MSG_RESULT([${has_libxml}])
if test x"${has_libxml}" = x"true"; then
  AC_DEFINE([HAVE_LIBXML], [1], [Has libxml2 development files])
  LIBXML_LIBS="-lxml2"
  LIBXML_CPPFLAGS="-I${sysroot}/usr/include/libxml2"
  AC_SUBST(LIBXML_CPPFLAGS)
  AC_SUBST(LIBXML_LIBS)
fi
AM_CONDITIONAL(BUILD_LIBXML, [ test x${has_libxml} = xtrue ])
dnl GNU_PKG_FIND(libxml2, [libxml/xmlstring.h], [libxml2], xmlNewReference)

dnl libpq. FIXME: add libpqcxx
db=mariadb
LDFLAGS="${save_LDFLAGS} -lpq"
CPPFLAGS="-I${sysroot}/usr/include/postgresql/9.6/server/libpq"
AC_MSG_CHECKING([For postgresql development files])
AC_TRY_COMPILE([#include <libpq-fe.h>], [
  PQpass(0);
  ],
  has_libpq=true,
  has_libpq=false
)
AC_MSG_RESULT([${has_libpq}])
if test x"${has_libpq}" = x"true"; then
  AC_DEFINE([HAVE_LIBPQ], [1], [Has postgresql development files])
  LIBPQ_LIBS="-lpq"
  LIBPQ_CPPFLAGS="-I${sysroot}/usr/include/postgresql/9.6/server/libpq"
  db=postgresql
  AC_SUBST(LIBPQ_CPPFLAGS)
  AC_SUBST(LIBPQ_LIBS)
fi
AM_CONDITIONAL(BUILD_POSTGRESQL, [ test x${has_libpq} = xtrue ])

dnl Net-SNMP
dnl net-snmp's headers require these two config options or you get
dnl an error. FIXME: this need to be real tests!
AC_DEFINE([HAVE_STRCASESTR], [1], [ Has strcasestr defined ])
AC_DEFINE([HAVE_IN_ADDR_T], [1], [ Has typedef in_addr_t])

CFLAGS="${save_CFLAGS} -Wall"
LDFLAGS="${save_LDFLAGS} -lnetsnmp -lnetsnmpagent"
AC_MSG_CHECKING([For Net SNMP development files])
AC_TRY_COMPILE([#include <net-snmp/net-snmp-config.h>
#include <net-snmp/types.h>], [
  netsnmp_pid_t foo;
  ],
  has_snmp=true,
  has_snmp=false
)
AC_MSG_RESULT([${has_snmp}])
if test x"${has_snmp}" = x"true"; then
  AC_DEFINE([HAVE_SNMP], [1], [Has Net SNMP development files])
  SNMP_LIBS="-lnetsnmp -lnetsnmpagent"
  AC_SUBST(SNMP_LIBS)
fi
AM_CONDITIONAL(BUILD_SNMP, [ test x${has_snmp} = xtrue ])

dnl mariadb development files
dnl FIXME

LDFLAGS="${save_LDFLAGS}"
CXXFLAGS="${save_CXXFLAGS}"
CPPFLAGS="${save_CPPFLAGS}"
CFLAGS="${save_CFLAGS}"

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

dnl AM_PATH_POSTGRESQL
dnl AC_PATH_MYSQL
AM_CONDITIONAL(BUILD_MARIADB, [ test x${has_mariadb} = xtrue ])

AC_ARG_ENABLE(sim, [  --enable-sim           enable support for Sim (default=no)],
[case "${enableval}" in
  yes) sim=true ;;
  no)  sim=false ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac], sim=false)

AM_CONDITIONAL(BUILD_SIM, test x${sim} = xtrue)

AC_ARG_ENABLE(client, [  --enable-client           enable support for Client (default=no)],
[case "${enableval}" in
  yes) client=true ;;
  no)  client=false ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac], client=false)

AM_CONDITIONAL(BUILD_CLIENT, test x${client} = xtrue)

AC_ARG_WITH(tty,
  AC_HELP_STRING([--with-tty=], [Specify which TTY's to read data from]),
  TTYS=${with_tty}
  AC_SUBST(TTYS)
)


dnl link to the images so we can browse the HTML output before
dnl installing it.
AC_LINK_FILES(doc/C/images,doc/C/images)

AC_OUTPUT([
Makefile
lib/Makefile
daemon/Makefile
sim/Makefile
devices/Makefile
snmp/Makefile
client/Makefile
po/Makefile
doc/Makefile
doc/C/Makefile
testsuite/Makefile
testsuite/libtests/Makefile
])

dnl we arent building this for now
dnl src/Makefile
echo ""
echo "Configurable options are:"

dnl if test x$sim = xtrue; then
dnl   echo "        Using the simulator"
dnl else
dnl   echo "        Not using the simulator"
dnl fi

dnl Docbook
dnl if test x$docbook = xyes; then
dnl   echo "        Docbook styles in $DOCBOOK_STYLES"
dnl fi

dnl
dnl Often warning messages get lost in the output from configure, so
dnl redisplay the few critical ones so they don't get ignored.
dnl
if test x"${has_libxml}" != x"true"; then
  echo "        To use XML, install the libxml2 development package for your system"
else
  echo "        Found libxml2, will configure XML support"
fi

if test x"${has_snmp}" != x"true"; then
  echo "        To use SNMP. install the net-snmp development package for your system"
else
  echo "        Found net-snmp. will configure SNMP support"
fi

if test x"${db}" = x"none"; then
  echo "        To use a database. install the mariadb or postgresql development package for your system"
else
  echo "        Found a database. will configure database support for ${db}"
fi

dnl if test x"$dom" != x"yes"; then
dnl   echo "        Using DOM parser"
dnl fi
dnl if test x"$reader" != x"yes"; then
dnl   echo "        Using XmlReader (SAX) parser"
dnl fi
