#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT(xgbox, 0.1.0, win@loracc.com)
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([source/xgbox.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([memset select strstr])

AC_CONFIG_FILES([3rd/log/Makefile
                 Makefile
                 source/Makefile
                 source/at/Makefile
                 source/sms/Makefile
                 source/tty/Makefile])
AC_OUTPUT
