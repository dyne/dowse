AC_PREREQ([2.65])
AC_INIT([dnscrypt-dowse],[1.7.0],[http://dowse.eu])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([dnscrypt-dowse.c])
AC_CONFIG_HEADER([config.h])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.9 dist-bzip2 tar-ustar gnu subdir-objects])
AM_MAINTAINER_MODE
AM_DEP_TRACK

AC_SUBST(VERSION)
ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

LX_CFLAGS=${CFLAGS-NONE}

dnl Switches.

AC_ARG_ENABLE(ssp,
[AS_HELP_STRING(--disable-ssp,Do not compile with -fstack-protector)],
[
  AS_IF([test "x$enableval" = "xno"], [
    enable_ssp="no"
  ], [
    enable_ssp="yes"
  ])
],
[
  enable_ssp="yes"
])

AC_ARG_ENABLE(pie,
[AS_HELP_STRING(--disable-pie,Do not produce position independent executables)],
 enable_pie=$enableval, enable_pie="maybe")

AS_CASE([$host_os], [mingw*], [enable_pie="no"])

AC_ARG_ENABLE(debug,
[AS_HELP_STRING(--enable-debug,For maintainers only - please do not use)],
[
  AS_IF([test "x$enableval" = "xyes"], [
    AS_IF([test "x$LX_CFLAGS" = "xNONE"], [
      nxflags=""
      for flag in `echo $CFLAGS`; do
        AS_CASE([$flag],
          [-O*], [ ],
          [-g*], [ ],
          [*], [AS_VAR_APPEND([nxflags], [" $flag"])])
      done
      CFLAGS="$nxflags -O0 -g3"
    ])
    CPPFLAGS="$CPPFLAGS -DDEBUG=1"
  ])
])

dnl Checks

AC_PROG_CC_C99
AC_USE_SYSTEM_EXTENSIONS

LT_INIT
AC_SUBST(LIBTOOL_DEPS)

AC_ARG_VAR([AR], [path to the ar utility])
AC_CHECK_TOOL([AR], [ar], [ar])


AS_IF([test -d /usr/local/include], [
  CPPFLAGS="$CPPFLAGS -I/usr/local/include"
])

AS_IF([test -d /usr/local/lib], [
  LDFLAGS="$LDFLAGS -L/usr/local/lib"
])

dnl Checks for header files.

AC_SYS_LARGEFILE
AC_CHECK_HEADERS([sys/cdefs.h sys/feature_tests.h])
AC_CHECK_HEADERS([execinfo.h paths.h pwd.h grp.h uuid/uuid.h])
AC_CHECK_HEADERS([sandbox.h])
AC_CHECK_HEADERS([ws2tcpip.h])

dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_INLINE
AC_C_BIGENDIAN
AC_C_RESTRICT

AC_CHECK_TYPE([in_port_t], , [AC_DEFINE(in_port_t, uint16_t,
[in_port_t type])],
[
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_WS2TCPIP_H
# include <ws2tcpip.h>
#else
# include <sys/socket.h>
# include <netinet/in.h>
#endif
])

AC_MSG_CHECKING([whether sin_len is defined])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_WS2TCPIP_H
# include <ws2tcpip.h>
#else
# include <sys/socket.h>
# include <netinet/in.h>
#endif
]], [[
do {
 struct sockaddr_in a;
 (void) a.sin_len;
} while(0)
]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_SIN_LEN,[1],[define if you have sin_len])
],[
  AC_MSG_RESULT(no)
])

AC_MSG_CHECKING([whether __ss_family is defined])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_WS2TCPIP_H
# include <ws2tcpip.h>
#else
# include <sys/socket.h>
# include <netinet/in.h>
#endif
]], [[
do {
 struct sockaddr_storage a;
 (void) a.__ss_family;
} while(0)
]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE___SS_FAMILY,[1],[define if you have __ss_family])
],[
  AC_MSG_RESULT(no)
])

AC_MSG_CHECKING([whether ss_len is defined])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_WS2TCPIP_H
# include <ws2tcpip.h>
#else
# include <sys/socket.h>
# include <netinet/in.h>
#endif
]], [[
do {
 struct sockaddr_storage a;
 (void) a.ss_len;
} while(0)
]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_SS_LEN,[1],[define if you have ss_len])
],[
  AC_MSG_RESULT(no)
])

AC_MSG_CHECKING([whether __ss_len is defined])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_WS2TCPIP_H
# include <ws2tcpip.h>
#else
# include <sys/socket.h>
# include <netinet/in.h>
#endif
]], [[
do {
 struct sockaddr_storage a;
 (void) a.__ss_len;
} while(0)
]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE___SS_LEN,[1],[define if you have __ss_len])
],[
  AC_MSG_RESULT(no)
])

dnl Checks for library functions.

AC_SEARCH_LIBS(pow, [m])
AC_SEARCH_LIBS(dlopen, [dl])
AC_SEARCH_LIBS(clock_gettime, [rt],
  [AC_DEFINE(HAVE_CLOCK_GETTIME,[1],[define if you have clock_gettime()])])

AC_SEARCH_LIBS(backtrace, [execinfo],
  [AC_DEFINE(HAVE_BACKTRACE,[1],[define if you have backtrace()])])

AC_SEARCH_LIBS(gethostbyname, [resolv nsl])
AC_SEARCH_LIBS(recvfrom, [socket])
AC_SEARCH_LIBS(kvm_open, [kvm])
AC_SEARCH_LIBS(sodium_hex2bin, [sodium], [ ], AC_ERROR([libsodium >= 0.7.0 not found]))

use_ldns=no
AC_CHECK_HEADER([ldns/ldns.h], [
  AC_CHECK_LIB(ldns, ldns_wire2pkt, [
    AC_DEFINE(USE_LDNS,[1],[define to use the ldns library])
    LDNS_LIBS=-lldns
    use_ldns=yes
  ])
], [ ],
[#include <sys/types.h>
 #include <stdlib.h>
 #include <stdint.h>
 #ifdef HAVE_WS2TCPIP_H
 # include <ws2tcpip.h>
 #endif
])
AM_CONDITIONAL(USE_LDNS, test x$use_ldns = xyes)
AC_SUBST([LDNS_LIBS])

AC_CHECK_FUNCS([getpwnam sandbox_init setrlimit putc_unlocked gmtime_r initgroups])

dnl Libtool.

dnl LT_INIT([dlopen])
dnl LT_CONFIG_LTDL_DIR([libltdl])
dnl LTDL_INIT

dnl Subconfig.

case $host_os in
  cygwin* | mingw* | pw32* | cegcc*) ;;
  *) ac_configure_args="$ac_configure_args --disable-thread-support" ;;
esac

ac_configure_args="$ac_configure_args CC='$CC'"
ac_configure_args="$ac_configure_args CFLAGS='$CFLAGS'"
ac_configure_args="$ac_configure_args CPP='$CPP'"
ac_configure_args="$ac_configure_args CPPFLAGS='$CPPFLAGS'"
ac_configure_args="$ac_configure_args LDFLAGS='$LDFLAGS'"
ac_configure_args="$ac_configure_args LIBS='$LIBS'"

dnl Output.

AH_VERBATIM([NDEBUG], [/* Never ever ignore assertions */
#ifdef NDEBUG
#/**/undef/**/ NDEBUG
#endif])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
