dnl file://qt4.m4
dnl -----------------------------------------------------------------------
dnl Author: Juergen Heinemann http://www.hjcms.de, (C) 2007-2009         dnl
dnl                                                                      dnl
dnl Copyright (C) 2004 by Juergen Heinemann                              dnl
dnl                                     nospam __AT__ hjcms (DOT) de     dnl
dnl                                                                      dnl
dnl This program is free software; you can redistribute it and/or modify dnl
dnl it under the terms of the GNU General Public License as published by dnl
dnl the Free Software Foundation; either version 2 of the License, or    dnl
dnl (at your option) any later version.                                  dnl
dnl                                                                      dnl
dnl This program is distributed in the hope that it will be useful,      dnl
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of       dnl
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        dnl
dnl GNU General Public License for more details.                         dnl
dnl                                                                      dnl
dnl You should have received a copy of the GNU General Public License    dnl
dnl along with this program; if not, write to the                        dnl
dnl Free Software Foundation, Inc.,                                      dnl
dnl 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.            dnl
dnl ------------------------------------------------------------------------

dnl -----------------------------------------------
dnl Phonon >= 4.2* http://www.kde.org
dnl -----------------------------------------------

AC_DEFUN([AC_CHECK_PHONON],
[
AC_REQUIRE([AC_PROG_MAKE_SET]) dnl
AC_REQUIRE([PKG_PROG_PKG_CONFIG]) dnl

dnl -----------------------------------------------
dnl Minimal Version
dnl -----------------------------------------------
PHONON_REQUIRED_VERSION=m4_default([$1], [4.2.70])
AC_SUBST(PHONON_REQUIRED_VERSION) dnl

PKG_CHECK_MODULES([PHONON],[phonon >= $PHONON_REQUIRED_VERSION],
  AC_DEFINE(HAVE_PHONON,[1],[phonon exists]),
  AC_MSG_ERROR([phonon >= $QT_REQUIRED_VERSION not found!])
)

])

dnl -----------------------------------------------
dnl Qt >= 4.* and gettext Dependeces
dnl -----------------------------------------------
AC_DEFUN([FUN_TYPE_BOOL],
[
  AC_REQUIRE([AC_PROG_CXX]) dnl
  AC_LANG_PUSH(C++) dnl
  AC_CHECK_TYPE(bool, ac_check_bool=yes, ac_check_bool=no) 
  AC_LANG_POP(C++) dnl
  if test "x$ac_check_bool" = "xyes" ; then
    AC_DEFINE(HAVE_BOOL,[],[define if bool is a built-in type])
  fi

  AH_BOTTOM([#ifndef HAVE_BOOL])
  AH_BOTTOM([enum booltyp { false, true }; typedef enum booltyp bool;])
  AH_BOTTOM([#endif])
]) dnl FUN_TYPE_BOOL

dnl -----------------------------------------------
dnl Find Qt4 Program
dnl @example AC_CHECK_QT4_PROG([qhelpconverter])
dnl @generate QHELPCONVERTER = qhelpconverter
dnl -----------------------------------------------
m4_define([_QT_PROG_DEFUN],[m4_toupper([$1])])

AC_DEFUN([AC_CHECK_QT4_PROG],
[
  AC_REQUIRE([AC_ARG_PROGRAM]) dnl
  AC_CHECK_PROGS(_QT_PROG_DEFUN($1),[$1 $14 $1-qt4]) dnl
  test -n "$_QT_PROG_DEFUN($1)" || AC_MSG_WARN([can not find $1 for build subs])
])

dnl -----------------------------------------------
dnl Qt >= 4.* http://www.trolltech.com
dnl -----------------------------------------------

AC_DEFUN([AC_CHECK_QT4],
[
AC_REQUIRE([AC_PROG_AWK]) dnl
AC_REQUIRE([AC_PROG_MAKE_SET]) dnl
AC_REQUIRE([AC_ARG_PROGRAM]) dnl
AC_REQUIRE([PKG_PROG_PKG_CONFIG]) dnl
AC_REQUIRE([FUN_TYPE_BOOL]) dnl

dnl -----------------------------------------------
dnl Minimal Version
dnl -----------------------------------------------
QT_REQUIRED_VERSION=m4_default([$1], [4.4.0])
AC_SUBST(QT_REQUIRED_VERSION) dnl

dnl -----------------------------------------------
dnl Required pkg-config Files
dnl -----------------------------------------------
QT_REQUIRED_CONFIG=m4_default([$2], ["QtCore"])
AC_SUBST(QT_REQUIRED_CONFIG) dnl

dnl -----------------------------------------------
dnl error if fail Section
dnl -----------------------------------------------
PKG_CHECK_MODULES([QT],[$QT_REQUIRED_CONFIG >= $QT_REQUIRED_VERSION],
  AC_DEFINE(HAVE_QT,[1],[$QT_REQUIRED_CONFIG exists]),
  AC_MSG_ERROR([$QT_REQUIRED_CONFIG >= $QT_REQUIRED_VERSION not found!])
)

dnl -----------------------------------------------
dnl check for moc compiler
dnl -----------------------------------------------
AC_CHECK_PROGS([MOC],[moc moc4 moc-qt4])
if test ! -n "$MOC" ; then
  MOC="`$PKG_CONFIG --variable=moc_location QtCore`"
  test -x "$MOC" || AC_MSG_ERROR([Qt moc MOC::Compiler not found!])
  AC_SUBST([MOC]) dnl
fi

dnl -----------------------------------------------
dnl check for uic
dnl -----------------------------------------------
AC_CHECK_PROGS([UIC],[uic uic4 uic-qt4])
if test ! -n "$UIC" ; then
  UIC="`$PKG_CONFIG --variable=uic_location QtCore`"
  test -x "$UIC" ||  AC_MSG_ERROR([Qt uic UI::Compiler not found!])
  AC_SUBST([UIC]) dnl
fi

dnl -----------------------------------------------
dnl check for rcc
dnl -----------------------------------------------
AC_CHECK_PROGS([RCC],[rcc rcc4 rcc-qt4])
if test ! -n "$RCC" ; then
  RCC="rcc"
  test -x "$RCC" ||  AC_MSG_ERROR([Qt rcc QRC::Compiler not found!])
  AC_SUBST([RCC]) dnl
fi

dnl -----------------------------------------------
dnl check for lupdate
dnl -----------------------------------------------
AC_CHECK_PROGS([LUPDATE],[lupdate lupdate4 lupdate-qt4])
test -n "$LUPDATE" || AC_MSG_ERROR([Qt lupdate Compiler not found!])

dnl -----------------------------------------------
dnl check for lrelease
dnl -----------------------------------------------
AC_CHECK_PROGS([LRELEASE],[lrelease lrelease4 lrelease-qt4])
test -n "$LRELEASE" || AC_MSG_ERROR([Qt lrelease Compiler not found!])

dnl -----------------------------------------------
dnl check for qmake
dnl -----------------------------------------------
AC_CHECK_PROGS([QMAKE],[qmake qmake4 qmake-qt4])
test -n "$QMAKE" || AC_MSG_ERROR([No QT3 to QT4 Support on this System]) dnl

dnl -----------------------------------------------
dnl set include dir
dnl -----------------------------------------------
Q_INCLUDE_DIR="`$QMAKE -query QT_INSTALL_HEADERS`"
AC_SUBST(Q_INCLUDE_DIR) dnl

dnl -----------------------------------------------
dnl set include dir
dnl -----------------------------------------------
Q_LIB_DIR="`$QMAKE -query QT_INSTALL_LIBS`"
AC_SUBST(Q_LIB_DIR) dnl

dnl -----------------------------------------------
dnl set plugin dir
dnl -----------------------------------------------
Q_PLUGIN_DIR="`$QMAKE -query QT_INSTALL_PLUGINS`"
AC_SUBST(Q_PLUGIN_DIR) dnl

dnl -----------------------------------------------
dnl set tr locale dir
dnl -----------------------------------------------
Q_LOCALE_DIR="`$QMAKE -query QT_INSTALL_TRANSLATIONS`"
AC_SUBST(Q_LOCALE_DIR) dnl

dnl -----------------------------------------------
dnl set tr locale dir
dnl -----------------------------------------------
Q_DATA_DIR="`$QMAKE -query QT_INSTALL_DATA`"
AC_SUBST(Q_DATA_DIR) dnl

dnl -----------------------------------------------
dnl set mkspecs dir
dnl -----------------------------------------------
Q_MKSPEC_DIR="`$QMAKE -query QT_INSTALL_DATA`/mkspecs"
AC_SUBST(Q_MKSPEC_DIR) dnl

])

dnl -----------------------------------------------
dnl Find Qt4 Library with pkg-config
dnl @example AC_CHECK_QT4_PKG([QtXml])
dnl @generate QTXML_CFLAGS and QTXML_LIBS
dnl -----------------------------------------------
AC_DEFUN([AC_CHECK_QT4_PKG],
[
AC_REQUIRE([AC_CHECK_QT4]) dnl
PKG_CHECK_MODULES(_QT_PROG_DEFUN($1),[$1 >= $QT_REQUIRED_VERSION])
])

dnl -----------------------------------------------
dnl eof

