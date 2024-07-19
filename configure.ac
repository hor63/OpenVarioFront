#    This file is part of OpenVarioFront, an electronic variometer for glider planes
#    Copyright (C) 2016  Kai Horstmann
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([OpenVarioFront],[0.1],[https://github.com/hor63/OpenVarioFront/issues],[OpenVarioFront],https://github.com/hor63/OpenVarioFront)
AC_CONFIG_HEADERS(config.h)

AC_CONFIG_MACRO_DIRS([m4])


AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE()
AM_DEFAULT_VERBOSITY=0


AC_PROG_CXX
AX_PROG_CXX_FOR_BUILD
AX_CXX_COMPILE_STDCXX_17(ext, mandatory)
AX_PTHREAD()


# get a suitable command like mkdir -p on LINUX systems use install if mkdir -p does not work.
AC_PROG_INSTALL
AC_PROG_MKDIR_P

# commands for the archiver
AM_PROG_AR
# obsoleted by LT_INIT
# AC_PROG_RANLIB

LT_INIT(win32-dll dlopen )

AC_SUBST([LIBTOOL_DEPS])

AC_LANG([C++])


AC_CHECK_HEADER([argp.h],[AC_DEFINE([HAVE_ARGP_H],[1],[Define to 1 if you have the <argp.h> header file.])],[])
AC_CHECK_HEADER([getopt.h],[AC_DEFINE([HAVE_GETOPT_H],[1],[Define to 1 if you have the <getopt.h> header file.])],[])
AC_SEARCH_LIBS([argp_parse],[],[AC_DEFINE([HAVE_ARGP_PARSE],[1],[Define to 1 if the function argp_parse is available.])],[])
AC_SEARCH_LIBS([getopt_long],[],[AC_DEFINE([HAVE_GETOPT_LONG],[1],[Define to 1 if the function getopt_long is available.])],[])
AC_SEARCH_LIBS([getopt],[],[AC_DEFINE([HAVE_GETOPT],[1],[Define to 1 if the function getopt is available.])],[])

AX_CHECK_COMPILE_FLAG([-fvisibility=internal],[DLL_VISIBLE_CFLAGS="-fvisibility=internal"])
AC_SUBST([DLL_VISIBLE_CFLAGS])

AX_CHECK_LINK_FLAG([-Wl,--no-undefined], [LD_NO_UNDEFINED_OPT=-no-undefined])
AC_SUBST([LD_NO_UNDEFINED_OPT])

# Check for log4cxx
# options are
# --with-log4cxx=no			Do not include logging support in the program
# --without-log4cxx 		Same as --with-log4cxx=no
# --with-log4cxx=<pathname> Use provided log4cxx package in directory <pathname>, e.g. /usr/local
AC_ARG_WITH([log4cxx],
  [AS_HELP_STRING([--with-log4cxx],
[support logging with Apache loc4cxx @<:@default=auto@:>@
[Possible values are: | ]
[<pathname>	: Path to log4cxx installation | ] 
no			: build completely without logging | 
yes (or empty): use the provided log4cxx library. Fatal error if it cannot be used. | 
auto		: (default) when an installation is detected it is used])],
[],
[])


# By default use the logger
HAVE_LOG4CXX_H=1

# Check for options given
AS_IF(
[test "x$with_log4cxx" = xauto -o "x$with_log4cxx" = x ],
[
	LOG4CXX_PREFIX_LIST="none /opt/log4cxx /opt /usr/local/log4cxx /usr/local"
],

[test "x$with_log4cxx" = xno ],
[
	AS_UNSET([HAVE_LOG4CXX_H])
],

[test "x$with_log4cxx" = xyes ],
[
	# Look only in the system directories for includes and lib
	LOG4CXX_PREFIX_LIST="none"
],

[
	# this must be the prefix to the installation of log4cxx
	LOG4CXX_PREFIX_LIST="$with_log4cxx"
]
)

# Save LIBS for link test
logger_test_save_LIBS="$LIBS"
logger_test_save_LDFLAGS="$LDFLAGS"
logger_test_save_CPPFLAGS="$CPPFLAGS"
LIBS="-llog4cxx $LIBS"

if test "x$LOG4CXX_PREFIX_LIST" != x
then
    # run through the list of potential installation prefixes
	for i in $LOG4CXX_PREFIX_LIST
	do
		if test x"$i" != xnone
		then
			LDFLAGS="$LDGLAGS -L$i/lib"
			CPPFLAGS="$CPPFLAGS -isystem $i/include"
			AC_MSG_CHECKING([log4cxx header and library usability in $i])
		else
			AC_MSG_CHECKING([log4cxx header and library usability])
		fi
		AC_LINK_IFELSE([AC_LANG_PROGRAM(
		[
		#include <log4cxx/logger.h>
		],
		[
		log4cxx::LoggerPtr  logger(log4cxx::Logger::getLogger("com.foo"));
		])],
		[
		HAVE_LOG4CXX_H=1
		AC_MSG_RESULT([yes])
		LOG4CXX_LIBS=-llog4cxx
		if test x"$i" != xnone
		then
			LOG4CXX_LDFLAGS=-L$i/lib
			LOG4CXX_CXXFLAGS="-isystem $i/include"
		fi
		break
		]
		,
		[
		AS_UNSET([HAVE_LOG4CXX_H])
		AC_MSG_RESULT([no])]
		)
		LDFLAGS="$logger_test_save_LDFLAGS"
	done # for i in $LOG4CXX_PREFIX_LIST
fi # if test "x$LOG4CXX_PREFIX_LIST" != x

# restore LIBS after test 
LIBS="$logger_test_save_LIBS"
LDFLAGS="$logger_test_save_LDFLAGS"
CPPFLAGS="$logger_test_save_CPPFLAGS"


if test x"$HAVE_LOG4CXX_H" = x1
then
	AC_DEFINE([HAVE_LOG4CXX_H], [1], [Determine if the log4cxx header is present and usable,
		and if log4cxx is to be used at all])
fi

AC_SUBST([LOG4CXX_LIBS])
AC_SUBST([LOG4CXX_LDFLAGS])
AC_SUBST([LOG4CXX_CXXFLAGS])


# Check if you have the Mali FBDEV headers and lib installed.
AC_CHECK_LIB([Mali], [eglGetError], [
		AC_DEFINE([HAVE_MALI_FBDEV], [1], [Defined if the Mali FBDEV EGL implementation is found])
		EGL_SYS_DIR=Mali_fbdev
		AC_SUBST([EGL_SYS_DIR])
	], [
		EGL_SYS_DIR=X11
		AC_SUBST([EGL_SYS_DIR])
		EGL_SYS_LIBS=-lX11
		AC_SUBST([EGL_SYS_LIBS])
		AC_DEFINE([USE_X11],[1],[Define to 1 if EGL based on X11 windows is being used.])
	])


# Obtain complile and link flags for Freetype 2 (Font rendering lib) and libpng
# Both are mandatory. Therefore leave the default action when not found: Error and stop
PKG_CHECK_MODULES([FREETYPE2], [freetype2])
AC_SUBST([FREETYPE2_CFLAGS])
AC_SUBST([FREETYPE2_LIBS])

PKG_CHECK_MODULES([LIBPNG], [libpng])
AC_SUBST([LIBPNG_CFLAGS])
AC_SUBST([LIBPNG_LIBS])

PKG_CHECK_MODULES([SDL2], [sdl2])
AC_SUBST([SDL2_CFLAGS])
AC_SUBST([SDL2_LIBS])

PKG_CHECK_MODULES([PANGO], [pango])
AC_SUBST([PANGO_CFLAGS])
AC_SUBST([PANGO_LIBS])

PKG_CHECK_MODULES([PANGO_FT2], [pangoft2])
AC_SUBST([PANGO_FT2_CFLAGS])
AC_SUBST([PANGO_FT2_LIBS])

DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(OpenVarioFront, $(srcdir)/doxygen.cfg, doc/doxygen)

AC_CONFIG_FILES(Makefile
                src/Makefile
                src/GLES/Makefile
                src/GLES/TexHelper/Makefile
                src/GLPrograms/Makefile
                src/Renderers/Makefile
                )

AC_OUTPUT

# create the doc/doxygen directory right away. If it does not exist the doxygen rules will not work :(
AS_ECHO(["configure: creating directory doc/doxygen"]) 
AS_MKDIR_P(doc/doxygen)
