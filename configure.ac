AC_INIT([libmtag], m4_esyscmd([build-aux/version-gen]),
        [felipe.contreras@gmail.com])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

AC_CONFIG_MACRO_DIR([m4])

PKG_CHECK_MODULES([TAGLIB], [taglib >= 1.4])

AC_CONFIG_FILES([Makefile \
                 lib/Makefile \
                 tests/Makefile \
                 src/Makefile \
                 libmtag.pc])

AC_OUTPUT
