AC_PREREQ([2.57])
AC_INIT([buddy], [2.3a])
AC_CONFIG_AUX_DIR([tools])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign nostdinc no-define 1.7.3])

AC_PROG_CC

AM_PROG_LEX
AC_PROG_YACC

AC_PROG_CXX

LT_INIT([win32-dll])

buddy_INTEL
buddy_DEBUG_FLAGS

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  doc/Makefile
  examples/Makefile
  examples/adder/Makefile
  examples/bddcalc/Makefile
  examples/bddtest/Makefile
  examples/cmilner/Makefile
  examples/fdd/Makefile
  examples/milner/Makefile
  examples/money/Makefile
  examples/queen/Makefile
  examples/solitare/Makefile
])

AC_OUTPUT
