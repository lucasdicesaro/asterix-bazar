dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(asterix_bazar, 0.1)

AM_INIT_AUTOMAKE(asterix_bazar, 0.1)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_RANLIB=libasterix_bazar.a










PKG_CHECK_MODULES(PACKAGE, libxml-2.0 libxml++-2.6 glibmm-2.4)
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

AC_OUTPUT([
Makefile
common/Makefile
src/Makefile
])
