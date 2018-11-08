#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T14:22:58
#
#-------------------------------------------------

QT       -= gui

TARGET = Subject
TEMPLATE = lib

DEFINES += SUBJECT_LIBRARY

SOURCES += Subject.cpp

HEADERS += Subject.h\
        subject_global.h \
    ../../../include/IObserver.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


include( ../../../../common.pri )
include( ../../../../lib.pri )
