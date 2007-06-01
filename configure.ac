AC_INIT([libmtag], [0.1.0], [felipe.contreras@gmail.com])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile lib/Makefile tests/Makefile src/Makefile libmtag.pc])

AC_PROG_CC
AC_PROG_LIBTOOL

PKG_CHECK_MODULES([TAGLIB], [taglib >= 1.4])

AC_OUTPUT
