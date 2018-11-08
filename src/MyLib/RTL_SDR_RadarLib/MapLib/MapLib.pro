#-------------------------------------------------
#
# Project created by QtCreator 2018-08-01T11:11:20
#
#-------------------------------------------------

QT       += network

TARGET = MapLib
TEMPLATE = lib

DEFINES += MAPLIB_LIBRARY

SOURCES += MapLib.cpp \
    ../../../include/coord/ScreenConversions.cpp \
    ../../../include/coord/Position.cpp \
    ../../../include/coord/Conversions.cpp

HEADERS += MapLib.h\
        maplib_global.h \
    ../../../include/IMapController.h \
    ../../../include/coord/ScreenConversions.h \
    ../../../include/coord/Position.h \
    ../../../include/coord/Conversions.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include( ../../../../common.pri )
include( ../../../../lib.pri )
