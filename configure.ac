#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/workspace.h])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lGL':
AC_CHECK_LIB([GL], [main])
# FIXME: Replace `main' with a function in `-lGLU':
AC_CHECK_LIB([GLU], [main])
# FIXME: Replace `main' with a function in `-lQtCore':
AC_CHECK_LIB([QtCore], [main])
# FIXME: Replace `main' with a function in `-lQtGui':
AC_CHECK_LIB([QtGui], [main])
# FIXME: Replace `main' with a function in `-lQtOpenGL':
AC_CHECK_LIB([QtOpenGL], [main])
# FIXME: Replace `main' with a function in `-lQtXml':
AC_CHECK_LIB([QtXml], [main])
# FIXME: Replace `main' with a function in `-lcfitsio':
AC_CHECK_LIB([cfitsio], [main])
# FIXME: Replace `main' with a function in `-lchealpix':
AC_CHECK_LIB([chealpix], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([floor mkdir pow sqrt])





















AC_MSG_CHECKING(QT4.7 directory)
QTDIR=xxx
AC_ARG_WITH([qt-dir],
             AC_HELP_STRING([--with-qt-dir=/path/to/Qt-4.7.0],
                            [to specify the path to the Qt-4.7.0 directory.]),
             [QTPATHS="$withval"],
             [QTPATHS="/usr/local/Trolltech/Qt-4.7.0 /c/Qt/4.7.0 /usr/include/qt4"])

for x in $QTPATHS; do
    if test -d $x ; then
       QTDIR="$x"
    fi
done
if test $QTDIR = xxx ; then
   AC_MSG_ERROR(Could not locate QT 4.7)
fi
AC_MSG_RESULT($QTDIR)


AC_PATH_PROG(RCC, rcc, rcc)
AC_PATH_PROGS(UIC, [uic-qt4 uic], uic)
AC_PATH_PROGS(MOC, [moc-qt4 moc], moc)


#QT_MOC="$QTDIR/bin/moc"
#AC_MSG_CHECKING(for $QT_MOC)
	#if test ! -x "$QT_MOC"; then
#		AC_MSG_ERROR("Unable to find moc")
	#fi
#AC_SUBST(QT_MOC)


host=`uname -a` # AC_CANONICAL_HOST is broken at the time of this writing.
case $host in
  MINGW32*)
    AC_MSG_NOTICE(MinGW detected.)
    QTLIBS="-L$QTDIR/lib -lopengl32 -lglu32 -lgdi32 -luser32 -lmingw32 -lqtmain -lQtOpenGL4 -lQtGui4 -lQtCore4 -mthreads -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc -Wl,-s -Wl,-s -Wl,-subsystem,windows"
    QTINC="-I$QTDIR/include -I$QTDIR/include/QtCore -I$QTDIR/include/QtGui -I$QTDIR/include/QtOpenGL -I$QTDIR/include/QtXml -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT -DQT_NEEDS_QMAIN -frtti -fexceptions"
    QTBIN="$QTDIR/bin"
    ;;
  *)
    AC_MSG_NOTICE(I'm assuming this is Linux)
    AC_PATH_XTRA
    QTLIBS="-Wl,-rpath,$QTDIR/lib -L$QTDIR/lib -lQtGui -lQtOpenGL -lQtCore -lqglviewer-qt4 -lQtXml -lchealpix -lm -lcfitsio $X_LIBS -lX11 $X_EXTRA_LIBS -lGLU -lGL -lpthread"
    QTINC="-I$QTDIR -I$QTDIR/QtGui -I$QTDIR/QtCore -I$QTDIR/QtOpenGL -I$QTDIR/QtXml $X_CFLAGS -DQT_XML_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED"
    QTBIN="$QTDIR/bin"
    ;;
esac

LIBS="$LIBS $QTLIBS"
INCLUDES="$INCLUDES $QTINC"
PATH="$PATH:$QTBIN"
CXXFLAGS="$CXXFLAGS $QTINC"


#AC_MSG_NOTICE(Configuring MOC)
#AC_PATH_PROGS(MOC, [moc-qt4 moc], moc)
#AC_SUBST(MOC)


AC_MSG_NOTICE($MOC)

AC_SUBST(MOC)
AC_SUBST(RCC)
AC_SUBST(UIC)


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
