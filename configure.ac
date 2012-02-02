#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.6])
AC_INIT([HealpixViewer], [0.1], [diogopinheiro@ua.pt])
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

SYSTEM=`uname`

GLPATHS="/usr/X11"
GLDIR=xxx
for x in $GLPATHS; do
	if test -d $x/include/GL; then
		GLDIR="$x"
	fi
done

if ! test $GLDIR = xxx; then
  LIBS="$LIBS -L$GLDIR/lib"
  INCLUDES="$INCLUDES -I$GLDIR/include"
  CXXFLAGS="$CXXFLAGS -I$GLDIR/include"
  CPPFLAGS="$CPPFLAGS -I$GLDIR/include"
  AC_MSG_NOTICE("Using GL at $GLDIR")
fi

if ! test $SYSTEM = "Darwin"; then
	AC_CHECK_LIB([GL], [main])
	AC_CHECK_HEADERS([GL/glu.h],[AC_DEFINE(HAVE_GLU_H)],[AC_MSG_ERROR("Could not find GLU headers")])
	AC_CHECK_LIB([GLU],[main],[AC_DEFINE(HAVE_GLU)],[AC_MSG_ERROR("Could not find GLU")])
fi


#QT
AC_MSG_CHECKING(QT4.7 directory)
QTDIR=xxx
QTINC_PREFIX=""
AC_ARG_WITH([qt-dir],
             AC_HELP_STRING([--with-qt-dir=/path/to/Qt-4.7.0],
                            [to specify the path to the Qt-4.7.0 directory.]),
             [QTPATHS="$withval"],
             [QTPATHS="/usr/local/Trolltech/Qt-4.7.0 /c/Qt/4.7.0 /usr /usr/local /opt/QtSDK/Desktop/Qt/474/gcc/"])

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


MOC="$QTDIR/bin/moc"
AC_MSG_CHECKING(for $MOC)
	if test ! -x "$MOC"; then
		AC_MSG_ERROR("Unable to find moc")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(MOC)

RCC="$QTDIR/bin/rcc"
AC_MSG_CHECKING(for $RCC)
	if test ! -x "$RCC"; then
		AC_MSG_ERROR("Unable to find rcc")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(RCC)

UIC="$QTDIR/bin/uic"
AC_MSG_CHECKING(for $UIC)
	if test ! -x "$UIC"; then
		AC_MSG_ERROR("Unable to find uic")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(UIC)

QMAKE="$QTDIR/bin/qmake"
AC_MSG_CHECKING(for $QMAKE)
	if test ! -x "$QMAKE"; then
		AC_MSG_ERROR("Unable to find qmake")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(QMAKE)

MACDEPLOYQT="$QTDIR/bin/macdeployqt"
AC_MSG_CHECKING(for $MACDEPLOYQT)
	if test ! -x "$MACDEPLOYQT"; then
		AC_MSG_ERROR("Unable to find macdeployqt")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(MACDEPLOYQT)


TMPDIR=tmp
tmp=`(umask 077 && mktemp -d -q "$TMPDIR/test-vim-qt-XXXXXXXX") 2>/dev/null`;
if test -z "$tmp" -a -d "$tmp"; then
	AC_MSG_ERROR(Unable to create temporary directory)
fi

cat > $tmp/qttest.cpp << EOF
#include <Qt/QtGui>
int main(int ac, char **av) {
    QApplication app(ac, av);
    return app.exec();
}
EOF



# Generate qmake project
qmake_try=`cd $tmp; $QMAKE -project -o test.pro`
AC_TRY_EVAL(qt_try)
if test x"$ac_status" != x0; then
	AC_MSG_ERROR(qmake failed)
fi

# add options
echo "QT += xml gui opengl core " >> $tmp/test.pro

# generate Makefile
qmake_try=`cd $tmp; $QMAKE`
AC_TRY_EVAL(qt_try)
if test x"$ac_status" != x0; then
	AC_MSG_ERROR(qmake failed)
fi

# LIBS
QT_LIBS=`grep LIBS.*= $tmp/Makefile | sed -e 's/\\$(SUBLIBS)//g' | awk '{sub(/^LIBS +=/, ""); print}'`
if test -z "$QT_LIBS"; then
	AC_MSG_ERROR(Unable to find Qt libraries)
fi
AC_MSG_NOTICE($QT_LIBS)

# Include directives
QT_INCPATH=`grep INCPATH.*= $tmp/Makefile | awk '{sub(/^INCPATH +=/, ""); print}'`
if test -z "$QT_LIBS"; then
    AC_MSG_ERROR(Unable to find Qt include path)
fi
AC_MSG_NOTICE($QT_INCPATH)

# LFLAGS
QT_LFLAGS=`grep LFLAGS.*= $tmp/Makefile | awk '{sub(/^LFLAGS +=/, ""); print}'`
if test -z "$QT_LFLAGS"; then
	AC_MSG_ERROR(Unable to find Qt link flags)
fi

AC_MSG_NOTICE($QT_LFLAGS)

LDFLAGS="$LDFLAGS $QT_LFLAGS -lstdc++"

LIBS="$LIBS $QT_LIBS"
INCLUDES="$INCLUDES $QT_INCPATH"
CXXFLAGS="$CXXFLAGS $QT_INCPATH"
CPPFLAGS="$CPPFLAGS $QT_INCPATH"
LDFLAGS="$LDFLAGS $QT_LFLAGS"

# Remove temp directory
#rm $tmp/Makefile
#rm $tmp/qttest.cpp
#rm $tmp/test.pro
#rmdir $tmp
if test $SYSTEM = "Linux"; then
AC_CHECK_HEADERS([QtCore/QtCore],[AC_DEFINE(HAVE_QTCORE_H)],[AC_MSG_ERROR("Could not find QtCore headers")])
AC_CHECK_LIB([QtCore],[main],[AC_DEFINE(HAVE_QTCORE)],[AC_MSG_ERROR("Could not find QtCore")])
AC_CHECK_HEADERS([QtGui/QtGui],[AC_DEFINE(HAVE_QTCORE_H)],[AC_MSG_ERROR("Could not find QtGui headers")])
AC_CHECK_LIB([QtGui],[main],[AC_DEFINE(HAVE_QTCORE)],[AC_MSG_ERROR("Could not find QtGui")])
AC_CHECK_HEADERS([QtOpenGL/QtOpenGL],[AC_DEFINE(HAVE_QTOPENGL_H)],[AC_MSG_ERROR("Could not find QtOpenGL headers")])
AC_CHECK_LIB([QtOpenGL],[main],[AC_DEFINE(HAVE_QTOPENGL)],[AC_MSG_ERROR("Could not find QtOpenGL")])
AC_CHECK_HEADERS([QtXml/QtXml],[AC_DEFINE(HAVE_QTXML_H)],[AC_MSG_ERROR("Could not find QtXml headers")])
AC_CHECK_LIB([QtXml],[main],[AC_DEFINE(HAVE_QTXML)],[AC_MSG_ERROR("Could not find QtXml")])
fi

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

LIBS="$LIBS $HEALPIX_DIR/lib/libchealpix.a"

#--------------------------------------

# Check QGLViewer
QGLVIEWER_DIR=xxx
AC_ARG_WITH([qglviewer-dir],
            AC_HELP_STRING([--with-qglviewer-dir=/path/to/QGLViewer],
                           [to specify the path to the Healpix directory.]),
            [QGLVIEWER_PATHS="$withval"],
            [QGLVIEWER_PATHS="/Library/Frameworks/QGLViewer.framework"])

for x in $QGLVIEWER_PATHS; do
	if test -d $x ; then
  		QGLVIEWER_DIR="$x"
  	fi
done

if test $SYSTEM = "Linux"; then
	if ! test $QGLVIEWER_DIR = xxx ; then
  		LIBS="$LIBS -L$QGLVIEWER_DIR/lib"
  		INCLUDES="$INCLUDES -I$QGLVIEWER_DIR/include"
  		CXXFLAGS="$CXXFLAGS -I$QGLVIEWER_DIR/include"
  		CPPFLAGS="$CPPFLAGS -I$QGLVIEWER_DIR/include"
	fi

	AC_CHECK_HEADERS([QGLViewer/qglviewer.h],[AC_DEFINE(HAVE_QGLVIEWER_H)],[AC_MSG_ERROR("Could not find qglviewer.h from the QGLViewer package")])
	AC_CHECK_LIB([qglviewer-qt4], [main],[AC_DEFINE(HAVE_QGLVIEWER)],[AC_MSG_ERROR("Could not link against QGLViewer")])

	LIBS="$LIBS -lqglviewer-qt4"
else
	if test $SYSTEM = "Darwin"; then
		if ! test -d $QGLVIEWER_DIR/Versions/Current; then
			AC_MSG_ERROR("QGLViewer Not Found");
		fi

		AC_MSG_NOTICE("QGLViewer found at $QGLVIEWER_DIR")

		LIBS="$LIBS -F$QGLVIEWER_DIR -framework QGLViewer"
  		INCLUDES="$INCLUDES -I$QGLVIEWER_DIR/Versions/Current/Headers/"
  		CXXFLAGS="$CXXFLAGS -I$QGLVIEWER_DIR/Versions/Current/Headers/"
  		CPPFLAGS="$CPPFLAGS -I$QGLVIEWER_DIR/Versions/Current/Headers/"
	fi
fi

#Final processing

AC_SUBST(CXXFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(PATH)
AC_SUBST(LIBS)
AC_SUBST(LDFLAGS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
