#-------------------------------------------------
#
# Project created by QtCreator 2011-05-25T10:54:53
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = Skyviewer
TEMPLATE = app

LIBS += -lchealpix -lm -lcfitsio

SOURCES += main.cpp\
        mainwindow.cpp \
    mapviewport.cpp \
    healpixmap.cpp \
    workspace.cpp

HEADERS  += mainwindow.h \
    mapviewport.h \
    healpixmap.h \
    workspace.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc
