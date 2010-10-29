#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([libsfta],[0.1],[ilengal@fit.vutbr.cz])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([include/sfta/sfta.hh])
AC_LANG([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_CPP

dnl *** includes ************************************************************
INCLUDES="-I../include"

dnl *** compiler characteristic *********************************************
CXXFLAGS=""
CXXFLAGS="$CXXFLAGS -std=c++98"
CXXFLAGS="$CXXFLAGS -pedantic-errors"
CXXFLAGS="$CXXFLAGS -Wextra"
CXXFLAGS="$CXXFLAGS -Wall"
CXXFLAGS="$CXXFLAGS -Wfloat-equal"
CXXFLAGS="$CXXFLAGS -Wctor-dtor-privacy"
CXXFLAGS="$CXXFLAGS -Weffc++"
CXXFLAGS="$CXXFLAGS -Wold-style-cast"
CXXFLAGS="$CXXFLAGS -Woverloaded-virtual"
CXXFLAGS="$CXXFLAGS -finline-functions"
CXXFLAGS="$CXXFLAGS -fdiagnostics-show-option"
CXXFLAGS="$CXXFLAGS -O2"
CXXFLAGS="$CXXFLAGS -march=core2"
CXXFLAGS="$CXXFLAGS -mtune=core2"
CXXFLAGS="$CXXFLAGS -msse2"
#CXXFLAGS="$CXXFLAGS -g"
#CXXFLAGS="$CXXFLAGS -pg"

LIBS=""
LIBS="$LIBS -lboost_unit_test_framework"
LIBS="$LIBS -llog4cpp"
LIBS="$LIBS -lloki"


dnl *** Add inlude directories to compiler flags ****************************
CXXFLAGS="$CXXFLAGS $INCLUDES"


# Checks for libraries.
# FIXME: Replace `main' with a function in `-lboost_unit_test_framework':
AC_CHECK_LIB([boost_unit_test_framework], [main],, AC_MSG_ERROR([Cannot find required libraries.]))
# FIXME: Replace `main' with a function in `-llog4cpp':
AC_CHECK_LIB([log4cpp], [main],, AC_MSG_ERROR([Cannot find required libraries.]))
# FIXME: Replace `main' with a function in `-lloki':
AC_CHECK_LIB([loki], [main],, AC_MSG_ERROR([Cannot find required libraries.]))
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main],, AC_MSG_ERROR([Cannot find required libraries.]))

# Checks for header files.
AC_CHECK_HEADERS([float.h limits.h stdlib.h string.h sys/file.h sys/time.h unistd.h])
AC_CHECK_HEADERS([cassert boost/algorithm/string.hpp log4cpp/Category.hh],,
                 AC_MSG_ERROR([Cannot find required header files.]))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_STRCOLL
AC_FUNC_WAIT3
AC_CHECK_FUNCS([bzero dup2 gethostname getpagesize gettimeofday memchr memmove memset pow sqrt strchr strcspn strerror strpbrk strrchr strspn strstr])

AC_CONFIG_FILES([Makefile
                 cudd_facade/Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
