#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T16:41:19
#
#-------------------------------------------------

QT       += widgets opengl core gui

TARGET = GraphicsWidget
TEMPLATE = lib

DEFINES += GRAPHICSWIDGET_LIBRARY

SOURCES += GraphicsWidget.cpp \
    ../../../include/coord/ScreenConversions.cpp

HEADERS += GraphicsWidget.h\
        graphicswidget_global.h \
     ../../../include/IMapController.h \
    ../../../include/coord/ScreenConversions.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include( ../../../../common.pri )
include( ../../../../lib.pri )


LIBS += -lSubject \
#        -lMapLib \
#        -lCarrier
