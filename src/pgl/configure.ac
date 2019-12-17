#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([pgl], [2.3.1], [http://sourceforge.net/p/peerguardian/bugs/])
AC_CONFIG_SRCDIR([pgld/src/pgld.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_PROG_AR
AM_INIT_AUTOMAKE([-Wall])
LT_INIT([disable-static])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h inttypes.h limits.h netinet/in.h stdlib.h string.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memchr memmove memset strdup strerror strstr])

# unset some vars
QT_SUB=

##
# let user specify INITDDIR
AC_ARG_WITH([initddir],
    [AS_HELP_STRING([--with-initddir=DIR],
        [path to init script directory (default: sysconfdir/init.d)])],
    [INITDDIR="$withval"],
    [INITDDIR='$(sysconfdir)/init.d'])

# let user specify LOGDIR
AC_ARG_WITH([logdir],
    [AS_HELP_STRING([--with-logdir=DIR],
        [path to log directory (default: localstatedir/log/pgl)])],
    [LOGDIR="$withval"],
    [LOGDIR='$(localstatedir)/log/'$PACKAGE])

# let user specify PIDDIR
AC_ARG_WITH([piddir],
    [AS_HELP_STRING([--with-piddir=DIR],
        [path to PID directory (default: localstatedir/run)])],
    [PIDDIR="$withval"],
    [PIDDIR='$(localstatedir)/run'])

# let user specify TMPDIR
AC_ARG_WITH([tmpdir],
    [AS_HELP_STRING([--with-tmpdir=DIR],
        [path to TMP directory (default: /tmp)])],
    [TMPDIR="$withval"],
    [TMPDIR="/tmp"])

# let user specify BLOCKLISTS_DIR
AC_ARG_WITH([blocklists],
    [AS_HELP_STRING([--with-blocklists=DIR],
        [path to blocklists directory (default: localstatedir/spool/pgl)])],
    [BLOCKLISTS_DIR="$withval"],
    [BLOCKLISTS_DIR='$(localstatedir)/spool/'$PACKAGE])

# let user specify LOCAL_BLOCKLIST_DIR
AC_ARG_WITH([localblocklist],
    [AS_HELP_STRING([--with-localblocklist=DIR],
        [path to local blocklist directory (default: sysconfdir/pgl/blocklists.local)])],
    [LOCAL_BLOCKLIST_DIR="$withval"],
    [LOCAL_BLOCKLIST_DIR='$(sysconfdir)'/$PACKAGE/blocklists.local])

# let user specify MASTER_BLOCKLIST_DIR
AC_ARG_WITH([masterblocklist],
    [AS_HELP_STRING([--with-masterblocklist=DIR],
        [path to master blocklist directory (default: localstatedir/lib/pgl)])],
    [MASTER_BLOCKLIST_DIR="$withval"],
    [MASTER_BLOCKLIST_DIR='$(localstatedir)/lib/'$PACKAGE])

# let user specify LSB
AC_ARG_WITH([lsb],
    [AS_HELP_STRING([--with-lsb=FILE],
        [path to lsb/init-functions file (default: none/built-in)])],
    [LSBFILE="$withval"],
    [LSBFILE=""])

# configure-switch for logrotate
AC_ARG_ENABLE([logrotate],
    [AS_HELP_STRING([--disable-logrotate],
        [disable logrotate support (default: enable)])],
    [enable_logrotate=$enableval],
    [enable_logrotate="yes"])

AM_CONDITIONAL(LOGROTATEMAKE, [test "x$enable_logrotate" = "xyes"])

# configure-switch for cron
AC_ARG_ENABLE([cron],
    [AS_HELP_STRING([--disable-cron],
        [disable support for cron services (default: enable)])],
    [enable_cron=$enableval],
    [enable_cron="yes"])

AM_CONDITIONAL(CRONMAKE, [test "x$enable_cron" = "xyes"])

# configure-switch for NetworkManager
AC_ARG_ENABLE([networkmanager],
    [AS_HELP_STRING([--disable-networkmanager],
        [disable support for NetworkManager (default: enable)])],
    [enable_networkmanager=$enableval],
    [enable_networkmanager="yes"])

AM_CONDITIONAL(NETWORKMANAGERMAKE, [test "x$enable_networkmanager" = "xyes"])

# use gentoo init script
AC_ARG_WITH([gentoo-init],
    [AS_HELP_STRING([--with-gentoo-init],
        [install a gentoo init script])],
    [gentoo_init="$withval"],
    [gentoo_init="no"])

AM_CONDITIONAL(GENTOOINIT, [test "x$gentoo_init" = "xyes"])

# use systemd service file (yes, if user specifies a path)
# http://www.freedesktop.org/software/systemd/man/daemon.html
AC_ARG_WITH([systemd],
        [AS_HELP_STRING([--with-systemd=UNITDIR], [install systemd unit file (into UNITDIR if given)])],
        [with_systemd="$withval"],
        [with_systemd="no"])
AS_IF([test "x$with_systemd" != xno],
	  [AS_IF([test "$with_systemd" = yes], [with_systemd="`$PKG_CONFIG --variable=systemdsystemunitdir systemd`"])]
	  [AC_SUBST([SYSTEMDUNITDIR], [$with_systemd])])
AM_CONDITIONAL(HAVE_SYSTEMD, [test -n "$with_systemd" -a "x$with_systemd" != xno ])

# let user specify iconsdir
AC_ARG_WITH([iconsdir],
    [AS_HELP_STRING([--with-iconsdir=DIR],
        [path where icons get installed (default: datadir/pixmaps)])],
    [ICONSDIR="$withval"],
    [ICONSDIR='$(datadir)/pixmaps'])




##
# initial PGLD_flags
# pkg-config module check, generates $1_LIBS and $1_CFLAGS vars
PKG_CHECK_MODULES([libnetfilterqueue],[libnetfilter_queue])
PGLD_CFLAGS=""
PGLD_CPPFLAGS="$libnetfilterqueue_CFLAGS -DVERSION=\\\"$VERSION\\\" -DPACKAGE_NAME=\\\"$PACKAGE\\\""
PGLD_LDFLAGS=""
PGLD_LIBS="$libnetfilterqueue_LIBS"

# configure-switch for zlib
AC_ARG_ENABLE([zlib],
    [AS_HELP_STRING([--disable-zlib],
        [disable support for zlib compressed blocklists (default: enable)])],
    [enable_zlib=$enableval],
    [enable_zlib="yes"])

# modify flags if zlib is enabled
AS_IF([test "x$enable_zlib" = "xyes"],
    [PKG_CHECK_MODULES([ZLIB],
        [zlib])]
    [PGLD_CPPFLAGS="$PGLD_CPPFLAGS $ZLIB_CFLAGS -DHAVE_ZLIB"]
    [PGLD_LIBS="$PGLD_LIBS $ZLIB_LIBS"])

# configure-switch for dbus
AC_ARG_ENABLE([dbus],
    [AS_HELP_STRING([--disable-dbus],
        [disable dbus support (default: enable)])],
    [enable_dbus=$enableval],
    [enable_dbus="yes"])

# check if dbus is enabled in general
AM_CONDITIONAL(DBUSMAKE, [test "x$enable_dbus" = "xyes"])

# modify PGLD_flags and LIBDBUS_flags if dbus is enabled
AS_IF([test "x$enable_dbus" = "xyes"],
    [PKG_CHECK_MODULES([DBUS],
        [dbus-1])]
    [PGLD_CFLAGS="$PGLD_CFLAGS -fPIC"]
    [PGLD_CPPFLAGS="$PGLD_CPPFLAGS $DBUS_CFLAGS -DHAVE_DBUS"]
    [PGLD_LDFLAGS="-Wl,-export-dynamic"]
    [PGLD_LIBS="$PGLD_LIBS $DBUS_LIBS -ldl"])

# configure-switch for lowmem
AC_ARG_ENABLE([lowmem],
    [AS_HELP_STRING([--enable-lowmem],
        [enable lowmem support (default: disable)])],
    [enable_lowmem=$enableval],
    [enable_lowmem="no"])

# modify PGLD_flags if lowmem is enabled
AS_IF([test "x$enable_lowmem" = "xyes"],
    [PGLD_CPPFLAGS="$PGLD_CPPFLAGS -DLOWMEM"])






##
# initial QT_flags
QT_CXXFLAGS=""
QT_CPPFLAGS="-DVERSION=\\\"$VERSION\\\" -D_REENTRANT -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED"
QT_LDFLAGS=""
QT_LIBS=""

# configure-switch for qt4
AC_ARG_WITH([qt4],
    [AS_HELP_STRING([--without-qt4],
        [build without gui])],
    [with_qt4="$withval"],
    [with_qt4="yes"]) # this only reflects the default ("if not given")

# modify QT4_flags if qt4 enabled and
# include pglgui as subdir in Makefile.am (QT_SUB)
AS_IF([test "x$with_qt4" = "xyes"],
    [AS_IF([test "x$enable_dbus" = "xyes"],
        [PKG_CHECK_MODULES([QT4DBUS], [QtDBus])],
        [AC_MSG_ERROR([qt4 needs dbus!])])]
    [AC_CHECK_QT4(["4.3.0"])] # from qt4.m4
    [PKG_CHECK_MODULES([QT4], [QtCore QtGui])]
    [AC_SEARCH_LIBS([pthread_create], [pthread])] # adds -lpthread to LIBS
    [QT_CPPFLAGS="$QT_CPPFLAGS $QT4_CFLAGS $QT4DBUS_CFLAGS"]
    [QT_LIBS="$QT_LIBS $QT4_LIBS $QT4DBUS_LIBS"]
    [QT_SUB="pglgui"])

# targets
AC_CONFIG_FILES([Makefile
    docs/Makefile
    pgld/Makefile
    pglcmd/Makefile
    pglgui/Makefile])

## finally substitute several variables
AC_SUBST([QT_SUB]) # ref in Makefile.am
AC_SUBST([INITDDIR])
AC_SUBST([PIDDIR])
AC_SUBST([LOGDIR])
AC_SUBST([TMPDIR])
AC_SUBST([BLOCKLISTS_DIR])
AC_SUBST([LOCAL_BLOCKLIST_DIR])
AC_SUBST([MASTER_BLOCKLIST_DIR])
AC_SUBST([LSBFILE])
AC_SUBST([ICONSDIR])
# PGLDF subst.
AC_SUBST([PGLD_CFLAGS])
AC_SUBST([PGLD_CPPFLAGS])
AC_SUBST([PGLD_LDFLAGS])
AC_SUBST([PGLD_LIBS])
# QT subst.
AC_SUBST([QT_CXXFLAGS])
AC_SUBST([QT_CPPFLAGS])
AC_SUBST([QT_LDFLAGS])
AC_SUBST([QT_LIBS])

AC_OUTPUT

# give some nice output
echo
echo
echo pgl.......................................... : Version $VERSION
echo
echo Prefix....................................... : $prefix
echo Exec_Prefix.................................. : $exec_prefix
echo Initd........................................ : $INITDDIR
echo Logdir....................................... : $LOGDIR
echo Piddir....................................... : $PIDDIR
echo Tmpdir....................................... : $TMPDIR
echo Blocklists................................... : $BLOCKLISTS_DIR
echo Localblocklist............................... : $LOCAL_BLOCKLIST_DIR
echo Masterblocklist.............................. : $MASTER_BLOCKLIST_DIR
echo "LSB (use built-in functions if empty)........ : $LSBFILE"
echo

if test "x$enable_logrotate" = "xyes"; then
echo logrotate.................................... : yes
else
echo logrotate.................................... : no
fi
if test "x$enable_cron" = "xyes"; then
echo cron......................................... : yes
else
echo cron......................................... : no
fi
if test "x$enable_networkmanager" = "xyes"; then
echo networkmanager............................... : yes
else
echo networkmanager............................... : no
fi
if test "x$enable_zlib" = "xyes"; then
echo zlib......................................... : yes
else
echo zlib......................................... : no
fi
if test "x$enable_dbus" = "xyes"; then
echo dbus......................................... : yes
else
echo dbus......................................... : no
fi
if test "x$enable_lowmem" = "xyes"; then
echo lowmem....................................... : yes
else
echo lowmem....................................... : no
fi
if test "x$with_qt4" = "xyes"; then
echo QT-gui....................................... : yes
else
echo QT-gui....................................... : no
fi
if test "x$gentoo_init" = "xyes"; then
echo Gentoo init script........................... : yes
else
echo Gentoo init script........................... : no
fi
if test -n "$with_systemd" -a "x$with_systemd" != xno; then
echo systemd ..................................... : yes
echo systemd unit dir ............................ : $with_systemd
else
echo systemd...................................... : no
fi
echo
echo .............................................
echo "Developer debug:"
echo "PGLD_CFLAGS: $PGLD_CFLAGS"
echo "PGLD_CPPFLAGS: $PGLD_CPPFLAGS"
echo "PGLD_LDFLAGS: $PGLD_LDFLAGS"
echo "PGLD_LIBS: $PGLD_LIBS"
echo "QT_CXXFLAGS: $QT_CXXFLAGS"
echo "QT_CPPFLAGS: $QT_CPPFLAGS"
echo "QT_LDFLAGS: $QT_LDFLAGS"
echo "QT_LIBS: $QT_LIBS"
echo .............................................
echo
echo Homepage http://peerguardian.sourceforge.net/
echo
echo
