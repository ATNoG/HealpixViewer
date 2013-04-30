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
ARCH=`getconf LONG_BIT`
X64="64"

GCC_VERSION=$(gcc -v 2>&1|grep version |cut -d ' ' -f 3)

if test $SYSTEM = "Darwin"; then
	CPPFLAGS="$CPPFLAGS -mmacosx-version-min=10.5"
	CXXFLAGS="$CXXFLAGS -mmacosx-version-min=10.5"
	CFLAGS="$CFLAGS -mmacosx-version-min=10.5"
	if test $ARCH = $X64; then
		CPPFLAGS="$CPPFLAGS -arch x86_64"
		CXXFLAGS="$CXXFLAGS -arch x86_64"
		CFLAGS="$CFLAGS -arch x86_64"
	else
		CPPFLAGS="$CPPFLAGS -arch i386"
		CXXFLAGS="$CXXFLAGS -arch i386"
		CFLAGS="$CFLAGS -arch i386"
	fi
	AC_MSG_NOTICE("Compiling for MAC OSX 10.5 and up with Universial binary")
fi

CFLAGS="$CFLAGS -I/usr/include -I/usr/local/include"
CXXFLAGS="$CXXFLAGS -I/usr/include -I/usr/local/include"
CPPFLAGS="$CPPFLAGS -I/usr/include -I/usr/local/include"

GLPATHS="/usr/X11"
GLDIR=xxx
for x in $GLPATHS; do
	if test -d $x/include/GL; then
		GLDIR="$x"
		break
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
            [QTPATHS="/usr/local/Trolltech/Qt-4.7.0 /c/Qt/4.7.0 /usr /usr/local /usr/local/Trolltech/Qt-4.8.0/ "$(pwd)"/deps/qt-everywhere-opensource-src-4.8.0"])

for x in $QTPATHS; do
    if test -d "$x/include/qt4/QtCore" ; then
        QTDIR="$x"
        QTINC_PREFIX="qt4"
    elif test -d "$x/include/qt/QtCore" ; then
        QTDIR="$x"
        QTINC_PREFIX="qt"
    elif test -d "$x/include/QtCore" ; then
        QTDIR="$x"
    else
        continue
    fi
    break
done
# TODO: check for framework (Mac OS)
#if test $QTDIR = xxx ; then
#   AC_MSG_ERROR(Could not locate QT 4.7)
#fi

QTBINDIR=xxx
AC_ARG_WITH([qtbin-dir],
             AC_HELP_STRING([--with-qtbin-dir=/path/to/QTbinaries],
                            [to specify the path to the QT binaries.]),
             [QTBIN_PATHS="$withval"],
             [QTBIN_PATHS="$QTDIR/bin /usr/lib/qt4/bin /usr/bin"])

for x in $QTBIN_PATHS; do
    if test -d "$x" && test -f "$x/qmake" ; then
       QTBINDIR="$x"
       break
    fi
done

if test $QTBINDIR = xxx ; then
  AC_MSG_ERROR(Could not locate QT binaries)
fi



MOC="$QTBINDIR/moc"
AC_MSG_CHECKING(for $MOC)
	if test ! -x "$MOC"; then
		AC_MSG_ERROR("Unable to find moc")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(MOC)

RCC="$QTBINDIR/rcc"
AC_MSG_CHECKING(for $RCC)
	if test ! -x "$RCC"; then
		AC_MSG_ERROR("Unable to find rcc")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(RCC)

UIC="$QTBINDIR/uic"
AC_MSG_CHECKING(for $UIC)
	if test ! -x "$UIC"; then
		AC_MSG_ERROR("Unable to find uic")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(UIC)

QMAKE="$QTBINDIR/qmake"
AC_MSG_CHECKING(for $QMAKE)
	if test ! -x "$QMAKE"; then
		AC_MSG_ERROR("Unable to find qmake")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(QMAKE)


if test $SYSTEM = "Darwin"; then

MACDEPLOYQT="$QTBINDIR/macdeployqt"
AC_MSG_CHECKING(for $MACDEPLOYQT)
	if test ! -x "$MACDEPLOYQT"; then
		AC_MSG_ERROR("Unable to find macdeployqt")
	fi
AC_MSG_RESULT(yes)
AC_SUBST(MACDEPLOYQT)

fi


TMPDIR=tmp
mkdir $TMPDIR 2>/dev/null
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
echo "QT += xml gui opengl core" >> $tmp/test.pro

if test $SYSTEM = "Darwin"; then
	if test $ARCH = $X64; then
		echo "CONFIG += x86 x86_64 " >> $tmp/test.pro
	else
		echo "CONFIG += x86 " >> $tmp/test.pro
	fi
fi

# generate Makefile
if ! test $SYSTEM = "Darwin"; then
	qmake_try=`cd $tmp; $QMAKE`
else
	qmake_try=`cd $tmp; $QMAKE test.pro -spec macx-g++`
fi
AC_TRY_EVAL(qt_try)
if test x"$ac_status" != x0; then
	AC_MSG_ERROR(qmake failed)
fi

# LIBS
QT_LIBS_AUX=`grep LIBS.*= $tmp/Makefile | sed -e 's/\\$(SUBLIBS)//g' | awk '{sub(/^LIBS +=/, ""); print}'`
if test -z "$QT_LIBS_AUX"; then
	AC_MSG_ERROR(Unable to find Qt libraries)
fi

ARR=$(echo $QT_LIBS_AUX | tr ";" "\n")
QT_LIBS=""
for x in $ARR
do
    prefix="${x:0:2}"
    path="${x:2}"
    if expr "$path" : '\..*' > /dev/null; then
    	abspath=`cd $tmp/$path; pwd`
        QT_LIBS="$QT_LIBS $prefix$abspath"
    else
        QT_LIBS="$QT_LIBS $x"
    fi
done
AC_MSG_NOTICE($QT_LIBS)

# Include directives
QT_INCPATH_AUX=`grep INCPATH.*= $tmp/Makefile | awk '{sub(/^INCPATH +=/, ""); print}'`
if test -z "$QT_INCPATH_AUX"; then
    AC_MSG_ERROR(Unable to find Qt include path)
fi

ARR=$(echo $QT_INCPATH_AUX | tr ";" "\n")
QT_INCPATH=""
for x in $ARR
do
    prefix="${x:0:2}"
    path="${x:2}"
    if expr "$path" : '\..*' > /dev/null; then
    	abspath=`cd "$tmp/$path"; pwd`
	QT_INCPATH="$QT_INCPATH $prefix$abspath"
    else
        QT_INCPATH="$QT_INCPATH $x"
    fi
done
AC_MSG_NOTICE($QT_INCPATH)

# LFLAGS
QT_LFLAGS=`grep LFLAGS.*= $tmp/Makefile | awk '{sub(/^LFLAGS +=/, ""); print}'`
if test -z "$QT_LFLAGS"; then
	AC_MSG_ERROR(Unable to find Qt link flags)
fi
AC_MSG_NOTICE("QTFLAGS: $QT_LFLAGS:)


LIBS="$LIBS $QT_LIBS"

if ! test $SYSTEM = "Darwin"; then
    LIBS="$LIBS -lGLU"
fi

INCLUDES="$INCLUDES $QT_INCPATH"
CXXFLAGS="$CXXFLAGS $QT_INCPATH"
CPPFLAGS="$CPPFLAGS $QT_INCPATH"
LDFLAGS="$LDFLAGS -lstdc++"


# Remove temp directory
rm $tmp/Makefile
rm $tmp/qttest.cpp
rm $tmp/test.pro
rmdir $tmp
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
CFITSIO_LIB_DIR=xxx
CFITSIO_HEADER_DIR=xxx
AC_ARG_WITH([cfitsio-dir],
             AC_HELP_STRING([--with-cfitsio-dir=/path/to/cfitsio],
                            [to specify the path to the CFITSIO directory.]),
             [CFITSIO_PATHS="$withval"],
             [CFITSIO_PATHS="/usr/local/cfitsio /usr /usr/local/homebrew/"])

for x in $CFITSIO_PATHS; do
    if test -f "$x/include/fitsio.h" ; then
  	CFITSIO_HEADER_DIR="$x/include"
       	CFITSIO_LIB_DIR="$x/lib"
    elif test -f "$x/include/cfitsio/fitsio.h" ; then
       	CFITSIO_HEADER_DIR="$x/include/cfitsio"
	CFITSIO_LIB_DIR="$x/lib"
    else
        continue
    fi
    break
done


if ! test $CFITSIO_HEADER_DIR = xxx ; then
   AC_MSG_NOTICE("Using cfitsio from $CFITSIO_HEADER_DIR")
  LIBS="$LIBS -L$CFITSIO_LIB_DIR"
  INCLUDES="$INCLUDES -I$CFITSIO_HEADER_DIR"
  CXXFLAGS="$CXXFLAGS -I$CFITSIO_HEADER_DIR"
  CPPFLAGS="$CPPFLAGS -I$CFITSIO_HEADER_DIR"
fi

AC_LANG_PUSH([C])
if ! test $SYSTEM = "Darwin"; then
AC_CHECK_HEADERS([fitsio.h],[AC_DEFINE(HAVE_FITSIO_H)],[AC_MSG_ERROR("Could not find fitsio.h from the cfitsio package")])
fi

AC_CHECK_LIB([cfitsio], [fits_init_cfitsio],[AC_DEFINE(HAVE_FITSIO)],[AC_MSG_ERROR("Could not link against cfitsio")])
AC_LANG_POP([C])

LIBS="$LIBS -lcfitsio"
#------------------------------------

# Check Healpix

if ! test [[ $HEALPIX_TARGET ]]; then
        if test $SYSTEM = "Darwin"; then
                HEALPIX_TARGET="osx"
        else
                HEALPIX_TARGET="generic_gcc"
        fi
fi

HEALPIX_DIR=xxx
AC_ARG_WITH([healpix-dir],
             AC_HELP_STRING([--with-healpix-dir=/path/to/Healpix],
                            [to specify the path to the Healpix directory.]),
             [HEALPIX_PATHS="$withval"],
             [HEALPIX_PATHS="$HEALPIX ./deps/Healpix_2.20a/"])

for x in $HEALPIX_PATHS; do
    if test -d $x ; then
       HEALPIX_DIR="$x"
       break
    fi
done

if ! test $HEALPIX_DIR = xxx ; then
  LIBS="$LIBS -L$HEALPIX_DIR/lib -L$HEALPIX_DIR/src/cxx/$HEALPIX_TARGET/lib"
  INCLUDES="$INCLUDES -I$HEALPIX_DIR/include -I$HEALPIX_DIR/src/cxx/$HEALPIX_TARGET/include -I$HEALPIX_DIR/src/cxx/cxxsupport -I$HEALPIX_DIR/src/cxx/Healpix_cxx"
  CXXFLAGS="$CXXFLAGS -I$HEALPIX_DIR/include -I$HEALPIX_DIR/src/cxx/$HEALPIX_TARGET/include -I$HEALPIX_DIR/src/cxx/cxxsupport -I$HEALPIX_DIR/src/cxx/Healpix_cxx"
  CPPFLAGS="$CPPFLAGS -I$HEALPIX_DIR/include -I$HEALPIX_DIR/src/cxx/$HEALPIX_TARGET/include -I$HEALPIX_DIR/src/cxx/cxxsupport -I$HEALPIX_DIR/src/cxx/Healpix_cxx"
fi


if ! test $SYSTEM = "Darwin"; then
AC_CHECK_HEADERS([chealpix.h],[AC_DEFINE(HAVE_CHEALPIX_H)],[AC_MSG_ERROR("Could not find chealpix.h from the Healpix package")])
fi
#AC_CHECK_LIB([chealpix], [read_healpix_map],[AC_DEFINE(HAVE_HEALPIX)],[AC_MSG_ERROR("Could not link against Healpix")])

if test $SYSTEM = "Darwin"; then
	LIBS="$LIBS $HEALPIX_DIR/lib/libchealpix.a -lhealpix_cxx -lcxxsupport -fopenmp"
else
	LIBS="$LIBS -lchealpix -lhealpix_cxx -lcxxsupport -fopenmp"
fi


#--------------------------------------

# Check QGLViewer
QGLVIEWER_DIR=xxx
AC_ARG_WITH([qglviewer-dir],
            AC_HELP_STRING([--with-qglviewer-dir=/path/to/QGLViewer],
                           [to specify the path to the Healpix directory.]),
            [QGLVIEWER_PATHS="$withval"],
            [QGLVIEWER_PATHS="${HOME}/Library/Frameworks /Library/Frameworks /usr /usr/local"])

if test $SYSTEM = "Darwin"; then
  for x in $QGLVIEWER_PATHS; do
    if test -d $x/QGLViewer.framework ; then
        QGLVIEWER_DIR=$x
        break
    fi
  done

  LIBS="$LIBS -F$QGLVIEWER_DIR -framework QGLViewer"
  INCLUDES="$INCLUDES -F$QGLVIEWER_DIR"
  CXXFLAGS="$CXXFLAGS -F$QGLVIEWER_DIR"
  CPPFLAGS="$CPPFLAGS -F$QGLVIEWER_DIR"

else
  for x in $QGLVIEWER_PATHS; do
    if test -d $x/include/QGLViewer ; then
        QGLVIEWER_DIR=$x
        break
    fi
  done

  LIBS="$LIBS -L$QGLVIEWER_DIR/lib"
  INCLUDES="$INCLUDES -I$QGLVIEWER_DIR/include"
  CXXFLAGS="$CXXFLAGS -I$QGLVIEWER_DIR/include"
  CPPFLAGS="$CPPFLAGS -I$QGLVIEWER_DIR/include"

	LIBS="$LIBS -lQGLViewer"
fi

LIBS="$LIBS -lGLU -lcfitsio"

AC_CHECK_HEADERS([QGLViewer/qglviewer.h],[AC_DEFINE(HAVE_QGLVIEWER_H)],[AC_MSG_ERROR("Could not find qglviewer.h from the QGLViewer package")])


#Final processing

AC_SUBST(CXXFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(PATH)
AC_SUBST(LIBS)
AC_SUBST(LDFLAGS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
