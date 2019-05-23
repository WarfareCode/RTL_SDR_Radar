#-------------------------------------------------
#
# Project created by QtCreator 2018-11-09T10:52:46
#
#-------------------------------------------------

QT       -= gui

TARGET = RTL_SDR_Reciver
TEMPLATE = lib

DEFINES += RTL_SDR_RECIVER_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        RTL_SDR_Reciver.cpp \
    ../../../../import/sdr_dev/src/librtlsdr.c \
    ../../../../import/sdr_dev/src/tuner_e4k.c \
    ../../../../import/sdr_dev/src/tuner_fc0012.c \
    ../../../../import/sdr_dev/src/tuner_fc0013.c \
    ../../../../import/sdr_dev/src/tuner_fc2580.c \
    ../../../../import/sdr_dev/src/tuner_r82xx.c

HEADERS += \
        RTL_SDR_Reciver.h \
        rtl_sdr_reciver_global.h \ 
    ../../../include/IReciverDevice.h \
    ../../../../import/sdr_dev/include/constant.h \
    ../../../../import/sdr_dev/include/reg_field.h \
    ../../../../import/sdr_dev/include/rtl-sdr_export.h \
    ../../../../import/sdr_dev/include/rtl-sdr.h \
    ../../../../import/sdr_dev/include/rtlsdr_i2c.h \
    ../../../../import/sdr_dev/include/tuner_e4k.h \
    ../../../../import/sdr_dev/include/tuner_fc0012.h \
    ../../../../import/sdr_dev/include/tuner_fc0013.h \
    ../../../../import/sdr_dev/include/tuner_fc2580.h \
    ../../../../import/sdr_dev/include/tuner_r82xx.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include( ../../../../common.pri )
include( ../../../../lib.pri )

DISTFILES += \
    ../../../../import/sdr_dev/rtl-sdr.rules

LIBS += -lusb-1.0 \
        -lLogger
