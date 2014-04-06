#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libaeondb], [0.1])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/common.h])
AM_CONFIG_HEADER(config.h)

AC_SUBST([LIBAEONDB_SO_VERSION], [0:0:0])
AC_SUBST([LIBAEONDB_API_VERSION], [0.1])

LT_INIT

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
