# Configure paths for snmp

AC_DEFUN([AM_PATH_SNMP],
[dnl 
dnl Get the cflags and libraries
dnl
dnl Use the Snmp library, if specified.
snmp=false
AC_ARG_ENABLE(snmp, [  --enable-snmp           enable support for Snmp (default=yes)],
[case "${enableval}" in
  yes) snmp=true ;;
  no)  snmp=false ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac])

AM_CONDITIONAL(SNMP, test x$snmp = xtrue)

if test x$snmp = xtrue; then
    AC_DEFINE([USE_SNMP], [], [Use SNMP])

AC_ARG_WITH(snmp,[  --with-snmp=PFX         prefix where net-snmp is installed], snmp_prefix="$withval", snmp_prefix="")
AC_ARG_WITH(snmp-libraries,[  --with-snmp-libraries=DIR   directory where snmp library is installed (optional)], snmp_libraries="$withval", snmp_libraries="")
AC_ARG_WITH(snmp-includes,[  --with-snmp-includes=DIR    directory where snmp header files are installed (optional)], snmp_includes="$withval", snmp_includes="")
dnl AC_ARG_ENABLE(snmptest, [  --disable-snmptest       Do not try to compile and run a test snmp program],, enable_snmptest=yes)

  if test "x$snmp_libraries" != "x" ; then
    SNMP_LIBS="-L$snmp_libraries"
  elif test "x$snmp_prefix" != "x" ; then
    SNMP_LIBS="-L$snmp_prefix/lib"
  elif test "x$prefix" != "xNONE" ; then
    SNMP_LIBS="-L$libdir"
  fi

dnl   if test "x$snmp_includes" != "x" ; then
dnl     SNMP_CFLAGS="-I$snmp_includes"
dnl   elif test "x$snmp_prefix" != "x" ; then
dnl     SNMP_CFLAGS="-I$snmp_prefix/include"
dnl   elif test "$prefix" != "xNONE"; then
dnl     SNMP_CFLAGS="-I$prefix/include"
dnl   fi

  AC_MSG_CHECKING(for NET-SNMP headers and flags)
  no_snmp=""

  if test "x$SNMP_CFLAGS" = "x" ; then
    dnl The -Wstrict-prototypes flags is for C/ObjC code only, and
    dnl this project is all written in C++.
    SNMP_CFLAGS=`net-snmp-config --cflags | sed -e 's: -Wstrict-prototypes::' `
    AC_MSG_RESULT($SNMP_CFLAGS)
  fi

  AC_MSG_CHECKING(for NET-SNMP library)
  if test "x$SNMP_LIBS" = "x" ; then
    SNMP_LIBS=`net-snmp-config --agent-libs`
    AC_MSG_RESULT($SNMP_LIBS)
  fi


AC_DEFINE(HAVE_SNMP, 1, [NET-SNMP package])
AC_SUBST(SNMP_CFLAGS)
AC_SUBST(SNMP_LIBS)
fi
])
