#-------------------------------------------------
#
# Project created by QtCreator 2011-05-25T10:54:53
#
#-------------------------------------------------

QT       += core gui opengl xml

TARGET = Skyviewer
TEMPLATE = app

INCLUDE_PATH = /usr/include
#INCLUDE_DIR = /usr/include
INCLUDE_DIR =/Library/Frameworks/QGLViewer.framework/Headers/
LIB_DIR = /usr/lib

LIBS +=  -lm -L$$LIB_DIR

SOURCES += main.cpp\
        mainwindow.cpp \
    mapviewport.cpp \
    healpixmap.cpp \
    workspace.cpp \
    mapviewer.cpp \
    face.cpp \
    boundary.cpp \
    vertice.cpp \
    tesselation.cpp \
    colortable.cpp \
    fieldmap.cpp \
    facevertices.cpp \
    facecache.cpp \
    texture.cpp \
    texturecache.cpp \
    maploader.cpp \
    mapoverlay.cpp \
    polarizationvectors.cpp \
    overlaycache.cpp \
    histogramviewer.cpp \
    histogramwidget.cpp \
    histogram.cpp \
    grid.cpp \
    viewportmanager.cpp

HEADERS  += mainwindow.h \
    mapviewport.h \
    healpixmap.h \
    workspace.h \
    mapviewer.h \
    face.h \
    boundary.h \
    vertice.h \
    tesselation.h \
    colortable.h \
    define_colortable.h \
    healpixutil.h \
    fieldmap.h \
    facevertices.h \
    facecache.h \
    blockingqueue.h \
    texture.h \
    texturecache.h \
    maploader.h \
    mapoverlay.h \
    polarizationvectors.h \
    overlaycache.h \
    histogramviewer.h \
    histogramwidget.h \
    histogram.h \
    grid.h \
    viewportmanager.h

FORMS    += mainwindow.ui \
    histogramwidget.ui \
    viewportmanager.ui

RESOURCES += \
    icons.qrc

 CONFIG += DEBUG

QMAKE_CXXFLAGS_DEBUG += -pg
QMAKE_LFLAGS_DEBUG += -pg -v -da -Q

macx: LIBS += $$PWD/../deps/Healpix_2.20a/lib/libchealpix.a

INCLUDEPATH += $$PWD/../deps/Healpix_2.20a/include
DEPENDPATH += $$PWD/../deps/Healpix_2.20a/include

macx: PRE_TARGETDEPS += $$PWD/../deps/Healpix_2.20a/lib/libchealpix.a

macx: LIBS += $$PWD/../deps/cfitsio/3.28/lib/libcfitsio.a

INCLUDEPATH += $$PWD/../deps/cfitsio/3.28/include

macx: PRE_TARGETDEPS += $$PWD/../deps/cfitsio/3.28/lib/libcfitsio.a

unix: CONFIG += link_pkgconfig
#unix: PKGCONFIG += gl



#QMAKE_LFLAGS += -F/Library/Frameworks/
#LIBS += -framework QGLViewer

QT_VERSION=$$[QT_VERSION]

### Unix configuration ###
unix {
  CONFIG -= debug debug_and_release
  CONFIG *= release

  isEmpty( PREFIX ) {
    # Try same INCLUDE_DIR and LIB_DIR parameters than for the make install.
    PREFIX=/usr
  }

  # LIB_NAME
  LIB_NAME = libQGLViewer*.so*
  macx|darwin-g++ {
    LIB_NAME = libQGLViewer.$${QMAKE_EXTENSION_SHLIB}
  }
  hpux {
    LIB_NAME = libQGLViewer*.sl*
  }

  !isEmpty( QGLVIEWER_STATIC ) {
    LIB_NAME = libQGLViewer*.a
  }

  # LIB_DIR
  isEmpty( LIB_DIR ) {
    LIB_DIR = $${PREFIX}/lib
  }

  !exists( $${LIB_DIR}/$${LIB_NAME} ) {
    exists( ../../../QGLViewer/$${LIB_NAME} ) {
      #message( The library was found in ../../../QGLViewer which will be set as the LIB_DIR )
      LIB_DIR = ../../../QGLViewer
    }
  }

  !exists( $${LIB_DIR}/$${LIB_NAME} ) {
    exists( ../../../QGLViewer-build-desktop/$${LIB_NAME} ) {
      #message( The library was found in ../../../QGLViewer-build-desktop which will be set as the LIB_DIR )
      LIB_DIR = ../../../QGLViewer-build-desktop
    }
  }

  macx|darwin-g++ {
    !exists( $${LIB_DIR}/$${LIB_NAME} ) {
      # DYLIB was not found, try to find FrameWork instead
      LIB_NAME = QGLViewer.framework/QGLViewer
      LIB_DIR = /Library/Frameworks
      !exists( $${LIB_DIR}/$${LIB_NAME} ) {
        exists( ../../../QGLViewer/$${LIB_NAME} ) {
          #message( The framework was found in ../../../QGLViewer which will be set as the LIB_DIR )
          LIB_DIR = ../../../QGLViewer
        }
      }

      !exists( $${LIB_DIR}/$${LIB_NAME} ) {
        exists( ../../../QGLViewer-build-desktop/$${LIB_NAME} ) {
          #message( The framework was found in ../../../QGLViewer-build-desktop which will be set as the LIB_DIR )
          LIB_DIR = ../../../QGLViewer-build-desktop
        }
      }

    }
  }

  !exists( $${LIB_DIR}/$${LIB_NAME} ) {
    message( Unable to find $${LIB_NAME} in $${LIB_DIR} )
    error( You should run qmake LIB_DIR=/path/to/QGLViewer/$${LIB_NAME} )
  }

  # The actual directory where the library/framework was found
  LIB_DIR_ABSOLUTE_PATH = $$system(cd $${LIB_DIR};pwd)


  # INCLUDE_DIR
  isEmpty( INCLUDE_DIR ) {
    INCLUDE_DIR = $${PREFIX}/include
  }

  !exists( $${INCLUDE_DIR}/QGLViewer/qglviewer.h ) {
    # qmake does not support Headers/QGLViewer FRAMEWORK_HEADERS.path
    #exists( $${LIB_DIR}/QGLViewer.framework/Headers/qglviewer.h ) {
    #  INCLUDE_DIR = $${LIB_DIR}/QGLViewer.framework/Headers
    #} else {
    exists( ../../../QGLViewer/qglviewer.h ) {
      # message( libQGLViewer header files were not installed in standard $${INCLUDE_DIR} directory )
      # message( Headers were found in ../../.. which will be set as the INCLUDE_DIR )
      INCLUDE_DIR = ../../..
    }
    #}
  }

  !exists( $${INCLUDE_DIR}/qglviewer.h ) {
    message( Unable to find QGLViewer/qglviewer.h in $${INCLUDE_DIR} )
    error( Use qmake INCLUDE_DIR=/path/to/QGLViewerDir )
  }


  macx|darwin-g++ {
    # On Mac, the lib path can be specified in the executable using install_name_tool
    contains( LIB_NAME, ".*QGLViewer.framework.*" ) {
      # If framework was not found in a standard directory
      !contains( LIB_DIR, ".*/Library/Frameworks/*$" ) {
        QMAKE_LFLAGS += -F$${LIB_DIR}
        !plugin:QMAKE_POST_LINK=install_name_tool -change QGLViewer.framework/Versions/2/QGLViewer $${LIB_DIR_ABSOLUTE_PATH}/QGLViewer.framework/Versions/2/QGLViewer $${TARGET}.app/Contents/MacOS/$${TARGET}
      }
      LIBS += -framework QGLViewer
    } else {
        !plugin:QMAKE_POST_LINK=install_name_tool -change libQGLViewer.2.dylib $${LIB_DIR_ABSOLUTE_PATH}/libQGLViewer.2.dylib $${TARGET}.app/Contents/MacOS/$${TARGET}
        LIBS *= -L$${LIB_DIR} -lQGLViewer
    }
  } else {
    isEmpty(QMAKE_RPATH) {
      !plugin:QMAKE_LFLAGS += -Wl,-rpath,$${LIB_DIR_ABSOLUTE_PATH}
    } else {
      !plugin:QMAKE_RPATHDIR *= $${LIB_DIR_ABSOLUTE_PATH}
    }
    LIBS *= -L$${LIB_DIR} -lQGLViewer
  }

  # Paths were correctly detected
  INCLUDEPATH *= $${INCLUDE_DIR}
  DEPENDPATH  *= $${INCLUDE_DIR}

  !isEmpty( QGLVIEWER_STATIC ) {
    LIBS *= $${LIB_DIR}/$${LIB_NAME}
  }

  macx|darwin-g++ {
    !contains( QT_VERSION, "^4.*" ) {
      # Qt3 only
      LIBS *= -lobjc
      CONFIG -= thread
    }
  }

  # Remove debugging options in release mode makes files much smaller
  release:QMAKE_CFLAGS_RELEASE -= -g
  release:QMAKE_CXXFLAGS_RELEASE -= -g

  # Intermediate files are created in an hidden folder
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}


win32 {
  ###### Uncomment this line to compile a debug version of your application
  CONFIG *= debug_and_release

  # Seems to be needed for Visual Studio with Intel compiler
  DEFINES *= WIN32

  !isEmpty( QGLVIEWER_STATIC ) {
    DEFINES *= QGLVIEWER_STATIC
  }

  # INCLUDE_DIR
  !exists( $${INCLUDE_DIR}/QGLViewer/qglviewer.h ) {
    exists( ../../../QGLViewer/qglviewer.h ) {
      # message( libQGLViewer header files were not installed in standard $${INCLUDE_DIR} directory )
      # message( Headers were found in ../../.. which will be set as the INCLUDE_DIR )
      INCLUDE_DIR = ../../..
    }
  }

  !exists( $${INCLUDE_DIR}/QGLViewer/qglviewer.h ) {
    message( Unable to find QGLViewer/qglviewer.h in $${INCLUDE_DIR} )
    error( Use qmake INCLUDE_DIR=/path/to/QGLViewerDir )
  }

  # Paths were correctly detected
  INCLUDEPATH *= $${INCLUDE_DIR}
  DEPENDPATH  *= $${INCLUDE_DIR}

  # LIB_NAME
  LIB_NAME = QGLViewer
  build_pass:CONFIG(debug, debug|release) {
    LIB_NAME = $$join(LIB_NAME,,,d)
  }
  LIB_NAME = $$join(LIB_NAME,,,2) #TODO 2

  win32-g++: LIB_FILE_NAME = lib$${LIB_NAME}.a
  !win32-g++: LIB_FILE_NAME = $${LIB_NAME}.lib

  isEmpty( LIB_DIR ) {
    LIB_DIR = C:\\Windows\\System32
  }

  !exists( $${LIB_DIR}/$${LIB_FILE_NAME} ) {
    exists( ../../../QGLViewer/$${LIB_FILE_NAME} ) {
      LIB_DIR = ../../../QGLViewer
    }
  }

  !exists( $${LIB_DIR}/$${LIB_FILE_NAME} ) {
     exists( ../../../QGLViewer/Release/$${LIB_FILE_NAME} ) {
       LIB_DIR = ../../../QGLViewer/Release
     }
  }

  !exists( $${LIB_DIR}/$${LIB_FILE_NAME} ) {
    exists( ../../../QGLViewer/Debug/$${LIB_FILE_NAME} ) {
      LIB_DIR = ../../../QGLViewer/Debug
    }
  }

  !exists( $${LIB_DIR}/$${LIB_FILE_NAME} ) {
    exists( ../../../QGLViewer-build-desktop/$${LIB_FILE_NAME} ) {
      LIB_DIR = ../../../QGLViewer-build-desktop
    }
  }

  !exists( $${LIB_DIR}/$${LIB_FILE_NAME} ) {
     exists( ../../../QGLViewer-build-desktop/Release/$${LIB_FILE_NAME} ) {
       LIB_DIR = ../../../QGLViewer/Release-build-desktop
     }
  }

  !exists( $${LIB_DIR}/$${LIB_FILE_NAME} ) {
    exists( ../../../QGLViewer/Debug-build-desktop/$${LIB_FILE_NAME} ) {
      LIB_DIR = ../../../QGLViewer/Debug-build-desktop
    }
  }

  !exists( $${LIB_DIR}/$${LIB_FILE_NAME} ) {
    message( Unable to find $${LIB_FILE_NAME} in $${LIB_DIR} )
    ########error( You should run qmake LIB_DIR=/path/to/QGLViewer/$${LIB_FILE_NAME} )
  }

  win32-g++ {
    # The actual directory where the library/framework was found
    # LIB_DIR_ABSOLUTE_PATH = $$system(cd $${LIB_DIR} && cd)

    isEmpty( QGLVIEWER_STATIC ) {
      LIBS *= -L$${LIB_DIR} -l$${LIB_NAME}
    } else {
      LIBS *= $${LIB_DIR}/$${LIB_FILE_NAME}
    }
  }

  !win32-g++ {
    # Use the Qt DLL version. Only needed with Qt3
    !contains( QT_VERSION, "^4.*" ) {
      DEFINES *= QT_DLL QT_THREAD_SUPPORT
    }

    LIBS *= -L$${LIB_DIR} -l$${LIB_NAME}
  }
}


macx|darwin-g++ {
  ICON = $${INCLUDEPATH}/QGLViewer/qglviewer.icns
}

win32 {
  RC_FILE = ../../qglviewer.rc
}

