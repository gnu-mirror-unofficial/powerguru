dnl Process this file with autoconf to produce a configure script.
dnl
dnl  Copyright (C) 2005, 2006-2018
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
AC_INIT([Powerguru], [0.5dev])
dnl AC_CANONICAL_SYSTEM
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([macros])
AM_PATH_PYTHON([3])

dnl Use the DMalloc library, if specified.
AM_WITH_DMALLOC
AC_PROG_LN_S

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

# Debian/Ubuntu
# STYLESDIR = /usr/share/xml/docbook/stylesheet/docbook-xsl
# Fedora
# STYLESDIR = /usr/share/sgml/docbook/xsl-stylesheets-1.79.2

optional=""
missing=""
devices=""
BUILD_OWNET="yes"
BUILD_OUTBgACK="no"
BUILD_XANTREX="no"
AC_ARG_WITH(devices, [  --with-devices          the device(s) to build (ownet, outback, xantrex)],
[devices="`echo ${withval} | tr ',' ' '`"
for i in ${devices}; do
    echo "FIXME: $i"
     case ${i} in
     	 serial)  BUILD_SERIAL="yes" ;;
      	 ownet)  BUILD_OWNET="yes" ;;
      	 outback) BUILD_OUTBACK="yes" ;;
      	 xantrex) BUILD_XANTREX="yes" ;;
     	 *) AC_MSG_ERROR([bad value ${i} for with-device option]) ;;
     esac
done
])
dnl AC_SUBST(BUILD_OWNET)
dnl AC_SUBST(BUILD_OUTBACK)
dnl AC_SUBST(BUILD_XANTREX)

AM_CONDITIONAL(BUILD_SERIAL, [ test x${BUILD_SERIAL} = xyes ])
if test x${BUILD_SERIAL} = xyes; then
   AC_DEFINE([BUILD_SERIAL], [1], [Build support for serial ports])
fi
AM_CONDITIONAL(BUILD_OWNET, [ test x${BUILD_OWNET} = xyes ])
if test x${BUILD_OWNET} = xyes; then
   AC_DEFINE([BUILD_OWNET], [1], [Build support for owfs])
fi
AM_CONDITIONAL(BUILD_OUTBACK, [ test x${BUILD_OUTBACK} = xyes ])
if test x${BUILD_OUTBACK} = xyes; then
   AC_DEFINE([BUILD_OUTBACK], [1], [Build support for the Outback protocol])
fi
AM_CONDITIONAL(BUILD_XANTREX, [ test x${BUILD_XANTREX} = xyes ])
if test x${BUILD_XANTREX} = xyes; then
   AC_DEFINE([BUILD_XANTREX], [1], [Build support for the Xantrex protocol])
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
AC_PATH_PROG(DOXYGEN, doxygen)
AM_CONDITIONAL(ENABLE_DOXYGEN, [ test x"$DOXYGEN" != x ])

dnl The Raspberry PI is currently an ARMv6, whereas the cross compiler packages
dnl one can download usually default to the ARMV7. If so, tweak the compiler
dnl flags to generate an executable for the ARMv6.
ops="`echo '' | ${CC} -v -E - 2>&1 | grep cc1`"
cpu="`echo ${ops} | grep -o "mcpu=[[0-9a-z-]]*" | cut -d '=' -f 2`"
fpu="`echo ${ops} | grep -o "fpu=[[0-9a-z-]]*" | cut -d '=' -f 2`"
mach="`${CXX} -dumpmachine | cut -d '-' -f 1`"
gcc_sysroot="`${CXX} -print-sysroot | sed -e 's:bin/../::'`"
dnl The distro supplied packages have no sysroot
if test x"${gcc_sysroot}" = x -a x"${mach}" != x'x86_64'; then
  cross_compiling=yes
  CXXFLAGS="${CXXFLAGS} -marm -mfpu=vfp -mfloat-abi=hard -pipe"
fi
dnl My custom built toolchains have a sysroot.
if test x"${gcc_sysroot}" != x -a x"${gcc_sysroot}" != x'/'; then
  cross_compiling=yes 
fi
AC_MSG_NOTICE([Default compiler architecture is: ${arch}])

sysroot=""
AC_ARG_WITH(sysroot,
  AC_HELP_STRING([--with-sysroot], [system rootfs directory for cross compiling]),
  sysroot=${withval}
  cross_compiling=yes
  )

dnl Sanity check the sysroot for cross compiling to avoid other errors
if test x"${cross_compiling}" = xyes; then
  AC_MSG_CHECKING([For a usable sysroot])
  dnl Check if /usr isn't part of the specified path
  for i in ${gcc_sysroot} ${sysroot}; do
    if test ! -d ${i}/usr && ! -d ${i}/usr/include; then
      AC_MSG_ERROR([Specified sysroot \"${i}\" doesn't exist!])
      AC_MSG_RESULT([not found])
    else
      AC_MSG_RESULT([found])
    fi
  done

  dnl If the user specifies a sysroot that isn't the default, add it to
  dnl the compilation
  if test x"${gcc_sysroot}" != x"${sysroot}" -a x"${sysroot}" != x; then
    newhost="`echo ${host_alias} | sed -e 's:-pi.-:-:'`"
    SYSROOT+=" --sysroot=${sysroot}"
    CFLAGS+=" --sysroot=${sysroot}"
    LDFLAGS+=" -Wl,-rpath=${sysroot}/usr/lib/${newhost}"
    LDFLAGS+=" -Wl,-rpath=${sysroot}/lib/${newhost}"
  else
    SYSROOT=""
fi
  AC_SUBST(SYSROOT)
fi
AM_CONDITIONAL(BUILD_CROSS, [ test x"${cross_compiling}" == xyes ])

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
  RASPI_FLAGS="-march=armv6z -mtune=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard"
  AC_SUBST(RASPI_FLAGS)
fi
#AM_CONDITIONAL(CROSS_COMPILING, [ test x$cross_compiling = xyes ])

save_LDFLAGS="${LDFLAGS}"
save_CXXFLAGS="${CXXFLAGS}"
save_CPPFLAGS="${CPPFLAGS}"
save_CFLAGS="${CFLAGS}"

dnl THe 2011 C++ standard added many Boost libraries to the
dnl standard so we use those instead of wrapping C code like
dnl the good old days...
CFLAGS="--std=c++11"
AC_MSG_CHECKING([For C++11 support])
AC_TRY_COMPILE([#include <time.h>], [
  int foo = CLOCKS_PER_SEC;
  ],
  has_c11=yes,
  has_c11=no
)
AC_MSG_RESULT([${has_c11}])
if test x"${has_c11}" = x"no"; then
  AC_MSG_ERROR([You need a C++ compiler that supports the C++11 standard])
fi

dnl Force warningto cause an error for compile tests
CFLAGS="${save_CFLAGS} -Wall -Werror"

# dnl libxml2's headers are sometimes in a libxml2 directory, but not always
headers="${sysroot:-${gcc_sysroot}}/usr ${sysroot:-${gcc_sysroot}}/usr/local"
for i in ${headers}; do
   if test -d "${i}/include/libxml2"; then
     CPPFLAGS="-I${i}/include/libxml2"
   fi
done

AC_MSG_CHECKING([For libxml2 development files])
AC_TRY_COMPILE([#include <libxml/xmlstring.h>], [
  xmlChar foo = 'x';
  foo += 'y';
  ],
  has_libxml=yes,
  has_libxml=no
)
AC_MSG_RESULT([${has_libxml}])
if test x"${has_libxml}" = x"yes"; then
  AC_DEFINE([HAVE_LIBXML], [1], [Has libxml2 development files])
  LIBXML_LIBS="-lxml2"
  LIBXML_CPPFLAGS="${CPPFLAGS}"
  AC_SUBST(LIBXML_CPPFLAGS)
  AC_SUBST(LIBXML_LIBS)
fi
AM_CONDITIONAL(BUILD_LIBXML, [ test x${has_libxml} = xyes ])
dnl GNU_PKG_FIND(libxml2, [libxml/xmlstring.h], [libxml2], xmlNewReference)

dnl libpq. FIXME: add libpqcxx
db=none
LDFLAGS="${save_LDFLAGS} -lpq"
headers="/usr/include/postgresql/9.6/server/libpq /usr/local/include/libpq /usr/local/include /usr/include/postgresql "
for i in ${headers}; do
if test ${sysroot}${i}/libpq-fe.h; then
   CPPFLAGS="-I${sysroot}${i}"
fi
done
AC_MSG_CHECKING([For postgresql development files])
AC_TRY_COMPILE([#include <libpq-fe.h>], [
  PQpass(0);
  ],
  has_libpq=yes,
  has_libpq=no
)
AC_MSG_RESULT([${has_libpq}])
if test x"${has_libpq}" = x"yes"; then
  AC_DEFINE([HAVE_LIBPQ], [1], [Has postgresql development files])
  PQ_LIBS="-lpq"
  PQ_CPPFLAGS="${CPPFLAGS}"
  db=postgresql
  AC_SUBST(PQ_CPPFLAGS)
  AC_SUBST(PQ_LIBS)
fi
AM_CONDITIONAL(BUILD_POSTGRESQL, [ test x${has_libpq} = xyes ])

dnl Net-SNMP
dnl net-snmp's headers require these two config options or you get
dnl an error. FIXME: this need to be real tests!
AC_DEFINE([HAVE_STRCASESTR], [1], [ Has strcasestr defined ])
AC_DEFINE([HAVE_IN_ADDR_T], [1], [ Has typedef in_addr_t])

dnl Custom build sysroots often have manually compiled projects in
dnl /usr/local to keep them separate from system installed ones.
CPPFLAGS=" -I${sysroot:-${gcc_sysroot}}/usr/include"
CFLAGS="${save_CFLAGS} -Wall"
LDFLAGS="${save_LDFLAGS} -lnetsnmp -lnetsnmpagent"
AC_MSG_CHECKING([For Net SNMP development files])
AC_TRY_COMPILE([#include <net-snmp/net-snmp-config.h>
#include <net-snmp/types.h>], [
  netsnmp_pid_t foo;
  ],
  has_snmp=yes,
  has_snmp=no
)
AC_MSG_RESULT([${has_snmp}])
if test x"${has_snmp}" = x"yes"; then
  AC_DEFINE([HAVE_SNMP], [1], [Has Net SNMP development files])
  SNMP_LIBS="-lnetsnmp -lnetsnmpagent"
  SNMP_CPPFLAGS="${CPPFLAGS}"
  AC_SUBST(SNMP_LIBS)
  AC_SUBST(SNMP_CPPFLAGS)
fi
dnl AM_CONDITIONAL(BUILD_SNMP, [ test x${has_snmp} = xyes ])
AM_CONDITIONAL(BUILD_SNMP, [ test x${has_snmp} = xno ])

dnl dnl OWFS
LDFLAGS="${save_LDFLAGS} -l"
AC_MSG_CHECKING([For libownet development files])
AC_TRY_COMPILE([#include <owcapi.h>], [
   char *foo = OWFS_VERSION;
   foo += 1;
  ],
  has_ownet=yes,
  has_ownet=no
)
AC_MSG_RESULT([${has_ownet}])
if test x"${has_ownet}" = x"yes"; then
  AC_DEFINE([HAVE_OWNET], [1], [Has ownet development files])
  OWNET_LIBS="-lownet -lowcapi"
  AC_SUBST(OWNET_LIBS)
fi
AM_CONDITIONAL(BUILD_OWNET, [ test x${has_ownet} = xyes ])
dnl ow-shell is a collection off shell utilities
AC_CHECK_PROG([owshell], [owdir], [yes], [no], [/usr/bin])
if test x"${owshell}" = x"no"; then
   AC_MSG_WARN([You should install the 'ow-shell' package])
   optional="${optional} owshell"
fi

dnl mariadb development files
dnl FIXME
dnl FIXME: add tests for libownet-dev

dnl These tests use the C++ compiler
save_CC="${CC}"
CC="${CXX}"
AC_MSG_CHECKING([For Boost development files])
dnl AC_TRY_COMPILE([#include <boost/algorithm/algorithm.hpp>], [
dnl   using boost::algorithm;
dnl   ],
   has_boost=yes,
dnl   has_boost=no
dnl )
if test x"${has_boost}" = x"no"; then
   AC_MSG_WARN([You should install the 'libboost-dev' package])
   optional="${optional} libboost-dev"
fi
AC_MSG_RESULT([${has_boost}])

AC_MSG_CHECKING([For Boost filesystem development files])
AC_TRY_COMPILE([#include <boost/filesystem.hpp>], [
  boost::filesystem::path p("/tmp");
  ],
  has_boost_filesystem=yes,
  has_boost_filesystem=no
)
AC_MSG_RESULT([${has_boost_filesystem}])

dnl FIXME: need test for boost log, which isn't in Centos 7.6, aka... boost 1.53
AC_MSG_CHECKING([For Boost log development files])
AC_TRY_COMPILE([#include <boost/log/core.hpp>], [
  int foo = BOOST_LOG_CORE_HPP_INCLUDED_;
  ],
  has_boost_log=yes,
  has_boost_log=no
)
AC_MSG_RESULT([${has_boost_log}])

AC_MSG_CHECKING([For Boost regex development files])
AC_TRY_COMPILE([#include <boost/regex.hpp>], [
  boost::regex re;
  ],
  has_boost_regex=yes,
  has_boost_regex=no
)
AC_MSG_RESULT([${has_boost_regex}])

AC_MSG_CHECKING([For Boost system development files])
AC_TRY_COMPILE([#include <boost/system/error_code.hpp>], [
  boost::system::error_code be;
  ],
  has_boost_system=yes,
  has_boost_system=no
)
AC_MSG_RESULT([${has_boost_system}])


dnl We need all of these boost libraries
if (test x"${has_boost_filesystem}" = x"no" \
   -o x"${has_boost_regex}" = x"no" -o x"${has_boost_system}" = x"np"); then
  AC_MSG_ERROR([You need all the boost libraries!])
fi
LIBS="-lboost_filesystem -lboost_system -lboost_regex -lboost_log -lboost_log_setup -lboost_thread -lboost_date_time"
AC_DEFINE([BOOST_LOG_DYN_LINK], [1], [Force Boost::logging to be dynamically linked])
dnl AC_DEFINE(]BOOST_ALL_DYN_LINK], [1], [Fo ])

AC_MSG_CHECKING([For DejaGnu])
AC_TRY_COMPILE([#include <dejagnu.h>], [
   TestState test;
  ],
  has_dejagnu=yes,
  has_dejagnu=no
)
AC_MSG_RESULT([${has_dejagnu}])
CC="${save_CC}"

dnl FIXME: Force this definition as there is a type conflict between the net-snmp
dnl defintion and the sys/types,h one.
AC_DEFINE([HAVE_SSIZE_T], [1], [Has ssize_t defined])
AC_DEFINE([HAVE_STRTOULL], [1], [Has strtoull defined])

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
dnl Link to the python code so it's easier to use from the builddir.
AC_LINK_FILES(doc/C/images,doc/C/images)
dnl AC_LINK_FILES(python,python)


AC_OUTPUT([
Makefile
lib/Makefile
daemon/Makefile
sim/Makefile
devices/Makefile
snmp/Makefile
client/Makefile
po/Makefile.in
doc/Makefile
doc/Doxyfile
doc/C/Makefile
python/Makefile
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
if test x"${has_libxml}" != x"yes"; then
  echo "        To use XML, install the libxml2 development package for your system"
else
  echo "        Found libxml2, will configure XML support"
fi

dnl if test x"${has_snmp}" != x"yes"; then
dnl   echo "        To use SNMP. install the net-snmp development package for your system"
dnl else
dnl   echo "        Found net-snmp. will configure SNMP support"
dnl fi

if test x"${build_outback}" = x"yes"; then
   echo "        Including Outback protocol"
fi

if test x"${build_xantrex}" = x"yes"; then
   echo "        Including Xantrex protocol"
fi

if test x"${build_ownet}" = x"yes"; then
   echo "        Including 1-wire protocol"
   if test x"${has_ownet}" != x"true"; then
      echo "        To use a 1-Wire device. install the ownet development package for your system"
   else
      echo "        Found ownet. will configure ownet support"
   fi
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
