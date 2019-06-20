#-------------------------------------------------
#
# Project created by QtCreator 2018-07-04T11:04:01
#
#-------------------------------------------------

QT       += gui

TARGET = Carrier
TEMPLATE = lib

DEFINES += CARRIER_LIBRARY

SOURCES += Carrier.cpp \
    ../../../include/coord/Conversions.cpp \
    ../../../include/coord/Position.cpp \
    NullCarrier.cpp \
    ServiceLocator.cpp \
    NavigationSystem.cpp \
    NullNavigationSystem.cpp

HEADERS += carrier_global.h \
    ../../../include/coord/Conversions.h \
    ../../../include/coord/Position.h \
    Carrier.h\
    NullCarrier.h \
    ServiceLocator.h \
    ../../../include/ICarrierClass.h \
    ../../../include/INavigationSystem.h \
    NavigationSystem.h \
    NullNavigationSystem.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include( ../../../../common.pri )
include( ../../../../lib.pri )

