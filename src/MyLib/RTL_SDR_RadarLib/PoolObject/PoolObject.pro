#-------------------------------------------------
#
# Project created by QtCreator 2018-04-16T17:21:54
#
#-------------------------------------------------

QT       += gui sql widgets

TARGET = PoolObject
TEMPLATE = lib

DEFINES += POOLOBJECT_LIBRARY

SOURCES += PoolObject.cpp \
    GraphicsObject.cpp \
    AirObject.cpp \
    ../../../include/coord/Position.cpp \
    ../../../include/coord/Conversions.cpp

HEADERS += PoolObject.h\
        poolobject_global.h \
    ../../../include/IObject.h \
    ../../../include/ILogger.h \
    ../../../include/IPoolObject.h \
    GraphicsObject.h \
    AirObject.h \
    ../../../include/coord/Position.h \
    ../../../include/coord/Conversions.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


include( ../../../../common.pri )
include( ../../../../lib.pri )
