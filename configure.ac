#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([SkyViewer], [0.1], [diogopinheiro@ua.pt])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/workspace.h])
#AC_CONFIG_HEADERS([src/configs.h])
AC_LANG([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([m], [main])
AC_CHECK_LIB([pthread], [pthread_create])

AC_HEADER_STDBOOL
AC_C_INLINE

AC_CHECK_FUNCS([floor mkdir pow sqrt])

AC_CHECK_LIB([GL], [main])
AC_CHECK_HEADERS([GL/glu.h],[AC_DEFINE(HAVE_GLU_H)],[AC_MSG_ERROR("Could not find GLU headers")])
AC_CHECK_LIB([GLU],[main],[AC_DEFINE(HAVE_GLU)],[AC_MSG_ERROR("Could not find GLU")])

#QT 
AC_MSG_CHECKING(QT4.7 directory)
QTDIR=xxx
QTINC_PREFIX=""
AC_ARG_WITH([qt-dir],
             AC_HELP_STRING([--with-qt-dir=/path/to/Qt-4.7.0],
                            [to specify the path to the Qt-4.7.0 directory.]),
             [QTPATHS="$withval"],
             [QTPATHS="/usr/local/Trolltech/Qt-4.7.0 /c/Qt/4.7.0 /usr /usr/local"])

for x in $QTPATHS; do
    if test -d "$x/include/qt4/QtCore" ; then
       QTDIR="$x"
		QTINC_PREFIX="qt4"
	else
    	if test -d "$x/include/qt/QtCore" ; then
       	QTDIR="$x"
		QTINC_PREFIX="qt"
		else
    		if test -d "$x/include/QtCore" ; then
       		QTDIR="$x"
			fi
		fi
    fi
done
if test $QTDIR = xxx ; then
   AC_MSG_ERROR(Could not locate QT 4.7)
fi

AC_MSG_RESULT($QTDIR)
host=`uname -a` # AC_CANONICAL_HOST is broken at the time of this writing.
case $host in
  MINGW32*)
    AC_MSG_NOTICE(MinGW detected.)
    QTLIBS="-L$QTDIR/lib -lopengl32 -lglu32 -lgdi32 -luser32 -lmingw32 -lqtmain -lQtOpenGL4 -lQtGui4 -lQtCore4 -mthreads -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc -Wl,-s -Wl,-s -Wl,-subsystem,windows"
    QTINC="-I$QTDIR/include/$QTINC_PREFIX -I$QTDIR/include/$QTINC_PREFIX/QtCore -I$QTDIR/include/$QTINC_PREFIX/QtGui -I$QTDIR/include/$QTINC_PREFIX/QtOpenGL -I$QTDIR/include/$QTINC_PREFIX/QtXml -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT -DQT_NEEDS_QMAIN -frtti -fexceptions"
    QTBIN="$QTDIR/bin"
    ;;
  *)
    AC_MSG_NOTICE(I am assuming this is Linux)
    AC_PATH_XTRA
    QTLIBS="-Wl,-rpath,$QTDIR/lib -L$QTDIR/lib -lQtGui -lQtOpenGL -lQtCore $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS -lGLU -lGL -lpthread"
    QTINC="-I$QTDIR/include/$QTINC_PREFIX -I$QTDIR/include/$QTINC_PREFIX/QtGui -I$QTDIR/include/$QTINC_PREFIX/QtCore -I$QTDIR/include/$QTINC_PREFIX/QtOpenGL -I$QTDIR/include/$QTINC_PREFIX/QtXml $X_CFLAGS -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED"
    QTBIN="$QTDIR/bin"
    ;;
esac

LIBS="$LIBS $QTLIBS"
INCLUDES="$INCLUDES $QTINC"
PATH="$PATH:$QTBIN"
CXXFLAGS="$CXXFLAGS $QTINC"
CPPFLAGS="$CPPFLAGS $QTINC"


AC_CHECK_HEADERS([QtCore/QtCore],[AC_DEFINE(HAVE_QTCORE_H)],[AC_MSG_ERROR("Could not find QtCore headers")])
AC_CHECK_LIB([QtCore],[main],[AC_DEFINE(HAVE_QTCORE)],[AC_MSG_ERROR("Could not find QtCore")])
AC_CHECK_HEADERS([QtGui/QtGui],[AC_DEFINE(HAVE_QTCORE_H)],[AC_MSG_ERROR("Could not find QtGui headers")])
AC_CHECK_LIB([QtGui],[main],[AC_DEFINE(HAVE_QTCORE)],[AC_MSG_ERROR("Could not find QtGui")])
AC_CHECK_HEADERS([QtOpenGL/QtOpenGL],[AC_DEFINE(HAVE_QTOPENGL_H)],[AC_MSG_ERROR("Could not find QtOpenGL headers")])
AC_CHECK_LIB([QtOpenGL],[main],[AC_DEFINE(HAVE_QTOPENGL)],[AC_MSG_ERROR("Could not find QtOpenGL")])
AC_CHECK_HEADERS([QtXml/QtXml],[AC_DEFINE(HAVE_QTXML_H)],[AC_MSG_ERROR("Could not find QtXml headers")])
AC_CHECK_LIB([QtXml],[main],[AC_DEFINE(HAVE_QTXML)],[AC_MSG_ERROR("Could not find QtXml")])


AC_MSG_CHECKING(does moc work?)
bnv_try_1="moc myqt.h -o moc_myqt.cpp"
AC_TRY_EVAL(bnv_try_1)
if test x"$ac_status" != x0; then
   AC_MSG_ERROR(moc doesn't work)
fi
AC_MSG_RESULT(yes)
AC_MSG_CHECKING(can I compile moc_myqt.cpp?)
bnv_try_2="$CXX -c $CXXFLAGS -o moc_myqt.o moc_myqt.cpp"
AC_TRY_EVAL(bnv_try_2)
if test x"$ac_status" != x0; then
   AC_MSG_ERROR(couldn't compile moc_myqt.cpp)
fi
AC_MSG_RESULT(yes)


MOC="$QTDIR/bin/moc"
AC_MSG_CHECKING(for $MOC)
	if test ! -x "$MOC"; then
		AC_MSG_ERROR("Unable to find moc")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(MOC)

#Fitsio
CFITSIO_DIR=xxx
AC_ARG_WITH([cfitsio-dir],
             AC_HELP_STRING([--with-cfitsio-dir=/path/to/cfitsio],
                            [to specify the path to the CFITSIO directory.]),
             [CFITSIO_PATHS="$withval"],
             [CFITSIO_PATHS="deps/cfitsio"])

for x in $CFITSIO_PATHS; do
    if test -f "$x/include/fitsio.h" ; then
       CFITSIO_DIR="$x"
    fi
done

  
if ! test $CFITSIO_DIR = xxx ; then
  LIBS="$LIBS -L$CFITSIO_DIR/lib"
  INCLUDES="$INCLUDES -I$CFITSIO_DIR/include"
  CXXFLAGS="$CXXFLAGS -I$CFITSIO_DIR/include"
  CPPFLAGS="$CPPFLAGS -I$CFITSIO_DIR/include"
fi

AC_CHECK_HEADERS([fitsio.h],[AC_DEFINE(HAVE_FITSIO_H)],[AC_MSG_ERROR("Could not find fitsio.h from the cfitsio package")])
AC_CHECK_LIB([cfitsio], [fits_init_cfitsio],[AC_DEFINE(HAVE_FITSIO)],[AC_MSG_ERROR("Could not link against cfitsio")])

LIBS="$LIBS -lcfitsio"
#------------------------------------

# Check Healpix
HEALPIX_DIR=xxx
AC_ARG_WITH([healpix-dir],
             AC_HELP_STRING([--with-healpix-dir=/path/to/Healpix],
                            [to specify the path to the Healpix directory.]),
             [HEALPIX_PATHS="$withval"],
             [HEALPIX_PATHS=""])

for x in $HEALPIX_PATHS; do
    if test -d $x ; then
       HEALPIX_DIR="$x"
    fi
done

if ! test $HEALPIX_DIR = xxx ; then
  LIBS="$LIBS -L$HEALPIX_DIR/lib"
  INCLUDES="$INCLUDES -I$HEALPIX_DIR/include"
  CXXFLAGS="$CXXFLAGS -I$HEALPIX_DIR/include"
  CPPFLAGS="$CPPFLAGS -I$HEALPIX_DIR/include"
fi


AC_CHECK_HEADERS([chealpix.h],[AC_DEFINE(HAVE_CHEALPIX_H)],[AC_MSG_ERROR("Could not find chealpix.h from the Healpix package")])
AC_CHECK_LIB([chealpix], [read_healpix_map],[AC_DEFINE(HAVE_HEALPIX)],[AC_MSG_ERROR("Could not link against Healpix")])

LIBS="$LIBS -lchealpix"

#--------------------------------------

# Check QGLViewer
QGLVIEWER_DIR=xxx
AC_ARG_WITH([qglviewer-dir],
             AC_HELP_STRING([--with-qglviewer-dir=/path/to/QGLViewer],
                            [to specify the path to the Healpix directory.]),
             [QGLVIEWER_PATHS="$withval"],
             [QGLVIEWER_PATHS=""])

for x in $QGLVIEWER_PATHS; do
    if test -d $x ; then
       QGLVIEWER_DIR="$x"
    fi
done

if ! test $QGLVIEWER_DIR = xxx ; then
  LIBS="$LIBS -L$QGLVIEWER_DIR/lib"
  INCLUDES="$INCLUDES -I$QGLVIEWER_DIR/include"
  CXXFLAGS="$CXXFLAGS -I$QGLVIEWER_DIR/include"
  CPPFLAGS="$CPPFLAGS -I$QGLVIEWER_DIR/include"
fi

AC_CHECK_HEADERS([QGLViewer/qglviewer.h],[AC_DEFINE(HAVE_QGLVIEWER_H)],[AC_MSG_ERROR("Could not find qglviewer.h from the QGLViewer package")])
AC_CHECK_LIB([qglviewer-qt4], [main],[AC_DEFINE(HAVE_QGLVIEWER)],[AC_MSG_ERROR("Could not link against QGLViewer")])

LIBS="$LIBS -lqglviewer-qt4"

#Final processing
#AC_SUBST(MOC)
#AC_SUBST(RCC)
#AC_SUBST(UIC)

AC_SUBST(CXXFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(PATH)
AC_SUBST(LIBS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
