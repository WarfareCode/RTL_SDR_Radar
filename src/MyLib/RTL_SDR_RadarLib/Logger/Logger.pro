#-------------------------------------------------
#
# Project created by QtCreator 2018-04-02T14:34:41
#
#-------------------------------------------------

QT       -= gui

TARGET = Logger
TEMPLATE = lib

DEFINES += LOGGER_LIBRARY

SOURCES += \
    Logger.cpp

HEADERS +=\
        logger_global.h \
        Logger.h \
        ../../../include/ILogger.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include( ../../../../common.pri )
include( ../../../../lib.pri )
